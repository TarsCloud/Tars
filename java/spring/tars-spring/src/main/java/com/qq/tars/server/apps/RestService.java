/**
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

package com.qq.tars.server.apps;

import org.springframework.context.ApplicationContext;

import com.qq.tars.server.core.AppContext;
import com.qq.tars.server.core.AppService;

public class RestService extends AppService {

    private AppContext appContext;
    private String source;
    private ApplicationContext parent;
    private String name;

    RestService(String name, String source, ApplicationContext parent) {
        this.name = name;
        this.source = source;
        this.parent = parent;
    }

    public void setAppContext(AppContext appContext) {
        this.appContext = appContext;
    }

    public String getSource() {
        return source;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public ApplicationContext getParent() {
        return parent;
    }

    public void setParent(ApplicationContext parent) {
        this.parent = parent;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    @Override
    public AppContext getAppContext() {
        return appContext;
    }

    @Override
    public String name() {
        return name;
    }
}
