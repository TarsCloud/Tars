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

package com.qq.tars.client.rpc.loadbalance;

import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.client.util.Pair;
import com.qq.tars.common.util.Constants;
import com.qq.tars.rpc.common.Invoker;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.*;

public class LoadBalanceHelper {

    public static <T> List<Invoker<T>> buildStaticWeightList(Collection<Invoker<T>> invokers,
                                                             ServantProxyConfig config) {
        List<Invoker<T>> weightInvokers = new ArrayList<Invoker<T>>();
        for (Invoker<T> invoker : invokers) {
            if (invoker.getUrl().getParameter(Constants.TARS_CLIENT_WEIGHT_TYPE, 0) != 1) {
                return null;
            }
            if (invoker.getUrl().getParameter(Constants.TARS_CLIENT_WEIGHT, 0) > 0) weightInvokers.add(invoker);
        }

        if (weightInvokers.isEmpty()) {
            return null;
        }

        if (ClientLogger.getLogger().isDebugEnabled()) {
            ClientLogger.getLogger().debug("[buildStaticWeightList]: weightInvokers size: " + weightInvokers.size());
        }

        int minWeight = Integer.MAX_VALUE;
        int maxWeight = Integer.MIN_VALUE;

        for (Invoker<T> invoker : weightInvokers) {
            int tmpWeight = invoker.getUrl().getParameter(Constants.TARS_CLIENT_WEIGHT, 0);

            if (tmpWeight > maxWeight) maxWeight = tmpWeight;

            if (tmpWeight < minWeight) minWeight = tmpWeight;
        }

        int maxRange = maxWeight / minWeight;
        if (maxRange < config.getMinStaticWeightLimit()) maxRange = config.getMinStaticWeightLimit();

        if (maxRange > config.getMaxStaticWeightLimit()) maxRange = config.getMaxStaticWeightLimit();

        Comparator<Pair<Integer, Invoker<T>>> comparator = new WeightToInvokerComparator<T>();
        TreeSet<Pair<Integer, Invoker<T>>> weightToInvoker = new TreeSet<Pair<Integer, Invoker<T>>>(comparator);
        Map<Invoker<T>, Integer> invokerToWeight = new HashMap<Invoker<T>, Integer>();

        int totalWeight = 0;
        for (Invoker<T> invoker : weightInvokers) {
            int weight = (invoker.getUrl().getParameter(Constants.TARS_CLIENT_WEIGHT, 0) * maxRange) / maxWeight;
            totalWeight += weight;
            weightToInvoker.add(new Pair<Integer, Invoker<T>>(weight, invoker));
            invokerToWeight.put(invoker, weight);
            if (ClientLogger.getLogger().isDebugEnabled()) {
                ClientLogger.getLogger().debug("[buildStaticWeightList]: invoker: " + invoker.hashCode() + ", weight: " + weight + ", host: " + invoker.getUrl().getHost() + ", port: " + invoker.getUrl().getPort());
            }
        }

        List<Invoker<T>> result = new ArrayList<Invoker<T>>();
        for (int i = 0; i < totalWeight; i++) {
            boolean first = true;
            TreeSet<Pair<Integer, Invoker<T>>> weightToInvokerTmp = new TreeSet<Pair<Integer, Invoker<T>>>(comparator);
            Iterator<Pair<Integer, Invoker<T>>> it = weightToInvoker.descendingIterator();
            while (it.hasNext()) {
                Pair<Integer, Invoker<T>> pair = it.next();
                if (first) {
                    first = false;
                    result.add(pair.second);
                    weightToInvokerTmp.add(new Pair<Integer, Invoker<T>>(pair.first - totalWeight + invokerToWeight.get(pair.second), pair.second));
/*                    if (ClientLogger.getLogger().isDebugEnabled()) {
                        ClientLogger.getLogger().debug("[buildStaticWeightList]: select: " + pair.getFirst() + ", " + pair.getSecond().hashCode());
                    }*/
                } else {
                    weightToInvokerTmp.add(new Pair<Integer, Invoker<T>>(pair.first + invokerToWeight.get(pair.second), pair.second));
                }
            }
            weightToInvoker = weightToInvokerTmp;
        }

        return result;
    }

    private static class WeightToInvokerComparator<T> implements Comparator<Pair<Integer, Invoker<T>>> {

        @Override
        public int compare(Pair<Integer, Invoker<T>> o1, Pair<Integer, Invoker<T>> o2) {
            if (o1.first == o2.first) return o1.second.hashCode() - o2.second.hashCode();
            else return o1.first.compareTo(o2.first);
        }
    }

    public static <T> TreeMap<Long, Invoker<T>> buildConsistentHashCircle(Collection<Invoker<T>> invokers,
                                                                          ServantProxyConfig config) {
        List<Invoker<T>> weightInvokers = new ArrayList<Invoker<T>>();
        for (Invoker<T> invoker : invokers) {
            if (invoker.getUrl().getParameter(Constants.TARS_CLIENT_WEIGHT_TYPE, 0) != 1) {
                weightInvokers.clear();
                break;
            }
            if (invoker.getUrl().getParameter(Constants.TARS_CLIENT_WEIGHT, 0) > 0) weightInvokers.add(invoker);
        }

        TreeMap<Long, Invoker<T>> result = new TreeMap<Long, Invoker<T>>();
        try {
            boolean staticWeight = !weightInvokers.isEmpty();
            Collection<Invoker<T>> srcInvokers = staticWeight ? weightInvokers : invokers;

            for (Invoker<T> invoker : srcInvokers) {
                int replicaNumber = staticWeight ? invoker.getUrl().getParameter(Constants.TARS_CLIENT_WEIGHT, 0) : config.getDefaultConHashVirtualNodes();
                if (replicaNumber > config.getDefaultConHashVirtualNodes())
                    replicaNumber = config.getDefaultConHashVirtualNodes();

                replicaNumber = replicaNumber / 4 <= 0 ? 1 : replicaNumber / 4;
                for (int i = 0; i < replicaNumber; i++) {
                    byte[] digest = md5(invoker.getUrl().toIdentityString() + i);
                    for (int h = 0; h < 4; h++) {
                        long m = hash(digest, h);
                        result.put(m, invoker);
                    }
                }
            }

//            if (ClientLogger.getLogger().isDebugEnabled()) {
//                StringBuilder sb = new StringBuilder("consistent hash circle:");
//                for (Entry<Long, Invoker<T>> entry : result.entrySet()) {
//                    sb.append(entry.getKey()).append("-").append(entry.getValue().getUrl().toIdentityString()).append(", ");
//                }
//                ClientLogger.getLogger().debug(sb.toString());
//            }
        } catch (Exception e) {
            ClientLogger.getLogger().error("build consistent hash circle err. ", e);
            return null;
        }
        return result;
    }

    private static byte[] md5(String value) {
        MessageDigest md5;
        try {
            md5 = MessageDigest.getInstance("MD5");
        } catch (NoSuchAlgorithmException e) {
            throw new IllegalStateException(e.getMessage(), e);
        }
        md5.reset();
        byte[] bytes = null;
        try {
            bytes = value.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            throw new IllegalStateException(e.getMessage(), e);
        }
        md5.update(bytes);
        return md5.digest();
    }

    private static long hash(byte[] digest, int number) {
        return (((long) (digest[3 + number * 4] & 0xFF) << 24) | ((long) (digest[2 + number * 4] & 0xFF) << 16) | ((long) (digest[1 + number * 4] & 0xFF) << 8) | (digest[0 + number * 4] & 0xFF)) & 0xFFFFFFFFL;
    }
}
