#include <iostream>
#include "participant.h"
#include "testMessage.pb.h"

using namespace std;
using namespace zpubsub;

static void PrintMsg(TestMessage* msg)
{
	cout << "    Id     : " << msg->id() << endl;
	cout << "    Message: " << msg->message() << endl;
	cout << "    Testint: " << msg->testint() << endl << endl;
}

static void PrintMenu()
{
	cout << endl;
	cout << "====================================" << endl;
	cout << " 1. Subscribe to TestMessage" << endl;
	cout << " 2. Unsubscribe from TestMessage" << endl;
	cout << " q  quit" << endl;
	cout << "====================================" << endl;
	cout << " Enter choice: ";
}

static void testCb(const char *topic, const char *partition, void *sample)
{
	cout << "Received message:" << endl;
	TestMessage *msg = (TestMessage*) sample;
	PrintMsg(msg);
	delete msg;
}

int main(int argc, char* argv[])
{
	Participant* part = new Participant(25);
	
	int subscriptions = 0;
	
	while (true)
	{
		PrintMenu();
		
		string s;
		cin >> s;
				
		if (s == "1")
		{
			part->Subscribe(new Topic(new TestMessage), testCb);
			++subscriptions;
		}
		else if (s == "2")
		{
			if (subscriptions > 0)
			{
				Topic* topic = new Topic(new TestMessage);
				part->Unsubscribe(topic);
				delete topic;
				
				--subscriptions;
			}
		}
		else if (s == "q" || s == "Q")
		{
			break;
		}
		
		cout << "Now have " << subscriptions << " subscriptions" << endl;
	}
	
	while (subscriptions--)
	{
		Topic* topic = new Topic(new TestMessage);
		part->Unsubscribe(topic);
		delete topic;
	}

	delete part;
	
	return 0;
}
