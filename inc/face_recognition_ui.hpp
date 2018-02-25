#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/image_transforms.h>
#include <sqlite3.h>

using namespace std;
using namespace dlib;

class face_recognition_ui : public image_window 
{
public:
    
    face_recognition_ui();
    ~face_recognition_ui();

    template<typename image_type>
    void set_webcam_image(const image_type& cimg);
    bool startCondition;
    std::vector<std::string> registered_students;
    std::map<std::string, std::string> present_absent_students;
    std::string maxDate;
    int rowCounter;
    text_grid reportGrid;
    label showAttendanceLabel;
    int dateCounter;
    // text_box results_box;
    // text_box registered_box;

private:
    // image_window img;
    image_widget img;
    button startAttendance, stopAttendance, showReport;
    void on_start_button_clicked();
    void on_stop_button_clicked();
    void on_show_button_clicked();
    // label registered_students_label;
    // label presented_students_label;
    // perspective_display disp;
    sqlite3 *db;
    int rc;
    char *zErrMsg = 0;
    char *sql;
};
