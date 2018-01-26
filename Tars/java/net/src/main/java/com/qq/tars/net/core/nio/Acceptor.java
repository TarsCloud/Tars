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

package com.qq.tars.net.core.nio;

import java.io.IOException;
import java.nio.channels.SelectionKey;

public abstract class Acceptor {

    protected SelectorManager selectorManager = null;

    public Acceptor(SelectorManager selectorManager) {
        this.selectorManager = selectorManager;
    }

    public abstract void handleConnectEvent(SelectionKey key) throws IOException;

    public abstract void handleAcceptEvent(SelectionKey key) throws IOException;

    public abstract void handleReadEvent(SelectionKey key) throws IOException;

    public abstract void handleWriteEvent(SelectionKey key) throws IOException;

}
