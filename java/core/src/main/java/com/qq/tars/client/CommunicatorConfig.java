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

package com.qq.tars.client;

import java.io.FileNotFoundException;
import java.io.IOException;

import com.qq.tars.common.util.Config;
import com.qq.tars.common.util.Constants;

public class CommunicatorConfig {

    private String locator = "";
    private int syncInvokeTimeout = 3000;
    private int asyncInvokeTimeout = 3000;
    private int refreshEndpointInterval = 60000;
    private int reportInterval = 60000;

    private String stat = Constants.default_stat;
    private String property = null;

    private int sampleRate = 0;
    private int maxSampleCount = 0;

    private int sendThread = 1;
    private int recvThread = 1;
    private int asyncThread = 1;
    private String moduleName = Constants.default_modulename;

    private boolean enableSet = false;
    private String setDivision = null;

    private int connections = Constants.default_connections;
    private int connectTimeout = Constants.default_connect_timeout;
    private int corePoolSize = Constants.default_core_pool_size;
    private int maxPoolSize = Constants.default_max_pool_size;
    private int keepAliveTime = Constants.default_keep_alive_time;
    private int queueSize = Constants.default_queue_size;
    private String charsetName = Constants.default_charset_name;

    private String logPath;
    private String logLevel = "INFO";
    private String dataPath;

    public CommunicatorConfig() {
    }

    public CommunicatorConfig load(Config conf) {
        locator = conf.get("/tars/application/client<locator>");
        logPath = conf.get("/tars/application/client<logpath>", null);
        logLevel = conf.get("/tars/application/client<loglevel>", "INFO");
        dataPath = conf.get("/tars/application/client<cdatapath>", null);
        syncInvokeTimeout = conf.getInt("/tars/application/client<sync-invoke-timeout>", 3000);
        asyncInvokeTimeout = conf.getInt("/tars/application/client<async-invoke-timeout>", 3000);
        refreshEndpointInterval = conf.getInt("/tars/application/client<refresh-endpoint-interval>", 60000);
        stat = conf.get("/tars/application/client<stat>");
        property = conf.get("/tars/application/client<property>");
        reportInterval = conf.getInt("/tars/application/client<report-interval>", 60000);
        sampleRate = conf.getInt("/tars/application/client<sample-rate>", 1000);
        maxSampleCount = conf.getInt("/tars/application/client<max-sample-count>", 100);
        sendThread = conf.getInt("/tars/application/client<sendthread>", 1);
        recvThread = conf.getInt("/tars/application/client<recvthread>", 1);
        asyncThread = conf.getInt("/tars/application/client<asyncthread>", 1);
        moduleName = conf.get("/tars/application/client<modulename>", Constants.default_modulename);
        String enableSetStr = conf.get("/tars/application<enableset>");
        setDivision = conf.get("/tars/application<setdivision>");
        if ("Y".equalsIgnoreCase(enableSetStr)) {
            enableSet = true;
        } else {
            enableSet = false;
            setDivision = null;
        }

        connections = conf.getInt("/tars/application/client<connections>", Constants.default_connections);
        connectTimeout = conf.getInt("/tars/application/client<connect-timeout>", Constants.default_connect_timeout);
        corePoolSize = conf.getInt("/tars/application/client<corepoolsize>", Constants.default_core_pool_size);
        maxPoolSize = conf.getInt("/tars/application/client<maxpoolsize>", Constants.default_max_pool_size);
        keepAliveTime = conf.getInt("/tars/application/client<keepalivetime>", Constants.default_keep_alive_time);
        queueSize = conf.getInt("/tars/application/client<queuesize>", Constants.default_queue_size);
        charsetName = conf.get("/tars/application/client<charsetname>", Constants.default_charset_name);
        return this;
    }

    public static CommunicatorConfig load(String confFile) throws FileNotFoundException, IOException {
        CommunicatorConfig cfg = new CommunicatorConfig();
        cfg.load(Config.parseFile(confFile));
        return cfg;
    }

    public String getLocator() {
        return locator;
    }

    public CommunicatorConfig setLocator(String locator) {
        this.locator = locator;
        return this;
    }

    public int getSyncInvokeTimeout() {
        return syncInvokeTimeout;
    }

    public CommunicatorConfig setSyncInvokeTimeout(int syncInvokeTimeout) {
        this.syncInvokeTimeout = syncInvokeTimeout;
        return this;
    }

    public int getAsyncInvokeTimeout() {
        return asyncInvokeTimeout;
    }

    public CommunicatorConfig setAsyncInvokeTimeout(int asyncInvokeTimeout) {
        this.asyncInvokeTimeout = asyncInvokeTimeout;
        return this;
    }

    public int getRefreshEndpointInterval() {
        return refreshEndpointInterval;
    }

    public CommunicatorConfig setRefreshEndpointInterval(int refreshEndpointInterval) {
        this.refreshEndpointInterval = refreshEndpointInterval;
        return this;
    }

    public String getStat() {
        return stat;
    }

    public CommunicatorConfig setStat(String stat) {
        this.stat = stat;
        return this;
    }

    public String getProperty() {
        return property;
    }

    public CommunicatorConfig setProperty(String property) {
        this.property = property;
        return this;
    }

    public int getReportInterval() {
        return reportInterval;
    }

    public CommunicatorConfig setReportInterval(int reportInterval) {
        this.reportInterval = reportInterval;
        return this;
    }

    public int getSampleRate() {
        return sampleRate;
    }

    public CommunicatorConfig setSampleRate(int sampleRate) {
        this.sampleRate = sampleRate;
        return this;
    }

    public int getMaxSampleCount() {
        return maxSampleCount;
    }

    public CommunicatorConfig setMaxSampleCount(int maxSampleCount) {
        this.maxSampleCount = maxSampleCount;
        return this;
    }

    @Deprecated
    public int getSendThread() {
        return sendThread;
    }

    @Deprecated
    public CommunicatorConfig setSendThread(int sendThread) {
        this.sendThread = sendThread;
        return this;
    }

    @Deprecated
    public int getRecvThread() {
        return recvThread;
    }

    @Deprecated
    public CommunicatorConfig setRecvThread(int recvThread) {
        this.recvThread = recvThread;
        return this;
    }

    @Deprecated
    public int getAsyncThread() {
        return asyncThread;
    }

    @Deprecated
    public CommunicatorConfig setAsyncThread(int asyncThread) {
        this.asyncThread = asyncThread;
        return this;
    }

    public String getModuleName() {
        return moduleName;
    }

    public CommunicatorConfig setModuleName(String moduleName) {
        this.moduleName = moduleName;
        return this;
    }

    public boolean isEnableSet() {
        return enableSet;
    }

    public CommunicatorConfig setEnableSet(boolean enableSet) {
        this.enableSet = enableSet;
        return this;
    }

    public String getSetDivision() {
        return setDivision;
    }

    public CommunicatorConfig setSetDivision(String setDivision) {
        this.setDivision = setDivision;
        return this;
    }

    public int getConnections() {
        return connections;
    }

    public CommunicatorConfig setConnections(int connections) {
        this.connections = connections;
        return this;
    }

    public int getConnectTimeout() {
        return connectTimeout;
    }

    public CommunicatorConfig setConnectTimeout(int connectTimeout) {
        this.connectTimeout = connectTimeout;
        return this;
    }

    public int getCorePoolSize() {
        return corePoolSize;
    }

    public CommunicatorConfig setCorePoolSize(int corePoolSize) {
        this.corePoolSize = corePoolSize;
        return this;
    }

    public int getMaxPoolSize() {
        return maxPoolSize;
    }

    public CommunicatorConfig setMaxPoolSize(int maxPoolSize) {
        this.maxPoolSize = maxPoolSize;
        return this;
    }

    public int getKeepAliveTime() {
        return keepAliveTime;
    }

    public CommunicatorConfig setKeepAliveTime(int keepAliveTime) {
        this.keepAliveTime = keepAliveTime;
        return this;
    }

    public int getQueueSize() {
        return queueSize;
    }

    public CommunicatorConfig setQueueSize(int queueSize) {
        this.queueSize = queueSize;
        return this;
    }

    public String getLogPath() {
        return logPath;
    }

    public CommunicatorConfig setLogPath(String logPath) {
        this.logPath = logPath;
        return this;
    }

    public String getLogLevel() {
        return logLevel;
    }

    public CommunicatorConfig setLogLevel(String logLevel) {
        this.logLevel = logLevel;
        return this;
    }

    public String getDataPath() {
        return dataPath;
    }

    public CommunicatorConfig setDataPath(String dataPath) {
        this.dataPath = dataPath;
        return this;
    }

    public String getCharsetName() {
        return charsetName;
    }

    public CommunicatorConfig setCharsetName(String charsetName) {
        this.charsetName = charsetName;
        return this;
    }

    public static CommunicatorConfig getDefault() {
        return new CommunicatorConfig();
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((locator == null) ? 0 : locator.hashCode());
        result = prime * result + ((moduleName == null) ? 0 : moduleName.hashCode());
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null) return false;
        if (getClass() != obj.getClass()) return false;
        CommunicatorConfig other = (CommunicatorConfig) obj;
        if (locator == null) {
            if (other.locator != null) return false;
        } else if (!locator.equals(other.locator)) return false;
        if (moduleName == null) {
            if (other.moduleName != null) return false;
        } else if (!moduleName.equals(other.moduleName)) return false;
        return true;
    }

    @Override
    public String toString() {
        return String.format("CommunicatorConfig [locator=%s, syncInvokeTimeout=%s, asyncInvokeTimeout=%s, refreshEndpointInterval=%s, reportInterval=%s, stat=%s, property=%s, sampleRate=%s, maxSampleCount=%s, sendThread=%s, recvThread=%s, asyncThread=%s, moduleName=%s, enableSet=%s, setDivision=%s, connections=%s, connectTimeout=%s, corePoolSize=%s, maxPoolSize=%s, keepAliveTime=%s, queueSize=%s, charsetName=%s, logPath=%s, logLevel=%s, dataPath=%s]", locator, syncInvokeTimeout, asyncInvokeTimeout, refreshEndpointInterval, reportInterval, stat, property, sampleRate, maxSampleCount, sendThread, recvThread, asyncThread, moduleName, enableSet, setDivision, connections, connectTimeout, corePoolSize, maxPoolSize, keepAliveTime, queueSize, charsetName, logPath, logLevel, dataPath);
    }
}
