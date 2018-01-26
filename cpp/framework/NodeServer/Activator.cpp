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

#include <sys/wait.h>
#include "Activator.h"
#include "NodeServer.h"

static pid_t *childpid = NULL; /* ptr to array allocated at run-time */
#define SHELL   "/bin/sh"

pid_t Activator::activate(const string& strExePath, const string& strPwdPath, const string& strRollLogPath, const vector<string>& vOptions, vector<string>& vEnvs)
{
    TC_ThreadLock::Lock lock(*this);
    addActivatingRecord();
    if (isActivatingLimited() == true)
    {
        TLOGERROR("Activator::activate The server " << strExePath << " activating is limited! it will not auto start until after " + TC_Common::tostr(_punishInterval) + " seconds" << endl);
    }

    if (strExePath.empty())
    {
        throw runtime_error("The server executable path is empty.");
    }

    if (TC_File::isFileExistEx(strExePath) && !TC_File::canExecutable(strExePath))
    {
        TC_File::setExecutable(strExePath, true);
    }
    vector<string> vArgs;
    vArgs.push_back(strExePath);
    vArgs.insert(vArgs.end(), vOptions.begin(), vOptions.end());

    TLOGDEBUG("Activator::activate activating server [exepath: " << strExePath << ", args: " << TC_Common::tostr(vArgs) << "]" << endl);

    int argc = static_cast<int>(vArgs.size());
    char **argv = static_cast<char **>(malloc((argc + 1) * sizeof(char *)));
    int i = 0;
    for (vector<string>::const_iterator p = vArgs.begin(); p != vArgs.end(); ++p, ++i)
    {
        assert(i < argc);
        argv[i] = strdup(p->c_str());
    }
    assert(i == argc);
    argv[argc] = 0;

    const char *pwdCStr = strPwdPath.c_str();

    pid_t pid = fork();
    if (pid == -1)
    {
        TLOGDEBUG("Activator::activate "<< strPwdPath << "|fork child process  catch exception|errno=" << errno << endl);
        throw runtime_error("fork child process  catch exception");
    }

    if (pid == 0)
    {
        int maxFd = static_cast<int>(sysconf(_SC_OPEN_MAX));
        for (int fd = 3; fd < maxFd; ++fd)
        {
            close(fd);
        }

        //server stdcout 日志在滚动日志显示
        if (_redirectPath != "")
        {
            TC_File::makeDirRecursive(TC_File::extractFilePath(_redirectPath));
            if ((freopen64(_redirectPath.c_str(), "ab", stdout)) != NULL && (freopen64(_redirectPath.c_str(), "ab", stderr)) != NULL)
            {
                cout << argv[0] << " redirect stdout and stderr  to " << _redirectPath << endl;
            }
            else
            {
                //重定向失败 直接退出
                exit(0);
                //cout << argv[0]<<" cannot redirect stdout and stderr  to log file" << _redirectPath << "|errno=" <<strerror(errno) << endl;
            }

        }
        else if (!strRollLogPath.empty())
        {
            TC_File::makeDirRecursive(TC_File::extractFilePath(strRollLogPath));
            if ((freopen64(strRollLogPath.c_str(), "ab", stdout)) != NULL && (freopen64(strRollLogPath.c_str(), "ab", stderr)) != NULL)
            {
                cout << argv[0] << " redirect stdout and stderr  to " << strRollLogPath << endl;
            }
            else
            {
                //重定向失败 直接退出
                exit(0);
                //cout << argv[0]<<" cannot redirect stdout and stderr  to log file" << strRollLogPath << "|errno=" <<strerror(errno) << endl;
            }
        }
        else
        {
            cout << argv[0] << " cannot redirect stdout and stderr  to log file strRollLogPath is empty" << endl;
        }

        for_each(vEnvs.begin(), vEnvs.end(), EnvVal());

        if (strlen(pwdCStr) != 0)
        {
            if (chdir(pwdCStr) == -1)
            {
                cerr << argv[0] << " cannot change working directory to " << pwdCStr << "|errno=" << errno << endl;
            }
        }

        if (execvp(argv[0], argv) == -1)
        {
            cerr << "cannot execute " << argv[0] << "|errno=" << strerror(errno) << endl;
        }
        exit(0);
    }
    else
    {
        for (i = 0; argv[i]; i++)
        {
            free(argv[i]);
        }
        free(argv);
    }
    return pid;
}


pid_t Activator::activate(const string& strServerId, const string& strStartScript, const string& strMonitorScript, string& strResult)
{
    addActivatingRecord();
    if (isActivatingLimited() == true)
    {
        TLOGERROR("Activator::activate The server " << strServerId << ":" << strStartScript  << " activating is limited! it will not auto start until after " + TC_Common::tostr(_punishInterval) + " seconds" << endl);
    }

    if (strStartScript.empty())
    {
        throw runtime_error("The script file " + strStartScript + " is empty.");
    }

    if (!TC_File::isFileExistEx(strStartScript))
    {
        throw runtime_error("The script file: " + strStartScript + " is not exist.");
    }

    map<string, string> mResult;
    if (doScript(strServerId, strStartScript, strResult, mResult) == false)
    {
        throw runtime_error("run script file " + strStartScript + " error :" + strResult);
    }

    pid_t pid = -1;
    if (!strMonitorScript.empty() && TC_File::isFileExistEx(strMonitorScript))
    {
        string s;
        mResult.clear();
        if (doScript(strServerId, strMonitorScript, s, mResult) == false)
        {
            throw runtime_error("run script " + strMonitorScript + " error :" + s);
        }
    }

    return pid;
}

int Activator::deactivate(int pid)
{
    if (pid != 0)
    {
        return sendSignal(pid, SIGKILL);
    }

    return -1;
}

int Activator::deactivateAndGenerateCore(int pid)
{
    if (pid != 0)
    {
        return sendSignal(pid, SIGABRT);
    }

    return -1;
}

int Activator::sendSignal(int pid, int signal) const
{
    assert(pid);
    int ret = ::kill(static_cast<pid_t>(pid), signal);
    if (ret != 0 && errno != ESRCH)
    {
        TLOGERROR("Activator::activate send signal " << signal << " to pid " << pid << " catch exception|" << errno << endl);
        return -1;
    }

    return ret;
}

bool Activator::isActivatingLimited()
{
    unsigned uLen = _activingRecord.size();

    //惩罚时间段内禁止启动
    if (_limited == true && uLen > 0 && TNOW - _activingRecord[uLen - 1] < _punishInterval)
    {
        return true;
    }

    return false;
}
void Activator::addActivatingRecord()
{
    time_t tNow     = TNOW;
    unsigned uLen   = _activingRecord.size();

    if (uLen > 0)
    {
        if (tNow - _activingRecord[uLen - 1] < _timeInterval)
        {
            _curCount++;
        }

        //完成惩罚时间,自动解除惩罚
        if (_limited == true && tNow - _activingRecord[uLen - 1] > _punishInterval * 1.5)
        {
            _curCount  = 0;
            _limited   = false;
            _activingRecord.clear();
        }

        if (_curCount > _maxCount)
        {
            _limited = true;
        }
    }
    _activingRecord.push_back(tNow);
}

bool Activator::doScript(const string& strServerId, const string& strScript, string& strResult, map<string, string>& mResult, const string& sEndMark)
{
    TLOGINFO("Activator::activate doScript " << strScript << " begin----" << endl);

    if (!TC_File::isFileExistEx(strScript))
    {
        strResult = "The script file: " + strScript + " is not exist";
        return false;
    }

    string sRealEndMark = sEndMark;
    if (sRealEndMark == "")
    {
        sRealEndMark = "end-" + TC_File::extractFileName(strScript);
    }

    TLOGINFO("Activator::activate doScript " << strScript << " endMark----" << sRealEndMark << endl);

    if (!TC_File::canExecutable(strScript))
    {
        TC_File::setExecutable(strScript, true);
    }

    string sRedirect = "";
    if (_redirectPath != "")
    {
        sRedirect =  " 2>&1 >>" + _redirectPath;
    }

    string sCmd = strScript + sRedirect + " " + strServerId + " &";
    FILE  *fp =  popen2(sCmd.c_str(), "r");
    if (fp == NULL)
    {
        strResult = "popen script file: " + strScript + " error ";
        return false;
    }

    //使文件变成非阻塞方式访问
    int flags;
    if ((flags = fcntl(fileno(fp), F_GETFL, 0)) < 0)
    {
        strResult = "fcntl get error.script file: " + strScript;
        fflush(fp);
        pclose2(fp);
        return false;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fileno(fp), F_SETFL, flags) < 0)
    {
        strResult = "fcntl set error.script file: " + strScript;
        fflush(fp);
        pclose2(fp);

        return false;
    }
    //strResult = "";

    int c;
    time_t tNow = TNOW;
    while (TNOW - 2 < tNow)
    {
        usleep(200000);
        while ((c = fgetc(fp)) != EOF)
        {
            strResult += (char)c;
        }
        if (sRealEndMark == "" || strResult.find(sRealEndMark) != string::npos)
        {
            TLOGINFO("Activator::doScript "<< sCmd << "|sEndMark " << sRealEndMark << " finded|" << strResult << endl);
            break;
        }
    }

    strResult = TC_Common::replace(strResult, "\n", "\r\n");
    LOG->info() << "Activator::doScript "<< sCmd << " result:" << strResult << endl;

    fflush(fp);
    pclose2(fp);

    mResult = parseScriptResult(strServerId, strResult);
    return true;
}

map<string, string> Activator::parseScriptResult(const string& strServerId, const string& strResult)
{
    map<string, string> mResult;
    vector<string> vResult = TC_Common::sepstr<string>(strResult, "\n");
    for (unsigned i = 0; i < vResult.size(); i++)
    {
        string::size_type pos = vResult[i].find('=');
        if (pos != string::npos)
        {
            string sName    = vResult[i].substr(0, pos);
            string sValue   = vResult[i].substr(pos + 1);
            sName           = TC_Common::lower(TC_Common::trim(sName));
            sValue          = TC_Common::trim(sValue);
            mResult[sName] = sValue;
            if (sName == "notify")
            {
                g_app.reportServer(strServerId, sValue);
            }
        }
    }

    return mResult;
}

FILE* Activator::popen2(const char *cmdstring, const char *type)
{
    int     i, pfd[2];
    pid_t   pid;
    FILE    *fp;
    /*only allow "r" or "w" */
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0)
    {
        errno = EINVAL;     /* required by POSIX.2 */
        return (NULL);
    }
    int maxfd = static_cast<int>(sysconf(_SC_OPEN_MAX));
    if (childpid == NULL)
    {  /* first time through */
        /* allocate zeroed out array for child pids */
        childpid = new pid_t[maxfd];
        memset(childpid, 0, sizeof(pid_t) * maxfd);
        //char** envArray = new char*[envCount];
        if (childpid  == NULL)
        {
            return (NULL);
        }
    }

    if (pipe(pfd) < 0)
    {
        return (NULL);   /* errno set by pipe() */
    }

    if ((pid = fork()) < 0)
    {
        return (NULL);   /* errno set by fork() */
    }
    else if (pid == 0)
    {                            /* child */
        if (*type == 'r')
        {
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            dup2(pfd[1], STDERR_FILENO);
            close(pfd[1]);
        }
        else
        {
            close(pfd[1]);
            dup2(pfd[0], STDIN_FILENO);
            close(pfd[0]);
        }
        /* close all descriptors opened in parent process*/
        for (i = 3; i < maxfd; i++)
        {
            if (i != pfd[0] && i !=  pfd[1])
            {
                close(i);
            }
        }
        execl(SHELL, "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    }
    /* parent */
    if (*type == 'r')
    {
        close(pfd[1]);
        if ((fp = fdopen(pfd[0], type)) == NULL)
        {
            return (NULL);
        }
    }
    else
    {
        close(pfd[0]);
        if ((fp = fdopen(pfd[1], type)) == NULL)
        {
            return (NULL);
        }
    }
    childpid[fileno(fp)] = pid; /* remember child pid for this fd */
    return (fp);
}

int Activator::pclose2(FILE *fp)
{
    int     fd, stat;
    pid_t   pid;

    if (childpid == NULL)
    {
        return (-1);     /* popen() has never been called */
    }
    fd = fileno(fp);
    if ((pid = childpid[fd]) == 0)
    {
        return (-1);     /* fp wasn't opened by popen() */
    }
    childpid[fd] = 0;
    if (fclose(fp) == EOF)
    {
        return (-1);
    }

    while (waitpid(pid, &stat, 0) < 0)
    {
        if (errno != EINTR)
        {
            return (-1); /* error other than EINTR from waitpid() */
        }
    }
    return (stat);   /* return child's termination status */
}
