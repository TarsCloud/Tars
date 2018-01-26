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

package com.qq.tars.support.log.util;

import java.util.Calendar;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Utils {

    public static String getFilePath(String path) {
        if (path == null) return "";
        String r = "\\$\\{[^${}]*\\}";
        Pattern p = Pattern.compile(r);
        Matcher m = p.matcher(path);
        while (m.find()) {
            int startIndex = m.start(); // index of start
            int endIndex = m.end(); // index of end + 1
            String currentMatch = path.substring(startIndex, endIndex);
            String property = System.getProperty(path.substring(startIndex + 2, endIndex - 1).trim());
            System.out.println("|tars-log|DEBUG|read java system property :" + currentMatch + "=" + property);
            if (property == null) {
                return "";
            }
            path = replaceAll(path, currentMatch, property);
            m = p.matcher(path);
        }
        return path;
    }

    private static String replaceAll(String s, String src, String dest) {
        if (s == null || src == null || dest == null || src.length() == 0) return s;
        int pos = s.indexOf(src);
        if (pos < 0) return s;
        int capacity = dest.length() > src.length() ? s.length() * 2 : s.length();
        StringBuilder sb = new StringBuilder(capacity);
        int writen = 0;
        for (; pos >= 0;) {
            sb.append(s, writen, pos);
            sb.append(dest);
            writen = pos + src.length();
            pos = s.indexOf(src, writen);
        }
        sb.append(s, writen, s.length());
        return sb.toString();
    }

    public static final String getDateSimpleInfo(long time) {
        Date date = new Date(time);
        Calendar ca = Calendar.getInstance();
        ca.setTime(date);
        int month = 1 + ca.get(Calendar.MONTH);
        String monthStr = String.valueOf(month);
        if (month < 10) {
            monthStr = "0" + monthStr;
        }
        int day = ca.get(Calendar.DAY_OF_MONTH);
        String dayStr = String.valueOf(day);
        if (day < 10) {
            dayStr = "0" + dayStr;
        }
        String result = ca.get(Calendar.YEAR) + "-" + monthStr + "-" + dayStr;
        return result;
    }

    public static final String getDateAllInfo(long time) {
        Date date = new Date(time);
        Calendar ca = Calendar.getInstance();
        ca.setTime(date);
        int month = 1 + ca.get(Calendar.MONTH);
        String monthStr = String.valueOf(month);
        if (month < 10) {
            monthStr = "0" + monthStr;
        }
        int day = ca.get(Calendar.DAY_OF_MONTH);
        String dayStr = String.valueOf(day);
        if (day < 10) {
            dayStr = "0" + dayStr;
        }
        int hour = ca.get(Calendar.HOUR_OF_DAY);
        String hourStr = String.valueOf(hour);
        if (hour < 10) {
            hourStr = "0" + hourStr;
        }
        int minute = ca.get(Calendar.MINUTE);
        String minuteStr = String.valueOf(minute);
        if (minute < 10) {
            minuteStr = "0" + minuteStr;
        }
        int second = ca.get(Calendar.SECOND);
        String secondStr = String.valueOf(second);
        if (second < 10) {
            secondStr = "0" + second;
        }

        StringBuilder strBuf = new StringBuilder();
        strBuf.append(ca.get(Calendar.YEAR)).append("-").append(monthStr).append("-").append(dayStr);
        strBuf.append(" ").append(hourStr).append(":").append(minuteStr).append(":").append(secondStr).append(".").append(ca.get(Calendar.MILLISECOND));
        return strBuf.toString();
    }
}
