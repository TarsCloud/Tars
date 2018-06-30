#include <iostream>
#include "AuthServer.h"

using namespace tars;

TC_Config * g_pconf;

int main(int argc, char *argv[])
{
    try
    {
        AuthServer app;
        g_pconf =  & app.getConfig();
        app.main(argc, argv);

        app.waitForShutdown();
    }
    catch(exception &ex)
    {
        cerr<< ex.what() << endl;
    }

    return 0;
}


