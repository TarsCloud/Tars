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

import com.qq.common.ServletRequestParameterException;
import com.qq.common.WrappedController;
import com.qq.tars.service.SetTriple;
import com.qq.tars.service.expand.ExpandPreviewServer;
import com.qq.tars.service.expand.ExpandServer;
import com.qq.tars.service.expand.ExpandServerPreview;
import com.qq.tars.service.expand.ExpandServerService;
import com.qq.tars.web.controller.server.ServerConfView;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.stream.Collectors;

@Controller
public class ExpandController extends WrappedController {

    @Autowired
    private ExpandServerService expandServerService;

    @RequestMapping(
            value = "server/api/cascade_select_server",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<String> select(@RequestParam int level,
                               @RequestParam(required = false) String application,
                               @RequestParam(value = "server_name", required = false) String serverName,
                               @RequestParam(required = false) String set) throws Exception {
        switch (level) {
            case 1:
                return expandServerService.getApplication();
            case 2:
                if (StringUtils.isBlank(application)) {
                    throw new ServletRequestParameterException("参数application为空");
                }
                return expandServerService.getServerName(application);
            case 3:
                if (StringUtils.isAnyBlank(application, serverName)) {
                    throw new ServletRequestParameterException("参数application或server_name为空");
                }
                return expandServerService.getSet(application, serverName);
            case 4:
                if (StringUtils.isAnyBlank(application, serverName)) {
                    throw new ServletRequestParameterException("参数application或server_name为空");
                }

                if (StringUtils.isBlank(set)) {
                    set = StringUtils.EMPTY;
                } else {
                    try {
                        SetTriple.parseSet(set);
                    } catch (IllegalArgumentException e) {
                        throw new ServletRequestParameterException("参数set格式错误");
                    }
                }
                return expandServerService.getNodeName(application, serverName, set);
            default:
                throw new ServletRequestParameterException();
        }
    }

    @RequestMapping(
            value = "server/api/expand_server_preview",
            method = {RequestMethod.POST},
            produces = {"application/json"}
    )
    @ResponseBody
    @Transactional(rollbackFor = {Exception.class})
    public List<ExpandPreviewServer> expandServerPreview(@RequestBody ExpandServerPreview expandServerPreview) throws Exception {
        return expandServerService.preview(expandServerPreview);
    }

    @RequestMapping(
            value = "server/api/expand_server",
            method = {RequestMethod.POST},
            produces = {"application/json"}
    )
    @ResponseBody
    @Transactional(rollbackFor = {Exception.class})
    public List<ServerConfView> expandServer(@RequestBody ExpandServer expandServer) throws Exception {
        return expandServerService.expand(expandServer)
                .stream()
                .map(server -> mapper.map(server, ServerConfView.class))
                .collect(Collectors.toList());
    }
}
