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

package com.qq.tars.web.controller.adapter;

import com.qq.common.WrappedController;
import com.qq.tars.service.adapter.AdapterService;
import com.qq.tars.service.adapter.AddAdapterConf;
import com.qq.tars.service.adapter.UpdateAdapterConf;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.validation.Valid;
import java.util.List;
import java.util.stream.Collectors;

@Controller
public class AdapterController extends WrappedController {

    @Autowired
    private AdapterService adapterService;

    @RequestMapping(
            value = "server/api/add_adapter_conf",
            produces = {"application/json"}
    )
    @ResponseBody
    public AdapterConfView addAdapterConf(@Valid @RequestBody AddAdapterConf addAdapterConf) throws Exception {
        return mapper.map(adapterService.addAdapterConf(addAdapterConf), AdapterConfView.class);
    }

    @RequestMapping(
            value = "server/api/update_adapter_conf",
            produces = {"application/json"}
    )
    @ResponseBody
    public AdapterConfView updateAdapterConf(@RequestBody UpdateAdapterConf updateAdapterConf) throws Exception {
        return mapper.map(adapterService.updateAdapterConf(updateAdapterConf), AdapterConfView.class);
    }

    @RequestMapping(
            value = "server/api/delete_adapter_conf",
            produces = {"application/json"}
    )
    @ResponseBody
    public long deleteAdapterConf(@RequestParam long id) throws Exception {
        adapterService.deleteAdapterConf(id);
        return id;
    }

    @RequestMapping(
            value = "server/api/adapter_conf",
            produces = {"application/json"}
    )
    @ResponseBody
    public AdapterConfView loadAdapterConf(@RequestParam long id) throws Exception {
        return mapper.map(adapterService.loadAdapterConf(id), AdapterConfView.class);
    }

    @RequestMapping(
            value = "server/api/adapter_conf_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<AdapterConfView> getAdapterConfListByServerId(@RequestParam long id) throws Exception {
        return adapterService.getAdapterConf(id).stream()
                .map(adapter -> mapper.map(adapter, AdapterConfView.class))
                .collect(Collectors.toList());
    }
}
