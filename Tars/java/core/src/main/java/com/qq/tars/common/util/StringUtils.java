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

package com.qq.tars.common.util;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.nio.charset.Charset;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

public class StringUtils {

    public static final String ENC_UTF8 = "UTF-8";

    public static final String ENC_GBK = "GBK";

    public static final Charset GBK = Charset.forName("GBK");

    public static final Charset UTF_8 = Charset.forName("UTF-8");

    public static final String FMT_DATETIME = "yyyy-MM-dd HH:mm:ss";

    public static String formatDate(String dateStr, String inputFormat, String format) {
        String resultStr = dateStr;
        try {
            Date date = new SimpleDateFormat(inputFormat).parse(dateStr);
            resultStr = formatDate(date, format);
        } catch (ParseException e) {
        }
        return resultStr;
    }

    public static String formatDate(String dateStr, String format) {
        String resultStr = dateStr;
        String inputFormat = "yyyy-MM-dd HH:mm:ss";
        if (dateStr == null) {
            return "";
        }
        if (dateStr.matches("\\d{1,4}\\-\\d{1,2}\\-\\d{1,2}\\s+\\d{1,2}:\\d{1,2}:\\d{1,2}\\.\\d{1,3}")) {
            inputFormat = "yyyy-MM-dd HH:mm:ss.SSS";
        } else if (dateStr.matches("\\d{4}\\-\\d{1,2}\\-\\d{1,2} +\\d{1,2}:\\d{1,2}")) {
            inputFormat = "yyyy-MM-dd HH:mm:ss";
        } else if (dateStr.matches("\\d{4}\\-\\d{1,2}\\-\\d{1,2} +\\d{1,2}:\\d{1,2}")) {
            inputFormat = "yyyy-MM-dd HH:mm";
        } else if (dateStr.matches("\\d{4}\\-\\d{1,2}\\-\\d{1,2} +\\d{1,2}")) {
            inputFormat = "yyyy-MM-dd HH";
        } else if (dateStr.matches("\\d{4}\\-\\d{1,2}\\-\\d{1,2} +\\d{1,2}")) {
            inputFormat = "yyyy-MM-dd";
        } else if (dateStr.matches("\\d{1,4}/\\d{1,2}/\\d{1,2}\\s+\\d{1,2}:\\d{1,2}:\\d{1,2}\\.\\d{1,3}")) {
            inputFormat = "yyyy/MM/dd HH:mm:ss.SSS";
        } else if (dateStr.matches("\\d{4}/\\d{1,2}/\\d{1,2} +\\d{1,2}:\\d{1,2}")) {
            inputFormat = "yyyy/MM/dd HH:mm:ss";
        } else if (dateStr.matches("\\d{4}/\\d{1,2}/\\d{1,2} +\\d{1,2}:\\d{1,2}")) {
            inputFormat = "yyyy/MM/dd HH:mm";
        } else if (dateStr.matches("\\d{4}/\\d{1,2}/\\d{1,2} +\\d{1,2}")) {
            inputFormat = "yyyy/MM/dd HH";
        } else if (dateStr.matches("\\d{4}/\\d{1,2}/\\d{1,2} +\\d{1,2}")) {
            inputFormat = "yyyy/MM/dd";
        }
        resultStr = formatDate(dateStr, inputFormat, format);
        return resultStr;
    }

    public static String formatDate(Date date, String format) {
        SimpleDateFormat sdf = new SimpleDateFormat(format);
        return sdf.format(date);
    }

    public static String convertString(String str, String defaults) {
        if (str == null) {
            return defaults;
        } else {
            return str;
        }
    }

    public static int convertInt(String str, int defaults) {
        if (str == null) {
            return defaults;
        }
        try {
            return Integer.parseInt(str);
        } catch (Exception e) {
            return defaults;
        }
    }

    public static long convertLong(String str, long defaults) {
        if (str == null) {
            return defaults;
        }
        try {
            return Long.parseLong(str);
        } catch (Exception e) {
            return defaults;
        }
    }

    public static double convertDouble(String str, double defaults) {
        if (str == null) {
            return defaults;
        }
        try {
            return Double.parseDouble(str);
        } catch (Exception e) {
            return defaults;
        }
    }

    public static short convertShort(String str, short defaults) {
        if (str == null) {
            return defaults;
        }
        try {
            return Short.parseShort(str);
        } catch (Exception e) {
            return defaults;
        }
    }

    public static float convertFloat(String str, float defaults) {
        if (str == null) {
            return defaults;
        }
        try {
            return Float.parseFloat(str);
        } catch (Exception e) {
            return defaults;
        }
    }

    public static boolean convertBoolean(String str, boolean defaults) {
        if (str == null) {
            return defaults;
        }
        try {
            return Boolean.parseBoolean(str);
        } catch (Exception e) {
            return defaults;
        }
    }

    public static String[] split(String line, String seperator) {
        if (line == null || seperator == null || seperator.length() == 0) return null;
        ArrayList<String> list = new ArrayList<String>();
        int pos1 = 0;
        int pos2;
        for (;;) {
            pos2 = line.indexOf(seperator, pos1);
            if (pos2 < 0) {
                list.add(line.substring(pos1));
                break;
            }
            list.add(line.substring(pos1, pos2));
            pos1 = pos2 + seperator.length();
        }
        for (int i = list.size() - 1; i >= 0 && list.get(i).length() == 0; --i) {
            list.remove(i);
        }
        return list.toArray(new String[0]);
    }

    public static int[] splitInt(String line, String seperator, int def) {
        String[] ss = split(line, seperator);
        int[] r = new int[ss.length];
        for (int i = 0; i < r.length; ++i) {
            r[i] = convertInt(ss[i], def);
        }
        return r;
    }

    public static long[] splitLong(String line, String seperator, long def) {
        String[] ss = split(line, seperator);
        long[] r = new long[ss.length];
        for (int i = 0; i < r.length; ++i) {
            r[i] = convertLong(ss[i], def);
        }
        return r;
    }

    public static String join(String separator, String[] s) {
        return joinArray(separator, s);
    }

    public static String joinArray(String separator, Object[] s) {
        if (s == null || s.length == 0) return "";
        StringBuilder sb = new StringBuilder();
        sb.append(s[0]);
        for (int i = 1; i < s.length; ++i) {
            if (s[i] != null) {
                sb.append(separator);
                sb.append(s[i].toString());
            }
        }
        return sb.toString();
    }

    public static String joinArray(String separator, int[] s) {
        if (s == null || s.length == 0) return "";
        StringBuilder sb = new StringBuilder();
        sb.append(s[0]);
        for (int i = 1; i < s.length; ++i) {
            sb.append(separator);
            sb.append(s[i]);
        }
        return sb.toString();
    }

    public static String joinArray(String separator, long[] s) {
        if (s == null || s.length == 0) return "";
        StringBuilder sb = new StringBuilder();
        sb.append(s[0]);
        for (int i = 1; i < s.length; ++i) {
            sb.append(separator);
            sb.append(s[i]);
        }
        return sb.toString();
    }

    public static String join(String separator, Object... c) {
        return joinArray(separator, c);
    }

    public static String replaceAll(String s, String src, String dest) {
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

    public static String replaceFirst(String s, String src, String dest) {
        if (s == null || src == null || dest == null || src.length() == 0) return s;
        int pos = s.indexOf(src);
        if (pos < 0) {
            return s;
        }
        StringBuilder sb = new StringBuilder(s.length() - src.length() + dest.length());

        sb.append(s, 0, pos);
        sb.append(dest);
        sb.append(s, pos + src.length(), s.length());
        return sb.toString();
    }

    public static boolean isEmpty(String s) {
        if (s == null) return true;
        return s.trim().isEmpty();
    }

    public static boolean isNotEmpty(String s) {
        return !isEmpty(s);
    }

    public static String trim(String s) {
        if (s == null) return null;
        return s.trim();
    }

    public static String removeAll(String s, String src) {
        return replaceAll(s, src, "");
    }

    public static byte[] getUTF8Bytes(String s) {
        if (s != null && s.length() >= 0) {
            return s.getBytes(UTF_8);
        }
        return null;
    }

    public static byte[] getGBKBytes(String s) {
        if (s != null && s.length() >= 0) {
            return s.getBytes(GBK);
        }
        return null;
    }

    public static String getUTF8String(byte[] b) {
        if (b != null) {
            return new String(b, UTF_8);
        }
        return null;
    }

    public static String getGBKString(byte[] b) {
        if (b != null) {
            return new String(b, GBK);
        }
        return null;
    }

    public static String URLEncodeGBK(String s) {
        if (s != null && s.length() > 0) {
            try {
                return URLEncoder.encode(s, ENC_GBK);
            } catch (UnsupportedEncodingException e) {
            }
        }
        return s;
    }

    public static String URLEncodeUTF8(String s) {
        if (s != null && s.length() > 0) {
            try {
                return URLEncoder.encode(s, ENC_UTF8);
            } catch (UnsupportedEncodingException e) {
            }
        }
        return s;
    }

    public static String URLDecodeGBK(String s) {
        if (s != null && s.length() > 0) {
            try {
                return URLDecoder.decode(s, ENC_GBK);
            } catch (UnsupportedEncodingException e) {
            }
        }
        return s;
    }

    public static String URLDecodeUTF8(String s) {
        if (s != null && s.length() > 0) {
            try {
                return URLDecoder.decode(s, ENC_UTF8);
            } catch (UnsupportedEncodingException e) {
            }
        }
        return s;
    }
}
