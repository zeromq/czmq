#include <iostream>
#include <czmq.h>
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
    cout << " 1. Publish TestMessage" << endl;
    cout << " q  quit" << endl;
    cout << "====================================" << endl;
    cout << " Enter choice: ";
}

int main(int argc, char* argv[])
{
    Participant* part = new Participant(25);

    unsigned int id = 1;
    char szMsg[256];

    while (1)
    {
        PrintMenu();

        string s;
        cin >> s;

        if (s == "1")
        {
            sprintf(szMsg, "This is message number %d", id);

            TestMessage* testMessage = new TestMessage;
            testMessage->set_id(id++);
            testMessage->set_message(szMsg);
            testMessage->set_testint((int) (zclock_time() & 0xffffffff));

            cout << endl << "Publishing message:" << endl;
            PrintMsg(testMessage);

            Topic* sample = new Topic(testMessage);
            part->Publish(*sample);
            delete sample;
        }
        else if (s == "q" || s == "Q")
        {
            break;
        }
    }

    delete part;
}
