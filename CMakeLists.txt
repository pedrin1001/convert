cmake_minimum_required(VERSION 2.8)
project( Convert )
find_package( OpenCV REQUIRED )
add_executable( Convert main.c convert.c convert.h )
target_link_libraries( Convert ${OpenCV_LIBS} )

