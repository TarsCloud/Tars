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

import com.qq.tars.entity.ConfigFile;
import com.qq.tars.entity.ConfigFileHistory;
import com.qq.tars.entity.ConfigRef;
import com.qq.tars.entity.ConfigRefExt;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.session.RowBounds;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface ConfigMapper {

    int insertConfigFile(ConfigFile configFile);

    int deleteConfigFile(ConfigFile configFile);

    int deleteUnusedNodeConfigFile();

    int updateConfigFile(ConfigFile configFile);

    ConfigFile loadConfigFile(@Param("id") long id);

    List<ConfigFile> loadConfigFiles(@Param("ids") List<Long> ids);

    List<ConfigFile> getUnusedApplicationConfigFile(@Param("application") String application,
                                                    @Param("configId") long configId);

    List<ConfigFile> getApplicationConfigFile(@Param("application") String application);

    List<ConfigFile> getSetConfigFile(@Param("application") String application,
                                      @Param("setName") String setName,
                                      @Param("setArea") String setArea,
                                      @Param("setGroup") String setGroup);

    List<ConfigFile> getServerConfigFile(@Param("application") String application,
                                         @Param("serverName") String serverName,
                                         @Param("setName") String setName,
                                         @Param("setArea") String setArea,
                                         @Param("setGroup") String setGroup);

    List<ConfigFile> getNodeConfigFile(@Param("application") String application,
                                       @Param("serverName") String serverName,
                                       @Param("setName") String setName,
                                       @Param("setArea") String setArea,
                                       @Param("setGroup") String setGroup);

    int insertConfigFileHistory(ConfigFileHistory configFileHistory);

    int deleteConfigFileHistory(ConfigFileHistory configFileHistory);

    int updateConfigFileHistory(ConfigFileHistory configFileHistory);

    ConfigFileHistory loadConfigFileHistory(@Param("id") long id);

    List<ConfigFileHistory> getConfigFileHistory(@Param("configId") long configId,
                                                 RowBounds rowBounds);

    int insertConfigRef(ConfigRef configRef);

    int deleteConfigRef(ConfigRef configRef);

    ConfigRef loadConfigRef(@Param("id") long id);

    List<ConfigRef> getConfigRefByRefId(@Param("refId") long refId);

    List<ConfigRefExt> getConfigRefByConfigId(@Param("configId") long configId);

}
