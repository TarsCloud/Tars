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

#ifndef __ADDFILE_COMMAND_H_
#define __ADDFILE_COMMAND_H_

#include "ServerCommand.h"

/**
 *自config服务下载文件 
 *
 */
class CommandAddFile : public ServerCommand
{
    
public:
    CommandAddFile(const ServerObjectPtr &pServerObjectPtr,const string &sFile,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);
private:
    int getScriptFile(string &sResult);
    bool                _byNode;
    string              _file; 
    string              _serverId;               
    ServerDescriptor    _desc;
    StatExChangePtr     _statExChange;
    ServerObjectPtr     _serverObjectPtr;
};

//////////////////////////////////////////////////////////////
//
inline CommandAddFile::CommandAddFile(const ServerObjectPtr &pServerObjectPtr,const string &sFile,bool bByNode)
:_byNode(bByNode)
,_file(sFile)
,_serverObjectPtr(pServerObjectPtr)
{ 
    _desc      = _serverObjectPtr->getServerDescriptor();
    _serverId  = _serverObjectPtr->getServerId();
}

//////////////////////////////////////////////////////////////////////////////
// 
inline ServerCommand::ExeStatus CommandAddFile::canExecute(string &sResult)
{
  
    TC_ThreadRecLock::Lock lock( *_serverObjectPtr );
    
    TLOGDEBUG("CommandAddFile::canExecute "<<_desc.application<< "." << _desc.serverName << " beging addFilesing------"<< endl);
    ServerObject::InternalServerState eState = _serverObjectPtr->getInternalState();
    
    if(_desc.application == "" || _desc.serverName == "" )
    {
        TLOGERROR("CommandAddFile::canExecute app or server name is empty"<<endl);
        return DIS_EXECUTABLE;
    }
    if(_file.empty())
    {
        TLOGERROR("CommandAddFile::canExecute file is empty. file:"<<_file<<endl);
        return DIS_EXECUTABLE;
    }
    //设当前状态为正在AddFilesing
    _statExChange = new StatExChange(_serverObjectPtr,ServerObject::AddFilesing, eState);
    
    return EXECUTABLE;      
}
 
//////////////////////////////////////////////////////////////
//
inline int CommandAddFile::execute(string &sResult)
{
  try
  {       
        //若File不合法采用默认路径
        if( TC_File::isAbsolute(_file ) ==  false )
        {
            _file = _serverObjectPtr->getExePath()+"/"+_file;
        }
        _file               = TC_File::simplifyDirectory(TC_Common::trim(_file));

        string sFilePath    = TC_File::extractFilePath(_file); 
        
        string sFileName    = TC_File::extractFileName(_file);      
        //创建目录
        if(!TC_File::makeDirRecursive(sFilePath ) )
        {

            TLOGERROR("CommandAddFile::execute cannot create dir:"<<sFilePath<<" erro:"<<strerror(errno)<<endl);
            return -1;
        }
        bool bRet = false;
        if(_serverObjectPtr->isScriptFile(sFileName) == true)
        {
            bRet = getScriptFile(sResult);
        }  
        else
        {
            TarsRemoteConfig tTarsRemoteConfig;
            tTarsRemoteConfig.setConfigInfo(Application::getCommunicator(), ServerConfig::Config,_desc.application, _desc.serverName, sFilePath,_desc.setId);
//            tTarsRemoteConfig.setConfigInfo(ServerConfig::Config,_desc.application, _desc.serverName, sFilePath,_desc.setId);
            bRet = tTarsRemoteConfig.addConfig(sFileName,sResult);
            g_app.reportServer(_serverObjectPtr->getServerId(),sResult); 
        }
        return bRet;               
  } 
  catch(exception &e)
  {
      TLOGERROR("CommandAddFile::execute get file: "<<_file<<" from configServer fail."<<e.what());
  }
  return -1;
}

inline int CommandAddFile::getScriptFile(string &sResult)
{
    try
    {     
        string sFileName    = TC_File::extractFileName(_file); 
                  
        ConfigPrx pPtr = Application::getCommunicator()->stringToProxy<ConfigPrx>(ServerConfig::Config);
        string sStream;
        int ret;
        if(_desc.setId.empty())
        {
            ret = pPtr->loadConfig(_desc.application, _desc.serverName, sFileName, sStream);
        }
        else
        {
            struct ConfigInfo confInfo;
            confInfo.appname     = _desc.application;
            confInfo.servername  = _desc.serverName;
            confInfo.filename    = sFileName;
            confInfo.setdivision = _desc.setId;
            ret = pPtr->loadConfigByInfo(confInfo,sStream);
        }

        if (ret != 0)
        {
            sResult = FILE_FUN_STR+"[fail] get remote file :" + sFileName;
            TLOGERROR(sResult<<endl);
            g_app.reportServer(_serverId,sResult); 
            return -1;
        }
        sStream = _serverObjectPtr->decodeMacro(sStream);
        string sTem;
        string::size_type pos = sStream.find('\n');
        while(pos != string::npos)
        {            
            sTem        = sTem + TC_Common::trim(sStream.substr(0,pos)," \r\t") +"\n";
            sStream     = sStream.substr(pos+1); 
            pos         = sStream.find('\n');     
        }               
        sStream = sTem + sStream;
        string sEndMark = "echo 'end-"+sFileName+"'";
        if(sStream.find(sEndMark) == string::npos)
        {
            sStream = sStream + "\n"+sEndMark ;   //tarsnode 生成的脚本都以 'echo 'end filename'' 结束 
        }                   
        if(TC_File::isFileExist(_file) && TC_File::load2str(_file) != sStream)
        {
            TC_File::copyFile(_file,_file+".bak");
        }
        ofstream out(_file.c_str());
        if(!out.good())
        {
            TLOGERROR("CommandAddFile::getScriptFile cannot create file: "<<(_file + " erro:"+ strerror(errno))<<endl);
            return -1;
        }
        out<<sStream;
        out.close();  
        sResult = FILE_FUN_STR+"[succ] get remote file :"+sFileName;
        g_app.reportServer(_serverObjectPtr->getServerId(),sResult); 
        TLOGDEBUG(sResult<<endl);
        return 0;          
  } 
  catch(exception &e)
  {

      TLOGERROR("CommandAddFile::getScriptFile get file"<<(_file+" from configServer fail."+ e.what())<<endl);
  }  
  return -1; 
}
#endif 
