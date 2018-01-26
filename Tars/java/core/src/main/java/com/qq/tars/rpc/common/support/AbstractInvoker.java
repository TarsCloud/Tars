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

package com.qq.tars.rpc.common.support;

import com.qq.tars.rpc.common.InvokeContext;
import com.qq.tars.rpc.common.Invoker;
import com.qq.tars.rpc.common.Url;

public abstract class AbstractInvoker<T> implements Invoker<T> {

    private final Url url;
    private final Class<T> api;
    private volatile boolean available = true;
    private volatile boolean destroyed = false;

    public AbstractInvoker(Class<T> api, Url url) {
        if (api == null || url == null) {
            throw new IllegalArgumentException();
        }
        this.api = api;
        this.url = url;
    }

    public Class<T> getApi() {
        return api;
    }

    public Url getUrl() {
        return url;
    }

    public boolean isAvailable() {
        return !isDestroyed() && available;
    }

    protected void setAvailable(boolean available) {
        this.available = available;
    }

    public void destroy() {
        if (isDestroyed()) {
            return;
        }
        destroyed = true;
        setAvailable(false);
    }

    public boolean isDestroyed() {
        return destroyed;
    }

    public String toString() {
        return getUrl().toIdentityString();
    }

    public Object invoke(InvokeContext context) throws Throwable {
        if (destroyed) {
            throw new RuntimeException("invoker for " + this + " is destroyed!");
        }
        context.setInvoker(this);
        return doInvoke(context);
    }

    protected abstract Object doInvoke(InvokeContext context) throws Throwable;
}
