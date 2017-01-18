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

package com.qq.tars.protocol.tars;

@SuppressWarnings("serial")
public abstract class TarsStructBase implements java.io.Serializable {

    public static final byte BYTE = 0;
    public static final byte SHORT = 1;
    public static final byte INT = 2;
    public static final byte LONG = 3;
    public static final byte FLOAT = 4;
    public static final byte DOUBLE = 5;
    public static final byte STRING1 = 6;
    public static final byte STRING4 = 7;
    public static final byte MAP = 8;
    public static final byte LIST = 9;
    public static final byte STRUCT_BEGIN = 10;
    public static final byte STRUCT_END = 11;
    public static final byte ZERO_TAG = 12;
    public static final byte SIMPLE_LIST = 13;

    public static final int MAX_STRING_LENGTH = 100 * 1024 * 1024;

    public abstract void writeTo(TarsOutputStream os);

    public abstract void readFrom(TarsInputStream is);

    public void display(StringBuilder sb, int level) {
    }

    public void displaySimple(StringBuilder sb, int level) {
    }

    public TarsStructBase newInit() {
        return null;
    }

    public void recyle() {

    }

    public boolean containField(String name) {
        return false;
    }

    public Object getFieldByName(String name) {
        return null;
    }

    public void setFieldByName(String name, Object value) {
    }

    public byte[] toByteArray() {
        TarsOutputStream os = new TarsOutputStream();
        writeTo(os);
        return os.toByteArray();
    }

    public byte[] toByteArray(String encoding) {
        TarsOutputStream os = new TarsOutputStream();
        os.setServerEncoding(encoding);
        writeTo(os);
        return os.toByteArray();
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        display(sb, 0);
        return sb.toString();
    }

    public static String toDisplaySimpleString(TarsStructBase struct) {
        if (struct == null) {
            return null;
        }
        StringBuilder sb = new StringBuilder();
        struct.displaySimple(sb, 0);
        return sb.toString();
    }
}
