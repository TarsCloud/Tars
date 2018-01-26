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

package com.qq.tars.support.log;

import java.io.PrintWriter;
import java.io.Writer;
import java.util.ArrayList;

final class ArrayWriter extends PrintWriter {

    private ArrayList<String> alist;

    public ArrayWriter() {
        super(new NullWriter());
        alist = new ArrayList<String>();
    }

    @Override
    public void print(Object o) {
        alist.add(o.toString());
    }

    @Override
    public void print(char[] chars) {
        alist.add(new String(chars));
    }

    @Override
    public void print(String s) {
        alist.add(s);
    }

    @Override
    public void println(Object o) {
        alist.add(o.toString());
    }

    @Override
    public void println(char[] chars) {
        alist.add(new String(chars));
    }

    @Override
    public void println(String s) {
        alist.add(s);
    }

    @Override
    public void write(char[] chars) {
        alist.add(new String(chars));
    }

    @Override
    public void write(char[] chars, int off, int len) {
        alist.add(new String(chars, off, len));
    }

    @Override
    public void write(String s, int off, int len) {
        alist.add(s.substring(off, off + len));
    }

    @Override
    public void write(String s) {
        alist.add(s);
    }

    public String[] toStringArray() {
        int len = alist.size();
        String[] result = new String[len];
        for (int i = 0; i < len; i++) {
            result[i] = alist.get(i);
        }
        return result;
    }
}

class NullWriter extends Writer {

    @Override
    public void close() {
    }

    @Override
    public void flush() {
    }

    @Override
    public void write(char[] cbuf, int off, int len) {
    }
}
