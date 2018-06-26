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

import com.qq.tars.entity.AdapterConf;

import org.apache.ibatis.annotations.Param;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

@Repository
public interface AdapterMapper {

    int insertAdapterConf(AdapterConf adapterConf);

    int deleteAdapterConf(AdapterConf adapterConf);

    int updateAdapterConf(AdapterConf adapterConf);

    AdapterConf loadAdapterConf(@Param("id") long id);

    AdapterConf loadAdapterConfByObj(@Param("application") String application,
                                     @Param("serverName") String serverName,
                                     @Param("nodeName") String nodeName,
                                     @Param("objName") String objName);

    List<AdapterConf> getAdapterConf(@Param("application") String application,
                                     @Param("serverName") String serverName,
                                     @Param("nodeName") String nodeName);
    
    List<Map<String,String>> getAdapterList(@Param("application") String application,@Param("serverName") String serverName);
}
