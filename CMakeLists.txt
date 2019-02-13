cmake_minimum_required(VERSION 2.8)

project (pbuff_zmq CXX) # PROJECT_NAME=hello_world

set(CMAKE_BUILD_TYPE Release)
set(CXX g++)

list( APPEND CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")
add_definitions(-std=c++11)



# PROTOBUF Related Declarations
set(PROTOBUF_LIBRARY /vagrant/install/lib/libprotobuf.so)
set(PROTOBUF_INCLUDE_DIR /vagrant/install/include)
set(PROTOBUF_PROTOC_EXECUTABLE /vagrant/install/bin/protoc)

find_package(Protobuf REQUIRED)



# MESSAGE Handling
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/messages/addressbook.proto)
add_subdirectory(messages)



# WRITING AND READING CODE
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/messages)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

#file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
#file(GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp)
#add_executable(${PROJECT_NAME} ${SOURCES} ${HDRS} ${PROTO_SRCS} ${PROTO_HDRS})
#target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})

add_executable(run_pubsub src/my_pub_sub_proto.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(run_pubsub ${PROTOBUF_LIBRARIES} zmq pthread)

#add_executable(zmq_pub src/write.cpp inc/write.hpp ${PROTO_SRCS} ${PROTO_HDRS})
#target_link_libraries(zmq_pub ${PROTOBUF_LIBRARIES} zmq)

#add_executable(zmq_sub src/read.cpp inc/read.hpp ${PROTO_SRCS} ${PROTO_HDRS})
#target_link_libraries(zmq_sub ${PROTOBUF_LIBRARIES} zmq)


#add_executable(file_write src/file_write.cpp ${PROTO_SRCS} ${PROTO_HDRS})
#target_link_libraries(file_write ${PROTOBUF_LIBRARIES} zmq)

#add_executable(file_read src/file_read.cpp  ${PROTO_SRCS} ${PROTO_HDRS})
#target_link_libraries(file_read ${PROTOBUF_LIBRARIES} zmq)

