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

import java.io.File;
import java.io.IOException;
import java.util.Collections;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.tars.client.Communicator;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.support.log.Logger.Level;
import com.qq.tars.support.log.Logger.LogType;
import com.qq.tars.support.log.prx.LogPrx;
import com.qq.tars.support.log.util.Utils;

public class LoggerFactory {

    private static final String LEVEL_NONE = "NONE";

    private static volatile boolean isInit = false;

    private static volatile boolean isNone = false;

    protected static String defaultLogRoot;

    protected static String dyeingLogRoot;

    protected static Level defaultLogLevel = Level.INFO;

    private static final String DEFAULT_LOG = "_default";

    private static String appName = "";

    private static String serviceName = "";

    private static String objName = "";

    private volatile static Communicator communicator = null;

    private static final ConcurrentHashMap<String, Logger> loggerMap = new ConcurrentHashMap<String, Logger>(128);

    private static Thread th;

    private static Thread thRemote;

    private static Thread threadAll;

    static {
        loadLogConfCache();
        start();
    }

    private static void loadLogConfCache() {
        String cacheLevel = LogConfCacheMngr.getInstance().getLevel();
        if (StringUtils.isEmpty(cacheLevel)) {
            return;
        }

        loadCacheLevel(cacheLevel, false);
    }

    private static void loadCacheLevel(String cacheLevel, boolean isNeedFreshMap) {
        if (LEVEL_NONE.equals(cacheLevel)) {
            isNone = true;
            System.out.println("INFO tars-log | reset tars-log default level=NONE");
            return;
        }

        if (defaultLogLevel.toString().equals(cacheLevel)) {
            return;
        }

        defaultLogLevel = Level.valueOf(cacheLevel);
        if (isNeedFreshMap) {
            refreshLoggerLevel();
        }

        System.out.println("INFO tars-log | reset tars-log default level=" + defaultLogLevel);
    }

    static Logger getLogger(String name) {
        Logger logger = loggerMap.get(name);
        if (logger == null) {
            synchronized (LoggerFactory.class) {
                logger = loggerMap.get(name);
                if (logger == null) {
                    logger = new Logger(name, defaultLogLevel, null);
                    addDefaultLocalLog(logger);
                    getLogger(name);
                }
            }
        }
        return logger;
    }

    static Logger getLogger(String name, LogType type) {
        Logger logger = getLogger(name);
        logger.setLogType(type);
        retouchRemoteLog(logger);
        return logger;
    }

    static Map<String, Logger> getLoggerMap() {
        return Collections.unmodifiableMap(loggerMap);
    }

    static LogPrx getLoggerPrxHelper() {
        if (communicator == null) {
            return null;
        }
        return communicator.stringToProxy(LogPrx.class, objName);

    }

    private static void start() {
        try {
            th = new Thread(new LogWorkThread(LogType.LOCAL), "LocalJ4logThread");
            th.start();
            thRemote = new Thread(new LogWorkThread(LogType.REMOTE), "RemoteJ4logThread");
            thRemote.start();
            threadAll = new Thread(new LogWorkThread(LogType.ALL), "AllJ4logThread");
            threadAll.start();
        } catch (Exception e) {
            System.out.println("FATAL tars-log | init failed:" + e.getMessage());
            e.printStackTrace(System.out);
        }

    }

    public static void stop() {
        try {
            th.interrupt();
            thRemote.interrupt();
            threadAll.interrupt();
        } catch (Exception e) {
            System.out.println("FATAL j4log | init failed:" + e.getMessage());
            e.printStackTrace(System.out);
        }

    }

    public synchronized static void config(Communicator communicator, String objName, String appName,
                                           String serviceName, String defaultLevel, String defaultRoot) {
        if (isInit) {
            return;
        }

        if (communicator == null || StringUtils.isEmpty(objName)) {
            System.out.println("ERROR tars-log | config error: locator and objName can't be empty.");
        } else {
            LoggerFactory.communicator = communicator;
            LoggerFactory.objName = objName;
        }

        LoggerFactory.appName = appName;
        LoggerFactory.serviceName = serviceName;

        configDefaultRootAndLevel(defaultLevel, defaultRoot);

        isInit = true;
    }

    public synchronized static void config(String defaultLevel, String defaultRoot) {
        configDefaultRootAndLevel(defaultLevel, defaultRoot);

    }

    public synchronized static void setDefaultLoggerLevel(String level) {
        if (StringUtils.isEmpty(level)) {
            System.out.println("ERROR tars-log | reset tars-log default level error, level is empty.");
            return;
        }

        loadCacheLevel(level, true);
    }

    private static void refreshLoggerLevel() {
        for (java.util.Map.Entry<String, Logger> loggerEntry : loggerMap.entrySet()) {
            Logger logger = loggerEntry.getValue();
            logger.setLevel(defaultLogLevel);
        }
    }

    private static void configDefaultRootAndLevel(String defaultLevel, String defaultRoot) {
        if (StringUtils.isNotEmpty(defaultLevel)) {
            defaultLogLevel = Level.valueOf(defaultLevel.trim().toUpperCase());
        }

        System.out.println("INFO tars-log | set tars-log default level=" + defaultLogLevel);

        String logRootStr = selectLogRoot(defaultRoot);
        if (logRootStr != null) {
            defaultLogRoot = formatPath(logRootStr);
            System.out.println("INFO tars-log | set tars-log defaultRoot=" + defaultLogRoot);
        } else {
            System.out.println("ERROR tars-log | config error, set tars-log defaultRoot=" + defaultLogRoot);
        }
        dyeingLogRoot = defaultLogRoot + "/tars_dyeing/";
        File logRoot = new File(dyeingLogRoot);
        if (!logRoot.exists()) {
            logRoot.mkdir();
        }
    }

    private static String selectLogRoot(String defaultRoot) {
        if (StringUtils.isNotEmpty(defaultRoot)) {
            File logRoot = new File(defaultRoot);
            if (logRoot.isDirectory() && logRoot.canWrite()) {
                return defaultRoot;
            }
        }

        if (StringUtils.isNotEmpty(defaultLogRoot)) {
            return null;
        }

        System.out.println("INFO tars-log | try to set tars-log.root from java system property (server.root)");
        String serverRoot = System.getProperty("server.root", "");
        serverRoot = serverRoot.trim() + File.separator + "log";
        File logRoot = new File(serverRoot);
        if (!logRoot.exists()) {
            logRoot.mkdir();
        }
        if (logRoot.isDirectory() && logRoot.canWrite()) {
            return serverRoot;
        }

        return null;
    }

    static String formatPath(String path) {
        if (path != null) {
            path = path.replaceAll("[/]+", "/");
            path = path.replaceAll("[/]+$", "");
            path = path.replaceAll("[\\\\]+$", "");
        }

        return path + File.separatorChar;
    }

    static boolean testFile(String filePath) {
        if (filePath == null || filePath.trim().isEmpty() || !filePath.contains("/") && !filePath.contains("\\")) {
            return false;
        }
        try {
            File f = new File(filePath + "." + Utils.getDateSimpleInfo(System.currentTimeMillis()));
            f.createNewFile();
            if (!f.canWrite()) {
                return false;
            }

            if (f.length() == 0) {
                f.delete();
            }
        } catch (IOException e) {
            return false;
        }
        return true;
    }

    private static void addDefaultLocalLog(Logger log) {
        retouchLocalLog(log);
        updateLoggerMap(log);
    }

    static void retouchLocalLog(Logger log) {
        if (log.getLevel() == null) {
            log.setLevel(defaultLogLevel);
        }
        if (log.getLogPath() == null || !testFile(log.getLogPath())) {
            String path = defaultLogRoot + log.getName();
            log.setLogPath(path);
            if (!testFile(path)) {
                log.setLogPath(null);
                System.out.println("ERROR tars-log | set logger(" + log.getName() + ")local log path to default path(" + path + ") failed");
            }
        }
    }

    private static void retouchRemoteLog(Logger logger) {
        if (logger.getLogType() == LogType.LOCAL) {
            return;
        }

        if (logger.getFailLogPath() == null || !testFile(logger.getFailLogPath())) {
            String failPath = defaultLogRoot + logger.getName() + ".lost";
            logger.setFailLogPath(failPath);
            if (!testFile(failPath)) {
                logger.setFailLogPath(null);

            }
        }
    }

    private static synchronized void updateLoggerMap(Logger logger) {
        Logger log = loggerMap.get(logger.getName());

        if (log == null) {
            loggerMap.put(logger.getName(), logger);
        } else {
            log.setLevel(logger.getLevel());
            log.setLogType(logger.getLogType());
            log.setLogPath(logger.getLogPath());
            log.setFailLogPath(logger.getFailLogPath());
        }
    }

    static String getAppName() {
        return appName;
    }

    static String getServiceNameName() {
        return serviceName;
    }

    static String getObjName() {
        return objName;
    }

    static String getDefaultLog() {
        return DEFAULT_LOG;
    }

    static boolean isNoneEnabled() {
        return isNone;
    }
}
