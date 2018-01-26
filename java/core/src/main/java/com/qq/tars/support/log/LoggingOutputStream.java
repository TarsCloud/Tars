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

import java.io.IOException;
import java.io.OutputStream;

public class LoggingOutputStream extends OutputStream {

    protected static final String LINE_SEPERATOR = System.getProperty("line.separator");

    protected boolean hasBeenClosed = false;

    protected byte[] buf;

    protected int count;

    private int bufLength;

    private Logger output = null;

    public static final int DEFAULT_BUFFER_LENGTH = 2048;

    public LoggingOutputStream(Logger output) {
        this.output = output;
        this.bufLength = DEFAULT_BUFFER_LENGTH;
        this.buf = new byte[DEFAULT_BUFFER_LENGTH];
        this.count = 0;
    }

    public void close() {
        _flush(true);
        hasBeenClosed = true;
    }

    public void flush() {
        _flush(false);
    }

    private void reset() {
        count = 0;
    }

    @Override
    public void write(int b) throws IOException {
        if (hasBeenClosed) {
            throw new IOException("The stream has been closed.");
        }

        if (b == 0) return;

        if (count == bufLength) {
            final int newBufLength = bufLength + DEFAULT_BUFFER_LENGTH;
            final byte[] newBuf = new byte[newBufLength];
            System.arraycopy(buf, 0, newBuf, 0, bufLength);
            buf = newBuf;
            bufLength = newBufLength;
        }

        buf[count] = (byte) b;
        count++;

    }

    private void _flush(boolean writeToFile) {
        if (count == 0) {
            if (writeToFile) doWriteLog();
            return;
        }

        if (count == LINE_SEPERATOR.length()) {
            if (((char) buf[0]) == LINE_SEPERATOR.charAt(0) && ((count == 1) || ((count == 2) && ((char) buf[1]) == LINE_SEPERATOR.charAt(1)))) {
                reset();
                return;
            }
        }

        final byte[] theBytes = new byte[count];
        System.arraycopy(buf, 0, theBytes, 0, count);
        this.output.info(new String(theBytes));

        if (writeToFile) doWriteLog();

        reset();
    }

    private void doWriteLog() {
        try {
            this.output.doWriteLog();
        } catch (Exception ex) {
        };
    }
}
