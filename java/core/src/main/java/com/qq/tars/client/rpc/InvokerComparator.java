package com.qq.tars.client.rpc;

import com.qq.tars.rpc.common.Invoker;

import java.util.Comparator;

public class InvokerComparator implements Comparator<Invoker<?>> {

    @Override
    public int compare(Invoker<?> o1, Invoker<?> o2) {
        return o1.getUrl().compareTo(o2.getUrl());
    }
}
