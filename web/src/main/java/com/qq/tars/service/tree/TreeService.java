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

package com.qq.tars.service.tree;

import com.qq.tars.db.ServerMapper;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@Service
public class TreeService {

    private final Logger log = LoggerFactory.getLogger(TreeService.class);

    @Autowired
    private ServerMapper serverMapper;

    public List<TreeNode> getTreeNodes() {
        Map<String, TreeNode> nodes = new HashMap<>();
        serverMapper.getServerConfs4Tree().forEach(server -> {
            TreeNode node = new TreeNode();

            String id;
            if ("Y".equalsIgnoreCase(server.getEnableSet())) {
                id = String.format("1%s.2%s.3%s.4%s.5%s", server.getApplication(),
                        server.getSetName(), server.getSetArea(), server.getSetGroup(), server.getServerName());
            } else {
                id = String.format("1%s.5%s", server.getApplication(), server.getServerName());
            }

            node.setId(id);
            node.setName(server.getServerName());
            node.setPid(id.substring(0, id.lastIndexOf(".")));
            node.setIsParent(false);
            node.setOpen(false);

            nodes.put(id, node);
            log.info("node, id={}", id);

            parents(nodes, node);
        });

        return nodes.values().stream()
                .filter(node -> !node.getId().contains("."))
                .sorted(Comparator.comparing(TreeNode::getId))
                .collect(Collectors.toList());
    }

    private void parents(Map<String, TreeNode> nodes, TreeNode child) {
        String id = child.getPid();

        // 不生成root节点，退出递归
        if ("root".equals(id)) {
            return;
        }

        // 父节点已经存在，当前节点加入到子节点列表后退出递归
        if (nodes.containsKey(id)) {
            nodes.get(id).addChild(child);
            return;
        }

        String name, pid;
        int idx = id.lastIndexOf(".");
        if (idx == -1) {
            pid = "root";
            name = id;
        } else {
            pid = id.substring(0, idx);
            name = id.substring(idx + 1);
        }

        TreeNode node = new TreeNode();
        node.setId(id);
        node.setName(name.substring(1));
        node.setPid(pid);
        node.setIsParent(true);
        node.setOpen(true);
        node.addChild(child);

        if ("root".equals(node.getPid())) {
            log.info("root, open={}", node.isOpen());
        }

        nodes.put(id, node);
        parents(nodes, node);
    }
}
