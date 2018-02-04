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
import java.nio.channels.FileLock;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

import com.qq.tars.common.util.DyeingSwitch;
import com.qq.tars.context.DistributedContext;
import com.qq.tars.context.DistributedContextManager;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServerConfig;
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
    
    protected final ConcurrentHashMap<String, LinkedBlockingQueue<LogItem>> dyeLogQueue = new ConcurrentHashMap<String, LinkedBlockingQueue<LogItem>>();

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
        
        boolean dyeFlag = false;
        String dyeFileName = null;
        String dyeServantName = null; 
        DistributedContext distributedContext = DistributedContextManager.getDistributedContext();
        if (null != distributedContext.get(DyeingSwitch.BDYEING)) {
        	dyeFlag = distributedContext.get(DyeingSwitch.BDYEING);
        	dyeFileName = distributedContext.get(DyeingSwitch.FILENAME);
        	ServerConfig config = ConfigurationManager.getInstance().getServerConfig();
        	dyeServantName = config == null ? "no Tars" : config.getServerName();
        }

        if (isNeedLocal() && this.logPath == null) {
            putLogToDefault(level, str, th);
            return;
        }

        LogItem logItem = new LogItem(level, str, dyeFlag, dyeFileName, dyeServantName, th);
        boolean result = getQueue().offer(logItem);
        
        if (dyeFlag == true) {
	        LinkedBlockingQueue<LogItem> subItems = dyeLogQueue.get(dyeFileName);
	        if (subItems == null) {
	        	subItems = new LinkedBlockingQueue<LogItem>();
	        	dyeLogQueue.putIfAbsent(dyeFileName, subItems);
	        	subItems = dyeLogQueue.get(dyeFileName);
	        }
	        subItems.offer(logItem);
        }
        
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
            writeDyeToLocal(false);
        } else if (LogType.REMOTE.value == this.logType.value) {
            writeRemoteLog();
            writeDyeToRemote();
        } else {
            writeLocalAndRemoteLog();
            writeDyeLocalAndRemoteLog();
        }
    }
    
    private void writeDyeToLocal(boolean isLost) {
    	if (dyeLogQueue == null || dyeLogQueue.isEmpty()) {
    		return;
    	}
    	BufferedWriter bw = null;
    	FileLock lock = null;
    	Iterator<Map.Entry<String, LinkedBlockingQueue<LogItem>>> iterator = dyeLogQueue.entrySet().iterator();
    	while (iterator.hasNext()) {
    		Map.Entry<String, LinkedBlockingQueue<LogItem>> entry = iterator.next();
	    	try {
	    		if (!entry.getValue().isEmpty()) {
	    			String fileName = isLost == true ? entry.getKey() + "_dyeing.lost." : entry.getKey() + "_dyeing.";
	    			FileOutputStream outputStream = new FileOutputStream(LoggerFactory.dyeingLogRoot + fileName + 
	    		                                            Utils.getDateSimpleInfo(System.currentTimeMillis()), true);
	    			do {
	    				lock = outputStream.getChannel().tryLock();
	    			} while (lock == null);
	    			
	    			bw = new BufferedWriter(new OutputStreamWriter(outputStream, "UTF-8"));
	    			LogItem item = null;
	    			while ((item = entry.getValue().poll()) != null) {
	    				bw.write(item.toDyeingString());
	    			}
	    		}
	    	} catch (IOException ex) {
	    		
	    	} finally {
	    		if (lock != null) {
	    			try {
	    				lock.release();
	    				lock = null;
	    			} catch (IOException e) {
	    			}
	    		}
	            if (bw != null) {
	                try {
	                    bw.close();
	                } catch (IOException e) {
	                }
	            }
	    	}
    	}  	
    }
    
    private void writeDyeToRemote() {
    	if (dyeLogQueue == null || dyeLogQueue.isEmpty()) {
    		return;
    	}
    	
    	LogPrx proxy = LoggerFactory.getLoggerPrxHelper();
    	if (proxy == null) {
    		writeDyeToLocal(true);
    		return;
    	}
    	Iterator<Map.Entry<String, LinkedBlockingQueue<LogItem>>> iterator = dyeLogQueue.entrySet().iterator();
    	while (iterator.hasNext()) {
    		Map.Entry<String, LinkedBlockingQueue<LogItem>> entry = iterator.next();
    		
    		if (proxy != null) {
    			boolean succ = false;
    			LogInfo logInfo = getDyeingInfo(entry.getKey());
    			while (!entry.getValue().isEmpty()) {
    				ArrayList<String> logs = new ArrayList<String>();
    				getDyeingLogsBySize(entry.getKey(), logs);
    				try {
    					proxy.loggerbyInfo(logInfo, logs);
    					succ = true;
    				} catch (Exception e) {
    					succ = false;
    				} finally {
    					if (succ == false) {
    						writeDyeToLocal(logs, LoggerFactory.dyeingLogRoot + entry.getKey() + "_dyeing.lost");
    					}
    				}
    			}
    		}
    	}   	
    }
    
    private void writeDyeLocalAndRemoteLog() {
    	if (dyeLogQueue == null || dyeLogQueue.isEmpty()) {
    		return;
    	}
    	
    	LogPrx proxy = LoggerFactory.getLoggerPrxHelper();
    	if (proxy == null) {
    		writeDyeToLocal(false);
    		return;
    	}
    	Iterator<Map.Entry<String, LinkedBlockingQueue<LogItem>>> iterator = dyeLogQueue.entrySet().iterator();
    	while (iterator.hasNext()) {
    		Map.Entry<String, LinkedBlockingQueue<LogItem>> entry = iterator.next();
    	    boolean succ = false;
			LogInfo logInfo = getDyeingInfo(entry.getKey());
			while (!entry.getValue().isEmpty()) {
				ArrayList<String> logs = new ArrayList<String>();
				getDyeingLogsBySize(entry.getKey(), logs);
				try {
					writeDyeToLocal(logs, LoggerFactory.dyeingLogRoot + entry.getKey() + "_dyeing");
					proxy.loggerbyInfo(logInfo, logs);
					succ = true;
				} catch (Exception e) {
					succ = false;
				} finally {
					if (!succ) {
						writeDyeToLocal(false);
					}
				}
    		}
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
    
    private LogInfo getDyeingInfo(String name) {
    	LogInfo loginfo = null;
    	if (dyeLogQueue.get(name) != null && !dyeLogQueue.get(name).isEmpty()) {
    		loginfo = new LogInfo();
    		loginfo.setAppname("tars_dyeing");
    		loginfo.setServername("dyeing");
    		loginfo.setSFilename(name + "_dyeing");
    		loginfo.setSFormat("%Y%m%d%H");
    	}
    	return loginfo;
    }
    
    private void writeDyeToLocal(ArrayList<String> content, String file) {
        Writer bw = null;
        FileLock lock = null;
        try {
            FileOutputStream os = null;
            try {
                os = new FileOutputStream(file + "." + Utils.getDateSimpleInfo(System.currentTimeMillis()), true);
                do {
                	lock = os.getChannel().tryLock();
                } while (null == lock);
                
                bw = new BufferedWriter(new OutputStreamWriter(os, "UTF-8"));
                for (String s : content) {
                    bw.write(s);
                }
                bw.flush();
                return;
            } catch (Exception e) {
            }
        } finally {
        	if (lock != null) {
        		try {
        			lock.release();
        			lock = null;
        		} catch (IOException e) {
        		}
        	}
            if (bw != null) {
                try {
                    bw.close();
                } catch (IOException e) {
                }
            }
        }
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
    
    private void getDyeingLogsBySize(String fileName, ArrayList<String> logs) {
        int size = 0;
        String logContent;
        LinkedBlockingQueue<LogItem> subItems = dyeLogQueue.get(fileName);
        while (size < MAX_BATCH_SIZE && subItems != null && !subItems.isEmpty()) {
        	LogItem item = subItems.poll();
            logContent = item.toDyeingString();
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
