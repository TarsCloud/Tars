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

package com.qq.tars.web.controller.config;

import com.qq.common.ServletRequestParameterException;
import com.qq.common.WrappedController;
import com.qq.tars.common.support.Holder;
import com.qq.tars.entity.ConfigFile;
import com.qq.tars.entity.ConfigRef;
import com.qq.tars.entity.ConfigRefExt;
import com.qq.tars.service.SetTriple;
import com.qq.tars.service.admin.AdminService;
import com.qq.tars.service.admin.CommandResult;
import com.qq.tars.service.admin.CommandTarget;
import com.qq.tars.service.config.AddConfigFile;
import com.qq.tars.service.config.ConfigService;
import com.qq.tars.service.config.UpdateConfigFile;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

//import com.qq.tars.entity.ConfigFile;

@Controller
public class ConfigController extends WrappedController {

    @Autowired
    private ConfigService configService;

    @Autowired
    private AdminService adminService;

    @RequestMapping(
            value = "server/api/unused_config_file_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ConfigFileView> getUnusedApplicationConfigFile(@RequestParam String application,
                                                               @RequestParam("config_id") long configId) throws Exception {
        return configService.getUnusedApplicationConfigFile(application, configId).stream()
                .map(config -> mapper.map(config, ConfigFileView.class))
                .collect(Collectors.toList());
    }


    @RequestMapping(
            value = "server/api/config_file_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ConfigFileView> configFileList(@RequestParam int level,
                                               @RequestParam String application,
                                               @RequestParam(value = "server_name", required = false) String serverName,
                                               @RequestParam(value = "set_name", required = false) String setName,
                                               @RequestParam(value = "set_area", required = false) String setArea,
                                               @RequestParam(value = "set_group", required = false) String setGroup) throws Exception {
        List<ConfigFile> rtnValue;
        switch (level) {
            case 1:
                if (StringUtils.isBlank(application)) {
                    throw new ServletRequestParameterException("参数application为空");
                }
                rtnValue = configService.getApplicationConfigFile(application);
                break;
            case 2:
                if (StringUtils.isAnyBlank(application, setName)) {
                    throw new ServletRequestParameterException("参数application或set_name为空");
                }
                rtnValue = configService.getSetConfigFile(application, setName, null, null);
                break;
            case 3:
                if (StringUtils.isAnyBlank(application, setName, setArea)) {
                    throw new ServletRequestParameterException("参数application、set_name或set_area为空");
                }
                rtnValue = configService.getSetConfigFile(application, setName, setArea, null);
                break;
            case 4:
                if (StringUtils.isAnyBlank(application, setName, setArea, setGroup)) {
                    throw new ServletRequestParameterException("参数application、set_name、set_area或set_group为空");
                }
                rtnValue = configService.getSetConfigFile(application, setName, setArea, setGroup);
                break;
            case 5:
                if (StringUtils.isAnyBlank(application, serverName)) {
                    throw new ServletRequestParameterException("参数application或server_name为空");
                }
                try {
                    SetTriple.formatSet(setName, setArea, setGroup);
                } catch (IllegalArgumentException e) {
                    setName = null;
                    setArea = null;
                    setGroup = null;
                }
                rtnValue = configService.getServerConfigFile(application, serverName, setName, setArea, setGroup);
                break;
            default:
                throw new ServletRequestParameterException();
        }

        return rtnValue.stream()
                .map(config -> mapper.map(config, ConfigFileView.class))
                .collect(Collectors.toList());
    }

    @RequestMapping(
            value = "server/api/node_config_file_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ConfigFileView> nodeConfigFileList(@RequestParam String application,
                                                   @RequestParam(value = "server_name", required = false) String serverName,
                                                   @RequestParam(value = "set_name", required = false) String setName,
                                                   @RequestParam(value = "set_area", required = false) String setArea,
                                                   @RequestParam(value = "set_group", required = false) String setGroup,
                                                   @RequestParam("config_id") long configId) throws Exception {
        if (StringUtils.isAnyBlank(application, serverName)) {
            throw new ServletRequestParameterException("参数application或server_name为空");
        }
        try {
            SetTriple.formatSet(setName, setArea, setGroup);
        } catch (IllegalArgumentException e) {
            setName = null;
            setArea = null;
            setGroup = null;
        }
        return configService.getNodeConfigFile(application, serverName, setName, setArea, setGroup, configId).stream()
                .map(config -> mapper.map(config, ConfigFileView.class))
                .collect(Collectors.toList());
    }

    @RequestMapping(
            value = "server/api/add_config_file",
            produces = {"application/json"}
    )
    @ResponseBody
    public ConfigFileView addConfigFile(@RequestBody AddConfigFile params) throws Exception {
        return mapper.map(configService.addConfigFile(params), ConfigFileView.class);
    }

    @RequestMapping(
            value = "server/api/delete_config_file",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<Long> deleteConfigFile(@RequestParam long id) throws Exception {
        long result = configService.deleteConfigFile(id);
        return new LinkedList<Long>() {{
            add(result);
        }};
    }

    @RequestMapping(
            value = "server/api/update_config_file",
            produces = {"application/json"}
    )
    @ResponseBody
    public ConfigFileView updateConfigFile(@RequestBody UpdateConfigFile params) throws Exception {
        return mapper.map(configService.updateConfigFile(params), ConfigFileView.class);
    }

    @RequestMapping(
            value = "server/api/config_file",
            produces = {"application/json"}
    )
    @ResponseBody
    public ConfigFileView configFile(@RequestParam("id") long id) throws Exception {
        return mapper.map(configService.loadConfigFile(id), ConfigFileView.class);
    }

    @RequestMapping(
            value = "server/api/config_file_history",
            produces = {"application/json"}
    )
    @ResponseBody
    public ConfigFileHistoryView loadConfigFileHistory(@RequestParam("id") long id) throws Exception {
        return mapper.map(configService.loadConfigFileHistory(id), ConfigFileHistoryView.class);
    }

    @RequestMapping(
            value = "server/api/config_file_history_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ConfigFileHistoryView> configFileHistoryList(@RequestParam("config_id") long configId,
                                                             @RequestParam(value = "cur_page", required = false, defaultValue = "0") int curPage,
                                                             @RequestParam(value = "page_size", required = false, defaultValue = "0") int pageSize) throws Exception {
        return configService.getConfigFileHistory(configId, curPage, pageSize).stream()
                .map(config -> mapper.map(config, ConfigFileHistoryView.class))
                .collect(Collectors.toList());
    }

    @RequestMapping(
            value = "server/api/add_config_ref",
            produces = {"application/json"}
    )
    @ResponseBody
    public ConfigRef addConfigRef(@RequestParam("config_id") long configId,
                                  @RequestParam("reference_id") long referenceId) throws Exception {
        return configService.addConfigRef(configId, referenceId);
    }

    @RequestMapping(
            value = "server/api/delete_config_ref",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<Long> deleteConfigRef(@RequestParam long id) throws Exception {
        long result = configService.deleteConfigRef(id);
        return new LinkedList<Long>() {{
            add(result);
        }};
    }

    @RequestMapping(
            value = "server/api/config_ref_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ConfigRefExt> configRefList(@RequestParam("config_id") long configId) throws Exception {
        return configService.getConfigRefByConfigId(configId);
    }

    @RequestMapping(
            value = "server/api/merged_node_config",
            produces = {"application/json"}
    )
    @ResponseBody
    public String mergedNodeConfig(@RequestParam long id) throws Exception {
        ConfigFile config = configService.loadConfigFile(id);
        return adminService.loadConfigByHost(config.getServerName(), config.getFilename(), config.getHost());
    }

    @RequestMapping(
            value = "server/api/push_config_file",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<CommandResult> pushConfigFile(@RequestParam("config_ids") String configIds) throws Exception {
        List<Long> ids = Arrays.stream(configIds.split(";"))
                .filter(StringUtils::isNotBlank)
                .map(Long::parseLong)
                .collect(Collectors.toList());

        Holder<String> holder = new Holder<>();
        List<CommandTarget> targets = configService.loadConfigFiles(ids).stream()
                .map(configFile -> {
                    holder.setValue(configFile.getFilename());
                    String[] tokens = configFile.getServerName().split("\\.");
                    return new CommandTarget(tokens[0], tokens[1], configFile.getHost());
                }).collect(Collectors.toList());

        String command = String.format("tars.loadconfig %s", holder.getValue());
        return adminService.doCommand(targets, command);
    }

}
