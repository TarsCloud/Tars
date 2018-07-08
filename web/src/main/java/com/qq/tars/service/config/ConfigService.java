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

package com.qq.tars.service.config;

import com.google.common.base.Preconditions;
import com.qq.tars.db.ConfigMapper;
import com.qq.tars.db.ServerMapper;
import com.qq.tars.entity.*;
import com.qq.tars.exception.DBConsistencyException;
import com.qq.tars.tools.DateTime;
import org.apache.commons.lang3.StringUtils;
import org.apache.ibatis.session.RowBounds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@Service
public class ConfigService {

    private final Logger log = LoggerFactory.getLogger(ConfigService.class);

    @Autowired
    private ConfigMapper configMapper;

    @Autowired
    private ServerMapper serverMapper;

    @Transactional(rollbackFor = Exception.class)
    public ConfigFile addConfigFile(AddConfigFile params) {
        int level = params.getLevel();
        Preconditions.checkState(level >= 1 && level <= 5);

        ConfigFile configFile = new ConfigFile();
        configFile.setLevel(level == 5 ? 2 : 1);

        String server;
        if (configFile.getLevel() == 1) {
            Preconditions.checkState(StringUtils.isNoneBlank(params.getApplication()));
            server = params.getApplication();
        } else {
            Preconditions.checkState(StringUtils.isNoneBlank(params.getApplication(), params.getServerName()));
            server = String.format("%s.%s", params.getApplication(), params.getServerName());
        }
        configFile.setServerName(server);

        if (StringUtils.isNotBlank(params.getSetName())) {
            configFile.setSetName(params.getSetName());
        }
        if (StringUtils.isNotBlank(params.getSetArea())) {
            configFile.setSetArea(params.getSetArea());

        }
        if (StringUtils.isNotBlank(params.getSetGroup())) {
            configFile.setSetGroup(params.getSetGroup());
        }

        Preconditions.checkState(StringUtils.isNotBlank(params.getFilename()));
        configFile.setFilename(params.getFilename());

        configFile.setConfig(StringUtils.trimToEmpty(params.getConfig()));

        configFile.setPosttime(LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")));

        configMapper.insertConfigFile(configFile);

        ConfigFileHistory history = new ConfigFileHistory();
        history.setConfigId(configFile.getId());
        history.setReason("创建配置文件");
        history.setContent(configFile.getConfig());
        history.setPosttime(configFile.getPosttime());
        configMapper.insertConfigFileHistory(history);

        // 插入默认节点配置
        if (configFile.getLevel() == 2) {
            boolean enableSet = StringUtils.isNoneBlank(params.getSetName(), params.getSetArea(), params.getSetGroup());
            addDefaultNodeConfigFile(params.getApplication(), params.getServerName(),
                    enableSet, params.getSetName(), params.getSetArea(), params.getSetGroup(),
                    params.getFilename());
        }

        return configFile;
    }

    @Transactional(rollbackFor = Exception.class)
    public void addDefaultNodeConfigFile(String application, String serverName, String nodeName,
                                         boolean enableSet, String setName, String setArea, String setGroup) {
        if (!enableSet) {
            setName = null;
            setArea = null;
            setGroup = null;
        }
        List<ConfigFile> configs = configMapper.getServerConfigFile(application, serverName, setName, setArea, setGroup);
        configs.forEach(config -> {
            ConfigFile newone = new ConfigFile();
            BeanUtils.copyProperties(config, newone, "id", "posttime", "lastuser", "level");
            newone.setHost(nodeName);
            newone.setLevel(3);
            newone.setPosttime(DateTime.now());
            configMapper.insertConfigFile(newone);

            ConfigFileHistory history = new ConfigFileHistory();
            history.setConfigId(newone.getId());
            history.setReason("创建配置文件");
            history.setContent(newone.getConfig());
            history.setPosttime(newone.getPosttime());
            configMapper.insertConfigFileHistory(history);
        });
    }

    @Transactional(rollbackFor = Exception.class)
    public void addDefaultNodeConfigFile(String application, String serverName,
                                         boolean enableSet, String setName, String setArea, String setGroup,
                                         String filename) {
        List<ServerConf> servers = serverMapper.getServerConf(application, serverName,
                enableSet, setName, setArea, setGroup,
                new RowBounds(0, 0));
        servers.forEach(server -> {
            ConfigFile newone = new ConfigFile();
            newone.setServerName(String.format("%s.%s", application, serverName));
            if (enableSet) {
                newone.setSetName(setName);
                newone.setSetArea(setArea);
                newone.setSetGroup(setGroup);
            }
            newone.setFilename(filename);
            newone.setConfig("");
            newone.setHost(server.getNodeName());
            newone.setLevel(3);
            newone.setPosttime(DateTime.now());
            configMapper.insertConfigFile(newone);

            ConfigFileHistory history = new ConfigFileHistory();
            history.setConfigId(newone.getId());
            history.setReason("创建配置文件");
            history.setContent(newone.getConfig());
            history.setPosttime(newone.getPosttime());
            configMapper.insertConfigFileHistory(history);
        });
    }

    @Transactional(rollbackFor = Exception.class)
    public long deleteConfigFile(long id) throws DBConsistencyException {
        List<ConfigRef> refs = configMapper.getConfigRefByRefId(id);
        if (refs.size() > 0) {
            throw new DBConsistencyException("被其它配置文件引用，不能删除");
        }
        ConfigFile configFile = configMapper.loadConfigFile(id);

        // 删除同名节点配置
        if (configFile.getLevel() == 2) {
            String[] tokens = StringUtils.split(configFile.getServerName(), "\\.");
            configMapper.getNodeConfigFile(tokens[0], tokens[1],
                    configFile.getSetName(), configFile.getSetArea(), configFile.getSetGroup())
                    .stream()
                    .filter(config -> configFile.getFilename().equals(config.getFilename()))
                    .forEach(config -> configMapper.deleteConfigFile(config));
        }

        configMapper.deleteConfigFile(configFile);
        return configFile.getId();
    }

    @Transactional(rollbackFor = Exception.class)
    public int deleteUnusedNodeConfigFiles() {
        int affected = configMapper.deleteUnusedNodeConfigFile();
        log.info("delete {} unused node config file(s)", affected);
        return affected;
    }

    @Transactional(rollbackFor = Exception.class)
    public ConfigFile updateConfigFile(UpdateConfigFile params) {
        ConfigFile configFile = configMapper.loadConfigFile(params.getId());
        configFile.setConfig(params.getConfig());
        configFile.setPosttime(LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")));
        configMapper.updateConfigFile(configFile);

        ConfigFileHistory history = new ConfigFileHistory();
        history.setConfigId(configFile.getId());
        history.setReason(params.getReason());
        history.setContent(configFile.getConfig());
        history.setPosttime(configFile.getPosttime());
        configMapper.insertConfigFileHistory(history);

        return configFile;
    }

    public List<ConfigFile> getApplicationConfigFile(String application) {
        return configMapper.getApplicationConfigFile(application);
    }

    public List<ConfigFile> getUnusedApplicationConfigFile(String application, long configId) {
        return configMapper.getUnusedApplicationConfigFile(application, configId);
    }

    public List<ConfigFile> getSetConfigFile(String application, String setName, String setArea, String setGroup) {
        return configMapper.getSetConfigFile(application, setName, setArea, setGroup);
    }

    public List<ConfigFile> getServerConfigFile(String application, String serverName,
                                                String setName, String setArea, String setGroup) {
        return configMapper.getServerConfigFile(application, serverName, setName, setArea, setGroup);
    }

    public List<ConfigFile> getNodeConfigFile(String application, String serverName,
                                              String setName, String setArea, String setGroup, long configId) {
        boolean enableSet = StringUtils.isNoneBlank(setName, setArea, setGroup);
        List<ServerConf> servers = serverMapper.getServerConf(application, serverName,
                enableSet, setName, setArea, setGroup, new RowBounds(0, 0));

        ConfigFile configFile = loadConfigFile(configId);
        Map<String, ConfigFile> map = configMapper.getNodeConfigFile(application, serverName, setName, setArea, setGroup)
                .stream()
                .filter(config -> config.getFilename().equals(configFile.getFilename()))
                .collect(Collectors.toMap(config -> String.format("%s.%s.%s.%s_%s", config.getServerName(),
                        config.getSetName(), config.getSetArea(), config.getSetGroup(),
                        config.getHost()), config -> config));

        List<ConfigFile> exists = new ArrayList<>(map.values());
        servers.stream()
                .filter(server -> {
                    String key = String.format("%s.%s.%s.%s.%s_%s", application, serverName,
                            enableSet ? setName : StringUtils.EMPTY,
                            enableSet ? setArea : StringUtils.EMPTY,
                            enableSet ? setGroup : StringUtils.EMPTY,
                            server.getNodeName());
                    return !map.containsKey(key);
                })
                .forEach(server -> {
                    ConfigFile newone = new ConfigFile();
                    newone.setServerName(String.format("%s.%s", application, serverName));
                    if(enableSet){
                    newone.setSetName(setName);
                    newone.setSetArea(setArea);
                    newone.setSetGroup(setGroup);
                    }
                    newone.setHost(server.getNodeName());
                    newone.setFilename(configFile.getFilename());
                    newone.setConfig("");
                    newone.setLevel(3);
                    newone.setPosttime(DateTime.now());
                    configMapper.insertConfigFile(newone);

                    log.info("insert default node config file, id={}, server_name={}, host={}, filename={}",
                            newone.getId(), newone.getServerName(), newone.getHost(), newone.getFilename());

                    ConfigFileHistory history = new ConfigFileHistory();
                    history.setConfigId(newone.getId());
                    history.setReason("创建配置文件");
                    history.setContent(newone.getConfig());
                    history.setPosttime(newone.getPosttime());
                    configMapper.insertConfigFileHistory(history);

                    exists.add(newone);
                });

        return exists;
    }

    public List<ConfigFile> loadConfigFiles(List<Long> ids) {
        return configMapper.loadConfigFiles(ids);
    }

    public ConfigFile loadConfigFile(long id) {
        return configMapper.loadConfigFile(id);
    }

    public ConfigFileHistory loadConfigFileHistory(long id) {
        return configMapper.loadConfigFileHistory(id);
    }

    public List<ConfigFileHistory> getConfigFileHistory(long configId, int curPage, int pageSize) {
        return configMapper.getConfigFileHistory(configId, new RowBounds(curPage, pageSize));
    }

    @Transactional(rollbackFor = Exception.class)
    public ConfigRef addConfigRef(long configId, long referenceId) {
        ConfigRef configRef = new ConfigRef();
        configRef.setConfigId(configId);
        configRef.setReferenceId(referenceId);
        configMapper.insertConfigRef(configRef);
        return configRef;
    }

    @Transactional(rollbackFor = Exception.class)
    public long deleteConfigRef(long id) {
        ConfigRef configRef = configMapper.loadConfigRef(id);
        configMapper.deleteConfigRef(configRef);
        return configRef.getId();
    }

    public List<ConfigRefExt> getConfigRefByConfigId(long configId) {
        return configMapper.getConfigRefByConfigId(configId);
    }
}
