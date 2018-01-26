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
package com.qq.tars.maven.model;

@SuppressWarnings("all")
public class Dependency extends ClasspathElement implements java.io.Serializable {

    private String groupId;
    private String artifactId;
    private String version;
    private String classifier;

    public Dependency() {
    }

    public Dependency(String groupId, String artifactId, String version, String classifier) {
        this.groupId = groupId;
        this.artifactId = artifactId;
        this.version = version;
        this.classifier = classifier;
    }

    public String getArtifactId() {
        return this.artifactId;
    }

    public String getClassifier() {
        return this.classifier;
    }

    public String getGroupId() {
        return this.groupId;
    }

    public String getVersion() {
        return this.version;
    }

    public void setArtifactId(String artifactId) {
        this.artifactId = artifactId;
    }

    public void setClassifier(String classifier) {
        this.classifier = classifier;
    }

    public void setGroupId(String groupId) {
        this.groupId = groupId;
    }

    public void setVersion(String version) {
        this.version = version;
    }
}
