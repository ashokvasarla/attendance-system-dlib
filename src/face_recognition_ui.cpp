
#include "face_recognition_ui.hpp"

face_recognition_ui::face_recognition_ui() : 
    img(*this), 
    registered_students_label(*this), 
    results_box(*this),
    presented_students_label(*this),
    registered_box(*this)
{
    img.set_pos(10,60);
    registered_students_label.set_pos(720,60);
    registered_students_label.set_text ("Total Registered Students::");
    registered_box.set_pos(720,80);
    registered_box.set_size(200,200);
    presented_students_label.set_pos(720, 300);
    presented_students_label.set_text ("Presented Students::");
    results_box.set_pos(720,320);
    results_box.set_size(200,200);
    
    set_title("Face Recognition Attendance system");
    set_size(1100,700);
    set_pos(0,0);
    show();
}

face_recognition_ui::~face_recognition_ui()
{
    close_window();
}

template<typename image_type>
void face_recognition_ui::set_webcam_image(const image_type& cimg) {
        img.set_image(cimg);
}
