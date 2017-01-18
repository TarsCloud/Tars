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

package com.qq.tars.service;

import com.google.common.base.Preconditions;
import org.apache.commons.lang3.StringUtils;
import org.apache.commons.lang3.tuple.Triple;

public class SetTriple {

    private String setName;
    private String setArea;
    private String setGroup;

    private SetTriple() {

    }

    public String getSetName() {
        return setName;
    }

    public void setSetName(String setName) {
        this.setName = setName;
    }

    public String getSetArea() {
        return setArea;
    }

    public void setSetArea(String setArea) {
        this.setArea = setArea;
    }

    public String getSetGroup() {
        return setGroup;
    }

    public void setSetGroup(String setGroup) {
        this.setGroup = setGroup;
    }

    public static SetTriple parseSet(String set) {
        Preconditions.checkArgument(null != set, "set is null");

        String[] tokens = StringUtils.split(set, '.');
        if (tokens.length != 3 || StringUtils.isAnyBlank(tokens)) {
            throw new IllegalArgumentException(String.format("set=%s", set));
        }
        return SetTriple.formatSet(tokens[0], tokens[1], tokens[2]);
    }

    public static SetTriple formatSet(String setName, String setArea, String setGroup) {
        Preconditions.checkArgument(StringUtils.isNoneBlank(setName, setArea, setGroup),
                "setName=%s, setArea=%s, setGroup=%s",
                setName, setArea, setGroup);
        SetTriple triple = new SetTriple();
        triple.setSetName(setName);
        triple.setSetArea(setArea);
        triple.setSetGroup(setGroup);
        return triple;
    }

    @Override
    public String toString() {
        return getTriple().toString();
    }

    public String getString() {
        return String.format("%s.%s.%s", setName, setArea, setGroup);
    }

    public Triple getTriple() {
        return Triple.of(setName, setArea, setGroup);
    }

    public String[] getArray() {
        return new String[]{setName, setArea, setGroup};
    }

}
