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
public class TARSStatMonitorService {

    private final Logger log = LoggerFactory.getLogger(TARSStatMonitorService.class);

    @Autowired
    private AdminService adminService;

    public List<TARSStatMonitorDataRow> getTARSStatMonitorData(TARSStatMonitorCondition condition) throws IOException {
        MultiKeyMap thedata = call(condition, true);
        MultiKeyMap predata = call(condition, false);
        return merge(condition, thedata, predata);
    }

    private List<String> getCallGroupBy(TARSStatMonitorCondition condition) {
        List<String> callGroupBy = new LinkedList<>();
        if (null != condition.getGroupBy()) {
            callGroupBy.add("f_date");
            callGroupBy.add(condition.getGroupBy());
        } else {
            callGroupBy.add("f_tflag");
        }
        return callGroupBy;
    }

    private Set<MultiKey> mergeKey(TARSStatMonitorCondition condition, MultiKeyMap thedata, MultiKeyMap predata) {
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

    private void dimension(TARSStatMonitorCondition condition, MultiKey key, TARSStatMonitorDataRow row) {
        if (!"master_name".equals(condition.getGroupBy())) {
            row.setMasterName(null != condition.getMasterName() ? condition.getMasterName() : "%");
        }

        if (!"slave_name".equals(condition.getGroupBy())) {
            row.setSlaveName(null != condition.getSlaveName() ? condition.getSlaveName() : "%");
        }

        if (!"interface_name".equals(condition.getGroupBy())) {
            row.setInterfaceName(null != condition.getInterfaceName() ? condition.getInterfaceName() : "%");
        }

        if (!"master_ip".equals(condition.getGroupBy())) {
            row.setMasterIp(null != condition.getMasterIp() ? condition.getMasterIp() : "%");
        }

        if (!"slave_ip".equals(condition.getGroupBy())) {
            row.setSlaveIp(null != condition.getSlaveIp() ? condition.getSlaveIp() : "%");
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
            } else if ("slave_name".equals(callGroup)) {
                row.setSlaveName(callGroupValue);
            } else if ("interface_name".equals(callGroup)) {
                row.setInterfaceName(callGroupValue);
            } else if ("master_ip".equals(callGroup)) {
                row.setMasterIp(callGroupValue);
            } else if ("slave_ip".equals(callGroup)) {
                row.setSlaveIp(callGroupValue);
            }
        }
    }

    private double[] translate(long[] data) {
        if (null == data) {
            return new double[]{-1, -1, -1, -1};
        }

        double[] result = new double[4];
        long total = data[0] + data[1] + data[2];
        result[0] = total;
        result[1] = total == 0 ? -1 : data[3] / result[0];
        result[2] = total == 0 ? -1 : data[2] / result[0];
        result[3] = total == 0 ? -1 : data[1] / result[0];
        return result;
    }

    private String[] translate(double[] data) {
        String[] result = new String[4];
        result[0] = data[0] < 0 ? "--" : String.format("%.0f", data[0]);
        result[1] = data[1] < 0 ? "--" : String.format("%.2f", data[1]);
        result[2] = data[2] < 0 ? "--" : String.format("%.2f", data[2] * 100) + "%";
        result[3] = data[3] < 0 ? "--" : String.format("%.2f", data[3] * 100) + "%";
        return result;
    }

    private List<TARSStatMonitorDataRow> merge(TARSStatMonitorCondition condition, MultiKeyMap thedata, MultiKeyMap predata) {
        Set<MultiKey> keys = mergeKey(condition, thedata, predata);

        List<TARSStatMonitorDataRow> result = new LinkedList<>();
        for (MultiKey key : keys) {
            TARSStatMonitorDataRow row = new TARSStatMonitorDataRow();
            result.add(row);

            double[] thevalue = translate((long[]) thedata.get(key));
            double[] prevalue = translate((long[]) predata.get(key));
            String[] thevalueoutput = translate(thevalue);
            String[] prevalueoutput = translate(prevalue);

            row.setTheTotalCount(thevalueoutput[0]);
            row.setTheAvgTime(thevalueoutput[1]);
            row.setTheFailRate(thevalueoutput[2]);
            row.setTheTimeoutRate(thevalueoutput[3]);

            row.setPreTotalCount(prevalueoutput[0]);
            row.setPreAvgTime(prevalueoutput[1]);
            row.setPreFailRate(prevalueoutput[2]);
            row.setPreTimeoutRate(prevalueoutput[3]);

            if (thevalue[0] < 0 || prevalue[0] < 0) {
                row.setTotalCountWave("--");
            } else {
                if (0 == prevalue[0]) {
                    if (0 == thevalue[0]) {
                        row.setTotalCountWave("0%");
                    } else {
                        row.setTotalCountWave("+∞%");
                    }
                } else {
                    double wave = (thevalue[0] - prevalue[0]) / prevalue[0];
                    row.setTotalCountWave(String.format("%.2f", wave * 100) + "%");
                }
            }

            dimension(condition, key, row);
        }
        return result;
    }

    private MultiKeyMap call(TARSStatMonitorCondition condition, boolean the) throws IOException {
        String date = the ? condition.getThedate() : condition.getPredate();

        List<String> callConditions = new LinkedList<>();
        callConditions.add("f_date='" + date + "'");
        callConditions.add("f_tflag>='" + condition.getTheshowtime() + "'");
        callConditions.add("f_tflag<='" + condition.getPreshowtime() + "'");

        if (null != condition.getMasterName()) {
            callConditions.add("master_name like '" + condition.getMasterName() + "'");
        }
        if (null != condition.getSlaveName()) {
            callConditions.add("slave_name like '" + condition.getSlaveName() + "'");
        }
        if (null != condition.getInterfaceName()) {
            callConditions.add("interface_name like '" + condition.getInterfaceName() + "'");
        }
        if (null != condition.getMasterIp()) {
            callConditions.add("master_ip like '" + condition.getMasterIp() + "'");
        }
        if (null != condition.getSlaveIp()) {
            callConditions.add("slave_ip like '" + condition.getSlaveIp() + "'");
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
        String template = "{\"groupby\":%s,\"method\":\"query\",\"dataid\":\"tars_stat\"," +
                "\"filter\":%s,\"indexs\":[\"succ_count\",\"timeout_count\",\"exce_count\",\"total_time\"]}";

        ObjectMapper mapper = new ObjectMapper();
        String request = String.format(template, mapper.writeValueAsString(groupBy), mapper.writeValueAsString(conditions));

        List<Pair<String, Integer>> addrs = adminService.getEndpoints("tars.tarsquerystat.NoTarsObj");
        if (addrs.isEmpty()) {
            throw new IOException("tars.tarsquerystat.NoTarsObj not found");
        }

        Pair<String, Integer> addr = addrs.get(0);
        log.info("tars.tarsquerystat.NoTarsObj, use {}:{}", addr.getLeft(), addr.getRight());

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
            if (tokens.length != groupBy.size() + 4) {
                throw new IOException(String.format("line format error, line=%s", line));
            }

            String[] key = new String[groupBy.size()];
            int j = 0;
            for (; j < key.length; j++) {
                key[j] = tokens[j];
            }

            long[] value = new long[]{
                    Long.parseLong(tokens[j++]),
                    Long.parseLong(tokens[j++]),
                    Long.parseLong(tokens[j++]),
                    Long.parseLong(tokens[j])
            };
            result.put(new MultiKey(key), value);
        }
        return result;
    }
}
