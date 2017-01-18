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

package com.qq.tars.net.core;

import java.nio.ByteBuffer;

public final class IoBuffer {

    private ByteBuffer buf = null;

    private IoBuffer(ByteBuffer buf) {
        this.buf = buf;
    }

    public static IoBuffer wrap(ByteBuffer buf) {
        return new IoBuffer(buf);
    }

    public static IoBuffer wrap(byte[] buf) {
        return new IoBuffer(ByteBuffer.wrap(buf));
    }

    public static IoBuffer allocate(int capacity) {
        return new IoBuffer(ByteBuffer.allocate(capacity));
    }

    public static IoBuffer allocateDirect(int capacity) {
        return new IoBuffer(ByteBuffer.allocateDirect(capacity));
    }

    public byte get() {
        return this.buf.get();
    }

    public IoBuffer get(byte[] dst) {
        this.buf.get(dst);
        return this;
    }

    public IoBuffer get(byte[] dst, int offset, int length) {
        this.buf.get(dst, offset, length);
        return this;
    }

    public short getShort() {
        return this.buf.getShort();
    }

    public int getInt() {
        return this.buf.getInt();
    }

    public IoBuffer put(byte value) {
        return put(new byte[] { value });
    }

    public IoBuffer put(byte[] src) {
        return put(src, 0, src.length);
    }

    public IoBuffer put(byte[] src, int offset, int length) {
        autoExpand(src.length, offset, length);
        this.buf.put(src, offset, length);
        return this;
    }

    public IoBuffer put(char c) {
        this.buf.putChar(c);
        return this;
    }

    public IoBuffer putShort(short value) {
        this.buf.putShort(value);
        return this;
    }

    public IoBuffer put(int value) {
        this.buf.putInt(value);
        return this;
    }

    public final IoBuffer flip() {
        this.buf.flip();
        return this;
    }

    public final byte get(int index) {
        return this.buf.get(index);
    }

    public final IoBuffer unflip() {
        buf.position(buf.limit());
        if (buf.limit() != buf.capacity()) buf.limit(buf.capacity());
        return this;
    }

    public final IoBuffer rewind() {
        this.buf.rewind();
        return this;
    }

    public final int remaining() {
        return this.buf.remaining();
    }

    public final IoBuffer mark() {
        this.buf.mark();
        return this;
    }

    public final IoBuffer reset() {
        this.buf.reset();
        return this;
    }

    public final int position() {
        return this.buf.position();
    }

    public final IoBuffer position(int newPosition) {
        this.buf.position(newPosition);
        return this;
    }

    public final IoBuffer duplicate() {
        return IoBuffer.wrap(this.buf.duplicate());
    }

    public final IoBuffer compact() {
        this.buf.compact();
        return this;
    }

    public final ByteBuffer buf() {
        return this.buf;
    }

    private void autoExpand(int size, int offset, int length) {
        int newCapacity = this.buf.capacity();
        int newSize = this.buf.position() + length;
        ByteBuffer newBuffer = null;

        if (size < length) throw new IndexOutOfBoundsException();

        while (newSize > newCapacity) {
            newCapacity = newCapacity * 2;
        }

        if (newCapacity != this.buf.capacity()) {
            if (this.buf.isDirect()) {
                newBuffer = ByteBuffer.allocateDirect(newCapacity);
            } else {
                newBuffer = ByteBuffer.allocate(newCapacity);
            }

            newBuffer.put(this.buf.array());
            newBuffer.position(this.buf.position());

            this.buf = newBuffer;
        }
    }
}
