/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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

package com.qq.tars.support.log;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

import com.qq.tars.support.log.prx.LogInfo;
import com.qq.tars.support.log.prx.LogPrx;
import com.qq.tars.support.log.util.Utils;

public class Logger {

    public static Logger getLogger(String name) {
        return LoggerFactory.getLogger(name);
    }

    public static Logger getLogger(String name, LogType logType) {
        return LoggerFactory.getLogger(name, logType);
    }

    public static enum LogType {
        LOCAL(0), REMOTE(1), ALL(2);

        private int value;

        private LogType(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }
    }

    public static enum Level {
        DEBUG(0), INFO(1), WARN(2), ERROR(3), FATAL(4);

        private int value;

        private Level(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }

    }

    private static final int MAX_BATCH_SIZE = 1024 * 1024;

    private static final String STDOUT_LOG = "stdout.log";

    private LogType logType = LogType.LOCAL;

    private String failLogPath;

    private AtomicInteger failedCount = new AtomicInteger(0);

    private static final int failedThreshold = 50;

    protected final String name;

    protected String logPath;

    protected Level level = null;

    protected final LinkedBlockingQueue<LogItem> logQueue = new LinkedBlockingQueue<LogItem>(50000);

    Logger(String name) {
        this(name, Level.INFO, null, null);
    }

    Logger(String name, Level level, String filePath) {
        this.name = name;
        this.level = level;
        this.logPath = filePath;
    }

    // Logger(String name, Level level, String path, String pattern)
    // {
    // this(name, level, path, null, pattern);
    // }

    Logger(String name, Level level, String filePath, String failLogPath) {
        this(name, level, filePath);
        this.failLogPath = failLogPath;

        // if (StringUtil.isEmpty(pattern))
        // {
        // formatter = new PatternFormatter();
        // }
        // else
        // {
        // formatter = new PatternFormatter(pattern);
        // }
    }

    Logger(String name, Level level, LogType logType, String localPath, String failLogPath) {
        this(name, level, localPath);
        this.logType = logType;
        this.failLogPath = failLogPath;
    }

    public boolean isDebugEnabled() {
        return Level.DEBUG.value >= this.level.value;
    }

    public boolean isInfoEnabled() {
        return Level.INFO.value >= this.level.value;
    }

    public void debug(String str) {
        debug(str, null, null);
    }

    public void debug(String str, Throwable th) {
        debug(str, th, null);

    }

    protected void debug(String format, Throwable th, Object[] arguments) {
        if (isDebugEnabled()) {
            // formatLogMsg(Level.DEBUG, format, arguments, th);
            log(Level.DEBUG, format, th);
        }
    }

    public void info(String str) {
        info(str, null, null);
    }

    public void info(String str, Throwable th) {
        info(str, th, null);
    }

    protected void info(String format, Throwable th, Object[] arguments) {
        if (this.isInfoEnabled()) {
            // formatLogMsg(Level.INFO, format, arguments, th);
            log(Level.INFO, format, th);
        }
    }

    public void warn(String str) {
        warn(str, null, null);
    }

    public void warn(String str, Throwable th) {
        warn(str, th, null);
    }

    protected void warn(String format, Throwable th, Object[] arguments) {
        // formatLogMsg(Level.WARN, format, arguments, th);
        log(Level.WARN, format, th);
    }

    public void error(String str) {
        error(str, null, null);
    }

    public void error(String str, Throwable th) {
        error(str, th, null);
    }

    protected void error(String format, Throwable th, Object[] arguments) {
        // formatLogMsg(Level.ERROR, format, arguments, th);
        log(Level.ERROR, format, th);
    }

    public void fatal(String str) {
        fatal(str, null, null);
    }

    public void fatal(String str, Throwable th) {
        fatal(str, th, null);
    }

    protected void fatal(String format, Throwable th, Object[] arguments) {
        // formatLogMsg(Level.FATAL, format, arguments, th);
        log(Level.FATAL, format, th);
    }

    protected void log(Level level, String str, Throwable th) {
        if (LoggerFactory.isNoneEnabled()) {
            return;
        }

        if (str == null) {
            str = "";
        }

        if (isNeedLocal() && this.logPath == null) {
            putLogToDefault(level, str, th);
            return;
        }

        boolean result = getQueue().offer(new LogItem(level, str, th));
        if (!result) { 
            putLogToDefault(level, str, th);
            if (isNeedRemote()) {
                failedCount.getAndIncrement();
            }
        } else if (failedCount.get() > 0) {
            failedCount.set(0);
        }
    }

    private void putLogToDefault(Level level, String str, Throwable th) {
        if (!LoggerFactory.getDefaultLog().equals(this.name)) {
            Logger.getLogger(LoggerFactory.getDefaultLog()).log(level, this.name + "\t" + str, th);
        }
    }

    protected void doWriteLog() {
        if (logQueue.isEmpty()) {
            return;
        }

        if (LogType.LOCAL.value == this.logType.value) {
            writeAllToLocal(logPath);
        } else if (LogType.REMOTE.value == this.logType.value) {
            writeRemoteLog();
        } else {
            writeLocalAndRemoteLog();
        }
    }

    private void writeAllToLocal(String file) {
        boolean succ = false;
        if (file != null) {
            BufferedWriter bw = null;
            try {
                if (!logQueue.isEmpty()) {
                    bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file + "." + Utils.getDateSimpleInfo(System.currentTimeMillis()), true), "UTF-8"));
                    LogItem item = null;
                    while ((item = logQueue.poll()) != null) {
                        bw.write(item.toString());
                    }
                }
                succ = true;
            } catch (IOException ex) {

            } finally {
                if (bw != null) {
                    try {
                        bw.close();
                    } catch (IOException e) {
                    }
                }
            }
        }
        if (!succ) {
            LogItem item = null;
            while ((item = logQueue.poll()) != null && !STDOUT_LOG.equals(name)) {
                System.out.print("\t" + item.toString());
            }
        }
    }

    private void writeLocalAndRemoteLog() {
        if (isReachedFailedThreshold()) {
            writeAllToLocal(getLogPath());
            return;
        }

        LogPrx proxy = LoggerFactory.getLoggerPrxHelper();
        if (proxy != null) {
            boolean succ = false;
            LogInfo loginfo = getLogInfo();

            while (!getQueue().isEmpty()) {
                ArrayList<String> logs = new ArrayList<String>();
                getLogsBySize(logs);

                try {
                    writeToLocal(logs, getLogPath());
                    proxy.loggerbyInfo(loginfo, logs);
                    succ = true;
                } catch (Exception e) {
                    succ = false;
                } finally {
                    if (!succ && getLogPath() != null) {
                        writeAllToLocal(getLogPath());
                    }
                }
            }
        } else {
            writeAllToLocal(getLogPath());
        }
    }

    private LogInfo getLogInfo() {
        LogInfo loginfo = null;
        if (!getQueue().isEmpty()) {
            loginfo = new LogInfo();
            loginfo.setAppname(LoggerFactory.getAppName());
            loginfo.setServername(LoggerFactory.getServiceNameName());
            loginfo.setSFilename(this.name);
            loginfo.setSFormat("%Y%m%d%H");
        }
        return loginfo;
    }

    private void writeRemoteLog() {
        if (isReachedFailedThreshold()) {
            writeAllToLocal(getLogPath());
            return;
        }

        LogPrx proxy = LoggerFactory.getLoggerPrxHelper();
        if (proxy != null) {
            boolean succ = false;
            LogInfo loginfo = getLogInfo();

            while (!getQueue().isEmpty()) {
                ArrayList<String> logs = new ArrayList<String>();
                getLogsBySize(logs);

                try {
                    try {
                        proxy.loggerbyInfo(loginfo, logs);
                        succ = true;
                    } catch (Exception e) {
                        succ = false;
                    }
                    //retry one
                    if (!succ) {
                        proxy.loggerbyInfo(loginfo, logs);
                        succ = true;
                    }
                } catch (Exception e) {
                    succ = false;
                } finally {
                    if (!succ && getFailLogPath() != null) {
                        writeToLocal(logs, getFailLogPath());
                    }
                }
            }
        } else {
            writeAllToLocal(getFailLogPath());
        }
    }

    private boolean isReachedFailedThreshold() {
        return failedCount.get() > failedThreshold;
    }

    private void getLogsBySize(ArrayList<String> logs) {
        int size = 0;
        String logContent;
        while (size < MAX_BATCH_SIZE && !getQueue().isEmpty()) {
            logContent = getQueue().poll().toString();
            logs.add(logContent);

            try {
                size += logContent.getBytes("UTF-8").length;
            } catch (UnsupportedEncodingException e) {
            }
        }
    }

    private void writeToLocal(ArrayList<String> content, String file) {
        Writer bw = null;
        try {
            OutputStream os = null;
            try {
                os = new FileOutputStream(file + "." + Utils.getDateSimpleInfo(System.currentTimeMillis()), true);
                bw = new BufferedWriter(new OutputStreamWriter(os, "UTF-8"));
                for (String s : content) {
                    bw.write(s);
                }
                bw.flush();
                return;
            } catch (Exception e) {
            }
        } finally {
            if (bw != null) {
                try {
                    bw.close();
                } catch (IOException e) {
                }
            }
        }
    }

    boolean isNeedLocal() {
        return LogType.LOCAL.value == this.logType.value || LogType.ALL.value == this.logType.value;
    }

    boolean isNeedRemote() {
        return LogType.REMOTE.value == this.logType.value || LogType.ALL.value == this.logType.value;
    }

    boolean isLocalLog() {
        return LogType.LOCAL.value == this.logType.value;
    }

    boolean isRemoteLog() {
        return LogType.REMOTE.value == this.logType.value;
    }

    boolean isAllLog() {
        return LogType.ALL.value == this.logType.value;
    }

    Level getLevel() {
        return level;
    }

    String getName() {
        return name;
    }

    String getLogPath() {
        return logPath;
    }

    void setLogPath(String logPath) {
        this.logPath = logPath;
    }

    String getFailLogPath() {
        return failLogPath;
    }

    Queue<LogItem> getQueue() {
        return logQueue;
    }

    void setFailLogPath(String path) {
        this.failLogPath = path;
    }

    void setLevel(Level level) {
        if (level == null) {
            throw new NullPointerException("Logger Level is null.");
        }
        this.level = level;
    }

    LogType getLogType() {
        return logType;
    }

    void setLogType(LogType logType) {
        this.logType = logType;
    }

    @Override
    public String toString() {
        return "Logger [name=" + name + ", level=" + level + ", logType=" + logType + ", logPath=" + logPath + ", failLogPath=" + failLogPath + "]";
    }
}
