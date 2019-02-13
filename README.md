# protobuf_zmq_test


This is a test area that was made to prove that protobuf messages can be passed around using ZMQ sockets. In this script, there are two threads that are started:
- runPub: Thread that serializes a protobuf message and publishes using a ZMQ PUB socket
- runSub: Thread that receives messages using ZMQ sub socket and deserializes protobuf messages

To run the script, do the following tasks

## Install Protobuf
- Clone protobuf from here: https://github.com/protocolbuffers/protobuf.git
- cd into protobuf/src
- follow their installtion instructions and install it to a local folder if you want (I'll assume that it's locally installed in /vagrant/install)

So here's the basic script that you run (assuming that you want to make a local build)
```
sudo apt-get install autoconf automake libtool curl make g++ unzip
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
git submodule update --init --recursive
./autogen.sh
./configure --prefix=/vagrant/install # assuming that you want to do a local build, use --prefix to dictate where you want to install to
make
make install # run using sudo if you're doing a global build
sudo ldconfig # if you do a global build
```

## Build protobuf_zmq_test
- clone this repo
- make a build folder
- run cmake then make (make sure you have cmake installed)

Here's the basic script of commands to run:

```
# this is kind of assuming that we're running on ubuntu 16.04 or 18.04
sudo apt-get install cmake libzmq3-dev
git clone https://github.com/aarif4/protobuf_zmq_test.git
cd protobuf_zmq_test
mkdir build 
cd build 
cmake ..
make 
```

## Run protobuf_zmq_test

To run the program **run_pubsub**, you need two arguments: 
1. IP:Port -> Need to provide an IP address and port number to use to pass data around (e.g. 127.0.0.1:50001)
2. Filter  -> Need to provide a filter that the PUB and SUB sockets will use to understand that they have a message to receive (e.g. test)

So an example script you should run to get this thing running is:

```
./run_pubsub 127.0.0.1:50001 test
```