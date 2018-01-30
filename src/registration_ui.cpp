// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This is an example illustrating the use of the gui api from the dlib C++ Library.


    This is a pretty simple example.  It makes a window with a user
    defined widget (a draggable colored box) and a button.  You can drag the
    box around or click the button which increments a counter. 
*/

#include <dlib/gui_widgets.h>
#include <sstream>
#include <string>
#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dlib/image_io.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/image_transforms.h>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
using namespace dlib;


cv_image<bgr_pixel> cimg;
std::mutex mutex_var;
cv::Mat temp;

int callback_to_update(void *NotUsed, int argc, char **argv, char **azColName);
int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      // argv[i] ?  
   }
   printf("\n");
   return 0;
}

//  ----------------------------------------------------------------------------

class registration_ui : public drawable_window 
{
    /*
        Here I am going to define our window.  In general, you can define as 
        many window types as you like and make as many instances of them as you want.
    */
public:
    registration_ui(
    ) : // All widgets take their parent window as an argument to their constructor.
        studentNameLabel(*this),
        studentRegnoLabel(*this),
        studentNameTextbox(*this),
        studentRegnoTextbox(*this),
        registerButton(*this),
        snapButton(*this),
        mbar(*this),
        img(*this),
        img_snap(*this),
        snap_flag(false)
    {
        std::shared_ptr<bdf_font> new_font(new bdf_font);
        //ifstream fin("C:/bindu_face_recog/attendance_system/gohu.bdf");
        //new_font->read_bdf_file(fin,0xFFFF);
        //studentRegnoLabel.set_main_font(new_font);

        img.set_pos(10,60);
        counter = 1;
        img_snap.set_pos(720,60);

        // img_snap.set_size (350,350);
        matrix<rgb_pixel> img;
        try {
        load_image(img, "camera-icon.png");
        }
        catch( exception e)
        {
            std::cerr << "exception raised in camera icon load:: " << e.what() << std::endl;
        }
        img_snap.set_image(img);

        snapButton.set_name("  Snap  ");
        snapButton.set_pos(800,540); // Y - 420 
        
        
        studentNameLabel.set_pos(700,505);
        studentNameLabel.set_text ("Student Name:");
        
        studentNameTextbox.set_pos(800,500);
        studentNameTextbox.set_width(250);

        studentRegnoLabel.set_pos(700,465);
        studentRegnoLabel.set_text ("Student RegNo:");

        studentRegnoTextbox.set_pos(800,460);
        studentRegnoTextbox.set_width(250);
        // studentRegnoTextbox.

        registerButton.set_pos(870,540);

        registerButton.set_name("Register");
        

        // set which function should get called when the button gets clicked.  In this case we want
        // the on_button_clicked member to be called on *this.
        registerButton.set_click_handler(*this,&registration_ui::on_registration_button_clicked);
        snapButton.set_click_handler(*this, &registration_ui::on_snap_button_clicked);
        // snapButton.set_size(50,50);
        // Let's also make a simple menu bar.  
        // First we say how many menus we want in our menu bar.  In this example we only want 1.
        mbar.set_number_of_menus(1);
        // Now we set the name of our menu.  The 'M' means that the M in Menu will be underlined
        // and the user will be able to select it by hitting alt+M
        mbar.set_menu_name(0,"Menu",'M');

        // Now we add some items to the menu.  Note that items in a menu are listed in the
        // order in which they were added.

        mbar.menu(0).add_menu_item(menu_item_text("About",*this,&registration_ui::show_about,'A'));

        // set the size of this window
        set_size(1100,700);
        set_pos(0,0);

        set_title("Student's Registration Form");
        show();

        /* Open database */
        rc = sqlite3_open("database/students.db", &db);
        if( rc ) {
              fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
              message_box("Message","Can't open database");
           } else {
              fprintf(stderr, "Opened database successfully\n");
           }
        /* Create SQL statement */
        sql = "CREATE TABLE STUDENT_REGISTRATION("  \
         "REGNO INT PRIMARY KEY     NOT NULL," \
         "NAME           TEXT    NOT NULL," \
         "PHOTO           TEXT     NOT NULL);";

         // Execute SQL statement 
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
        if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        // message_box("Message",zErrMsg);
        sqlite3_free(zErrMsg);
        } else {
        fprintf(stdout, "Table created successfully\n");
        }

        sql = "SELECT MAX(REGNO) FROM STUDENT_REGISTRATION";
       /* Execute SQL statement */
       rc = sqlite3_exec(db, sql, callback_to_update, this, &zErrMsg);
       
       if( rc != SQLITE_OK ) {
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
          sqlite3_free(zErrMsg);
       } else {
          fprintf(stdout, "Operation done successfully\n");
       }

    } 

    ~registration_ui(
    )
    {
        // You should always call close_window() in the destructor of window
        // objects to ensure that no events will be sent to this window while 
        // it is being destructed.  
        close_window();
    }

    template <
            typename image_type
            >
    void set_webcam_image(const image_type& cimg) {
        img.set_image(cimg);
    }

    text_field studentRegnoTextbox;

private:

    void on_registration_button_clicked ()
    {
        std::string name, regno,directory, filename;
        directory = "photos/";
        name = studentNameTextbox.text();
        regno = studentRegnoTextbox.text();
        filename = directory+name+".png";
        if( name == "\0" || regno == "\0")
            message_box("Message","Please enter details : name and number");
        else
        {
        if(snap_flag == true)
        {

                /* Create SQL statement */
                char sql_insert[100];
                int num = atoi(regno.c_str());
                sprintf(sql_insert, "INSERT INTO STUDENT_REGISTRATION (REGNO,NAME,PHOTO) " \
                     "VALUES ( %d, '%s', '%s');", num, name.c_str(), filename.c_str());

               /* Execute SQL statement */
               rc = sqlite3_exec(db, sql_insert, callback, 0, &zErrMsg);
               
               if( rc != SQLITE_OK ){
                  fprintf(stderr, "SQL error: %s\n", zErrMsg);
                  message_box("INSERT ERROR", zErrMsg);
                  sqlite3_free(zErrMsg);
               } else {
                fprintf(stdout, "Records created successfully\n"); 
                matrix<rgb_pixel> img;
                try {
                load_image(img, "snap.png");
                save_png(img, filename);
                }
                catch(exception e )
                {
                }
                message_box("Message","Registration success");
                studentNameTextbox.set_text("");
                studentRegnoTextbox.set_text(std::to_string(++num));
               }

        }
        else
        {
            message_box("Message","Take a snap first");
        }
        }

    }

    void on_snap_button_clicked ()
    {
        mutex_var.lock();
        // snapped_image = cimg;
        array2d<bgr_pixel> sizeImg(350, 350);
        // 
        // img_snap.set_image(cimg);
        cv::imwrite("snap.png", temp);
        matrix<rgb_pixel> img;
        try {
        load_image(img, "snap.png");
        }
        catch(exception e )
        {

        }
        resize_image(img, sizeImg);
        img_snap.set_image(sizeImg);
        snap_flag = true;
        mutex_var.unlock();
        
    }

    void show_about()
    {
        message_box("About","This is registration window");
    }

    unsigned long counter;
    label studentNameLabel;
    label studentRegnoLabel;
    text_field studentNameTextbox;
    button registerButton;
    button snapButton;
    cv_image<bgr_pixel> snapped_image;
    // color_box cb;
    menu_bar mbar;
    image_widget img;
    image_widget img_snap;
    sqlite3 *db;
    int rc;
    char *zErrMsg = 0;
    char *sql;
    bool snap_flag;
};

int callback_to_update(void *NotUsed, int argc, char **argv, char **azColName)
{
    registration_ui *ui_pointer = static_cast<registration_ui*>(NotUsed);

    for(int i = 0; i<argc; i++) {
      // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if( argv[i] != NULL )
        {
            int num = atoi(argv[i]);
            ++num;
            ui_pointer->studentRegnoTextbox.set_text(std::to_string(num));
        }
        else
            {ui_pointer->studentRegnoTextbox.set_text("1");}
   }
    
    return 0;
}
//  ----------------------------------------------------------------------------

int main()
{
    // create our window
    registration_ui my_window;

    // matrix<rgb_pixel> img;
    // load_image(img, "ashok.png");
    // my_window.set_webcam_image(img);

    try
    {
        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }
        while( !my_window.is_closed())
        {
            // Grab a frame
            mutex_var.lock();
            if (!cap.read(temp))
            {
                break;
            }
            // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            
             cimg = temp;
            mutex_var.unlock();

            my_window.set_webcam_image(cimg);

        }
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
    // wait until the user closes this window before we let the program 
    // terminate.
    // my_window.wait_until_closed();

    return 0;
}

//  ----------------------------------------------------------------------------

// Normally, if you built this application on MS Windows in Visual Studio you
// would see a black console window pop up when you ran it.  The following
// #pragma directives tell Visual Studio to not include a console window along
// with your application.  However, if you prefer to have the console pop up as
// well then simply remove these #pragma statements.
#ifdef _MSC_VER
#   pragma comment( linker, "/entry:mainCRTStartup" )
#   pragma comment( linker, "/SUBSYSTEM:WINDOWS" )
#endif

//  ----------------------------------------------------------------------------

