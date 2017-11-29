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

import com.qq.tars.client.util.ParseTools;
import com.qq.tars.common.util.Constants;
import com.qq.tars.common.util.StringUtils;

public final class ServantProxyConfig {

    private String communicatorId;

    private String protocol;

    private String locator;
    private String stat = Constants.default_stat;

    private String objectName;
    private String simpleObjectName;
    private String moduleName = Constants.default_modulename;
    private int connections = Constants.default_connections;
    private int connectTimeout = Constants.default_connect_timeout;
    private int syncTimeout = Constants.default_sync_timeout;
    private int asyncTimeout = Constants.default_async_timeout;
    private boolean enableSet = false;
    private String setDivision = "";

    private int refreshInterval = Constants.default_refresh_interval;
    private int reportInterval = Constants.default_report_interval;

    private int checkInterval = 60 * 1000;
    private int tryTimeInterval = 30;
    private int minTimeoutInvoke = 20;
    private int frequenceFailInvoke = 50;
    private float frequenceFailRadio = 0.5f;

    private boolean tcpNoDelay = false;
    private String charsetName = Constants.default_charset_name;

    private boolean directConnection = false;

    private int minStaticWeightLimit = 10;
    private int maxStaticWeightLimit = 100;

    private int defaultConHashVirtualNodes = 100;

    public ServantProxyConfig(String objectName) {
        this(null, null, objectName);
    }

    ServantProxyConfig(String communicatorId, String locator, String objectName) {
        this(communicatorId, locator, objectName, ParseTools.hasServerNode(objectName));
    }

    private ServantProxyConfig(String communicatorId, String locator, String objectName, boolean directConnection) {
        this.setCommunicatorId(communicatorId);
        this.setLocator(locator);
        this.setObjectName(objectName);
        if (directConnection && StringUtils.isNotEmpty(locator) && locator.startsWith(this.getSimpleObjectName())) {
            this.setDirectConnection(false);
        } else {
            this.setDirectConnection(directConnection);
        }
    }

    public String getCommunicatorId() {
        return communicatorId;
    }

    void setCommunicatorId(String communicatorId) {
        this.communicatorId = communicatorId;
    }

    public String getProtocol() {
        return protocol;
    }

    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    public String getLocator() {
        return locator;
    }

    void setLocator(String locator) {
        this.locator = locator;
    }

    public String getStat() {
        return stat;
    }

    public void setStat(String stat) {
        this.stat = stat;
    }

    public String getObjectName() {
        return objectName;
    }

    public String getSimpleObjectName() {
        return simpleObjectName;
    }

    public void setObjectName(String objectName) {
        this.objectName = objectName;
        this.simpleObjectName = ParseTools.parseSimpleObjectName(objectName);
    }

    public String getModuleName() {
        return moduleName;
    }

    public void setModuleName(String moduleName) {
        this.moduleName = moduleName;
    }

    public int getConnectTimeout() {
        return connectTimeout;
    }

    public void setConnectTimeout(int connectTimeout) {
        if (connectTimeout > 0) {
            this.connectTimeout = connectTimeout;
        }
    }

    public int getSyncTimeout() {
        return syncTimeout;
    }

    public void setSyncTimeout(int syncTimeout) {
        if (syncTimeout > 0) {
            this.syncTimeout = syncTimeout;
        }
    }

    public int getAsyncTimeout() {
        return asyncTimeout;
    }

    public void setAsyncTimeout(int asyncTimeout) {
        if (asyncTimeout > 0) {
            this.asyncTimeout = asyncTimeout;
        }
    }

    public boolean isEnableSet() {
        return enableSet;
    }

    public void setEnableSet(boolean enableSet) {
        this.enableSet = enableSet;
    }

    public String getSetDivision() {
        return setDivision;
    }

    public void setSetDivision(String setDivision) {
        this.setDivision = setDivision;
    }

    public int getMinTimeoutInvoke() {
        return minTimeoutInvoke;
    }

    public void setMinTimeoutInvoke(int minTimeoutInvoke) {
        if (minTimeoutInvoke > 0) {
            this.minTimeoutInvoke = minTimeoutInvoke;
        }
    }

    public int getCheckInterval() {
        return checkInterval;
    }

    public void setCheckInterval(int checkInterval) {
        if (checkInterval > 0) {
            this.checkInterval = checkInterval;
        }
    }

    public int getFrequenceFailInvoke() {
        return frequenceFailInvoke;
    }

    public void setFrequenceFailInvoke(int frequenceFailInvoke) {
        if (frequenceFailInvoke > 0) {
            this.frequenceFailInvoke = frequenceFailInvoke;
        }
    }

    public float getFrequenceFailRadio() {
        return frequenceFailRadio;
    }

    public void setFrequenceFailRadio(float frequenceFailRadio) {
        this.frequenceFailRadio = frequenceFailRadio;
    }

    public int getTryTimeInterval() {
        return tryTimeInterval;
    }

    public void setTryTimeInterval(int tryTimeInterval) {
        if (tryTimeInterval > 0) {
            this.tryTimeInterval = tryTimeInterval;
        }
    }

    public String getCharsetName() {
        return charsetName;
    }

    public void setCharsetName(String charsetName) {
        this.charsetName = charsetName;
    }

    public boolean isTcpNoDelay() {
        return tcpNoDelay;
    }

    public void setTcpNoDelay(boolean tcpNoDelay) {
        this.tcpNoDelay = tcpNoDelay;
    }

    public boolean isDirectConnection() {
        return directConnection;
    }

    private void setDirectConnection(boolean directConnection) {
        this.directConnection = directConnection;
    }

    public int getConnections() {
        return connections;
    }

    public void setConnections(int connections) {
        if (connections > 0) {
            this.connections = connections;
        }
    }

    public int getRefreshInterval() {
        return refreshInterval;
    }

    public void setRefreshInterval(int refreshInterval) {
        this.refreshInterval = refreshInterval;
    }

    public int getReportInterval() {
        return reportInterval;
    }

    public void setReportInterval(int reportInterval) {
        this.reportInterval = reportInterval;
    }

    public int getMaxStaticWeightLimit() {
        return maxStaticWeightLimit;
    }

    public void setMaxStaticWeightLimit(int maxStaticWeightLimit) {
        this.maxStaticWeightLimit = maxStaticWeightLimit;
    }


    public int getMinStaticWeightLimit() {
        return minStaticWeightLimit;
    }

    public void setMinStaticWeightLimit(int minStaticWeightLimit) {
        this.minStaticWeightLimit = minStaticWeightLimit;
    }

    public int getDefaultConHashVirtualNodes() {
        return defaultConHashVirtualNodes;
    }

    public void setDefaultConHashVirtualNodes(int defaultConHashVirtualNodes) {
        this.defaultConHashVirtualNodes = defaultConHashVirtualNodes;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((communicatorId == null) ? 0 : communicatorId.hashCode());
        result = prime * result + ((simpleObjectName == null) ? 0 : simpleObjectName.hashCode());
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null) return false;
        if (getClass() != obj.getClass()) return false;
        ServantProxyConfig other = (ServantProxyConfig) obj;
        if (communicatorId == null) {
            if (other.communicatorId != null) return false;
        } else if (!communicatorId.equals(other.communicatorId)) return false;
        if (simpleObjectName == null) {
            if (other.simpleObjectName != null) return false;
        } else if (!simpleObjectName.equals(other.simpleObjectName)) return false;
        return true;
    }

    @Override
    public String toString() {
        return String.format("ServantProxyConfig [communicatorId=%s, locator=%s, stat=%s, objectName=%s, simpleObjectName=%s, moduleName=%s, connections=%s, connectTimeout=%s, syncTimeout=%s, asyncTimeout=%s, enableSet=%s, setDivision=%s, refreshInterval=%s, reportInterval=%s, checkInterval=%s, tryTimeInterval=%s, minTimeoutInvoke=%s, frequenceFailInvoke=%s, frequenceFailRadio=%s, tcpNoDelay=%s, charsetName=%s, directConnection=%s]", communicatorId, locator, stat, objectName, simpleObjectName, moduleName, connections, connectTimeout, syncTimeout, asyncTimeout, enableSet, setDivision, refreshInterval, reportInterval, checkInterval, tryTimeInterval, minTimeoutInvoke, frequenceFailInvoke, frequenceFailRadio, tcpNoDelay, charsetName, directConnection);
    }
}
