package com.qq.tars.client.util;

import java.io.Serializable;

/**
 * @author meteor
 * 
 * @param <F>
 * @param <S>
 */
public final class Pair<F, S> implements Serializable {

    /**
     * 
     */
    private static final long serialVersionUID = -1037592552817068942L;
    public F first;
    public S second;

    public Pair() {
    }

    public Pair(F f, S s) {
        this.first = f;
        this.second = s;
    }

    private static <T> boolean eq(T o1, T o2) {
        return o1 == null ? o2 == null : o1.equals(o2);
    }

    public boolean equals(Object o) {
        if (o == null) return false;
        if (!(o instanceof Pair)) return false;
        @SuppressWarnings("unchecked")
        Pair<F, S> pr = (Pair<F, S>) o;
        return eq(first, pr.first) && eq(second, pr.second);
    }

    private static int h(Object o) {
        return o == null ? 0 : o.hashCode();
    }

    public int hashCode() {
        int seed = h(first);
        seed ^= h(second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }

    public F getFirst() {
        return first;
    }

    public S getSecond() {
        return second;
    }

    public void setFirst(F first) {
        this.first = first;
    }

    public void setSecond(S second) {
        this.second = second;
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("{").append(first).append(", ").append(second).append("}");
        return sb.toString();
    }
}
