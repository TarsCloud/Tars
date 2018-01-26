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

package com.qq.tars.server.core;

import java.util.HashMap;
import java.util.Map;

import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;

public final class Context<REQ extends Request, RESP extends Response> {

    private REQ request = null;

    private RESP response = null;

    private Map<String, Object> attributes = new HashMap<String, Object>();

    public static final String INTERNAL_START_TIME = "internal.startTime";

    public static final String INTERNAL_CLIENT_IP = "internal.requestIp";

    public static final String INTERNAL_APP_NAME = "internal.requestApp";

    public static final String INTERNAL_SERVICE_NAME = "internal.requestService";

    public static final String INTERNAL_METHOD_NAME = "internal.requestMethod";

    public static final String INTERNAL_SESSION_DATA = "internal.sessionData";

    public Context(REQ req, RESP resp) {
        this.request = req;
        this.response = resp;
    }

    public REQ request() {
        return this.request;
    }

    public RESP response() {
        return this.response;
    }

    @SuppressWarnings("unchecked")
    public <T> T getAttribute(String name) {
        return (T) getAttribute(name, null);
    }

    @SuppressWarnings("unchecked")
    public <T> T getAttribute(String name, T defaultValue) {
        Object value = null;
        value = attributes.get(name);
        if (value == null) return defaultValue;
        return (T) value;
    }

    public <T> void setAttribute(String name, T value) {
        this.attributes.put(name, value);
    }
}
