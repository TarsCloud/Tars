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

package com.qq.tars.support.admin.impl;

import java.util.Map.Entry;

import com.qq.tars.client.CommunicatorConfig;
import com.qq.tars.common.util.DyeingKeyCache;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.server.ServerVersion;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.support.admin.AdminFServant;
import com.qq.tars.support.admin.CommandHandler;
import com.qq.tars.support.admin.CustemCommandHelper;
import com.qq.tars.support.config.ConfigHelper;
import com.qq.tars.support.node.NodeHelper;
import com.qq.tars.support.notify.NotifyHelper;
import com.qq.tars.support.om.OmConstants;
import com.qq.tars.support.om.OmLogger;

public class AdminFServantImpl implements AdminFServant {

    private static final String CMD_LOAD_CONFIG = "tars.loadconfig";

    private static final String CMD_LOAD_LOCATOR = "tars.loadproperty";

    private static final String CMD_VIEW_VERSION = "tars.viewversion";

    private static final String CMD_SET_LEVEL = "tars.setloglevel";

    private static final String CMD_VIEW_CONN = "tars.connection";

    private static final String CMD_VIEW_STATUS = "tars.viewstatus";
    
    private static final String CMD_SET_DYEING = "tars.setdyeing";

    private static final String ADATER_CONN = "[adater:%sAdapter] [connections:%d]\n";

    @Override
    public void shutdown() {
        try {
            System.out.println(ConfigurationManager.getInstance().getServerConfig().getApplication() + "." + ConfigurationManager.getInstance().getServerConfig().getServerName() + " is stopped.");
            NotifyHelper.getInstance().syncReport("[alarm] server is stopped.");
        } catch (Exception e) {
            OmLogger.record("shutdown error", e);
        }

        System.exit(0);
    }

    @Override
    public String notify(String command) {
        String params = "";
        String comm = command;
        if (command == null) {
        	return "command is null";
        }
        int i = command.indexOf(" ");
        if (i != -1) {
            comm = command.substring(0, i);
            params = command.substring(i + 1);
        }

        StringBuilder result = new StringBuilder();
        result.append("\n");

        if (CMD_VIEW_STATUS.equals(comm)) {
            result.append(viewStatus() + "\n");
        } else if (CMD_VIEW_CONN.equals(comm)) {
            result.append(viewConn() + "\n");
        } else if (CMD_SET_LEVEL.equals(comm)) {
            result.append(setLoggerLevel(params) + "\n");
        } else if (CMD_LOAD_CONFIG.equals(comm)) {
            result.append(loadConfig(params) + "\n");
        } else if (CMD_LOAD_LOCATOR.equals(comm)) {
            result.append(loadLocator() + "\n");
        } else if (CMD_VIEW_VERSION.equals(comm)) {
            result.append(reportServerVersion() + "\n");
        } else if (CMD_SET_DYEING.equals(comm)) {
        	result.append(loadDyeing(params) + "\n");
        } else {
            final CommandHandler handler = CustemCommandHelper.getInstance().getCommandHandler(comm);
            final String cmdName = comm;
            final String cmdParam = params;

            if (handler != null) {
                Thread handleThread = new Thread(new Runnable() {

                    @Override
                    public void run() {
                        handler.handle(cmdName, cmdParam);
                    }
                });

                handleThread.start();

                result.append("custem command: cmdName=" + cmdName + ", params=" + cmdParam + "\n");
            } else {
                result.append("invalid command.\n");
            }
        }

        NotifyHelper.getInstance().syncReport(command);
        return result.toString();
    }

    private String setLoggerLevel(String level) {
        String result = null;

        if (StringUtils.isEmpty(level)) {
            result = "set log level failed, level is empty";
        } else {
            level = level.trim();
            com.qq.tars.support.log.LoggerFactory.setDefaultLoggerLevel(level);
            com.qq.tars.support.log.LogConfCacheMngr.getInstance().updateLevel(level);

            result = "set log level [" + level + "] ok";
        }

        return result;
    }

    private String viewConn() {
        StringBuilder builder = new StringBuilder(128);
        String adminConnInfo = String.format(ADATER_CONN, "Admin", 128);
        builder.append(adminConnInfo);

        for (Entry<String, ServantAdapterConfig> adapterConfigEntry : ConfigurationManager.getInstance().getServerConfig().getServantAdapterConfMap().entrySet()) {
            if (OmConstants.AdminServant.equals(adapterConfigEntry.getKey())) {
                continue;
            }
            String adapterConnInfo = String.format(ADATER_CONN, adapterConfigEntry.getKey(), adapterConfigEntry.getValue().getMaxConns());
            builder.append(adapterConnInfo);
        }
        return builder.toString();
    }

    private String viewStatus() {
        StringBuilder builder = new StringBuilder(2048);
        String proxyConfigInfo = makeProxyConfigInfo();
        String serverConfigInfo = makeServerConfigInfo();
        String adapterConfigInfo = makeAdapterConfigInfo();

        builder.append(proxyConfigInfo).append("--------------------------------------------------\n");
        builder.append(serverConfigInfo).append("--------------------------------------------------\n");
        builder.append(adapterConfigInfo);
        return builder.toString();
    }

    private String makeAdapterConfigInfo() {
        StringBuilder builder = new StringBuilder(1024);

        builder.append("name \t AdminAdapter\n");
        builder.append("servant \t AdminObj\n");
        builder.append("endpoint \t tcp -h 127.0.0.1 -p " + ConfigurationManager.getInstance().getServerConfig().getLocalPort() + " -t 3000" + "\n");
        builder.append("maxconns \t 128\n");
        builder.append("queuecap \t 128\n");
        builder.append("queuetimeout \t 3000\n");
        builder.append("connections \t 128\n");
        builder.append("protocol \t tars\n");
        builder.append("handlegroup \t AdminAdapter\n");
        builder.append("handlethread  \t 1\n");

        builder.append("--------------------------------------------------\n");

        for (Entry<String, ServantAdapterConfig> adapterConfigEntry : ConfigurationManager.getInstance().getServerConfig().getServantAdapterConfMap().entrySet()) {
            if (OmConstants.AdminServant.equals(adapterConfigEntry.getKey())) {
                continue;
            }
            ServantAdapterConfig adapterConfig = adapterConfigEntry.getValue();

            builder.append("name \t" + adapterConfigEntry.getKey() + "Adapter\n");
            builder.append("servant \t" + adapterConfig.getServant() + "\n");
            builder.append("endpoint \t" + adapterConfig.getEndpoint() + "\n");
            builder.append("maxconns \t" + adapterConfig.getMaxConns() + "\n");
            builder.append("queuecap \t" + adapterConfig.getQueueCap() + "\n");
            builder.append("queuetimeout \t" + adapterConfig.getQueueTimeout() + "\n");
            builder.append("connections \t" + adapterConfig.getMaxConns() + "\n");
            builder.append("protocol \t" + adapterConfig.getProtocol() + "\n");
            builder.append("handlegroup \t" + adapterConfigEntry.getKey() + "\n");
            builder.append("handlethread  \t" + adapterConfig.getThreads() + "\n");

            builder.append("--------------------------------------------------\n");
        }

        return builder.toString();
    }

    private String makeServerConfigInfo() {
        StringBuilder builder = new StringBuilder(1024);
        ServerConfig serverConfig = ConfigurationManager.getInstance().getServerConfig();
        builder.append("[server config]:\n");
        builder.append("Application \t" + serverConfig.getApplication() + "\n");
        builder.append("ServerName \t" + serverConfig.getServerName() + "\n");
        builder.append("BasePath \t" + serverConfig.getBasePath() + "\n");
        builder.append("DataPath \t" + serverConfig.getDataPath() + "\n");
        builder.append("LocalIp \t" + serverConfig.getLocalIP() + "\n");
        builder.append("Local \ttcp -h 127.0.0.1 -p " + serverConfig.getLocalPort() + " -t 3000" + "\n");
        builder.append("LogPath \t" + serverConfig.getLogPath() + "\n");
        builder.append("Log \t" + serverConfig.getLog() + "\n");
        builder.append("Node \t" + serverConfig.getNode() + "\n");
        builder.append("Config \t" + serverConfig.getConfig() + "\n");
        builder.append("Notify \t" + serverConfig.getNotify() + "\n");
        builder.append("logLevel \t" + serverConfig.getLogLevel() + "\n");

        return builder.toString();
    }

    private String makeProxyConfigInfo() {
        StringBuilder builder = new StringBuilder(1024);
        CommunicatorConfig commConfig = ConfigurationManager.getInstance().getServerConfig().getCommunicatorConfig();
        builder.append("[proxy config]:\n");
        builder.append("locator \t" + commConfig.getLocator() + "\n");
        builder.append("sync-invoke-timeout \t" + commConfig.getSyncInvokeTimeout() + "\n");
        builder.append("async-invoke-timeout \t" + commConfig.getAsyncInvokeTimeout() + "\n");
        builder.append("refresh-endpoint-interval \t" + commConfig.getRefreshEndpointInterval() + "\n");
        builder.append("stat \t" + commConfig.getStat() + "\n");
        builder.append("property \t" + commConfig.getProperty() + "\n");
        builder.append("report-interval \t" + commConfig.getReportInterval() + "\n");
        builder.append("sample-rate \t" + commConfig.getSampleRate() + "\n");
        builder.append("max-sample-count \t" + commConfig.getMaxSampleCount() + "\n");
        builder.append("recvthread \t" + commConfig.getRecvThread() + "\n");
        builder.append("asyncthread \t" + commConfig.getAsyncThread() + "\n");
        builder.append("modulename \t" + commConfig.getModuleName() + "\n");
        builder.append("enableset \t" + commConfig.isEnableSet() + "\n");
        builder.append("setdivision \t" + commConfig.getSetDivision() + "\n");

        return builder.toString();
    }

    private String reportServerVersion() {
        String version = ServerVersion.getVersion();
        NodeHelper.getInstance().reportVersion(version);
        return version;
    }

    private String loadLocator() {
        String result = null;

        try {
            result = "execute success.";
            OmLogger.record("Reload locator success.");
        } catch (Exception e) {
            OmLogger.record("Reload locator failed.", e);
            result = "execute exception: " + e.getMessage();
        }

        return result;
    }

    private String loadConfig(String params) {
		if (params == null) {
			return "invalid params";
		}
        String fileName = params.trim();
        if (StringUtils.isEmpty(fileName)) {
            return "invalid params.";
        }

        String result = null;
        try {
            boolean isSucc = ConfigHelper.getInstance().loadConfig(fileName);
            if (isSucc) {
                result = "execute success.";
            } else {
                result = "execute failed.";
            }
        } catch (Exception e) {
            result = "execute exception: " + e.getMessage();
        }

        return result;
    }
    
    private String loadDyeing(String params) {
		String result = null;
		if (params == null) {
			return "invalid params";
		}
		String[] paramArray = params.split(" ");
		if (paramArray.length < 2) {
			return "invalid params";
		}
		try {
			String routeKey = paramArray[0];
			String servantName = paramArray[1];
			String interfaceName = (paramArray.length >= 3) ? paramArray[2] : "DyeingAllFunctionsFromInterface";
			DyeingKeyCache.getInstance().set(servantName, interfaceName, routeKey);
			result = "execute success";
		} catch (Exception e) {
			result = "execute exception: " + e.getMessage();
		}
		return result;
	}

}
