#include <string>
#include <thread>
#include <stdio.h>
#include <zmq.hpp>
#include <istream>
#include <iostream>
#include <unistd.h>
#include "num_msg.pb.h"

#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))




using namespace std;
// Iterates though all people in the AddressBook and prints info about them.
void ListPeople(const tutorial::person person) 
{
	std::cout << "  Name:     " << person.name() << std::endl;
	std::cout << "Person ID:  " << person.id() << std::endl;
	std::cout << "Person ID2: " << person.id2() << std::endl;
	std::cout << "Person ID3: " << person.id3() << std::endl;
}


uint64_t pack754(long double f, unsigned bits, unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Function that forever sends random weather parameters via
//   a socket in a string delimited with a space
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void runPub(std::string ipAddr, std::string filter)
{
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Prepare our context and publisher
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    zmq::context_t context(1);
    zmq::socket_t  publisher(context, ZMQ_PUB);
    publisher.bind(ipAddr.c_str());
    printf("runPub(): ZMQ PUB connected to \"%s\" with filter \"%s\"\n", 
                                                ipAddr.c_str(), filter.c_str());
    printf("runPub(): Going to send 1 message every 2 seconds\n");
    printf("runPub(): Waiting for 5 seconds before starting sending msgs...\n");
    usleep(5000000);
//}


  
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Generate and publish AddressBook Protobuf messages
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    // Initialize random number generator
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	tutorial::person new_person;
	
	new_person.set_id(0);
	*new_person.mutable_name() = "Doe";
        std::string name = "Doe";
        double ID = 0;
        double ID2 = 0.5;
        double ID3 = 11;

	int cnt = 0;
    while (++cnt > 0) // loop indefinitely
    { /* Forever increment ID number and send it */
		
///*
	// Periodically send the message out via ZMQ PUB
        int field_number;
        int wire_type;
        uint8_t hdr_dbl = 0;
        uint16_t hdr_str = 0;
        uint64_t dbl_serial;
        zmq::message_t message( (2+name.size()) + (1+8)*3);
        // make the string's header first
        field_number = 1;
        wire_type = 2;
        hdr_str |= (((field_number << 3) | wire_type) << 8) | name.size(); 
        // make the double's header next
        field_number = 2;
        wire_type = 1;
        hdr_dbl = (field_number << 3) | wire_type;
        // so the way we'll pack stuff is: [hdr_str] [name] [hdr_dbl] [ID (in binary)]
        //memcpy(((char*)message.data())+0,               &hdr_str,     2);
        memcpy(((char*)message.data())+0,               (char*)&hdr_str+1,     1);
        memcpy(((char*)message.data())+1,               (char*)&hdr_str+0,     1);
        memcpy(((char*)message.data())+2,               name.c_str(), name.size());
        memcpy(((char*)message.data())+2+name.size(),   &hdr_dbl,     1);
        ID+=1.234;
        ID2 += 5.12;
        ID3 -= 1.5;

        field_number = 2;
        wire_type = 1;
        hdr_dbl = (field_number << 3) | wire_type;
        dbl_serial = pack754_64(ID);
        memcpy(((char*)message.data())+2+name.size()+1, &dbl_serial, sizeof(uint64_t));
        field_number = 3;
        wire_type = 1;
        hdr_dbl = (field_number << 3) | wire_type;
        dbl_serial = pack754_64(ID2);
        memcpy(((char*)message.data())+2+name.size()+1+8,   &hdr_dbl,     1);
        memcpy(((char*)message.data())+2+name.size()+1+8+1, &dbl_serial, sizeof(uint64_t));
        field_number = 4;
        wire_type = 1;
        hdr_dbl = (field_number << 3) | wire_type;
        dbl_serial = pack754_64(ID3);
        memcpy(((char*)message.data())+2+name.size()+1+8+1+8,   &hdr_dbl,     1);
        memcpy(((char*)message.data())+2+name.size()+1+8+1+8+1, &dbl_serial, sizeof(uint64_t));

        FILE * fp = fopen("./myfile.bin","wb");
        fwrite(message.data(), sizeof(char), message.size(), fp);
        fclose(fp);
//*/
/*
	zmq::message_t message(new_person.ByteSizeLong());
	new_person.SerializeToArray(message.data(), new_person.ByteSizeLong()); 
        // save to file
        FILE * fp = fopen("./file.bin","wb");
        fwrite(message.data(), sizeof(char), message.size(), fp);
        fclose(fp);
*/	
        // 3. Send the message via PUB socket
        if (filter.size() != 0)
        { /* If we have a filter, send that first before sending the message */
            size_t filt_len = filter.size()+1; // get filter message's size
            zmq::message_t filt_msg(filt_len); // create zmq msg with proper sz
            memset((void *) filt_msg.data(),   // Preallocate msg with '/0'
                   '\0', 
                   filt_len);
            snprintf((char *) filt_msg.data(), // Copy filter string to msg
                     filt_len, 
                     "%s", 
                     filter.c_str());

            // Send the filter via socket
            publisher.send(filt_msg, ZMQ_SNDMORE);
        }
		
        // Send the message via socket
        publisher.send(message);
        
		// 4. Sleep for 2 seconds before sending the next message
        usleep(2000000);
    }
//}

}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Function that forever subscribes to a particular ip+port 
//   and forever receives info
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void runSub(std::string ipAddr, std::string filter)
{
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Prepare our context and subscriber
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect(ipAddr.c_str());
	if (filter.size())
	{
		subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), sizeof(char));
	}
	else
	{
		subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
	}
	
    // Let the user know
    printf("runSub(): Conencting ZMQ SUB to \"%s\" with filter \"%s\"\n", 
                                                ipAddr.c_str(), filter.c_str());
    printf("runSub(): Waiting for PUB socket to send message...\n");
//}



    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Receive and translate serialized protobuf messages
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	tutorial::person new_person;
	
	int cnt = 0;
    while (++cnt > 0)
    {
        // 1. Block until you receive a message
        zmq::message_t message;
        subscriber.recv(&message);

		// 2. Check to see if you got the right message by verifying filter
		if (filter.size())
		{ /* If there is a filter, then see if it's the one that we want */
			std::string rcvd_filter((char *)message.data());
			if (strcmp(rcvd_filter.c_str(), filter.c_str()) == 0)
			{
				// 3. Block until you receive desired message
				subscriber.recv(&message);
			}
		}
		
        FILE * fp = fopen("./rvcd_file.bin","wb");
        fwrite(message.data(), sizeof(char), message.size(), fp);
        fclose(fp);
		// 4. Parse the message that was received 
		if (!new_person.ParseFromArray(message.data(), message.size())) 
		{
			std::cerr << "Failed to parse message. Skipping..." << std::endl;
			continue;
		}
		ListPeople(new_person);
		

        // 5. Sleep for 0.5 sec
        usleep(500000);
    }
//}

}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Maine function
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
int main(int argc, char *argv[])
{
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Validate that executable was called with the right number of args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
	// We're currently "assuming" that our user is giving us "correct" values
	std::string ipAddr("tcp://127.0.0.1:50011");
	std::string filter("");

	bool CAUGHT_ERROR = false;
	switch (argc)
	{
		case 1:
		{
			// Use default values
			break;
		}
		case 2:
		{
			ipAddr = "tcp://" + std::string(argv[1]);
			break;
		}
		case 3:
		{
			ipAddr = "tcp://" + std::string(argv[1]);
			filter = std::string(argv[2]);
			break;
		}
		default:
		{
			printf("ERROR: Wrong number of input arguments!\n");
			printf("./pbuff_zmq [ipAddr] [filter]\n");
			printf("\t[ipAddr] -> IP Address (DEFAULT 127.0.0.1:50011)\n");
			printf("\t[filter] -> ZMQ Filter (DEFAULT none)\n\n");
			printf("\t\tExample: ./pbuff_zmq 127.0.0.1:50011 filt_str\n\n\n");
			fflush(stdout); // Flush the contents of stdout buffer
			
			CAUGHT_ERROR = true;
			
			break;
		}
	}
	
	if (CAUGHT_ERROR)
		return 1;
//}



    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Let user know ZMQ version running on their system
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    printf("Current ZMQ version is %d.%d.%d\n\n", major, minor, patch); 
//}  


 
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Run the Publisher's Thread and Subscriber's Thread using inputted args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    std::thread pub_thread(runPub, ipAddr, filter);
    std::thread sub_thread(runSub, ipAddr, filter);
//}



    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Run the Publisher's Thread and Subscriber's Thread using inputted args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    pub_thread.join();
    sub_thread.join();
//}


    return 0;

}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Some notes to do
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// TODO: Validate input arguments
// TODO: Handle keyboard interrupt in runPub() and runSub()
// TODO: Why is it that zmq::message_t must be give a size of .size()+1?
//       Without that, the string/filter will be missing its last character
