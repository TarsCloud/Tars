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

package com.qq.tars.web.controller;

import com.qq.common.WrappedController;
import com.qq.tars.exception.TARSRequestException;
import com.qq.tars.service.task.AddTask;
import com.qq.tars.service.task.TaskResp;
import com.qq.tars.service.task.TaskService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import java.util.List;
import java.util.stream.Collectors;

@Controller
public class TaskController extends WrappedController {

    private final Logger log = LoggerFactory.getLogger(TaskController.class);

    @Autowired
    private TaskService taskService;

    @RequestMapping(
            value = "server/api/add_task",
            produces = {"application/json"}
    )
    @ResponseBody
    public TaskResp addTask(@RequestBody AddTask addTask) throws Exception {
        String taskNo = taskService.addTask(addTask);
        return taskService.getTaskResp(taskNo);
    }

    @RequestMapping(
            value = "server/api/task",
            produces = {"application/json"}
    )
    @ResponseBody
    public TaskResp getTask(@RequestParam("task_no") String taskNo) throws Exception {
        return taskService.getTaskResp(taskNo);
    }

    @RequestMapping(
            value = "server/api/task_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<TaskResp> getTasks(@RequestParam(required = false) String application,
                                   @RequestParam(value = "server_name", required = false) String serverName,
                                   @RequestParam(required = false) String command,
                                   @RequestParam(required = false) String from,
                                   @RequestParam(required = false) String to) throws Exception {
        return taskService.getTasks(application, serverName, command, from, to)
                .stream()
                .map(task -> {
                    try {
                        return taskService.getTaskResp(task.getTaskNo());
                    } catch (TARSRequestException e) {
                        log.error("get task error, task no={}", task.getTaskNo(), e);

                        // 循环取任务详情，如果失败，则任务状态为特殊的-1，页面显示的时候应该特殊处理
                        TaskResp resp = new TaskResp();
                        resp.setTaskNo(task.getTaskNo());
                        resp.setStatus(-1);
                        return resp;
                    }
                })
                .collect(Collectors.toList());
    }
}
