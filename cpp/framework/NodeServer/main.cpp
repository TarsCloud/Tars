/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "NodeServer.h"
#include "servant/Communicator.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include <iostream>

using namespace std;
using namespace tars;

NodeServer g_app;
TC_Config* g_pconf;

static string outfill(const string& s, char c = ' ', int n = 29)
{
    return (s + string(abs(n - (int)s.length()), c));
}

bool getConfig(const string &sLocator,const string &sRegistryObj,string &sNodeId,string &sLocalIp, string &sConfigFile, bool bCloseCout)
{
    try
    {
        //string          sLocalIp;
        string          sResult;
        string          sTemplate;
        TC_Config       tConf;
        CommunicatorFactory::getInstance()->getCommunicator()->setProperty("locator",sLocator);
        RegistryPrx pRegistryPrx = CommunicatorFactory::getInstance()->getCommunicator()->stringToProxy<RegistryPrx>(sRegistryObj);
        if( sLocalIp.empty() && pRegistryPrx->getClientIp(sLocalIp) != 0)
        {
            cerr<<"cannot get localip: " <<sLocalIp << endl;
            return false;
        }

        if(sNodeId == "" )
        {
            sNodeId = sLocalIp;
        }

        pRegistryPrx->getNodeTemplate(sNodeId,sTemplate);
        if(TC_Common::trim(sTemplate) == "" )
        {
            cerr << "cannot get node Template nodeid:"<<sNodeId <<",Template:"<<sTemplate<< endl;
            return false;
        }

        sTemplate = TC_Common::replace(sTemplate, "${locator}",sLocator);
        sTemplate = TC_Common::replace(sTemplate, "${registryObj}",sRegistryObj);
        sTemplate = TC_Common::replace(sTemplate, "${localip}",sLocalIp);
        //sTemplate = TC_Common::replace(sTemplate, "${nodeid}",sNodeId);
        tConf.parseString(sTemplate);
        if (!bCloseCout)
        {
            string sCloseCoutValue = tConf.get("/tars/application/server<closecout>");
            if (sCloseCoutValue == "1")
            {
                sTemplate = TC_Common::replace(sTemplate, "closecout=1", "closecout=0");
                tConf.parseString(sTemplate);
            }
            else if (sCloseCoutValue.empty())
            {
                map<string, string> m;
                m["closecout"] = "0";
                tConf.insertDomainParam("/tars/application/server", m,true);
            }
            else
            {
                cerr << "failed to set closeout value" << endl;
            }
        }

        string sConfigPath    = TC_File::extractFilePath(sConfigFile);
        if(!TC_File::makeDirRecursive( sConfigPath ))
        {
            cerr<<"cannot create dir: " <<sConfigPath << endl;
            exit( 0 );
        }
        string sFileTemp    = sConfigPath + "/config.conf.tem";
        ofstream configfile(sFileTemp.c_str());
        if(!configfile.good())
        {
            cerr << "cannot create configuration file: "<<sConfigFile<< endl;
            return false;
        }
        configfile <<TC_Common::replace(tConf.tostr(),"\\s"," ") << endl;
        configfile.close();

        string sFileBak     = sConfigFile+"."+TC_Common::now2str() + ".bak";
        if(TC_File::isFileExist(sConfigFile))
        {
            TC_File::copyFile(sConfigFile,sFileBak,true);
        }

        TC_File::copyFile(sFileTemp,sConfigFile,true);
        TC_File::removeFile(sFileTemp,false);

        return true;
    }
    catch(exception &e)
    {
       cerr<< "load config  erro:" <<e.what()<< endl;
    }
    return false;
}

void monitorNode(const string &configFile)
{
    TC_Config conf;
    conf.parseFile(configFile);

    string nodeObj = "AdminObj@"+conf["/tars/application/server<local>"];

    ServantPrx prx = CommunicatorFactory::getInstance()->getCommunicator()->stringToProxy<ServantPrx>(nodeObj);
    prx->tars_ping();
}

void parseConfig(int argc, char *argv[])
{

    TC_Option tOp;//consider
    tOp.decode(argc, argv);

     if (tOp.hasParam("nodeversion"))
    {
        cout << "Node:" << TARS_VERSION << "_" << NODE_VERSION << endl;
        exit(0);
    }

    if (tOp.hasParam("monitor"))
    {
        try
        {
            string configFile = tOp.getValue("config");

            monitorNode(configFile);
        }
        catch (exception &ex)
        {
            cout << "failed:" << ex.what() << endl;
            exit(-1);
        }
        exit(0);
        return;
    }

    string sLocator         = tOp.getValue("locator");
    string sNodeId          = tOp.getValue("nodeid");
    string sConfigFile      = tOp.getValue("config");
    string sRegistryObj     = tOp.getValue("registryObj");
    string sLocalIp         = tOp.getValue("localip");

    if(sConfigFile == "")
    {
        cerr << endl;
        cerr <<"start server with locator config, for example: "<<endl;
        cerr << argv[0] << " --locator=tars.tarsregistry.QueryObj@tcp -h 172.25.38.67 -p 17890"  " --config=config.conf [--nodeid = 172.25.38.67 --registryObj=tars.tarsregistry.RegistryObj]" << endl;
        cerr << argv[0] << " --config=config.conf" << endl;
        exit(0);
    }

    if(!TC_File::isAbsolute(sConfigFile))
    {
        char sCwd[PATH_MAX];
        if ( getcwd( sCwd, PATH_MAX ) == NULL )
        {
            TLOGERROR("cannot get the current directory:\n" << endl);
            exit( 0 );
        }
        sConfigFile = string(sCwd) +"/"+ sConfigFile;
    }
    sConfigFile = TC_File::simplifyDirectory(sConfigFile);
    if(sLocator != "")
    {
        if(sRegistryObj == "")
        {
            sRegistryObj = "tars.tarsregistry.RegistryObj";
        }
        bool bCloseOut=true;
        if(tOp.hasParam("closecout"))
        {
            if(TC_Common::lower(tOp.getValue("closecout"))=="n")
            {
                bCloseOut = false;
            }
        }
        bool bRet = getConfig(sLocator,sRegistryObj,sNodeId, sLocalIp, sConfigFile, bCloseOut);
        if(bRet == false)
        {
            cerr<<"reload config erro. use old config";
        }
    }
    cout <<endl;
    map<string,string> mOp = tOp.getMulti();
    for(map<string,string>::const_iterator it = mOp.begin(); it != mOp.end(); ++it)
    {
        cout << outfill( it->first)<< it->second << endl;
    }

}

int main( int argc, char* argv[] )
{
    try
    {
        bool bNoDaemon = false;
        for (int i = 1; i < argc; ++i)
        {
            if (::strcmp(argv[i], "--monitor") == 0)
            {
                bNoDaemon = true;
                break;
            }
        }

        if (!bNoDaemon)
        {
            TC_Common::daemon();
        }

        parseConfig(argc,argv);

        g_pconf = &g_app.getConfig();

        g_app.main( argc, argv );
        g_app.waitForShutdown();
    }
    catch ( exception& ex )
    {
        cout<< ex.what() << endl;
    }
    catch ( ... )
    {
        cout<< "main unknow exception cathed" << endl;
    }

    return 0;
}
