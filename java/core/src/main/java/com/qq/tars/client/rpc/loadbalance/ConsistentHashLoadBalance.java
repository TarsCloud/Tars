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

package com.qq.tars.client.rpc.loadbalance;

import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.client.cluster.ServantInvokerAliveStat;
import com.qq.tars.client.cluster.ServantnvokerAliveChecker;
import com.qq.tars.client.rpc.InvokerComparator;
import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.common.util.Constants;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.rpc.common.InvokeContext;
import com.qq.tars.rpc.common.Invoker;
import com.qq.tars.rpc.common.LoadBalance;
import com.qq.tars.rpc.common.exc.NoInvokerException;

import java.util.*;

public class ConsistentHashLoadBalance<T> implements LoadBalance<T> {

    private final ServantProxyConfig config;
    private final InvokerComparator comparator = new InvokerComparator();

    private volatile TreeMap<Long, Invoker<T>> conHashInvokersCache = null;
    private volatile List<Invoker<T>> sortedInvokersCache = null;

    public ConsistentHashLoadBalance(ServantProxyConfig config) {
        this.config = config;
    }

    @Override
    public Invoker<T> select(InvokeContext invocation) throws NoInvokerException {
        long consistentHash = Math.abs(StringUtils.convertLong(invocation.getAttachment(Constants.TARS_CONSISTENT_HASH), 0));
        //hash空间是0 ~ 2^32-1
        consistentHash = consistentHash & 0xFFFFFFFFL;

        TreeMap<Long, Invoker<T>> conHashInvokers = conHashInvokersCache;
        //使用一致性hash
        if (conHashInvokers != null && !conHashInvokers.isEmpty()) {
            if (!conHashInvokers.containsKey(consistentHash)) {
                SortedMap<Long, Invoker<T>> tailMap = conHashInvokers.tailMap(consistentHash);
                if (tailMap.isEmpty()) {
                    consistentHash = conHashInvokers.firstKey();
                } else {
                    consistentHash = tailMap.firstKey();
                }
            }

            Invoker<T> invoker = conHashInvokers.get(consistentHash);
            if (invoker.isAvailable()) return invoker;

            ServantInvokerAliveStat stat = ServantnvokerAliveChecker.get(invoker.getUrl());
            if (stat.isAlive() || (stat.getLastRetryTime() + (config.getTryTimeInterval() * 1000)) < System.currentTimeMillis()) {
                //屏敝后尝试重新调用    
                ClientLogger.getLogger().info("try to use inactive invoker|" + invoker.getUrl().toIdentityString());
                stat.setLastRetryTime(System.currentTimeMillis());
                return invoker;
            }
        }

        if (ClientLogger.getLogger().isDebugEnabled()) {
            ClientLogger.getLogger().debug(config.getSimpleObjectName() + " can't find active invoker using consistent hash loadbalance. try to use normal hash");
        }

        //使用普通hash
        List<Invoker<T>> sortedInvokers = sortedInvokersCache;
        if (sortedInvokers == null || sortedInvokers.isEmpty()) {
            throw new NoInvokerException("no such active connection invoker");
        }

        List<Invoker<T>> list = new ArrayList<Invoker<T>>();
        for (Invoker<T> invoker : sortedInvokers) {
            if (!invoker.isAvailable()) {
                /**
                 * 屏敝后尝试重新调用    
                 */
                ServantInvokerAliveStat stat = ServantnvokerAliveChecker.get(invoker.getUrl());
                if (stat.isAlive() || (stat.getLastRetryTime() + (config.getTryTimeInterval() * 1000)) < System.currentTimeMillis()) {
                    list.add(invoker);
                }
            } else {
                list.add(invoker);
            }
        }
        // TODO 如果全死，是否需要随机取一个尝试？
        if (list.isEmpty()) {
            throw new NoInvokerException(config.getSimpleObjectName() + " try to select active invoker, size=" + sortedInvokers.size() + ", no such active connection invoker");
        }

        Invoker<T> invoker = list.get((int) (consistentHash % list.size()));

        if (!invoker.isAvailable()) {
            //屏敝后尝试重新调用    
            ClientLogger.getLogger().info("try to use inactive invoker|" + invoker.getUrl().toIdentityString());
            ServantnvokerAliveChecker.get(invoker.getUrl()).setLastRetryTime(System.currentTimeMillis());
        }
        return invoker;
    }

    @Override
    public void refresh(Collection<Invoker<T>> invokers) {
        ClientLogger.getLogger().info(config.getSimpleObjectName() + " try to refresh ConsistentHashLoadBalance's invoker cache, size=" + (invokers == null || invokers.isEmpty() ? 0 : invokers.size()));
        if (invokers == null || invokers.isEmpty()) {
            sortedInvokersCache = null;
            conHashInvokersCache = null;
            return;
        }

        List<Invoker<T>> sortedInvokersTmp = new ArrayList<Invoker<T>>(invokers);
        Collections.sort(sortedInvokersTmp, comparator);

        sortedInvokersCache = sortedInvokersTmp;
        conHashInvokersCache = LoadBalanceHelper.buildConsistentHashCircle(sortedInvokersTmp, config);

        ClientLogger.getLogger().info(config.getSimpleObjectName() + " refresh ConsistentHashLoadBalance's invoker cache done, conHashInvokersCache size=" + (conHashInvokersCache == null || conHashInvokersCache.isEmpty() ? 0 : conHashInvokersCache.size()) + ", sortedInvokersCache size=" + (sortedInvokersCache == null || sortedInvokersCache.isEmpty() ? 0 : sortedInvokersCache.size()));
    }

}
