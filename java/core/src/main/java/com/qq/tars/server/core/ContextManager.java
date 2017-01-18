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

import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;

public final class ContextManager {

    private static ThreadLocal<Context<? extends Request, ? extends Response>> contexts = new ThreadLocal<Context<?, ?>>();

    public static <REQ extends Request, RESP extends Response> Context<REQ, RESP> registerContext(REQ req, RESP resp) {
        Context<REQ, RESP> context = new Context<REQ, RESP>(req, resp);
        contexts.set(context);
        return context;
    }

    @SuppressWarnings("unchecked")
    public static <REQ extends Request, RESP extends Response> Context<REQ, RESP> getContext() {
        return (Context<REQ, RESP>) contexts.get();
    }

    public static void releaseContext() {
        contexts.remove();
    }
}
