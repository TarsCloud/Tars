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

package com.qq.tars.web.controller.notify;

import com.google.common.base.Preconditions;
import com.qq.common.ServletRequestParameterException;
import com.qq.common.WrappedController;
import com.qq.tars.service.NotifyService;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import java.util.List;
import java.util.stream.Collectors;

@Controller
public class NotifyController extends WrappedController {

    @Autowired
    private NotifyService notifyService;

    @RequestMapping(
            value = "server/api/server_notify_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ServerNotifyView> serverNotifyList(@RequestParam(value = "tree_node_id") String treeNodeId,
                                                   @RequestParam(value = "cur_page", required = false, defaultValue = "0") int curPage,
                                                   @RequestParam(value = "page_size", required = false, defaultValue = "0") int pageSize) throws Exception {
        try {
            int count = StringUtils.countMatches(treeNodeId, '.');
            Preconditions.checkArgument(count == 1 || count == 4, "参数%s格式错误", "tree_node_id");
        } catch (IllegalArgumentException e) {
            throw new ServletRequestParameterException(e);
        }

        return notifyService.getServerNotifyList(treeNodeId, curPage, pageSize).stream()
                .map(notify -> mapper.map(notify, ServerNotifyView.class))
                .collect(Collectors.toList());
    }
}
