# protobuf_zmq_test

## Introduction

This project was made as a test stub to test the capabilities of protobuf as a serialization tool when sending ZMQ messages. This project spawns two threads, a ZMQ Publisher and ZMQ Subscriber. They communicate based on a given IP address, port, and filter string. The publisher thread serializes an addressbook Protobuf message that is incremented after each transmission. The subscriber thread receives the message, it deserializes it and prints the contents of the message. Once again, this a proof-of-concept that verifies that we are capable of creating complicated protobuf messages and send them using ZMQ sockets.

## Build Process

### Prepare the Environment

To spin up a virtual machine (VM) that sets up a Linux environment (Ubuntu 16.04) for you to run this project, a vagrant file has been prepared. Build and compile the project by going to `/vagrant` folder in the VM. Otherwise, I'll assume that the following has already been done:

1. You're compiling this project on Ubuntu 16.04 machine or better (preferred Ubuntu 16.04)
2. Dependencies for both protobuf and this project have been met
   ```sudo apt-get install -yq git cmake autoconf automake libtool curl make g++ unzip pkg-config build-essential libzmq3-dev```
3. Protobuf has been installed globally (can be found [here](https://github.com/protocolbuffers/protobuf.git))
   - If you've built protobuf as a local build, then be sure to modify the toolkit **LOCAL_BUILD.cmake**

#### New To Vagrant?

1. Download vagrant [here](https://www.vagrantup.com/downloads.html)
2. Download the project 
2. Open your command line and navigate to your working copy of this project
3. Run the following vagrant-specific command:
```vagrant up```

What will happen are two crucial things:
- Vagrant will spin up a VM and 
- it will run a script that installs all dependencies needed to run this project (including installing Protobuf)

It will take several minutes to set up. However, this process only happens the first time; the next time you spin up the VM, it will not attempt to set up the environment again. 

Once it's done setting up the environment, you can **ssh** into the VM using the vagrant-specific command `vagrant ssh`. Note that vagrant shares the project folder that holds the vagrant file with the VM in the head location `/vagrant`, so after accessing the VM, navigate to that area to build this project.

As a summary, you will run the following commands after running `vagrant up`:
```
vagrant ssh
cd /vagrant
```

### Compile the Project

To compile this project, perform the following steps:
1. Go to the project folder (if you're using the vagrant file, the project is located in `/vagrant`)
```
# if you're using the vagrant file, do this
cd /vagrant

# if you're compiling this project on a machine of your choosing, 
git clone https://github.com/aarif4/protobuf_zmq_test.git
cd ./protobuf_zmq_test
```
2. Make a build folder 
```
mkdir build
cd build
```
3. Make the project using CMake
```
cmake .. # To perform local build, pass in the flag -DCMAKE_INSTALL_PREFIX=/path/to/staging/area
make
```

## Run the Executable

The executable **pbuff_zmq** is the program that you want to run. This program can takes in two arguments:

1. IP Address -> Information of what IP and Port the program should use. 
                 **BY DEFAULT**, the program will use 127.0.0.1:50011 
2. Filter     -> ZMQ filter to use when passing messages around from the publisher thread to the subscriber thread. The string does not need any quotes when passed in (e.g. test). 
                 **BY DEFAULT**, the program will use an empty filter "".


```
# Use default arguments
./pbuf_zmq

# Define the IP Address to use and use default Filter argument
./pbuff_zmq 127.0.0.1:12131

# Define the IP Address and Filter
./pbuff_zmq 127.0.0.1:12131 test_filter_string
```

## Credit

Major thanks to [benoitrosa's project](https://github.com/benoitrosa/Cpp_python_protobuf) that helped me structure and write my CMakeLists files.


