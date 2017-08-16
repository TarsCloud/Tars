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

package com.qq.tars.support.property;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.tars.client.Communicator;
import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.support.om.OmLogger;
import com.qq.tars.support.property.prx.PropertyFPrx;
import com.qq.tars.support.property.prx.StatPropInfo;
import com.qq.tars.support.property.prx.StatPropMsgBody;
import com.qq.tars.support.property.prx.StatPropMsgHead;

public class PropertyReportHelper {

    public static interface Policy {

        public String desc();

        public String get();

        public void set(int value);

    }

    private static class ReportData {

        public StatPropMsgHead head;
        public StatPropMsgBody body;
        public int len;

        public ReportData(StatPropMsgHead head, StatPropMsgBody body, int len) {
            this.head = head;
            this.body = body;
            this.len = len;
        }

    }

    public static class PropertyReporter {

        private static final int PROPERTY_PROTOCOL_LEN = 50;
        private Collection<Policy> policies;
        private String propRptName;
        private String moduleName;

        public PropertyReporter(String moduleName, String propRptName, Policy[] policies) {
            this.moduleName = moduleName;
            this.propRptName = propRptName;
            Map<String, Policy> map = new HashMap<String, Policy>();
            for (Policy policy : policies)
                map.put(policy.desc(), policy);
            this.policies = map.values();
        }

        public void report(int value) {
            for (Policy policy : policies)
                policy.set(value);
        }

        public ReportData getReportData() {
            int len = 0;
            StatPropMsgHead head = new StatPropMsgHead(moduleName, "", propRptName, null, null, null, null, 1);
            len += moduleName.length() + propRptName.length();
            int size = policies.size();
            len += PROPERTY_PROTOCOL_LEN + size;
            List<StatPropInfo> statPropInfos = new ArrayList<StatPropInfo>();
            for (Policy policy : policies) {
                String desc = policy.desc();
                String value = policy.get();
                statPropInfos.add(new StatPropInfo(desc, value));
                len += desc.length() + value.length();
            }
            StatPropMsgBody body = new StatPropMsgBody();
            body.setVInfo(statPropInfos);
            return new ReportData(head, body, len);
        }

    }

    private static final PropertyReportHelper Instance = new PropertyReportHelper();
    private static final int MAX_REPORT_SIZE = 1400;
    private Map<String, PropertyReporter> reporters;
    private Communicator communicator;
    private String moduleName;
    private boolean configed;

    private PropertyReportHelper() {
        reporters = new ConcurrentHashMap<String, PropertyReporter>();
        configed = false;
    }

    public static PropertyReportHelper getInstance() {
        return Instance;
    }

    public synchronized void setPropertyInfo(Communicator comm, String moduleName) {
        if (configed) {
            throw new TarsException("PropertyReporter|setPropertyInfo method should be called once at most");
        }

        this.communicator = comm;
        this.moduleName = moduleName;
        configed = true;
    }

    public synchronized void createPropertyReporter(String propRptName, Policy... policies) {
        if (policies.length == 0) {
            throw new TarsException("PropertyReporter|at least one policy instance should be given");
        }

        if (!reporters.containsKey(propRptName)) {
            PropertyReporter reporter = new PropertyReporter(moduleName, propRptName, policies);
            reporters.put(propRptName, reporter);
        }
    }

    public void reportPropertyValue(String propRptName, int value) {
        PropertyReporter reporter = reporters.get(propRptName);
        if (reporter == null) {
            return;
        }

        reporter.report(value);
    }

    public void report() {
        try {
            if (!configed) {
                return;
            }

            PropertyFPrx propertyFPrx = communicator.stringToProxy(PropertyFPrx.class, ConfigurationManager.getInstance().getServerConfig().getCommunicatorConfig().getProperty());

            Map<StatPropMsgHead, StatPropMsgBody> sendData = new HashMap<StatPropMsgHead, StatPropMsgBody>();
            int sendLen = 0;
            for (PropertyReporter reporter : reporters.values()) {
                ReportData data = reporter.getReportData();
                if (sendLen + data.len <= MAX_REPORT_SIZE) {
                    sendData.put(data.head, data.body);
                    sendLen += data.len;
                } else {
                    propertyFPrx.async_reportPropMsg(null, sendData);
                    sendData.clear();
                    sendData.put(data.head, data.body);
                    sendLen = data.len;
                }
            }

            if (sendData.size() != 0) {
                propertyFPrx.async_reportPropMsg(null, sendData);
            }
        } catch (Throwable t) {
            OmLogger.record("PropertyReporter|ReportThread error", t);
        }
    }
}
