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

import java.lang.reflect.Method;

import com.qq.tars.net.core.Processor;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.net.core.Session;
import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.rpc.protocol.ext.ExtendedServantRequest;
import com.qq.tars.rpc.protocol.ext.ExtendedServantResponse;
import com.qq.tars.server.apps.AppContextImpl;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.support.om.OmServiceMngr;

public abstract class ExtendedServantProcessor<RES extends ExtendedServantRequest, RESP extends ExtendedServantResponse> extends Processor {

    private final ServantAdapter servantAdapter;

    public ExtendedServantProcessor(ServantAdapter servantAdapter) {
        this.servantAdapter = servantAdapter;
    }

    @Override
    public Response process(Request req, Session session) {
        AppContainer container = null;
        RES request = null;
        RESP response = null;

        ServantHomeSkeleton skeleton = null;
        AppContextImpl appContext = null;
        ClassLoader oldClassLoader = null;
        int waitingTime = -1;
        long startTime = req.getProcessTime();
        try {
            oldClassLoader = Thread.currentThread().getContextClassLoader();
            request = (RES) req;
            response = createResponse(request, session);
            response.setTicketNumber(req.getTicketNumber());

            String servantName = servantAdapter.getServantAdapterConfig().getServant();
            request.setServantName(servantAdapter.getServantAdapterConfig().getServant());

            int maxWaitingTimeInQueue = ConfigurationManager.getInstance().getServerConfig().getServantAdapterConfMap().get(servantName).getQueueTimeout();
            waitingTime = (int) (startTime - req.getBornTime());
            if (waitingTime > maxWaitingTimeInQueue) {
                throw new TarsException("Wait too long, server busy.");
            }

            // 3. Register the context for the business layer.
            container = ContainerManager.getContainer(AppContainer.class);
            Context<?, ?> context = ContextManager.registerContext(request, response);
            context.setAttribute(Context.INTERNAL_START_TIME, startTime);
            context.setAttribute(Context.INTERNAL_CLIENT_IP, session.getRemoteIp());
            context.setAttribute(Context.INTERNAL_APP_NAME, container.getDefaultAppContext().name());
            context.setAttribute(Context.INTERNAL_SERVICE_NAME, request.getServantName());
            context.setAttribute(Context.INTERNAL_METHOD_NAME, request.getFunctionName());
            context.setAttribute(Context.INTERNAL_SESSION_DATA, session);

            // 4. Get the AppContext
            appContext = (AppContextImpl) container.getDefaultAppContext();
            if (appContext == null) throw new RuntimeException("failed to find the application named:[ROOT]");

            // 5. Set the AppClassLoader
            Thread.currentThread().setContextClassLoader(appContext.getAppContextClassLoader());

            // 6. Find the skeleton
            skeleton = servantAdapter.getSkeleton();
            if (skeleton == null) throw new RuntimeException("failed to find the servant named[" + servantName + "]");

            Method method = skeleton.getApiClass().getMethods()[0];
            skeleton.invoke(method, request, response);
        } catch (Throwable cause) {
            // 7. Write exception to client
            if (cause instanceof TarsException) {
                System.out.println("ERROR: " + cause.getMessage());
            } else {
                cause.printStackTrace();
            }
            response.setThrowable(cause);
        } finally {
            if (oldClassLoader != null) {
                Thread.currentThread().setContextClassLoader(oldClassLoader);
            }
            ContextManager.releaseContext();
            OmServiceMngr.getInstance().reportWaitingTimeProperty(waitingTime);
        }
        return response;
    }

    protected abstract RESP createResponse(RES request, Session session);
}
