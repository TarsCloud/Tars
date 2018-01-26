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

package com.qq.tars.web.controller.server;

import com.qq.common.WrappedController;
import com.qq.tars.entity.ServerConf;
import com.qq.tars.service.admin.AdminService;
import com.qq.tars.service.admin.CommandResult;
import com.qq.tars.service.admin.CommandTarget;
import com.qq.tars.service.server.ServerService;
import com.qq.tars.service.server.UpdateServer;
import com.qq.tars.validate.Application;
import com.qq.tars.validate.ServerName;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.validation.Valid;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.stream.Collectors;

@Validated
@Controller
public class ServerController extends WrappedController {

    private final Logger log = LoggerFactory.getLogger(ServerController.class);

    @Autowired
    private ServerService serverService;

    @Autowired
    private AdminService adminService;

    @RequestMapping(
            value = "server/api/server",
            produces = {"application/json"}
    )
    @ResponseBody
    public ServerConfView loadServerConf(@RequestParam long id) throws Exception {
        return mapper.map(serverService.loadServerConf(id), ServerConfView.class);
    }

    @RequestMapping(
            value = "server/api/server_exist",
            produces = {"application/json"}
    )
    @ResponseBody
    public Boolean loadServerConf(@Application @RequestParam String application,
                                         @ServerName @RequestParam("server_name") String serverName,
                                         @RequestParam("node_name") String nodeName) throws Exception {
        return serverService.loadServerConf(application, serverName, nodeName) != null;
    }

    @RequestMapping(
            value = "server/api/server_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ServerConfView> serverList(@RequestParam(value = "tree_node_id") String treeNodeId,
                                           @RequestParam(value = "cur_page", required = false, defaultValue = "0") int curPage,
                                           @RequestParam(value = "page_size", required = false, defaultValue = "0") int pageSize) throws Exception {
        return serverService.getServerConf(treeNodeId, curPage, pageSize)
                .stream()
                .map(server -> mapper.map(server, ServerConfView.class))
                .collect(Collectors.toList());
    }

    @RequestMapping(
            value = "server/api/inactive_server_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ServerConfView> inactiveServerList(@RequestParam(required = false) String application,
                                                   @RequestParam(value = "server_name", required = false) String serverName,
                                                   @RequestParam(value = "node_name", required = false) String nodeName,
                                                   @RequestParam(value = "cur_page", required = false, defaultValue = "0") int curPage,
                                                   @RequestParam(value = "page_size", required = false, defaultValue = "0") int pageSize) throws Exception {
        application = StringUtils.trimToNull(application);
        serverName = StringUtils.trimToNull(serverName);
        nodeName = StringUtils.trimToNull(nodeName);

        return serverService.queryInactiveServerConfs(application, serverName, nodeName, curPage, pageSize)
                .stream()
                .map(server -> mapper.map(server, ServerConfView.class))
                .collect(Collectors.toList());
    }

    @RequestMapping(
            value = "server/api/get_realtime_state",
            produces = {"application/json"}
    )
    @ResponseBody
    public Object getRealtimeState(@RequestParam long id) throws Exception {
        ServerConf serverConf = serverService.loadServerConf(id);
        return new HashMap<String, String>() {{
            put("realtime_state", serverConf.getPresentState());
        }};
    }


    @RequestMapping(
            value = "server/api/load_server",
            produces = {"application/json"}
    )
    @ResponseBody
    public String loadServer(@RequestParam String application,
                             @RequestParam("server_name") String serverName,
                             @RequestParam("node_name") String nodeName) throws Exception {
        return adminService.loadServer(application, serverName, nodeName);
    }

    @RequestMapping(
            value = "server/api/send_command",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<CommandResult> sendCommand(@RequestParam("server_ids") String serverIds,
                                           @RequestParam String command) throws Exception {
        log.info("servers={}, command={}", serverIds, command);

        List<Long> ids = Arrays.stream(serverIds.split(";"))
                .filter(StringUtils::isNotBlank)
                .map(Long::parseLong)
                .collect(Collectors.toList());

        List<CommandTarget> targets = serverService.loadServerConfs(ids).stream()
                .map(configFile -> mapper.map(configFile, CommandTarget.class))
                .collect(Collectors.toList());

        return adminService.doCommand(targets, command);
    }

    @RequestMapping(
            value = "server/api/update_server",
            produces = {"application/json"}
    )
    @ResponseBody
    public ServerConfView upadateServer(@Valid @RequestBody UpdateServer updateServer) throws Exception {
        ServerConf server = serverService.loadServerConf(updateServer.getId());
        mapper.map(updateServer, server);
        if (updateServer.isBak()) {
            server.setBakFlag(1);
        }
        server.setEnableSet(updateServer.isEnableSet() ? "Y" : "N");
        serverService.updateServerConf(server);
        return mapper.map(serverService.loadServerConf(server.getId()), ServerConfView.class);
    }

}
