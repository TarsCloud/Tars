/*
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

package com.qq.tars.service.monitor;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.qq.common.TCPClient;
import com.qq.tars.service.admin.AdminService;
import org.apache.commons.collections.keyvalue.MultiKey;
import org.apache.commons.collections.map.MultiKeyMap;
import org.apache.commons.lang3.StringUtils;
import org.apache.commons.lang3.tuple.Pair;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

@Service
public class TARSPropertyMonitorService {

    private final Logger log = LoggerFactory.getLogger(TARSPropertyMonitorService.class);

    @Autowired
    private AdminService adminService;

    public List<TARSPropertyMonitorDataRow> getTARSPropertyMonitorData(TARSPropertyMonitorCondition condition) throws IOException {
        MultiKeyMap thedata = call(condition, true);
        MultiKeyMap predata = call(condition, false);
        return merge(condition, thedata, predata);
    }

    private List<String> getCallGroupBy(TARSPropertyMonitorCondition condition) {
        List<String> callGroupBy = new LinkedList<>();
        if (null != condition.getGroupBy()) {
            callGroupBy.add("f_date");
            callGroupBy.add(condition.getGroupBy());
        } else {
            callGroupBy.add("f_tflag");
        }
        return callGroupBy;
    }

    private Set<MultiKey> mergeKey(TARSPropertyMonitorCondition condition, MultiKeyMap thedata, MultiKeyMap predata) {
        Set<MultiKey> keys = new TreeSet<>((left, right) -> {
            for (int i = 0; i < left.size(); i++) {
                Object leftKey = left.getKey(i);
                Object rightKey = right.getKey(i);
                if (leftKey.equals(rightKey)) {
                    continue;
                }
                return ((String) leftKey).compareTo((String) rightKey);
            }
            return 0;
        });
        keys.addAll(thedata.keySet());
        for (Object obj : predata.keySet()) {
            Object[] keyparts = ((MultiKey) obj).getKeys();
            if (keyparts.length > 1) {
                keyparts[0] = condition.getThedate();
            }
            keys.add(new MultiKey(keyparts));
        }
        return keys;
    }

    private void dimension(TARSPropertyMonitorCondition condition, MultiKey key, TARSPropertyMonitorDataRow row) {
        if (!"master_name".equals(condition.getGroupBy())) {
            row.setMasterName(null != condition.getMasterName() ? condition.getMasterName() : "%");
        }

        if (!"master_ip".equals(condition.getGroupBy())) {
            row.setMasterIp(null != condition.getMasterIp() ? condition.getMasterIp() : "%");
        }

        if (!"property_name".equals(condition.getGroupBy())) {
            row.setPropertyName(null != condition.getPropertyName() ? condition.getPropertyName() : "%");
        }

        if (!"policy".equals(condition.getGroupBy())) {
            row.setPolicy(null != condition.getPolicy() ? condition.getPolicy() : "%");
        }

        List<String> callGroupBy = getCallGroupBy(condition);
        for (int i = 0; i < callGroupBy.size(); i++) {
            String callGroup = callGroupBy.get(i);
            String callGroupValue = (String) key.getKey(i);

            if ("f_date".equals(callGroup)) {
                row.setShowDate(callGroupValue);
            } else if ("f_tflag".equals(callGroup)) {
                row.setShowTime(callGroupValue);
            } else if ("master_name".equals(callGroup)) {
                row.setMasterName(callGroupValue);
            } else if ("master_ip".equals(callGroup)) {
                row.setMasterIp(callGroupValue);
            } else if ("property_name".equals(callGroup)) {
                row.setPropertyName(callGroupValue);
            } else if ("policy".equals(callGroup)) {
                row.setPolicy(callGroupValue);
            }
        }
    }

    private String[] translate(double[] data) {
        if (null == data) {
            return new String[] { "--" };
        }

        String[] result = new String[1];
        result[0] = data[0] < 0 ? "--" : String.format("%.3f", data[0]);
        result[0] = StringUtils.removePattern(result[0], "[\\.]*[0]+$");
        return result;
    }

    private List<TARSPropertyMonitorDataRow> merge(TARSPropertyMonitorCondition condition, MultiKeyMap thedata, MultiKeyMap predata) {
        Set<MultiKey> keys = mergeKey(condition, thedata, predata);

        List<TARSPropertyMonitorDataRow> result = new LinkedList<>();
        for (MultiKey key : keys) {
            TARSPropertyMonitorDataRow row = new TARSPropertyMonitorDataRow();
            result.add(row);

            double[] thevalue = (double[]) thedata.get(key);
            double[] prevalue = (double[]) predata.get(key);
            String[] thevalueoutput = translate(thevalue);
            String[] prevalueoutput = translate(prevalue);


            row.setTheValue(thevalueoutput[0]);
            row.setPreValue(prevalueoutput[0]);

            dimension(condition, key, row);
        }
        return result;
    }

    private MultiKeyMap call(TARSPropertyMonitorCondition condition, boolean the) throws IOException {
        String date = the ? condition.getThedate() : condition.getPredate();

        List<String> callConditions = new LinkedList<>();
        callConditions.add("f_date='" + date + "'");
        callConditions.add("f_tflag>='" + condition.getTheshowtime() + "'");
        callConditions.add("f_tflag<='" + condition.getPreshowtime() + "'");

        if (null != condition.getMasterName()) {
            callConditions.add("master_name like '" + condition.getMasterName() + "'");
        }
        if (null != condition.getMasterIp()) {
            callConditions.add("master_ip like '" + condition.getMasterIp() + "'");
        }
        if (null != condition.getPropertyName()) {
            callConditions.add("property_name like '" + condition.getPropertyName() + "'");
        }
        if (null != condition.getPolicy()) {
            callConditions.add("policy like '" + condition.getPolicy() + "'");
        }

        List<String> callGroupBy = new LinkedList<>();
        if (null != condition.getGroupBy()) {
            callGroupBy.add("f_date");
            callGroupBy.add(condition.getGroupBy());
        } else {
            callGroupBy.add("f_tflag");
        }

        return call(callGroupBy, callConditions);
    }


    private MultiKeyMap call(List<String> groupBy, List<String> conditions) throws IOException {
        String template = "{\"groupby\":%s,\"method\":\"query\",\"dataid\":\"tars_property\"," +
                "\"filter\":%s,\"indexs\":[\"value\"]}";

        ObjectMapper mapper = new ObjectMapper();
        String request = String.format(template, mapper.writeValueAsString(groupBy), mapper.writeValueAsString(conditions));

        List<Pair<String, Integer>> addrs = adminService.getEndpoints("tars.tarsqueryproperty.NoTarsObj");
        if (addrs.isEmpty()) {
            throw new IOException("tars.tarsqueryproperty.NoTarsObj not found");
        }

        Pair<String, Integer> addr = addrs.get(0);
        log.info("tars.tarsqueryproperty.NoTarsObj, use {}:{}", addr.getLeft(), addr.getRight());

        TCPClient client = new TCPClient(addr.getLeft(), addr.getRight());
        List<String> response = client.sendAndReceive(request.getBytes(), 60000);

        log.debug("request={}", request);
        log.debug("reponse={}", StringUtils.join(response, "\n"));

        String line1 = response.get(0);
        if (!line1.startsWith("Ret:")) {
            throw new IOException(String.format("line #1, doesn't start with \"Ret:\", line=%s", line1));
        }
        int ret = Integer.parseInt(line1.substring(line1.lastIndexOf(':') + 1));
        if (ret == -1) {
            throw new IOException(String.format("line #1, Ret=%s", ret));
        }

        String line6 = response.get(5);
        if (!line6.startsWith("linecount:")) {
            throw new IOException(String.format("line #6, doesn't start with \"linecount:\", line=%s", line6));
        }
        int count = Integer.parseInt(line6.substring(line6.lastIndexOf(':') + 1));
        if (count + 7 != response.size()) {
            throw new IOException(String.format("line #6, size not match, %s vs %s", count + 7, response.size()));
        }

        String lastLine = response.get(response.size() - 1);
        if (!"endline".equals(lastLine)) {
            throw new IOException(String.format(
                    "line #%s, doesn't equal to \"endline\", line=%s", response.size(), lastLine));
        }

        MultiKeyMap result = new MultiKeyMap();
        for (int i = 6; i < response.size() - 1; i++) {
            String line = StringUtils.removeEnd(response.get(i), ",");
            String[] tokens = line.split(",");
            if (tokens.length != groupBy.size() + 1) {
                throw new IOException(String.format("line format error, line=%s", line));
            }

            String[] key = new String[groupBy.size()];
            int j = 0;
            for (; j < key.length; j++) {
                key[j] = tokens[j];
            }

            double[] value = new double[]{
                    Double.parseDouble(tokens[j])
            };
            result.put(new MultiKey(key), value);
        }
        return result;
    }
}
