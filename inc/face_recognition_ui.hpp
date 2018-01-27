#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/image_transforms.h>

using namespace std;
using namespace dlib;

class face_recognition_ui : public image_window 
{
public:
    
    face_recognition_ui();
    ~face_recognition_ui();

    template<typename image_type>
    void set_webcam_image(const image_type& cimg);
    text_box results_box;
    text_box registered_box;
private:
    // image_window img;
    image_widget img;
    label registered_students_label;
    label presented_students_label;
    // perspective_display disp;
};
