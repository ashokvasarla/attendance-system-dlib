
#    _____  ______          _____      _______ _    _ ______       
#   |  __ \|  ____|   /\   |  __ \    |__   __| |  | |  ____|      
#   | |__) | |__     /  \  | |  | |      | |  | |__| | |__         
#   |  _  /|  __|   / /\ \ | |  | |      | |  |  __  |  __|        
#   | | \ \| |____ / ____ \| |__| |      | |  | |  | | |____       
#   |_|__\_\______/_/_ __\_\_____/__ _   |_|__|_|_ |_|______|_ _ _ 
#  / ____/ __ \|  \/  |  \/  |  ____| \ | |__   __/ ____| | | | | |
# | |   | |  | | \  / | \  / | |__  |  \| |  | | | (___   | | | | |
# | |   | |  | | |\/| | |\/| |  __| | . ` |  | |  \___ \  | | | | |
# | |___| |__| | |  | | |  | | |____| |\  |  | |  ____) | |_|_|_|_|
#  \_____\____/|_|  |_|_|  |_|______|_| \_|  |_| |_____/  (_|_|_|_)
#                                                                  
#
#
# This is a CMake makefile.  CMake is a tool that helps you build C++ programs.
# You can download CMake from http://www.cmake.org.  This CMakeLists.txt file
#


cmake_minimum_required(VERSION 2.8.12)
#project(examples)

set(CMAKE_C_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(
   ../dlib-library/include
   ../sqlite
   inc
)

link_directories(
   ../dlib-library/lib
   ../sqlite
)

find_package(OpenCV QUIET)
   if (OpenCV_FOUND)
      include_directories(${OpenCV_INCLUDE_DIRS})
      message(OpencvIncludeDirs-- ${OpenCV_INCLUDE_DIRS})
      add_executable(registration_ui src/registration_ui.cpp)
      target_link_libraries(registration_ui dlib ${OpenCV_LIBS} sqlite3)
      add_executable(webcam_face_pose_ex src/webcam_face_pose_ex.cpp src/face_recognition_ui.cpp)
      target_link_libraries(webcam_face_pose_ex dlib ${OpenCV_LIBS} sqlite3)
   else()
      message("OpenCV not found, so we won't build the webcam_face_pose_ex example.")
endif()


