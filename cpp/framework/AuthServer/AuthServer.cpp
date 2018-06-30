#include "AuthServer.h"
#include "AuthImp.h"

void AuthServer::initialize()
{
    //增加对象
    addServant<AuthImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".AuthObj");
}

void AuthServer::destroyApp()
{
    cout << "AuthServer::destroyApp ok" << endl;
}

