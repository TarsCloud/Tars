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

package com.qq.tars.client.rpc.tars;

import java.lang.reflect.Method;
import java.util.concurrent.ThreadPoolExecutor;

import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.client.rpc.ServantProtocolInvoker;
import com.qq.tars.net.protocol.ProtocolFactory;
import com.qq.tars.rpc.common.InvokeContext;
import com.qq.tars.rpc.common.Invoker;
import com.qq.tars.rpc.common.Url;
import com.qq.tars.rpc.protocol.tars.support.AnalystManager;

public class TarsProtocolInvoker<T> extends ServantProtocolInvoker<T> {

    public TarsProtocolInvoker(Class<T> api, ServantProxyConfig config, ProtocolFactory protocolFactory,
                               ThreadPoolExecutor threadPoolExecutor) {
        super(api, config, protocolFactory, threadPoolExecutor);
        AnalystManager.getInstance().registry(api, servantProxyConfig.getSimpleObjectName());
    }

    @Override
    public Invoker<T> create(Class<T> api, Url url) throws Exception {
        return new TarsInvoker<T>(servantProxyConfig, api, url, getClients(url));
    }

    @Override
    public InvokeContext createContext(Object proxy, Method method, Object[] args) throws Exception {
        return new TarsInvokeContext(method, args, null);
    }
}
