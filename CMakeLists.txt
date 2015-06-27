cmake_minimum_required(VERSION 3.2)
project(Free_Copter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    deploy/Build.sh
    Free-Copter.xcodeproj/project.xcworkspace/xcshareddata/Free-Copter.xccheckout
    Free-Copter.xcodeproj/project.xcworkspace/xcuserdata/master.xcuserdatad/UserInterfaceState.xcuserstate
    Free-Copter.xcodeproj/project.xcworkspace/contents.xcworkspacedata
    Free-Copter.xcodeproj/xcuserdata/master.xcuserdatad/xcschemes/xcschememanagement.plist
    Free-Copter.xcodeproj/project.pbxproj
    src/main.cpp
    Makefile)

include_directories(src/include)

add_executable(Free_Copter ${SOURCE_FILES})