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

package com.qq.tars.server.startup;

import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.server.apps.XmlAppContext;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.core.AppContext;
import com.qq.tars.server.core.Server;

import java.io.File;
import java.net.URL;

public class Main {

    public static void main(String[] args) throws Exception {
        Server.loadServerConfig();
        Server.initCommunicator();
        Server.configLogger();
        Server.startManagerService();

        AppContext context = null;
        URL servantXML = Main.class.getClassLoader().getResource("servants.xml");
        if (servantXML != null) {
            context = new XmlAppContext();
        } else if (Main.class.getClassLoader().getResource("servants-spring.xml") != null){
            System.out.println("[SERVER] find servants-spring.xml, use Spring mode.");
            Class clazz = Class.forName("com.qq.tars.server.apps.SpringAppContext");
            context = (AppContext) clazz.newInstance();
        } else {
            System.out.println("[SERVER] servants profile does not exist, start failed.");
            throw new TarsException("servants profile does not exist");
        }
        new Server(ConfigurationManager.getInstance().getServerConfig()).startUp(context);
    }
}
