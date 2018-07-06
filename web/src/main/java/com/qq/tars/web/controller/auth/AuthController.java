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
package com.qq.tars.web.controller.auth;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import com.qq.common.WrappedController;
import com.qq.tars.generated.tars.ApplyTokenRequest;
import com.qq.tars.generated.tars.ApplyTokenResponse;
import com.qq.tars.generated.tars.DeleteTokenRequest;
import com.qq.tars.generated.tars.TokenKey;
import com.qq.tars.generated.tars.TokenRequest;
import com.qq.tars.generated.tars.TokenResponse;
import com.qq.tars.service.adapter.AdapterService;
import com.qq.tars.service.admin.AdminService;
import com.qq.tars.service.config.ConfigService;
import com.qq.tars.service.monitor.TARSStatMonitorService;

@Validated
@Controller
public class AuthController extends WrappedController {
	private final Logger log = LoggerFactory.getLogger(AuthController.class);

	@Autowired
	private ConfigService configService;

	@Autowired
	private AdminService adminService;

	@Autowired
	private AdapterService adapterService;

	@RequestMapping(value = "server/api/get_obj_list", produces = { "application/json" })
	@ResponseBody
	public List<String> getObjList(@RequestParam("application") String application,
			@RequestParam("server_name") String serverName) throws Exception {

		return adapterService.getAdapterConfList(application, serverName);
	}

	@RequestMapping(value = "server/api/get_tokens", produces = { "application/json" })
	@ResponseBody
	public List<TokenResponse> getTokens(@RequestParam("application") String application,
			@RequestParam("server_name") String serverName) throws Exception {
		List<String> objList = adapterService.getAdapterConfList(application, serverName);
		TokenRequest request = new TokenRequest();
		request.setVObjName(objList);
		log.info("getTokens  objList "+ objList);
		List<TokenResponse> tList = adminService.getTokens(request);
		
		for(int i=0;i<tList.size();i++){
			TokenResponse tr = tList.get(i);
			
			log.info("getTokens  sObjName "+tr.getSObjName());
			
			log.info("getTokens  tokens "+tr.getMTokens());
			
		}
		
		return tList;
	}

	@RequestMapping(value = "server/api/add_token", produces = { "application/json" })
	@ResponseBody
	public ApplyTokenResponse addTokens(@RequestParam("application") String application,
			@RequestParam("server_name") String serverName, @RequestParam("obj_name") String objName) throws Exception {

		ApplyTokenRequest request = new ApplyTokenRequest();
		TokenKey tokenKey = new TokenKey();
		tokenKey.setSApplication(application);
		tokenKey.setSServer(serverName);
		tokenKey.setSObjName(objName);
		request.setSKey(tokenKey);
		return adminService.applyToken(request);
	}

	@RequestMapping(value = "server/api/delete_token", produces = { "application/json" })
	@ResponseBody
	public int delTokens(@RequestParam("application") String application,
			@RequestParam("server_name") String serverName, @RequestParam("obj_name") String objName) throws Exception {

		DeleteTokenRequest request = new DeleteTokenRequest();

		TokenKey tokenKey = new TokenKey();
		tokenKey.setSApplication(application);
		tokenKey.setSServer(serverName);
		tokenKey.setSObjName(objName);
		request.setSKey(tokenKey);
		return adminService.deleteToken(request);
	}
}
