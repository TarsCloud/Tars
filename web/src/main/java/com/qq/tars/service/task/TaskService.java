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

package com.qq.tars.service.task;

import com.qq.tars.db.TaskMapper;
import com.qq.tars.entity.ServerConf;
import com.qq.tars.entity.Task;
import com.qq.tars.exception.TARSRequestException;
import com.qq.tars.generated.tars.TaskItemReq;
import com.qq.tars.generated.tars.TaskReq;
import com.qq.tars.generated.tars.TaskRsp;
import com.qq.tars.service.admin.AdminService;
import com.qq.tars.service.server.ServerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.stream.Collectors;

@Service
public class TaskService {

    @Autowired
    private AdminService adminService;

    @Autowired
    private TaskMapper taskMapper;

    @Autowired
    private ServerService serverService;

    private String generateUniqId() {
        return UUID.randomUUID().toString().replace("-", "");
    }

    public String addTask(AddTask addTask) throws Exception {
        TaskReq taskReq = new TaskReq();
        taskReq.setTaskNo(generateUniqId());
        taskReq.setTaskItemReq(new ArrayList<>());

        addTask.getItems().forEach(item -> {
            TaskItemReq taskItemReq = new TaskItemReq();
            taskReq.getTaskItemReq().add(taskItemReq);
            taskItemReq.setTaskNo(taskReq.getTaskNo());
            taskItemReq.setItemNo(generateUniqId());
            taskItemReq.setCommand(item.getCommand());
            Map<String, String> parameters = item.getParameters();
            if (null != parameters && parameters.containsKey("bak_flag")) {
                parameters.put("bak_flag", "true".equals(parameters.get("bak_flag")) ? "1" : "0");
            }
            taskItemReq.setParameters(parameters);

            ServerConf serverConf = serverService.loadServerConf(item.getServerId());
            taskItemReq.setApplication(serverConf.getApplication());
            taskItemReq.setServerName(serverConf.getServerName());
            taskItemReq.setNodeName(serverConf.getNodeName());
        });

        adminService.addTask(taskReq);
        return taskReq.getTaskNo();
    }

    public TaskResp getTaskResp(String taskNo) throws TARSRequestException {
        TaskRsp rsp = adminService.getTaskRsp(taskNo);

        TaskResp resp = new TaskResp();
        resp.setTaskNo(rsp.getTaskNo());
        resp.setSerial(rsp.getSerial());
        resp.setStatus(rsp.getStatus());
        resp.setItems(rsp.getTaskItemRsp().stream()
                .map(itemRsp -> {
                    TaskItemResp itemResp = new TaskItemResp();
                    itemResp.setItemNo(itemRsp.req.getItemNo());
                    itemResp.setTaskNo(itemRsp.req.getTaskNo());
                    itemResp.setApplication(itemRsp.req.getApplication());
                    itemResp.setServerName(itemRsp.req.getServerName());
                    itemResp.setNodeName(itemRsp.req.getNodeName());
                    itemResp.setCommand(itemRsp.req.getCommand());
                    itemResp.setParameters(itemRsp.req.getParameters());
                    itemResp.setStatus(itemRsp.getStatus());
                    itemResp.setStartTime(itemRsp.getStartTime());
                    itemResp.setEndTime(itemRsp.getEndTime());
                    itemResp.setExecuteLog(itemRsp.getExecuteLog());
                    itemResp.setStatusInfo(itemRsp.statusInfo);
                    return itemResp;
                })
                .collect(Collectors.toList()));

        return resp;
    }

    public List<Task> getTasks(String application, String serverName, String command, String from, String to) {
        return taskMapper.getTask(application, serverName, command, from, to);
    }

}
