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

package com.qq.tars.net.client;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import com.qq.tars.net.client.ticket.Ticket;

public final class FutureImpl<V> implements Future<V> {

    private Ticket<V> ticket = null;

    public FutureImpl(Ticket<V> ticket) {
        this.ticket = ticket;
    }

    public boolean cancel(boolean mayInterruptIfRunning) {
        return false;
    }

    public V get() throws InterruptedException, ExecutionException {
        this.ticket.await();
        return this.ticket.response();
    }

    public V get(long timeout, TimeUnit unit) throws InterruptedException, ExecutionException, TimeoutException {
        this.ticket.await(timeout, unit);
        return this.ticket.response();
    }

    public boolean isCancelled() {
        return false;
    }

    public boolean isDone() {
        return ticket.isDone();
    }
}
