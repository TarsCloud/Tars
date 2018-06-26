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

package com.qq.tars.service.admin;

import com.qq.tars.client.Communicator;
import com.qq.tars.client.CommunicatorConfig;
import com.qq.tars.client.CommunicatorFactory;
import com.qq.tars.common.support.Holder;
import com.qq.tars.common.util.Config;
import com.qq.tars.exception.TARSRequestException;
import com.qq.tars.generated.tars.AdminRegPrx;
import com.qq.tars.generated.tars.ApplyTokenRequest;
import com.qq.tars.generated.tars.ApplyTokenResponse;
import com.qq.tars.generated.tars.AuthPrx;
import com.qq.tars.generated.tars.ConfigPrx;
import com.qq.tars.generated.tars.DeleteTokenRequest;
import com.qq.tars.generated.tars.TaskReq;
import com.qq.tars.generated.tars.TaskRsp;
import com.qq.tars.generated.tars.TokenRequest;
import com.qq.tars.generated.tars.TokenResponse;
import com.qq.tars.support.query.prx.EndpointF;

import org.apache.commons.lang3.tuple.Pair;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.core.io.ClassPathResource;
import org.springframework.stereotype.Service;

import java.nio.charset.Charset;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class AdminService {

    private final Logger log = LoggerFactory.getLogger(AdminService.class);

    private Communicator communicator;

    public AdminService() {
        String path = null;
        try {
            path = new ClassPathResource("tars.conf").getFile().getCanonicalPath();

            CommunicatorConfig cfg = new CommunicatorConfig();
            cfg.load(Config.parseFile(path, Charset.forName("UTF-8")));
            communicator = CommunicatorFactory.getInstance().getCommunicator(cfg);
        } catch (Exception e) {
            log.error("init error, path={}", path, e);
            throw new RuntimeException(e);
        }
    }

    private AdminRegPrx getAdminRegPrx() {
        return communicator.stringToProxy(AdminRegPrx.class, "tars.tarsAdminRegistry.AdminRegObj");

    }

    private ConfigPrx getConfigPrx() {
        return communicator.stringToProxy(ConfigPrx.class, "tars.tarsconfig.ConfigObj");
    }
    
    private AuthPrx getAuthPrx(){
    	return communicator.stringToProxy(AuthPrx.class, "tars.tarsauth.AuthObj");
    }

    public String loadServer(String application, String serverName, String nodeName) throws TARSRequestException {
        Holder<String> holder = new Holder<>();
        int ret = getAdminRegPrx().loadServer(application, serverName, nodeName, holder);
        if (ret == 0) {
            return holder.getValue();
        } else {
            throw new TARSRequestException(ret);
        }
    }

    public String loadConfigByHost(String server, String filename, String host) throws TARSRequestException {
        Holder<String> holder = new Holder<>();
        int ret = getConfigPrx().loadConfigByHost(server, filename, host, holder);
        if (ret == 0) {
            log.info("server={}, filename={}, host={}, result={}", server, filename, host, holder.getValue());
            return holder.getValue();
        } else {
            throw new TARSRequestException(ret);
        }
    }

    public Pair<Integer, String> doCommand(String application, String serverName, String nodeName,
                                           String command) throws TARSRequestException {
        Holder<String> holder = new Holder<>();
        int ret = getAdminRegPrx().notifyServer(application, serverName, nodeName, command, holder);
        if (ret == 0) {
            return Pair.of(ret, holder.getValue());
        } else {
            throw new TARSRequestException(ret);
        }
    }

    public List<CommandResult> doCommand(List<CommandTarget> targets, String command) {
        return targets.stream()
                .map(target -> {
                    Pair<Integer, String> pair;
                    try {
                        pair = doCommand(
                                target.getApplication(),
                                target.getServerName(),
                                target.getNodeName(),
                                command
                        );

                        log.info("application={}, server name={}, node name={}",
                                target.getApplication(), target.getServerName(), target.getNodeName());
                    } catch (TARSRequestException e) {
                        log.error("application={}, server name={}, node name={}, command={}",
                                target.getApplication(), target.getServerName(), target.getNodeName(), e);
                        pair = Pair.of(-1, e.getMessage());
                    }

                    CommandResult result = new CommandResult();
                    BeanUtils.copyProperties(target, result);
                    result.setRet(pair.getLeft());
                    result.setErrMsg(pair.getRight());
                    return result;
                })
                .collect(Collectors.toList());
    }

    public void addTask(TaskReq taskReq) throws TARSRequestException {
        taskReq.getTaskItemReq().forEach(req -> log.info(
                "item={}, applicaiton={}, server_name={}, node_name={}, command={}, parameters={}",
                req.getItemNo(), req.getApplication(), req.getServerName(), req.getNodeName(),
                req.getCommand(), req.getParameters()
        ));

        int ret = getAdminRegPrx().addTaskReq(taskReq);
        if (ret != 0) {
            throw new TARSRequestException(ret);
        }
    }

    public TaskRsp getTaskRsp(String taskNo) throws TARSRequestException {
        Holder<TaskRsp> holder = new Holder<>();
        int ret = getAdminRegPrx().getTaskRsp(taskNo, holder);
        if (ret == 0) {
            return holder.getValue();
        } else {
            throw new TARSRequestException(ret);
        }
    }

    public List<Pair<String, Integer>> getEndpoints(String objectName) {
        List<EndpointF> endpoints = communicator.getEndpoint4All(objectName);
        log.info("object name={}, endpoints size={}", objectName, endpoints.size());
        return endpoints.stream()
                .map(endpoint -> Pair.of(endpoint.getHost(), endpoint.getPort()))
                .collect(Collectors.toList());
    }
    
    public List<TokenResponse> getTokens(TokenRequest request){
    	
    	return getAuthPrx().getTokens(request);
    }
    
    public  ApplyTokenResponse applyToken(ApplyTokenRequest request){
    	
    	return  getAuthPrx().applyToken(request);
    }
    
    public int deleteToken(DeleteTokenRequest request){
    	
    	return getAuthPrx().deleteToken(request);
    }
    
}
