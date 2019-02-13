#include <thread>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <zmq.hpp>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include "addressbook.pb.h"

#include <unistd.h>
#include <zmq.hpp>
#include <iostream>
#include <streambuf>
#include <string>
#include <istream>
#include <sstream>

//TODO: So this ZMQ socket isn't working...maybe try writing it to a file and reading it to make sure that it is readable? Then if that's OK, then it's most likely the conversion from message_t to istringstream that is the problem

using namespace std;
// Iterates though all people in the AddressBook and prints info about them.
void ListPeople(const tutorial::AddressBook& address_book) {
  for (int i = 0; i < address_book.people_size(); i++) {
    const tutorial::Person& person = address_book.people(i);

    cout << "Person ID: " << person.id() << endl;
    cout << "  Name: " << person.name() << endl;
    if (person.has_email()) {
      cout << "  E-mail address: " << person.email() << endl;
    }

    for (int j = 0; j < person.phones_size(); j++) {
      const tutorial::Person::PhoneNumber& phone_number = person.phones(j);

      switch (phone_number.type()) {
        case tutorial::Person::MOBILE:
          cout << "  Mobile phone #: ";
          break;
        case tutorial::Person::HOME:
          cout << "  Home phone #: ";
          break;
        case tutorial::Person::WORK:
          cout << "  Work phone #: ";
          break;
      }
      cout << phone_number.number() << endl;
    }
  }
}

// Make function definition to get random value that's b/w [0,num)
#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))






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
  // Generate and publish random weather messages
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    // Initialize random number generator
    srandom((unsigned) time (NULL));
    int update_nbr = 0;
    long total_temp = 0; 
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	tutorial::AddressBook address_book;
	tutorial::Person* new_person = address_book.add_people();
	   
	*new_person->mutable_name() = "Matt";
	new_person->set_email(std::string("matt@email.com"));
	tutorial::Person::PhoneNumber* phone_number = new_person->add_phones();
	
	phone_number->set_type(tutorial::Person::MOBILE);
    while (++update_nbr > 0) // loop indefinitely
    { /* Forever generate random weather stuff and send it */
		//zmq::message_t message;
		//std::string str("this is a test message\0");
		//message.rebuild(str.size());
		//memcpy((void *) message.data(), (void *) str.c_str(), str.size());
		new_person->set_id(update_nbr);
		phone_number->set_number(std::string("1234567889") + std::to_string(update_nbr));
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
		//printf("\tSending %d/%d bytes of data\n", (int) message.size(), (int) address_book.ByteSizeLong());
        publisher.send(message);
		//printf("\tSent %d bytes of data\n", (int) message.size());
        // 4. Inform the user message's contents and sleep for 2 seconds
        //printf("runPub(): Sent message #%d: \"%s\"\n", update_nbr, 
        //                                                (char *)message.data());
		
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
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), sizeof(char));

    // Let the user know
    printf("runSub(): Conencting ZMQ SUB to \"%s\" with filter \"%s\"\n", 
                                                ipAddr.c_str(), filter.c_str());
    printf("runSub(): Waiting for PUB socket to send message...\n");
//}


	zmq::message_t simple_msg; int sz = 0;
	{
		static tutorial::AddressBook address_book1;
		static tutorial::Person* new_person = address_book1.add_people();
		new_person->set_id(0);   
		*new_person->mutable_name() = "Matt";
		new_person->set_email(std::string("matt@email.com"));

		tutorial::Person::PhoneNumber* phone_number = new_person->add_phones();
		phone_number->set_number(std::string("1234567889") + std::to_string(0));
		phone_number->set_type(tutorial::Person::MOBILE);
		
		simple_msg.rebuild(address_book1.ByteSizeLong());
		sz = address_book1.ByteSizeLong();
		address_book1.SerializeToArray(simple_msg.data(), address_book1.ByteSizeLong());
		FILE* fp = fopen("./a.out","wb");
		fwrite(simple_msg.data(), sizeof(char), address_book1.ByteSizeLong(), fp);
		fclose(fp);
		
		// Write the new address book back to disk.
		fstream output("./b.out", ios::out | ios::trunc | ios::binary);
		if (!address_book1.SerializeToOstream(&output)) {
		  cerr << "Failed to write address book." << endl;
		  return;
		}
	}
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Receive and translate messages
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	int update_nbr = 0;
	tutorial::AddressBook address_book;
    while (++update_nbr > 0)
    {
        // 1. Block until you receive a message
        zmq::message_t update;
        subscriber.recv(&update);

        // 2. Check to see if you got the right message by verifying filter
        std::string rcvd_filter((char *)update.data());
        if (strcmp(rcvd_filter.c_str(), filter.c_str()) == 0)
        {
            // 3. Block until you receive desired message
            subscriber.recv(&update);

            // 4. Parse the message that was received 
			printf("\tRec'd %d bytes of data\n", (int) update.size());
			//printf("runSub(): Got message  #%d: \"%s\"\n", update_nbr, 
			//									(char *) update.data()); 
													
			//std::istringstream iss(static_cast<char *> (update.data()));
			//std::istringstream iss(static_cast<char *> (simple_msg.data()));
			fstream iss("./a.out", ios::in | ios::binary);
			if (!address_book.ParseFromArray(update.data(), update.size())) 
			{
			  std::cerr << "Failed to parse address book. Retrying" << std::endl;
			  continue;
			}
			ListPeople(address_book);
		}

        // 6. Sleep for 0.5 sec
        usleep(500000);
    }
}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Maine function
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
int main(int argc, char *argv[])
{
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Validate that executable was called with the right number of args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    if (argc != 3)
    { /* If the user hasn't provided the right no. of args, let the user know */
        printf("ERROR: Not the right number of arguments!\n");
        printf("pubSubTest [ipAddr] [filter]\n");
        printf("\t[ipAddr] -> IP+Port to be used at publisher & subscriber\n");
        printf("\t[filter] -> ZMQ Filter to be used during communication\n\n");
        printf("\t\texample: pubSubTest 127.0.0.1:50000 abcdef\n\n\n");
        fflush(stdout); // Flush the contents of stdout buffer

        return 1;
    }


    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Let user know ZMQ version running on their system
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    printf("Current ZMQ version is %d.%d.%d\n\n", major, minor, patch); 
  
    
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Run the Publisher's Thread and Subscriber's Thread using inputted args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // We're currently "assuming" that our user is giving us "correct" values
    std::string filter (argv[2]);
    std::string ipAddr (argv[1]); ipAddr = "tcp://" + ipAddr;
    std::thread pub_thread(runPub, ipAddr, filter);
    std::thread sub_thread(runSub, ipAddr, filter);


    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Run the Publisher's Thread and Subscriber's Thread using inputted args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    pub_thread.join();
    sub_thread.join();


    return 0;


}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Some notes to do
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// TODO: Validate input arguments
// TODO: Handle keyboard interrupt in runPub() and runSub()