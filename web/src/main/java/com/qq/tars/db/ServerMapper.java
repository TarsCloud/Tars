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

package com.qq.tars.db;

import com.qq.tars.entity.ServerConf;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.session.RowBounds;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface ServerMapper {

    int insertServerConf(ServerConf serverConf);

    int deleteServerConf(ServerConf serverConf);

    int updateServerConf(ServerConf serverConf);

    ServerConf loadServerConf(@Param("id") long id);

    List<ServerConf> loadServerConfs(@Param("ids") List<Long> ids);

    ServerConf loadServerConfByName(@Param("application") String application,
                                    @Param("serverName") String serverName,
                                    @Param("nodeName") String nodeName);

    List<ServerConf> getServerConf(@Param("application") String application,
                                   @Param("serverName") String serverName,
                                   @Param("enableSet") boolean enableSet,
                                   @Param("setName") String setName,
                                   @Param("setArea") String setArea,
                                   @Param("setGroup") String setGroup,
                                   RowBounds rowBounds);

    List<ServerConf> queryInactiveServerConf(@Param("application") String application,
                                             @Param("serverName") String serverName,
                                             @Param("nodeName") String nodeName,
                                             RowBounds rowBounds);

    List<ServerConf> getServerConfsByTemplate(@Param("templateName") String templateName);

    List<String> getApplication();

    List<String> getServerName(String application);

    List<String> getSet(@Param("application") String application,
                        @Param("serverName") String serverName);

    List<String> getNodeName(@Param("application") String application,
                             @Param("serverName") String serverName,
                             @Param("enableSet") boolean enableSet,
                             @Param("setName") String setName,
                             @Param("setArea") String setArea,
                             @Param("setGroup") String setGroup);

    List<ServerConf> getServerConfs4Tree();
}
