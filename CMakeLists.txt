cmake_minimum_required(VERSION 2.8)

project (pbuff_zmq CXX) 
set(CMAKE_BUILD_TYPE Release)

set(CXX g++)
list( APPEND CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")
add_definitions(-std=c++11)





################################################################################
# Add necessary libraries and make certain directories available to CMake
################################################################################
find_package(Protobuf REQUIRED)

# Include certain directories 
include_directories(${PROTOBUF_INCLUDE_DIRS}) # For protobuf calls in pb.* files
include_directories(${CMAKE_CURRENT_BINARY_DIR}/messages) # will have pb.* files



################################################################################
# Compile Protobuf message for usage in C++ codebase
################################################################################
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/messages/addressbook.proto)
add_subdirectory(messages) # Will make ${PROTO_SRCS} and ${PROTO_HDRS} available



################################################################################
# Compile C++ Codebase
################################################################################
add_executable(${PROJECT_NAME} src/my_pub_sub_proto.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} zmq pthread)


