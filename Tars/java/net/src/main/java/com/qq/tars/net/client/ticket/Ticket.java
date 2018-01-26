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

package com.qq.tars.net.client.ticket;

import java.io.IOException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import com.qq.tars.net.client.Callback;
import com.qq.tars.net.core.Request;

public class Ticket<T> {

    public static final int DEFAULT_TICKET_NUMBER = -1;
    private CountDownLatch latch = new CountDownLatch(1);

    private T response = null;
    private Request request = null;
    private volatile boolean expired = false;
    protected long timeout = 1000;
    public long startTime = System.currentTimeMillis();
    private Callback<T> callback = null;
    private int ticketNumber = -1;
    private static TicketListener ticketListener = null;

    public Ticket(Request request, long timeout) {
        this.request = request;
        this.ticketNumber = request.getTicketNumber();
        this.timeout = timeout;
    }

    public Request request() {
        return this.request;
    }

    public boolean await(long timeout, TimeUnit unit) throws InterruptedException {
        boolean status = this.latch.await(timeout, unit);
        checkExpired();
        return status;
    }

    public void await() throws InterruptedException {
        this.latch.await();
        checkExpired();
    }

    public void expired() {
        this.expired = true;
        if (callback != null) callback.onExpired();
        this.countDown();
        if (ticketListener != null) ticketListener.onResponseExpired(this);
    }

    public void countDown() {
        this.latch.countDown();
    }

    public boolean isDone() {
        return this.latch.getCount() == 0;
    }

    public void notifyResponse(T response) {
        this.response = response;
        if (this.callback != null) this.callback.onCompleted(response);
        if (ticketListener != null) ticketListener.onResponseReceived(this);
    }

    public T response() {
        return this.response;
    }

    public Callback<T> getCallback() {
        return callback;
    }

    public void setCallback(Callback<T> callback) {
        this.callback = callback;
    }

    public int getTicketNumber() {
        return this.ticketNumber;
    }

    protected void checkExpired() {
        if (this.expired) throw new RuntimeException("", new IOException("The operation has timed out."));
    }

    public static void setTicketListener(TicketListener listener) {
        ticketListener = listener;
    }
}
