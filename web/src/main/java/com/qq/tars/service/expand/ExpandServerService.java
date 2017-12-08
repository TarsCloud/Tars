/*
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

package com.qq.tars.service.expand;

import com.google.common.base.Preconditions;
import com.qq.tars.db.AdapterMapper;
import com.qq.tars.db.ConfigMapper;
import com.qq.tars.db.ServerMapper;
import com.qq.tars.entity.AdapterConf;
import com.qq.tars.entity.ConfigFile;
import com.qq.tars.entity.ServerConf;
import com.qq.tars.service.SetTriple;
import com.qq.tars.tools.DateTime;
import org.apache.commons.collections.map.MultiKeyMap;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

@Service
public class ExpandServerService {

    @Autowired
    private ServerMapper serverMapper;

    @Autowired
    private AdapterMapper adapterMapper;

    @Autowired
    private ConfigMapper configMapper;

    public List<String> getApplication() {
        return serverMapper.getApplication();
    }

    public List<String> getServerName(String application) {
        Preconditions.checkArgument(StringUtils.isNotBlank(application));
        return serverMapper.getServerName(application);
    }

    public List<String> getSet(String application, String serverName) {
        Preconditions.checkArgument(StringUtils.isNoneBlank(application, serverName));
        return serverMapper.getSet(application, serverName);
    }

    public List<String> getNodeName(String application, String serverName, String set) {
        Preconditions.checkArgument(StringUtils.isNoneBlank(application, serverName));

        boolean enableSet = StringUtils.isNotBlank(set);
        String setName = null;
        String setArea = null;
        String setGroup = null;

        if (enableSet) {
            SetTriple setTriple = SetTriple.parseSet(set);
            setName = setTriple.getSetName();
            setArea = setTriple.getSetArea();
            setGroup = setTriple.getSetGroup();
        }

        return serverMapper.getNodeName(application, serverName, enableSet, setName, setArea, setGroup);
    }

    public List<ExpandPreviewServer> preview(ExpandServerPreview expandServerPreview) {
        Preconditions.checkArgument(null != expandServerPreview);

        String application = expandServerPreview.getApplication();
        String serverName = expandServerPreview.getServerName();

        ServerConf sourceServer = serverMapper.loadServerConfByName(application, serverName, expandServerPreview.getNodeName());
        List<AdapterConf> sourceAdapters = adapterMapper.getAdapterConf(application, serverName, expandServerPreview.getNodeName());

        List<ExpandPreviewServer> result = new LinkedList<>();
        for (String expandNode : expandServerPreview.getExpandNodes()) {
            for (AdapterConf adapter : sourceAdapters) {
                ExpandPreviewServer preserver = new ExpandPreviewServer();
                preserver.setApplication(application);
                preserver.setServerName(serverName);
                preserver.setNodeName(expandNode);
                preserver.setTemplateName(sourceServer.getTemplateName());

                if (expandServerPreview.isEnableSet()) {
                    SetTriple setTriple = SetTriple.formatSet(expandServerPreview.getSetName(), expandServerPreview.getSetArea(), expandServerPreview.getSetGroup());
                    preserver.setSet(setTriple.getString());
                } else {
                    preserver.setSet(StringUtils.EMPTY);
                }

                String servant = adapter.getServant();
                preserver.setObjName(servant.substring(servant.lastIndexOf('.') + 1));
                preserver.setBindIp(expandNode);

                preserver.setStatus(expandNode.equals(sourceServer.getNodeName()) ? "已存在" : "未扩容");

                result.add(preserver);
            }
        }

        return result;
    }

    @Transactional(rollbackFor = {Exception.class})
    public List<ServerConf> expand(ExpandServer expandServer) {
        String application = expandServer.getApplication();
        String serverName = expandServer.getServerName();

        ServerConf sourceServer = serverMapper.loadServerConfByName(application, serverName, expandServer.getNodeName());
        MultiKeyMap sourceAdapters = getSoruceAdapterMap(application, serverName, expandServer.getNodeName());

        Map<Long, ServerConf> serverAdded = new HashMap<>();
        for (ExpandPreviewServer preserver : expandServer.getExpandPreviewServers()) {
            ServerConf targetServer = serverMapper.loadServerConfByName(application, serverName, preserver.getNodeName());
            if (null == targetServer) {
                ServerConf server = new ServerConf();
                server.setApplication(application);
                server.setServerName(serverName);
                server.setNodeName(preserver.getNodeName());

                String set = preserver.getSet();
                boolean enableSet = StringUtils.isNotBlank(set);
                server.setEnableSet(enableSet ? "Y" : "N");
                if (enableSet) {
                    SetTriple setTriple = SetTriple.parseSet(set);
                    server.setSetName(setTriple.getSetName());
                    server.setSetArea(setTriple.getSetArea());
                    server.setSetGroup(setTriple.getSetGroup());
                }

                // 复制
                server.setServerType(sourceServer.getServerType());
                server.setTemplateName(sourceServer.getTemplateName());
                server.setBakFlag(sourceServer.getBakFlag());
                server.setBasePath(sourceServer.getBasePath());
                server.setExePath(sourceServer.getExePath());
                server.setStartScriptPath(sourceServer.getStartScriptPath());

                // 私有模板和节点配置
                if (expandServer.isCopyNodeConfig()) {
                    server.setProfile(sourceServer.getProfile());

                    List<ConfigFile> configs = configMapper.getNodeConfigFile(
                            sourceServer.getApplication(),
                            sourceServer.getServerName(),
                            sourceServer.getSetName(),
                            sourceServer.getSetArea(),
                            sourceServer.getSetGroup());

                    configs.stream()
                            .filter(config -> sourceServer.getNodeName().equals(config.getHost()))
                            .forEach(config -> {
                                ConfigFile copy = new ConfigFile();
                                BeanUtils.copyProperties(config, copy);
                                copy.setId(null);
                                copy.setPosttime(DateTime.now());
                                copy.setHost(server.getNodeName());
                                configMapper.insertConfigFile(copy);
                            });
                }

                serverMapper.insertServerConf(server);

                serverAdded.put(server.getId(), server);
            }

            AdapterConf targetAdapter = adapterMapper.loadAdapterConfByObj(application, serverName, preserver.getNodeName(), preserver.getObjName());
            if (null == targetAdapter) {
                AdapterConf sourceAdapter = (AdapterConf) sourceAdapters.get(application, serverName, expandServer.getNodeName(), preserver.getObjName());

                AdapterConf adapter = new AdapterConf();
                adapter.setApplication(application);
                adapter.setServerName(serverName);
                adapter.setNodeName(preserver.getNodeName());

                // 复制
                adapter.setServant(sourceAdapter.getServant());
                adapter.setAdapterName(sourceAdapter.getAdapterName());
                adapter.setThreadNum(sourceAdapter.getThreadNum());
                adapter.setMaxConnections(sourceAdapter.getMaxConnections());
                adapter.setQueuecap(sourceAdapter.getQueuecap());
                adapter.setQueuetimeout(sourceAdapter.getQueuetimeout());

                adapter.setAllowIp(sourceAdapter.getAllowIp());
                adapter.setProtocol(sourceAdapter.getProtocol());
                adapter.setHandlegroup(sourceAdapter.getHandlegroup());

                adapter.setEndpoint(String.format("tcp -h %s -t %s -p %s -e %s",
                        preserver.getBindIp(), sourceAdapter.getQueuetimeout(), preserver.getPort(), preserver.getAuth()));

                adapterMapper.insertAdapterConf(adapter);
            }
        }

        return new LinkedList<>(serverAdded.values());
    }

    private MultiKeyMap getSoruceAdapterMap(String application, String serverName, String nodeName) {
        MultiKeyMap map = new MultiKeyMap();
        adapterMapper.getAdapterConf(application, serverName, nodeName).forEach(adapter -> {
            String servant = adapter.getServant();
            map.put(adapter.getApplication(), adapter.getServerName(), adapter.getNodeName(), servant.substring(servant.lastIndexOf('.') + 1), adapter);
        });
        return map;
    }


}
