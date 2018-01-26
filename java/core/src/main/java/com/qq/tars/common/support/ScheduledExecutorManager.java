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

package com.qq.tars.common.support;

import java.util.Collection;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import com.qq.tars.common.util.Constants;
import com.qq.tars.common.util.concurrent.TaskThreadFactory;

public final class ScheduledExecutorManager implements ScheduledExecutorService {

    private final static ScheduledExecutorManager instance = new ScheduledExecutorManager();

    private ScheduledThreadPoolExecutor taskExecutor = new ScheduledThreadPoolExecutor(4);
    private final Lock lock = new ReentrantLock();

    private ScheduledExecutorManager() {
        this.taskExecutor.setThreadFactory(new TaskThreadFactory("tars-schedule-executor-"));
    }

    public static ScheduledExecutorManager getInstance() {
        return instance;
    }

    @Override
    public void shutdown() {
        taskExecutor.shutdown();
    }

    @Override
    public List<Runnable> shutdownNow() {
        return taskExecutor.shutdownNow();
    }

    @Override
    public boolean isShutdown() {
        return taskExecutor.isShutdown();
    }

    @Override
    public boolean isTerminated() {
        return taskExecutor.isTerminated();
    }

    @Override
    public boolean awaitTermination(long timeout, TimeUnit unit) throws InterruptedException {
        return taskExecutor.awaitTermination(timeout, unit);
    }

    @Override
    public <T> Future<T> submit(Callable<T> task) {
        checkMaxSize();
        return taskExecutor.submit(task);
    }

    @Override
    public <T> Future<T> submit(Runnable task, T result) {
        checkMaxSize();
        return taskExecutor.submit(task, result);
    }

    @Override
    public Future<?> submit(Runnable task) {
        checkMaxSize();
        return taskExecutor.submit(task);
    }

    @Override
    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks) throws InterruptedException {
        return taskExecutor.invokeAll(tasks);
    }

    @Override
    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks, long timeout, TimeUnit unit) throws InterruptedException {
        return taskExecutor.invokeAll(tasks, timeout, unit);
    }

    @Override
    public <T> T invokeAny(Collection<? extends Callable<T>> tasks) throws InterruptedException, ExecutionException {
        return taskExecutor.invokeAny(tasks);
    }

    @Override
    public <T> T invokeAny(Collection<? extends Callable<T>> tasks, long timeout, TimeUnit unit) throws InterruptedException, ExecutionException, TimeoutException {
        return taskExecutor.invokeAny(tasks, timeout, unit);
    }

    @Override
    public void execute(Runnable command) {
        checkMaxSize();
        taskExecutor.execute(command);
    }

    @Override
    public ScheduledFuture<?> schedule(Runnable command, long delay, TimeUnit unit) {
        checkMaxSize();
        return taskExecutor.schedule(command, delay, unit);
    }

    @Override
    public <V> ScheduledFuture<V> schedule(Callable<V> callable, long delay, TimeUnit unit) {
        checkMaxSize();
        return taskExecutor.schedule(callable, delay, unit);
    }

    @Override
    public ScheduledFuture<?> scheduleAtFixedRate(Runnable command, long initialDelay, long period, TimeUnit unit) {
        checkMaxSize();
        return taskExecutor.scheduleAtFixedRate(command, initialDelay, period, unit);
    }

    @Override
    public ScheduledFuture<?> scheduleWithFixedDelay(Runnable command, long initialDelay, long delay, TimeUnit unit) {
        checkMaxSize();
        return taskExecutor.scheduleWithFixedDelay(command, initialDelay, delay, unit);
    }

    private void checkMaxSize() throws RejectedExecutionException {
        int count = taskExecutor.getQueue().size();
        lock.lock();
        try {
            int maxQueueSize = Constants.default_background_queuesize;
            if (count >= maxQueueSize) {
                throw new RejectedExecutionException("the queue is full, the max queue size is " + maxQueueSize);
            }
        } finally {
            lock.unlock();
        }
    }
}
