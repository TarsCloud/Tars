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

package com.qq.tars.service;

import com.qq.tars.db.NotifyMapper;
import com.qq.tars.entity.ServerConf;
import com.qq.tars.entity.ServerNotify;
import com.qq.tars.service.server.ServerService;
import org.apache.ibatis.session.RowBounds;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

@Service
public class NotifyService {

    @Autowired
    private ServerService serverService;

    @Autowired
    private NotifyMapper notifyMapper;

    public List<ServerNotify> getServerNotifyList(String treeNodeId, int curPage, int pageSize) {
        List<ServerConf> serverConfs = serverService.getServerConf(treeNodeId, 0, 0);

        // 找出所有serverid
        Set<String> serverids = new HashSet<>();
        serverConfs.forEach(serverConf ->
                serverids.add(String.format("%s.%s_%s",
                        serverConf.getApplication(), serverConf.getServerName(), serverConf.getNodeName()))
        );
        return notifyMapper.getServerNotify(serverids, new RowBounds(curPage, pageSize));
    }


}
