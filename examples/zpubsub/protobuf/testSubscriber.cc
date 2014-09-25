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

static void testCb(const Topic &sample)
{
	cout << "Received message:" << endl;
    PrintMsg((TestMessage *) sample.GetMessage ());
}

int main(int argc, char* argv[])
{
	Participant* part = new Participant(25);

	while (true)
	{
		PrintMenu();

		string s;
		cin >> s;

		if (s == "1")
		{
            Topic t (new TestMessage);
			part->Subscribe(t, testCb);
		}
		else if (s == "2")
		{
            Topic t (new TestMessage);
            part->Unsubscribe(t);
		}
		else if (s == "q" || s == "Q")
		{
			break;
		}
	}

	delete part;
	return 0;
}
