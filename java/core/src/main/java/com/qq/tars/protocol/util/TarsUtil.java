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

package com.qq.tars.protocol.util;

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.List;

import com.qq.tars.protocol.tars.TarsStructBase;

public final class TarsUtil {

    private static final int iConstant = 37;

    private static final int iTotal = 17;

    public static boolean equals(boolean l, boolean r) {
        return l == r;
    }

    public static boolean equals(byte l, byte r) {
        return l == r;
    }

    public static boolean equals(char l, char r) {
        return l == r;
    }

    public static boolean equals(short l, short r) {
        return l == r;
    }

    public static boolean equals(int l, int r) {
        return l == r;
    }

    public static boolean equals(long l, long r) {
        return l == r;
    }

    public static boolean equals(float l, float r) {
        return l == r;
    }

    public static boolean equals(double l, double r) {
        return l == r;
    }

    public static boolean equals(Object l, Object r) {
        return (l == null && r == null) || (l != null && l.equals(r));
    }

    public static int compareTo(boolean l, boolean r) {
        return (l ? 1 : 0) - (r ? 1 : 0);
    }

    public static int compareTo(byte l, byte r) {
        return l < r ? -1 : (l > r ? 1 : 0);
    }

    public static int compareTo(char l, char r) {
        return l < r ? -1 : (l > r ? 1 : 0);
    }

    public static int compareTo(short l, short r) {
        return l < r ? -1 : (l > r ? 1 : 0);
    }

    public static int compareTo(int l, int r) {
        return l < r ? -1 : (l > r ? 1 : 0);
    }

    public static int compareTo(long l, long r) {
        return l < r ? -1 : (l > r ? 1 : 0);
    }

    public static int compareTo(float l, float r) {
        return l < r ? -1 : (l > r ? 1 : 0);
    }

    public static int compareTo(double l, double r) {
        return l < r ? -1 : (l > r ? 1 : 0);
    }

    public static <T extends Comparable<T>> int compareTo(T l, T r) {
        return l.compareTo(r);
    }

    public static <T extends Comparable<T>> int compareTo(List<T> l, List<T> r) {
        Iterator<T> li = l.iterator(), ri = r.iterator();
        for (; li.hasNext() && ri.hasNext();) {
            int n = li.next().compareTo(ri.next());
            if (n != 0) return n;
        }
        return compareTo(li.hasNext(), ri.hasNext());
    }

    public static <T extends Comparable<T>> int compareTo(T[] l, T[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = l[li].compareTo(r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(boolean[] l, boolean[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(byte[] l, byte[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(char[] l, char[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(short[] l, short[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(int[] l, int[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(long[] l, long[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(float[] l, float[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int compareTo(double[] l, double[] r) {
        for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
            int n = compareTo(l[li], r[ri]);
            if (n != 0) return n;
        }
        return compareTo(l.length, r.length);
    }

    public static int hashCode(boolean o) {
        return iTotal * iConstant + (o ? 0 : 1);
    }

    public static int hashCode(boolean[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + (array[i] ? 0 : 1);
            }
            return tempTotal;
        }
    }

    public static int hashCode(byte o) {
        return iTotal * iConstant + o;
    }

    public static int hashCode(byte[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + array[i];
            }
            return tempTotal;
        }
    }

    public static int hashCode(char o) {
        return iTotal * iConstant + o;
    }

    public static int hashCode(char[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + array[i];
            }
            return tempTotal;
        }
    }

    public static int hashCode(double o) {
        return hashCode(Double.doubleToLongBits(o));
    }

    public static int hashCode(double[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + ((int) (Double.doubleToLongBits(array[i]) ^ (Double.doubleToLongBits(array[i]) >> 32)));
            }
            return tempTotal;
        }
    }

    public static int hashCode(float o) {
        return iTotal * iConstant + Float.floatToIntBits(o);
    }

    public static int hashCode(float[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + Float.floatToIntBits(array[i]);
            }
            return tempTotal;
        }
    }

    public static int hashCode(short o) {
        return iTotal * iConstant + o;
    }

    public static int hashCode(short[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + array[i];
            }
            return tempTotal;
        }
    }

    public static int hashCode(int o) {
        return iTotal * iConstant + o;
    }

    public static int hashCode(int[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + array[i];
            }
            return tempTotal;
        }
    }

    public static int hashCode(long o) {
        return iTotal * iConstant + ((int) (o ^ (o >> 32)));
    }

    public static int hashCode(long[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + ((int) (array[i] ^ (array[i] >> 32)));
            }
            return tempTotal;
        }
    }

    public static int hashCode(TarsStructBase[] array) {
        if (array == null) {
            return iTotal * iConstant;
        } else {
            int tempTotal = iTotal;
            for (int i = 0; i < array.length; i++) {
                tempTotal = tempTotal * iConstant + (array[i].hashCode());
            }
            return tempTotal;
        }
    }

    public static int hashCode(Object object) {
        if (object == null) {
            return iTotal * iConstant;
        } else {
            if (object.getClass().isArray()) {
                if (object instanceof long[]) {
                    return hashCode((long[]) object);
                } else if (object instanceof int[]) {
                    return hashCode((int[]) object);
                } else if (object instanceof short[]) {
                    return hashCode((short[]) object);
                } else if (object instanceof char[]) {
                    return hashCode((char[]) object);
                } else if (object instanceof byte[]) {
                    return hashCode((byte[]) object);
                } else if (object instanceof double[]) {
                    return hashCode((double[]) object);
                } else if (object instanceof float[]) {
                    return hashCode((float[]) object);
                } else if (object instanceof boolean[]) {
                    return hashCode((boolean[]) object);
                } else if (object instanceof TarsStructBase[]) {
                    return hashCode((TarsStructBase[]) object);
                } else {
                    return hashCode((Object[]) object);
                }
            } else if (object instanceof TarsStructBase) {
                return object.hashCode();
            } else {
                return iTotal * iConstant + object.hashCode();
            }
        }
    }

    public static byte[] getBufArray(ByteBuffer buffer) {
        byte[] bytes = new byte[buffer.position()];
        System.arraycopy(buffer.array(), 0, bytes, 0, bytes.length);
        return bytes;
    }

    private static final byte[] highDigits;

    private static final byte[] lowDigits;

    static {
        final byte[] digits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

        int i;
        byte[] high = new byte[256];
        byte[] low = new byte[256];

        for (i = 0; i < 256; i++) {
            high[i] = digits[i >>> 4];
            low[i] = digits[i & 0x0F];
        }

        highDigits = high;
        lowDigits = low;
    }

    public static String getHexdump(byte[] array) {
        return getHexdump(ByteBuffer.wrap(array));
    }

    public static String getHexdump(ByteBuffer in) {
        int size = in.remaining();
        if (size == 0) {
            return "empty";
        }
        StringBuilder out = new StringBuilder((in.remaining() * 3) - 1);
        int mark = in.position();
        int byteValue = in.get() & 0xFF;
        out.append((char) highDigits[byteValue]);
        out.append((char) lowDigits[byteValue]);
        size--;
        for (; size > 0; size--) {
            out.append(' ');
            byteValue = in.get() & 0xFF;
            out.append((char) highDigits[byteValue]);
            out.append((char) lowDigits[byteValue]);
        }
        in.position(mark);
        return out.toString();
    }

}
