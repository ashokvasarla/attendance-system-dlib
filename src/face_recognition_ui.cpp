
#include "face_recognition_ui.hpp"

#define GRID_ROWS 50

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int callback_to_update(void *NotUsed, int argc, char **argv, char **azColName) {
    face_recognition_ui *thisPtr = static_cast<face_recognition_ui*>(NotUsed);
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "now");
        if (argv[i])
        {
        if(strcmp(azColName[i],"MAX(AttendanceDT)") == 0) {
            thisPtr->maxDate = argv[i] ? argv[i] : "now";
            thisPtr->dateCounter=1;
        }
        }
        else
        {
            thisPtr->dateCounter=0;
            thisPtr->maxDate = "now";
        }
    }
    // printf("\n");
    return 0;
}

int callback_to_showreport(void *NotUsed, int argc, char **argv, char **azColName) {
    face_recognition_ui *thisPtr = static_cast<face_recognition_ui*>(NotUsed);
    int i;
    for(i = 0; i<argc; i++) {
        // printf("rowCounter= %d , columnCounter = %d -- %s = %s\n", thisPtr->rowCounter, i, azColName[i], argv[i] ? argv[i] : "NULL");
        if(strcmp(azColName[i],"NAME") == 0 && thisPtr->rowCounter != GRID_ROWS)
        {
            thisPtr->reportGrid.set_text(thisPtr->rowCounter,0,cast_to_string(argv[i]));
        }
        else if (strcmp(azColName[i] , "AttendanceDT") == 0 && thisPtr->rowCounter != GRID_ROWS)
        {
            thisPtr->reportGrid.set_text(thisPtr->rowCounter,1,cast_to_string(argv[i]));
        }
        else if (thisPtr->rowCounter != GRID_ROWS)
        {
            thisPtr->reportGrid.set_text(thisPtr->rowCounter,2,cast_to_string(argv[i]));
        }
    }
    // printf("\n");
    thisPtr->rowCounter++;
    return 0;
}

face_recognition_ui::face_recognition_ui() : 
    img(*this), 
    // registered_students_label(*this),
    // results_box(*this),
    // presented_students_label(*this),
    // registered_box(*this),
    startAttendance(*this),
    startTimeLabel(*this),
    stopAttendance(*this),
    endTimeLabel(*this),
    showReport(*this),
    reportGrid(*this),
    startCondition(false),
    dateCounter(0),
    maxDate("\0"),
    showAttendanceLabel(*this),
    start_time_box(*this),
    stop_time_box(*this),
    mon(*this),tue(*this),wed(*this),thu(*this),fri(*this),sat(*this),sun(*this),
    mon_flag("Mon"), tue_flag("Tue"), wed_flag("Wed"), thu_flag("Thu"), fri_flag("Fri"), sat_flag(""), sun_flag("")
{
    set_title("Face Recognition Attendance system");
    set_size(1100,700);
    set_pos(0,0);
    show();
    startTimeLabel.set_text("Start Time:");
    startTimeLabel.set_pos(725,30);
    start_time_box.set_pos(785,30);
    start_time_box.set_size(80,20);
    endTimeLabel.set_text("End Time:");
    endTimeLabel.set_pos(875,30);
    stop_time_box.set_pos(935,30);
    stop_time_box.set_size(80,20);
    mon.set_pos(725,60);
    mon.set_name("Monday");
    mon.set_checked();
    mon.set_click_handler(*this, &face_recognition_ui::cb_check_box_enabled);
    tue.set_pos(805,60);
    tue.set_name("Tuesday");
    tue.set_checked();
    tue.set_click_handler(*this, &face_recognition_ui::cb_check_box_enabled);
    wed.set_pos(885,60);
    wed.set_name("Wednesday");
    wed.set_checked();
    wed.set_click_handler(*this, &face_recognition_ui::cb_check_box_enabled);
    thu.set_pos(985,60);
    thu.set_name("Thursday");
    thu.set_checked();
    thu.set_click_handler(*this, &face_recognition_ui::cb_check_box_enabled);
    fri.set_pos(725,90);
    fri.set_name("Friday");
    fri.set_checked();
    fri.set_click_handler(*this, &face_recognition_ui::cb_check_box_enabled);
    sat.set_pos(805,90);
    sat.set_name("Saturday");
    sat.set_click_handler(*this, &face_recognition_ui::cb_check_box_enabled);
    sun.set_pos(885,90);
    sun.set_name("Sunday");
    sun.set_click_handler(*this, &face_recognition_ui::cb_check_box_enabled);

    dlib::queue<string>::kernel_2a_c qos;
    string a;
    a = "9:00"; qos.enqueue(a);
    a = "9:30"; qos.enqueue(a);
    a = "10:00"; qos.enqueue(a);
    a = "10:30"; qos.enqueue(a);
    a = "11:00"; qos.enqueue(a);
    a = "11:30"; qos.enqueue(a);
    a = "12:00"; qos.enqueue(a);
    a = "12:30"; qos.enqueue(a);
    a = "13:00"; qos.enqueue(a);
    a = "13:30"; qos.enqueue(a);
    a = "14:00"; qos.enqueue(a);
    a = "14:30"; qos.enqueue(a);
    a = "15:00"; qos.enqueue(a);
    a = "15:30"; qos.enqueue(a);
    a = "16:00"; qos.enqueue(a);
    a = "16:30"; qos.enqueue(a);

    start_time_box.load(qos);
    start_time_box.select(2);
    string d;
    dlib::queue<string>::kernel_2a_c qos1;
    d = "14:00"; qos1.enqueue(d);
    d = "14:30"; qos1.enqueue(d);
    d = "15:00"; qos1.enqueue(d);
    d = "15:30"; qos1.enqueue(d);
    d = "16:00"; qos1.enqueue(d);
    d = "16:30"; qos1.enqueue(d);

    stop_time_box.load(qos1);

    img.set_pos(10,60);
    startAttendance.set_pos(725,130);
    startAttendance.set_name("Start Attendance");
    stopAttendance.set_pos(895,130);
    stopAttendance.set_name("Stop Attendance");

    showReport.set_pos(830, 180);
    showReport.set_name("Show Report");

    startAttendance.set_click_handler(*this,&face_recognition_ui::on_start_button_clicked);
    stopAttendance.set_click_handler(*this, &face_recognition_ui::on_stop_button_clicked);
    showReport.set_click_handler(*this, &face_recognition_ui::on_show_button_clicked);

    showAttendanceLabel.set_pos(250,600);

    // registered_students_label.set_pos(720,60);
    // registered_students_label.set_text ("Total Registered Students::");
    // registered_box.set_pos(720,80);
    // registered_box.set_size(200,200);
    // presented_students_label.set_pos(720, 300);
    // presented_students_label.set_text ("Presented Students::");
    // results_box.set_pos(720,320);
    // results_box.set_size(200,200);

    reportGrid.set_pos(700, 220);
    reportGrid.set_size(370,500);
    reportGrid.set_grid_size(GRID_ROWS,3);
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
    /* Open database */
    rc = sqlite3_open("database/attendance.db", &db);
    if( rc ) {
          fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
          message_box("Message","Can't open database");
       } else {
          fprintf(stderr, "Opened database successfully\n");
       }
    /* Create SQL statement */
    sql = "CREATE TABLE IF NOT EXISTS STUDENT_ATTENDANCE("  \
     "NAME TEXT NOT NULL," \
     "AttendanceDT date default current_date   NOT NULL," \
     "Attendance           TEXT     NOT NULL);";

     // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    // message_box("Message",zErrMsg);
    sqlite3_free(zErrMsg);
    } else {
    fprintf(stdout, "Table created successfully\n");
    }
    // sql = "SELECT MAX(AttendanceDT) FROM STUDENT_ATTENDANCE";
    // /* Execute SQL statement */
    // rc = sqlite3_exec(db, sql, callback_to_update, this, &zErrMsg);

    // if( rc != SQLITE_OK ) {
    //    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    //    sqlite3_free(zErrMsg);
    // } else {
    //    fprintf(stdout, "Operation done successfully\n");
    // }
}

face_recognition_ui::~face_recognition_ui()
{
    if ( startCondition == true)
    {
    startCondition = false;
    attendance_database_store();
    }
    close_window();
}

template<typename image_type>
void face_recognition_ui::set_webcam_image(const image_type& cimg) {
        img.set_image(cimg);
}

void face_recognition_ui::on_start_button_clicked() {
    char buffer[3];
    struct tm *info;
    time_t rawtime;
    time( &rawtime );
    info = localtime(&rawtime);
    strftime(buffer, 9, "%a", info);
    if (strcmp(buffer, sat_flag.c_str()) == 0 || strcmp(buffer, sun_flag.c_str()) == 0 || strcmp(buffer, mon_flag.c_str()) == 0 || strcmp(buffer, tue_flag.c_str()) == 0 || strcmp(buffer, wed_flag.c_str()) == 0 || strcmp(buffer, thu_flag.c_str()) == 0 || strcmp(buffer, fri_flag.c_str()) == 0)
        startCondition = true;
    else
        startCondition = false;
}

void face_recognition_ui::on_stop_button_clicked() {
    if ( startCondition == true)
    {
    startCondition = false;
    delete_records();
    attendance_database_store();
    show_report();
    }
}

void face_recognition_ui::delete_records()
{
    std::string del_string = "DELETE FROM STUDENT_ATTENDANCE WHERE AttendanceDT=date('now')";

    rc = sqlite3_exec(db,del_string.c_str(), callback, 0, &zErrMsg);
}

void face_recognition_ui::attendance_database_store()
{

    for(std::map<std::string, std::string>::iterator it = present_absent_students.begin() ; it != present_absent_students.end(); ++it)
    {
        std::cout << "Name:: " << it->first << "    " << "Attendance:: " << it->second << std::endl;
        /* Create SQL statement */
        // sprintf(sql_insert, "INSERT INTO STUDENT_ATTENDANCE (NAME,AttendanceDT, Attendance) " \
              "VALUES ( '%s', date('now', '%d days'), '%s');", it->first.c_str(), dateCounter, it->second.c_str());

        std::string sql_string = "INSERT INTO STUDENT_ATTENDANCE (NAME,AttendanceDT, Attendance) VALUES('" + it->first +"'," + "date('now','0 days'),'"+ it->second + "');";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql_string.c_str(), callback, 0, &zErrMsg);

        if( rc != SQLITE_OK ){
           fprintf(stderr, "SQL error: %s\n", zErrMsg);
           sqlite3_free(zErrMsg);
        } else {
         fprintf(stdout, "Records created successfully\n");
        }
    }
}

void face_recognition_ui::on_show_button_clicked() {
    show_report();
}

void face_recognition_ui::show_report()
{
    rowCounter=1;
    sql = "SELECT * FROM STUDENT_ATTENDANCE";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback_to_showreport, this, &zErrMsg);

    if( rc != SQLITE_OK ) {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    } else {
       fprintf(stdout, "Operation done successfully\n");
    }
}

void face_recognition_ui::cb_check_box_enabled (toggle_button&) {
    if (mon.is_checked()) {
        mon_flag = "Mon";
    }
    else {
        mon_flag =  "";   
    }
    if (tue.is_checked()) {
        tue_flag = "Tue";
    }
    else {
        tue_flag = "";
    }
    if (wed.is_checked()) {
        wed_flag = "Wed"; 
    }
    else {
        wed_flag = "";
    }
    if (thu.is_checked()) { 
        thu_flag = "Thu";
    }
    else { 
        thu_flag = "";
    }
    
    if (fri.is_checked()){ 
        fri_flag = "Fri";
    }
    else { 
        fri_flag = "";
    }

    if (sat.is_checked()){ 
        sat_flag = "Sat";
    }
    else{ 
        sat_flag = "";
    }
    if (sun.is_checked()){ 
        sun_flag = "Sun";
    }
    else{ 
        sun_flag = "";
    }
}
