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

package com.qq.tars.client.rpc;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;

import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.rpc.common.InvokeContext;
import com.qq.tars.rpc.common.Url;
import com.qq.tars.rpc.common.support.AbstractInvoker;

public abstract class ServantInvoker<T> extends AbstractInvoker<T> {

    protected final String objName;
    protected final ServantProxyConfig config;
    protected final ServantClient[] clients;
    protected final AtomicInteger index = new AtomicInteger();
    protected final ReentrantLock destroyLock = new ReentrantLock();

    public ServantInvoker(ServantProxyConfig config, Class<T> api, Url url, ServantClient[] clients) {
        super(api, url);
        this.config = config;
        this.objName = config.getSimpleObjectName();
        this.clients = clients;
    }

    protected Object doInvoke(final InvokeContext inv) throws Throwable {
        return doInvokeServant((ServantInvokeContext) inv);
    }

    protected abstract Object doInvokeServant(final ServantInvokeContext inv) throws Throwable;

    public void destroy() {
        if (super.isDestroyed()) {
            return;
        }
        destroyLock.lock();
        try {
            if (super.isDestroyed()) {
                return;
            }
            super.destroy();
            for (ServantClient client : clients) {
                try {
                    ClientLogger.getLogger().info("try to close client " + client);
                    client.close();
                    ClientLogger.getLogger().info("closed client " + client);
                } catch (Throwable t) {
                    ClientLogger.getLogger().error("error in close " + client, t);
                }
            }
        } finally {
            destroyLock.unlock();
        }
    }
}
