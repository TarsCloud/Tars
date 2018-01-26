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
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.client.cluster.ServantnvokerAliveChecker;
import com.qq.tars.client.rpc.ServantClient;
import com.qq.tars.client.rpc.ServantInvokeContext;
import com.qq.tars.client.rpc.ServantInvoker;
import com.qq.tars.common.util.Constants;
import com.qq.tars.common.util.DyeingSwitch;
import com.qq.tars.context.DistributedContext;
import com.qq.tars.context.DistributedContextManager;
import com.qq.tars.net.client.Callback;
import com.qq.tars.protocol.tars.support.TarsMethodInfo;
import com.qq.tars.protocol.tars.support.TarsMethodParameterInfo;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.common.Url;
import com.qq.tars.rpc.exc.NotConnectedException;
import com.qq.tars.rpc.exc.ServerException;
import com.qq.tars.rpc.exc.TimeoutException;
import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;
import com.qq.tars.rpc.protocol.tars.support.AnalystManager;
import com.qq.tars.support.stat.InvokeStatHelper;

public class TarsInvoker<T> extends ServantInvoker<T> {

    public TarsInvoker(ServantProxyConfig config, Class<T> api, Url url, ServantClient[] clients) {
        super(config, api, url, clients);
    }

    protected Object doInvokeServant(final ServantInvokeContext inv) throws Throwable {
        long begin = System.currentTimeMillis();

        int ret = Constants.INVOKE_STATUS_SUCC;
        boolean isAsync = TarsHelper.isAsync(inv.getMethodName());
        try {
            Method method = getApi().getMethod(inv.getMethodName(), inv.getParameterTypes());
            if (isAsync) {
                invokeWithAsync(method, inv.getArguments(), inv.getAttachments());
                return null;
            } else {
                TarsServantResponse response = invokeWithSync(method, inv.getArguments(), inv.getAttachments());
                ret = response.getRet() == TarsHelper.SERVERSUCCESS ? Constants.INVOKE_STATUS_SUCC : Constants.INVOKE_STATUS_EXEC;
                if (response.getRet() != TarsHelper.SERVERSUCCESS) {
                    throw ServerException.makeException(response.getRet());
                }
                return response.getResult();
            }
        } catch (Throwable e) {
            if (e instanceof TimeoutException) {
                ret = Constants.INVOKE_STATUS_TIMEOUT;
            } else if (e instanceof NotConnectedException) {
                ret = Constants.INVOKE_STATUS_NETCONNECTTIMEOUT;
            } else {
                ret = Constants.INVOKE_STATUS_EXEC;
            }
            throw e;
        } finally {
            setAvailable(ServantnvokerAliveChecker.isAlive(getUrl(), config, ret));
            if (!isAsync) {
                InvokeStatHelper.getInstance().addProxyStat(objName).addInvokeTime(config.getModuleName(), objName, config.getSetDivision(), inv.getMethodName(), getUrl().getHost(), getUrl().getPort(), ret, System.currentTimeMillis() - begin);
            }
        }
    }

    private ServantClient getClient() {
        return clients.length == 1 ? clients[0] : clients[(index.getAndIncrement() & Integer.MAX_VALUE) % clients.length];
    }

    private TarsServantResponse invokeWithSync(Method method, Object args[], Map<String, String> context) throws Throwable {
        ServantClient client = getClient();
        TarsServantRequest request = new TarsServantRequest(client.getIoSession());
        request.setVersion(TarsHelper.VERSION);
        request.setMessageType(isHashInvoke(context) ? TarsHelper.MESSAGETYPEHASH : TarsHelper.MESSAGETYPENULL);
        request.setPacketType(TarsHelper.NORMAL);
        request.setServantName(objName);
        request.setFunctionName(method.getName());
        request.setMethodInfo(AnalystManager.getInstance().getMethodMap(objName).get(method));
        request.setMethodParameters(args);
        request.setContext(context);
        DistributedContext distributedContext = DistributedContextManager.getDistributedContext();
        Boolean bDyeing = distributedContext.get(DyeingSwitch.BDYEING);        
        if (bDyeing != null && bDyeing == true) {
        	request.setMessageType(request.getMessageType() | TarsHelper.MESSAGETYPEDYED);
        	HashMap<String, String> status = new HashMap<String, String>();
        	String routeKey = distributedContext.get(DyeingSwitch.DYEINGKEY);
        	String fileName = distributedContext.get(DyeingSwitch.FILENAME);
        	status.put(DyeingSwitch.STATUS_DYED_KEY, routeKey == null ? "" : routeKey);
        	status.put(DyeingSwitch.STATUS_DYED_FILENAME, fileName == null ? "" : fileName);
        	request.setStatus(status);
        	
        }
        return client.invokeWithSync(request);
    }

    @SuppressWarnings("unchecked")
    private void invokeWithAsync(Method method, Object args[], Map<String, String> context) throws Throwable {
        ServantClient client = getClient();
        TarsServantRequest request = new TarsServantRequest(client.getIoSession());
        request.setVersion(TarsHelper.VERSION);
        request.setMessageType(isHashInvoke(context) ? TarsHelper.MESSAGETYPEHASH : TarsHelper.MESSAGETYPENULL);
        request.setPacketType(TarsHelper.NORMAL);
        request.setServantName(objName);
        request.setFunctionName(method.getName().replaceAll("async_", ""));
        request.setContext(context);

        TarsMethodInfo methodInfo = AnalystManager.getInstance().getMethodMap(objName).get(method);
        request.setMethodInfo(methodInfo);
        request.setMethodParameters(args);

        Callback<TarsServantResponse> callback = null;
        List<TarsMethodParameterInfo> parameterInfoList = methodInfo.getParametersList();
        for (TarsMethodParameterInfo parameterInfo : parameterInfoList) {
            if (TarsHelper.isCallback(parameterInfo.getAnnotations())) {
                callback = (Callback<TarsServantResponse>) args[parameterInfo.getOrder()];
                break;
            }
        }

        if (callback == null) {
            request.setPacketType(TarsHelper.ONEWAY);
        }
        
        DistributedContext distributedContext = DistributedContextManager.getDistributedContext();
        Boolean bDyeing = distributedContext.get(DyeingSwitch.BDYEING);        
        if (bDyeing != null && bDyeing == true) {
        	request.setMessageType(request.getMessageType() | TarsHelper.MESSAGETYPEDYED);
        	HashMap<String, String> status = new HashMap<String, String>();
        	String routeKey = distributedContext.get(DyeingSwitch.DYEINGKEY);
        	String fileName = distributedContext.get(DyeingSwitch.FILENAME);
        	status.put(DyeingSwitch.STATUS_DYED_KEY, routeKey == null ? "" : routeKey);
        	status.put(DyeingSwitch.STATUS_DYED_FILENAME, fileName == null ? "" : fileName);
        	request.setStatus(status);
        	
        }
        client.invokeWithAsync(request, new TarsCallbackWrapper(config, request.getFunctionName(), getUrl().getHost(), getUrl().getPort(), request.getBornTime(), request, callback));
    }

    private boolean isHashInvoke(Map<String, String> context) {
        return context != null && context.containsKey(Constants.TARS_HASH);
    }
}
