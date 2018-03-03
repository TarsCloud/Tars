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

import java.util.Collection;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import com.qq.tars.net.client.Callback;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Session;

public class TicketManager {

    private static ConcurrentHashMap<Integer, Ticket<?>> tickets = new ConcurrentHashMap<Integer, Ticket<?>>();

    private static ScheduledExecutorService executor = Executors.newSingleThreadScheduledExecutor();

    static {
        executor.scheduleAtFixedRate(new Runnable() {

            long currentTime = -1;

            public void run() {
                Collection<Ticket<?>> values = tickets.values();
                currentTime = System.currentTimeMillis();
                for (Ticket<?> t : values) {
                    if ((currentTime - t.startTime) > t.timeout) {
                        removeTicket(t.getTicketNumber());
                        t.expired();
                    }
                }
            }

        }, 500, 500, TimeUnit.MILLISECONDS);
    }

    public static void shutdown() {
        executor.shutdownNow();
    }

    @SuppressWarnings("unchecked")
    public static <T> Ticket<T> getTicket(int seq) {
        return (Ticket<T>) tickets.get(seq);
    }

    public static <T> Ticket<T> createTicket(Request req, Session session, long timeout) {
        return createTicket(req, session, timeout, null);
    }

    public static <T> Ticket<T> createTicket(Request req, Session session, long timeout, Callback<T> callback) {
        if (req.getTicketNumber() == Ticket.DEFAULT_TICKET_NUMBER) {
            req.setTicketNumber(session.hashCode());
        }

        Ticket<T> ticket = new Ticket<T>(req, timeout);
        ticket.setCallback(callback);

        if (tickets.putIfAbsent(ticket.getTicketNumber(), ticket) != null) {
            throw new IllegalArgumentException("duplicate ticket number.");
        }

        return ticket;
    }

    public static void removeTicket(int ticketNumber) {
        tickets.remove(ticketNumber);
    }
}
