#include "TestPushServer.h"
#include "TestPushServantImp.h"

using namespace std;

TestPushServer g_app;

/////////////////////////////////////////////////////////////////

static int parse(string &in, string &out)
{
    if(in.length() < sizeof(unsigned int))
    {
        return TC_EpollServer::PACKET_LESS;
    }

    unsigned int iHeaderLen;

    memcpy(&iHeaderLen, in.c_str(), sizeof(unsigned int));

    iHeaderLen = ntohl(iHeaderLen);

    if(iHeaderLen < (unsigned int)(sizeof(unsigned int))|| iHeaderLen > 1000000)
    {
        return TC_EpollServer::PACKET_ERR;
    }

    if((unsigned int)in.length() < iHeaderLen)
    {
        return TC_EpollServer::PACKET_LESS;
    }

    out = in.substr(0, iHeaderLen);

    in  = in.substr(iHeaderLen);

    return TC_EpollServer::PACKET_FULL;
}


void
TestPushServer::initialize()
{
    //initialize application here:
    //...

    addServant<TestPushServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".TestPushServantObj");

    addServantProtocol("Test.TestPushServer.TestPushServantObj", parse);

    pushThread.start();

}
/////////////////////////////////////////////////////////////////
void
TestPushServer::destroyApp()
{
    //destroy application here:
    //...
    pushThread.terminate();
    pushThread.getThreadControl().join();

}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    try
    {
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}
/////////////////////////////////////////////////////////////////
