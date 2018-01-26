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

package com.qq.tars.support.config;

import java.io.File;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.tars.client.Communicator;
import com.qq.tars.common.support.Holder;
import com.qq.tars.common.util.FileUtil;
import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.support.config.prx.ConfigInfo;
import com.qq.tars.support.config.prx.ConfigPrx;
import com.qq.tars.support.om.OmLogger;

public class ConfigHelper {

    private static final ConfigHelper Instance = new ConfigHelper();
    private Communicator comm;
    private String app;
    private String serverName;
    private String basePath;
    private int maxBakNum;

    private Map<String, UpdateConfigCallback> updateCallbackMap = new ConcurrentHashMap<String, UpdateConfigCallback>();

    private ConfigHelper() {
    }

    public static ConfigHelper getInstance() {
        return Instance;
    }

    public int setConfigInfo(Communicator comm, String app, String serverName, String basePath, int maxBakNum) {
        this.comm = comm;
        this.app = app;
        this.serverName = serverName;
        this.basePath = basePath + "/conf/";
        this.maxBakNum = maxBakNum;
        return 0;
    }

    public int setConfigInfo(Communicator comm, String app, String serverName, String basePath) {
        return setConfigInfo(comm, app, serverName, basePath, 5);
    }

    public boolean registerUpdateCallback(String configFileName, UpdateConfigCallback callback) {
        if (com.qq.tars.common.util.StringUtils.isEmpty(configFileName) || callback == null) {
            return false;
        }

        updateCallbackMap.put(configFileName, callback);
        return true;
    }

    private synchronized boolean loadConfig(String fileName, boolean appConfigOnly) {
        try {
            String fullFileName = getConfFilePath(fileName);
            String newFile = getRemoteFile(fileName, appConfigOnly);
            if (new File(fullFileName).exists() && !FileUtil.readFileToString(fullFileName).equals(FileUtil.readFileToString(newFile))) {
                for (int i = maxBakNum - 1; i >= 1; i--) {
                    String bakFileI = index2file(fullFileName, i);
                    if (new File(bakFileI).canRead()) {
                        localRename(bakFileI, index2file(fullFileName, i + 1));
                    }
                }
                if (new File(fullFileName).canRead()) {
                    localRename(fullFileName, index2file(fullFileName, 1));
                }
            }
            localRename(newFile, fullFileName);
        } catch (TarsException e) {
            OmLogger.record("Config|load config failed: " + fileName, e);
            throw e;
        } catch (Exception e) {
            OmLogger.record("Config|load config failed: " + fileName, e);
            throw new TarsException("Config|load config failed: " + fileName, e);
        }

        return true;
    }

    public boolean loadConfig(String fileName) {
        if (com.qq.tars.common.util.StringUtils.isEmpty(fileName)) {
            return false;
        }

        boolean result = loadConfig(fileName, false);

        if (result) {
            UpdateConfigCallback callback = updateCallbackMap.get(fileName);
            if (callback != null) {
                result = callback.update(fileName);
            }
        }

        return result;
    }

    private String getRemoteFile(String fileName, boolean appConfigOnly) {
        Holder<String> config = new Holder<String>();
        ConfigPrx configPrx = comm.stringToProxy(ConfigPrx.class, ConfigurationManager.getInstance().getServerConfig().getConfig());
        ConfigInfo configInfo = new ConfigInfo(app, appConfigOnly ? "" : serverName, fileName, appConfigOnly, ConfigurationManager.getInstance().getServerConfig().getLocalIP(), ConfigurationManager.getInstance().getServerConfig().getCommunicatorConfig().getSetDivision());
        int ret = configPrx.loadConfigByInfo(configInfo, config);
        String stream = config.value;
        if (ret != 0 || stream == null || stream.length() == 0) {
            throw new TarsException("Config|config file is empty: " + fileName);
        }

        String newFile = getConfFilePath(fileName) + "." + System.currentTimeMillis() / 100;
        FileUtil.writeStringToFile(newFile, stream);
        return newFile;
    }

    private String getConfFilePath(String fileName) {
        return basePath + fileName;
    }

    private String index2file(String fullFileName, int index) {
        return fullFileName + "." + index + ".bak";
    }

    private void localRename(String oldFile, String newFile) {
        File srcFile = new File(oldFile);
        File destFile = new File(newFile);
        boolean ok = (!destFile.exists() || destFile.delete()) && srcFile.renameTo(destFile);
        if (!ok) {
            throw new TarsException("Config|rename file error: " + newFile);
        }
    }

}
