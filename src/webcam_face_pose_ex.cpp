
#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

#include <dlib/image_io.h>
#include <dlib/dnn.h>
#include <map>
#include <iterator>
#include "face_recognition_ui.hpp"
#include <sqlite3.h>
#include <ctime>

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------
// The next bit of code defines a ResNet network.  It's basically copied
// and pasted from the dnn_imagenet_ex.cpp example, except we replaced the loss
// layer with loss_metric and made the network somewhat smaller.  Go read the introductory
// dlib DNN examples to learn what all this stuff means.
//
// Also, the dnn_metric_learning_on_images_ex.cpp example shows how to train this network.
// The dlib_face_recognition_resnet_model_v1 model used by this example was trained using
// essentially the code shown in dnn_metric_learning_on_images_ex.cpp except the
// mini-batches were made larger (35x15 instead of 5x5), the iterations without progress
// was set to 10000, and the training dataset consisted of about 3 million images instead of
// 55.  Also, the input layer was locked to images of size 150.
template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET> 
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
                            alevel0<
                            alevel1<
                            alevel2<
                            alevel3<
                            alevel4<
                            max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
                            input_rgb_image_sized<150>
                            >>>>>>>>>>>>;

// std::map<string,matrix<rgb_pixel>> known_image_map;
// std::map<string,matrix<rgb_pixel>> known_face_map;
std::map<string, matrix<float,0,1>> known_face_descriptors_map;
frontal_face_detector detector;
shape_predictor pose_model;
// And finally we load the DNN responsible for face recognition.
anet_type net;
std::mutex global_lock;
std::map<std::string, std::string> total_students;

static int callback(void *NotUsed, int columns, char **columnValue, char **azColName) {
    string regName, regPhoto;
    face_recognition_ui *thisPtr = static_cast<face_recognition_ui*>(NotUsed);
    for (int i = 0; i<columns; i++) {
        if(strcmp(azColName[i],"NAME") == 0)
        {
            regName = columnValue[i];
            // total_students.append(regName +'\n');
            total_students.insert(std::pair<std::string,std::string>(regName,"A"));
        }
        else if (strcmp(azColName[i] , "PHOTO") == 0)
        {
            regPhoto = columnValue[i];
        }
    }
    printf("%s\n", regPhoto);
    matrix<rgb_pixel> known_img;
    try {
        load_image(known_img, regPhoto);
    }
    catch (exception e) {
            printf("Load Image exception:: %s\n",e.what());
    }
    
    // if (detector != NULL && pose_model != NULL)
    // {

    for (auto face : detector(known_img))
    {
    auto shape = pose_model(known_img, face);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(known_img, get_face_chip_details(shape,150,0.25), face_chip);
    // known_face_map.insert(std::pair<string,matrix<rgb_pixel>>(it->first, move(face_chip)));
    matrix<float,0,1> face_desc = net(face_chip);
    global_lock.lock();
    known_face_descriptors_map.insert(std::pair<string, matrix<float,0,1>>(regName, move(face_desc)));
    global_lock.unlock();
    // printf("StudentInfo:: %s\n", regName);
    }
    // thisPtr->registered_box.set_text(total_students);
    // }    

    // known_image_map.insert(std::pair<string,matrix<rgb_pixel>>(regName,known_img));
    return 0;
}

int main(int argc, char** argv)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    char buffer[9];
    struct tm *info;
    time_t rawtime;
    face_recognition_ui faceUI;
    std::vector<std::string> results;
    // std::cout << date('now');

    // current date/time based on current system
    time( &rawtime );

    info = localtime(&rawtime);

    strftime(buffer, 9, "%x", info);
    cout << "Time:: " << asctime(info) << endl;
    cout << "The local time is: " << buffer << endl;



    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Unable to connect to camera" << endl;
            return 1;
    }

    /*
    if (argc != 4)
    {
        cout << "Run this example by invoking it like this: " << endl;
        cout << "   ./webcam ashok.jpg sai.png bindu.png" << endl;
        cout << endl;
        return 1;
    }*/

    try {
        // Load face detection and pose estimation models.
        detector = get_frontal_face_detector();
        
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        deserialize("dlib_face_recognition_resnet_model_v1.dat") >> net;

        printf("Load complete \n");

        /*
        std::map<string,matrix<rgb_pixel>> known_image_map;
        
        for(int i=1;i<4;i++)
        {
            matrix<rgb_pixel> known_img;
            load_image(known_img, argv[i]);
            known_image_map.insert(std::pair<string,matrix<rgb_pixel>>(argv[i],move(known_img)));
            printf("loop:: %s\n", argv[i]);
        }
        printf("known_image_map size::%d \n", known_image_map.size());
        
        std::map<string,matrix<rgb_pixel>> known_face_map;
        
        for( std::map<string,matrix<rgb_pixel>>::iterator it = known_image_map.begin(); it != known_image_map.end(); ++it)
        {
            for (auto face : detector(it->second))
            {
            // std::vector<rectangle> faces = detector(it->second);
            // printf("%d\n", faces.size());
            auto shape = pose_model(it->second, face);
            matrix<rgb_pixel> face_chip;
            extract_image_chip(it->second, get_face_chip_details(shape,150,0.25), face_chip);
            known_face_map.insert(std::pair<string,matrix<rgb_pixel>>(it->first, move(face_chip)));
            printf("Loop1:: %s\n", it->first);
            }
            // printf("Loop1:: %s\n", it->first);
        }
        printf("known_face_map size::%d \n", known_face_map.size());
        std::map<string, matrix<float,0,1>> known_face_descriptors_map;
        for(std::map<string,matrix<rgb_pixel>>::iterator it = known_face_map.begin(); it != known_face_map.end(); it++)
        {
            printf("Test loop:: %s\n", it->first);
            // save_png(it->second, "test.png");
            matrix<float,0,1> face_desc = net(it->second);
            known_face_descriptors_map.insert(std::pair<string, matrix<float,0,1>>(it->first, move(face_desc)));
        }
        */        
    }
    catch(serialization_error& e) {
        cout << "You need dlib's default face landmarking model file to run." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << "   http://dlib.net/files/dlib_face_recognition_resnet_model_v1.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e) {
        cout << "exception raised " << e.what() << endl;
    }

    /* Open database */
    rc = sqlite3_open("database/students.db", &db);
    if ( rc ) {
        printf( "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "SELECT * from STUDENT_REGISTRATION";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, &faceUI , &zErrMsg);
   
    if ( rc != SQLITE_OK ) {
      printf("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      printf("Operation done successfully\n");
    }

    // Grab and process frames until the main window is closed by the user.
    while (!faceUI.is_closed()) {
        // Grab a frame
        cv::Mat temp;
        if (!cap.read(temp)) {
            break;
        }
        // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
        // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
        // long as temp is valid.  Also don't do anything to temp that would cause it
        // to reallocate the memory which stores the image as that will make cimg
        // contain dangling pointers.  This basically means you shouldn't modify temp
        // while using cimg.
        cv_image<bgr_pixel> cimg(temp);

        std::vector<matrix<rgb_pixel>> faces;

        std::vector<full_object_detection> shapes;
        for (auto face : detector(cimg)) {
        auto shape = pose_model(cimg, face);
        matrix<rgb_pixel> face_chip;
        extract_image_chip(cimg, get_face_chip_details(shape,150,0.25), face_chip);
        faces.push_back(move(face_chip));
        // Also put some boxes on the faces so we can see that the detector is finding them.
        shapes.push_back(shape);
        // win.clear_overlay();
        // win.set_image(cimg);
        // win.add_overlay(render_face_detections(shapes));
        }
        // std::cout << "Number of shapes::" << shapes.size() << std::endl;
        // std::cout << "Number of faces::" << faces.size() << std::endl;
        
        faceUI.clear_overlay();
        faceUI.set_image(cimg);
        faceUI.add_overlay(render_face_detections(shapes));
        
        std::vector<matrix<float,0,1>> face_descriptors = net(faces);

        for ( std::vector<matrix<float,0,1>>::iterator it = face_descriptors.begin() ; it != face_descriptors.end() ; ++it)
        {
            global_lock.lock();
            for (std::map<string, matrix<float,0,1>>::iterator it_desc = known_face_descriptors_map.begin() ; it_desc != known_face_descriptors_map.end() ; ++it_desc)
            {
                float diff = length((*it) - it_desc->second);
                if( diff < 0.40 )
                {
                    // std::cout << it_desc->first << std::endl;
                    if(std::find(std::begin(results), std::end(results), it_desc->first) == std::end(results) ) {
                        results.push_back(it_desc->first);
                        std::cout << it_desc->first << std::endl;
                    }
                }
            }
            global_lock.unlock();

        }
        // faceUI.results_box.set_text(results);
    for(std::vector<std::string>::iterator it = results.begin(); it != results.end(); ++it)
    {
        std::map<std::string, std::string>::iterator map_it;
        map_it = total_students.find(*it);
        if(map_it != total_students.end())
        {
          map_it->second = "P";
        }
    }

    for(std::map<std::string, std::string>::iterator it = total_students.begin() ; it != total_students.end(); ++it)
    {
        std::cout << "Name:: " << it->first << "Attendance:: " << it->second << std::endl;
    }

    } // window close while
    sqlite3_close(db);
    return 0;
} // main end
