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

package com.qq.tars.service.adapter;

import com.qq.tars.db.AdapterMapper;
import com.qq.tars.entity.AdapterConf;
import com.qq.tars.entity.ServerConf;
import com.qq.tars.service.server.ServerService;
import com.qq.tars.tools.DateTime;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

@Service
public class AdapterService {

    private final Logger log = LoggerFactory.getLogger(AdapterService.class);

    @Autowired
    private ServerService serverService;

    @Autowired
    private AdapterMapper adapterMapper;

    @Transactional(rollbackFor = Exception.class)
    public AdapterConf addAdapterConf(AddAdapterConf addAdapterConf) {
        AdapterConf adapter = new AdapterConf();
        BeanUtils.copyProperties(addAdapterConf, adapter);
        adapter.setAdapterName(String.format("%sAdapter", adapter.getServant()));
        adapter.setPosttime(DateTime.now());
        adapterMapper.insertAdapterConf(adapter);
        return adapter;
    }

    @Transactional(rollbackFor = Exception.class)
    public int deleteAdapterConf(long id) {
        AdapterConf adapter = loadAdapterConf(id);
        return adapterMapper.deleteAdapterConf(adapter);
    }

    @Transactional(rollbackFor = Exception.class)
    public AdapterConf updateAdapterConf(UpdateAdapterConf updateAdapterConf) {
        AdapterConf adapter = adapterMapper.loadAdapterConf(updateAdapterConf.getId());
        BeanUtils.copyProperties(updateAdapterConf, adapter);
        adapter.setAdapterName(String.format("%sAdapter", adapter.getServant()));
        adapter.setPosttime(DateTime.now());
        adapterMapper.updateAdapterConf(adapter);
        return loadAdapterConf(adapter.getId());
    }

    public AdapterConf loadAdapterConf(long id) {
        return adapterMapper.loadAdapterConf(id);
    }

    public List<AdapterConf> getAdapterConf(long serverConfId) {
        ServerConf server = serverService.loadServerConf(serverConfId);
        if (null == server) {
            return new LinkedList<>();
        }
        return getAdapterConf(server.getApplication(), server.getServerName(), server.getNodeName());
    }

    public List<AdapterConf> getAdapterConf(String application, String serverName, String nodeName) {
        return adapterMapper.getAdapterConf(application, serverName, nodeName);
    }
    
    public List<String> getAdapterConfList(String application,String serverName){
    	List<Map<String,String>> list = adapterMapper.getAdapterList(application, serverName);
    	
    	List<String> rList =  new ArrayList<String>();
    	
    	for(int i=0;i<list.size();i++){
    		Map<String,String> map = list.get(i);   		
    		String servant = map.get("servant");    		
    		rList.add(servant);  		
    	}
    	return rList;
    }


}
