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
package com.qq.tars.maven.gensrc;

import org.apache.maven.plugins.annotations.Parameter;

public class Tars2JavaConfig {

    @Parameter(defaultValue = "${basedir}/src/main/java", required = true)
    public String srcPath;

    @Parameter(required = true)
    public String[] tarsFiles;

    @Parameter(defaultValue = "UTF-8", required = true)
    public String tarsFileCharset;

    @Parameter(defaultValue = "UTF-8")
    public String charset;

    @Parameter(defaultValue = "", required = true)
    public String packagePrefixName;

//    @Parameter
//    public boolean forceArray;

    @Parameter
    public boolean checkDefault;

    @Parameter
    public boolean servant;

    @Parameter
    public boolean tup;

    public String getSrcPath() {
        return srcPath;
    }

    public void setSrcPath(String srcPath) {
        this.srcPath = srcPath;
    }

    public String[] getTafFiles() {
        return tarsFiles;
    }

    public void setTafFiles(String[] tarsFiles) {
        this.tarsFiles = tarsFiles;
    }

    public String getTafFileCharset() {
        return tarsFileCharset;
    }

    public void setTafFileCharset(String tarsFileCharset) {
        this.tarsFileCharset = tarsFileCharset;
    }

    public String getCharset() {
        return charset;
    }

    public void setCharset(String charset) {
        this.charset = charset;
    }

    public String getPackagePrefixName() {
        return packagePrefixName;
    }

    public void setPackagePrefixName(String packagePrefixName) {
        this.packagePrefixName = packagePrefixName;
    }

//    public boolean isForceArray() {
//        return forceArray;
//    }
//
//    public void setForceArray(boolean forceArray) {
//        this.forceArray = forceArray;
//    }

    public boolean isCheckDefault() {
        return checkDefault;
    }

    public void setCheckDefault(boolean checkDefault) {
        this.checkDefault = checkDefault;
    }

    public boolean isServant() {
        return servant;
    }

    public void setServant(boolean servant) {
        this.servant = servant;
    }

    public boolean isTup() {
        return tup;
    }

    public void setTup(boolean tup) {
        this.tup = tup;
    }
}
