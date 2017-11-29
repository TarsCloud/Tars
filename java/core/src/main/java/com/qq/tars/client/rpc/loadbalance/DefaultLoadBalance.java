package com.qq.tars.client.rpc.loadbalance;

import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.common.util.Constants;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.rpc.common.InvokeContext;
import com.qq.tars.rpc.common.Invoker;
import com.qq.tars.rpc.common.LoadBalance;
import com.qq.tars.rpc.common.exc.NoInvokerException;

import java.util.Collection;

public class DefaultLoadBalance<T> implements LoadBalance<T> {

    private final RoundRobinLoadBalance<T> roundRobinLoadBalance;
    private final ServantProxyConfig config;

    private volatile Collection<Invoker<T>> lastRefreshInvokers = null;

    private volatile HashLoadBalance<T> hashLoadBalance = null;
    private final Object hashLoadBalanceLock = new Object();

    private volatile ConsistentHashLoadBalance<T> consistentHashLoadBalance = null;
    private final Object consistentHashLoadBalanceLock = new Object();

    public DefaultLoadBalance(ServantProxyConfig config) {
        this.config = config;
        this.roundRobinLoadBalance = new RoundRobinLoadBalance<T>(config);
    }

    @Override
    public Invoker<T> select(InvokeContext invocation) throws NoInvokerException {
        long hash = Math.abs(StringUtils.convertLong(invocation.getAttachment(Constants.TARS_HASH), 0));
        long consistentHash = Math.abs(StringUtils.convertLong(invocation.getAttachment(Constants.TARS_CONSISTENT_HASH), 0));

        if (consistentHash > 0) {
            if (consistentHashLoadBalance == null) {
                synchronized (consistentHashLoadBalanceLock) {
                    if (consistentHashLoadBalance == null) {
                        ConsistentHashLoadBalance<T> tmp = new ConsistentHashLoadBalance<T>(config);
                        tmp.refresh(lastRefreshInvokers);
                        consistentHashLoadBalance = tmp;
                    }
                }
            }
            return consistentHashLoadBalance.select(invocation);
        }

        if (hash > 0) {
            if (hashLoadBalance == null) {
                synchronized (hashLoadBalanceLock) {
                    if (hashLoadBalance == null) {
                        HashLoadBalance<T> tmp = new HashLoadBalance<T>(config);
                        tmp.refresh(lastRefreshInvokers);
                        hashLoadBalance = tmp;
                    }
                }
            }
            return hashLoadBalance.select(invocation);
        }

        return roundRobinLoadBalance.select(invocation);
    }

    @Override
    public void refresh(Collection<Invoker<T>> invokers) {
        lastRefreshInvokers = invokers;

        synchronized (hashLoadBalanceLock) {
            if (hashLoadBalance != null) {
                hashLoadBalance.refresh(invokers);
            }
        }

        synchronized (consistentHashLoadBalanceLock) {
            if (consistentHashLoadBalance != null) {
                consistentHashLoadBalance.refresh(invokers);
            }
        }

        roundRobinLoadBalance.refresh(invokers);
    }

}
