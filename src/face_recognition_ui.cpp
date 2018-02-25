
#include "face_recognition_ui.hpp"

face_recognition_ui::face_recognition_ui() : 
    img(*this), 
    // registered_students_label(*this),
    // results_box(*this),
    // presented_students_label(*this),
    // registered_box(*this),
    startAttendance(*this),
    stopAttendance(*this),
    showReport(*this),
    reportGrid(*this)
{
    set_title("Face Recognition Attendance system");
    set_size(1100,700);
    set_pos(0,0);
    show();

    img.set_pos(10,60);
    startAttendance.set_pos(725,60);
    startAttendance.set_name("Start Attendance");
    stopAttendance.set_pos(895,60);
    stopAttendance.set_name("Stop Attendance");

    showReport.set_pos(830, 120);
    showReport.set_name("Show Report");

    startAttendance.set_click_handler(*this,&face_recognition_ui::on_start_button_clicked);
    stopAttendance.set_click_handler(*this, &face_recognition_ui::on_stop_button_clicked);
    showReport.set_click_handler(*this, &face_recognition_ui::on_show_button_clicked);

    // registered_students_label.set_pos(720,60);
    // registered_students_label.set_text ("Total Registered Students::");
    // registered_box.set_pos(720,80);
    // registered_box.set_size(200,200);
    // presented_students_label.set_pos(720, 300);
    // presented_students_label.set_text ("Presented Students::");
    // results_box.set_pos(720,320);
    // results_box.set_size(200,200);

    reportGrid.set_pos(720, 160);
    reportGrid.set_size(330,330);
    reportGrid.set_grid_size(13,3);
    reportGrid.set_column_width(0,150);
    reportGrid.set_column_width(1,100);
    reportGrid.set_column_width(2,50);
    reportGrid.set_text(0,0,cast_to_string("Name"));
    reportGrid.set_editable(0,0,false);

    reportGrid.set_background_color(0,0,rgb_pixel(150,150,250));
    reportGrid.set_text(0,1,cast_to_string("Date"));
    reportGrid.set_editable(0,0,false);
    reportGrid.set_background_color(0,1,rgb_pixel(150,150,250));
    reportGrid.set_text(0,2,cast_to_string("Attendance"));
    reportGrid.set_editable(0,0,false);
    reportGrid.set_background_color(0,2,rgb_pixel(150,150,250));

    // reportGrid.set_text(1,0,cast_to_string("Ashok kumar"));
    // reportGrid.set_text(1,1,cast_to_string("02/24/18"));
    // reportGrid.set_text(1,2,cast_to_string("P"));

    // reportGrid.set_text(1,2,"test");
    // reportGrid.set_text(1,3,"test");
}

face_recognition_ui::~face_recognition_ui()
{
    close_window();
}

template<typename image_type>
void face_recognition_ui::set_webcam_image(const image_type& cimg) {
        img.set_image(cimg);
}

void face_recognition_ui::on_start_button_clicked() {

}

void face_recognition_ui::on_stop_button_clicked() {

}

void face_recognition_ui::on_show_button_clicked() {

}
