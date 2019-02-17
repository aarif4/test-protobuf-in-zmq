#include <string>
#include <thread>
#include <stdio.h>
#include <zmq.hpp>
#include <istream>
#include <iostream>
#include <unistd.h>
#include "addressbook.pb.h"





using namespace std;
// Iterates though all people in the AddressBook and prints info about them.
void ListPeople(const tutorial::AddressBook& address_book) 
{
	for (int i = 0; i < address_book.people_size(); i++) 
	{
		const tutorial::Person& person = address_book.people(i);

		std::cout << "Person ID: " << person.id() << std::endl;
		std::cout << "  Name: " << person.name() << std::endl;
		if (person.has_email()) 
		{
			std::cout << "  E-mail address: " << person.email() << std::endl;
		}

		for (int j = 0; j < person.phones_size(); j++) 
		{
			const tutorial::Person::PhoneNumber& phone_number = person.phones(j);

			switch (phone_number.type()) 
			{
				case tutorial::Person::MOBILE:
					std::cout << "  Mobile phone #: ";
					break;
				case tutorial::Person::HOME:
					std::cout << "  Home phone #: ";
					break;
				case tutorial::Person::WORK:
					std::cout << "  Work phone #: ";
					break;
			}
			std::cout << phone_number.number() << std::endl << std::endl;
		}
	}
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
	tutorial::AddressBook address_book;
	tutorial::Person* new_person = address_book.add_people();
	
	*new_person->mutable_name() = "Jane Doe";
	new_person->set_email(std::string("jane.doe@email.com"));
	tutorial::Person::PhoneNumber* phone_number = new_person->add_phones();
	phone_number->set_type(tutorial::Person::MOBILE);
	phone_number->set_number(std::string("(123) 456-7889"));

	int cnt = 0;
    while (++cnt > 0) // loop indefinitely
    { /* Forever increment ID number and send it */
		new_person->set_id(cnt);
		
		// Periodically send the message out via ZMQ PUB
		zmq::message_t message(address_book.ByteSizeLong());
		address_book.SerializeToArray(message.data(), address_book.ByteSizeLong()); 
		
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
	tutorial::AddressBook address_book;
	
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
		
		// 4. Parse the message that was received 
		if (!address_book.ParseFromArray(message.data(), message.size())) 
		{
			std::cerr << "Failed to parse message. Skipping..." << std::endl;
			continue;
		}
		ListPeople(address_book);
		

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