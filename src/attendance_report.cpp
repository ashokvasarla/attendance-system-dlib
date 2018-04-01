#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/image_transforms.h>
#include <sqlite3.h>
#include <ctime>

using namespace dlib;
#define GRID_ROWS 50
#define GRID_COLS 32
class attendance_report : public drawable_window
{
public:
    attendance_report();
    ~attendance_report();
    void on_menu_file_open();
    void on_open_file_selected ( const std::string& file_name);
    text_grid reportGrid;
    int columnCounter; 
    int rowCounter;
    std::string date_save;
    std::string name_save;
private:
    menu_bar mbar;
    sqlite3 *db;
    int rc;
    char *zErrMsg = 0;
};

int callback_to_showreport(void *NotUsed, int argc, char **argv, char **azColName) {
    attendance_report *thisPtr = static_cast<attendance_report*>(NotUsed);
    
    for(int i = 0; i<argc; i++) {
        // printf("columnCounter= %d , i = %d -- %s = %s\n", thisPtr->columnCounter, i, azColName[i], argv[i] ? argv[i] : "NULL");
        if (strcmp(azColName[i] , "AttendanceDT") == 0 && thisPtr->columnCounter != GRID_COLS)
        {
            if( strcmp(thisPtr->date_save.c_str(), argv[i]) != 0)
            {
                thisPtr->date_save = cast_to_string(argv[i]);
                thisPtr->columnCounter++;
                thisPtr->rowCounter=1;
            }
            else
            {
                thisPtr->reportGrid.set_text(0,thisPtr->columnCounter,cast_to_string(argv[i]));
                thisPtr->reportGrid.set_background_color(0,thisPtr->columnCounter,rgb_pixel(150,150,250));
            } 
        }
        else if(strcmp(azColName[i],"NAME") == 0 && thisPtr->columnCounter != GRID_COLS)
        {
            thisPtr->reportGrid.set_text(thisPtr->rowCounter, 0, cast_to_string(argv[i]));
        }
        else if (thisPtr->columnCounter != GRID_COLS)
        {
            thisPtr->reportGrid.set_text(thisPtr->rowCounter,thisPtr->columnCounter,cast_to_string(argv[i]));
        }
    }
    thisPtr->rowCounter++;
    return 0;
}

attendance_report::attendance_report()
    : mbar(*this),
    reportGrid(*this)
{
    // Now setup the menu bar.  We will have two menus.  A File and Help menu.
    mbar.set_number_of_menus(2);
    mbar.set_menu_name(0,"File",'F');
    mbar.set_menu_name(1,"Help",'H');

    // add the entries to the File menu.
    mbar.menu(0).add_menu_item(menu_item_text("Open",   *this, &attendance_report::on_menu_file_open,    'O'));
    reportGrid.set_pos(10, 60);
    reportGrid.set_size(1000,700);
    reportGrid.set_grid_size(GRID_ROWS,GRID_COLS);
    reportGrid.set_column_width(0,150);
    reportGrid.set_column_width(1,100);
    reportGrid.set_column_width(2,100);
    reportGrid.set_text(0,0,cast_to_string("Name"));
    reportGrid.set_editable(0,0,false);

    reportGrid.set_background_color(0,0,rgb_pixel(150,150,250));
    // reportGrid.set_text(0,1,cast_to_string("Date"));
    // reportGrid.set_editable(0,0,false);
    // reportGrid.set_background_color(0,1,rgb_pixel(150,150,250));
    // reportGrid.set_text(0,2,cast_to_string("Attendance"));
    // reportGrid.set_editable(0,0,false);
    // reportGrid.set_background_color(0,2,rgb_pixel(150,150,250));

    set_size(1100,700);
    set_pos(0,0);
    show();
}

attendance_report::~attendance_report()
{

}

void attendance_report::on_menu_file_open()
{
    // display a file chooser window and when the user choses a file
    // call the on_open_file_selected() function
    open_existing_file_box(*this, &attendance_report::on_open_file_selected);
}

void attendance_report::on_open_file_selected ( const std::string& file_name)
{
    columnCounter=0;
    rowCounter=1;
    /* Open database */
    rc = sqlite3_open(file_name.c_str(), &db);
    if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      message_box("Message","Can't open database");
    } else {
      fprintf(stderr, "Opened database successfully\n");
    }
    std::string del_string = "SELECT * FROM STUDENT_ATTENDANCE";

    rc = sqlite3_exec(db,del_string.c_str(), callback_to_showreport, this, &zErrMsg);
    if( rc != SQLITE_OK ) {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    } else {
       fprintf(stdout, "Operation done successfully\n");
    }
}

int main()
{
    attendance_report win_report;
    while( !win_report.is_closed())
    {

    }


}