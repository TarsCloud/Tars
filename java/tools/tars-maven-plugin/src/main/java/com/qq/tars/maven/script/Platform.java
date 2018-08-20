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
package com.qq.tars.maven.script;

import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.codehaus.plexus.util.StringUtils;

import com.qq.tars.maven.model.ClasspathElement;
import com.qq.tars.maven.model.Dependency;
import com.qq.tars.maven.model.Directory;

public class Platform {

    public static final String UNIX_NAME = "unix";

    public static final String WINDOWS_NAME = "windows";

    private static final Map<String, Platform> ALL_PLATFORMS;

    private static final String DEFAULT_UNIX_BIN_FILE_EXTENSION = "";

    private static final String DEFAULT_WINDOWS_BIN_FILE_EXTENSION = ".bat";

    private String binFileExtension;

    private String name;

    private boolean isWindows;

    private boolean showConsoleWindow = true;

    static {
        ALL_PLATFORMS = new HashMap<String, Platform>();
        addPlatform(new Platform(UNIX_NAME, false, DEFAULT_UNIX_BIN_FILE_EXTENSION));
        addPlatform(new Platform(WINDOWS_NAME, true, DEFAULT_WINDOWS_BIN_FILE_EXTENSION));
    }

    private static Platform addPlatform(Platform platform) {
        ALL_PLATFORMS.put(platform.name, platform);
        return platform;
    }

    public static Platform getInstance(String platformName) throws Exception {
        Platform platform = ALL_PLATFORMS.get(platformName);

        if (platform == null) {
            throw new Exception("Unknown platform name '" + platformName + "'");
        }

        return platform;
    }

    public static Set<String> getAllPlatformNames() {
        return ALL_PLATFORMS.keySet();
    }

    public static Set<Platform> getAllPlatforms() {
        return new HashSet<Platform>(ALL_PLATFORMS.values());
    }

    public static Set<Platform> getPlatformSet(List<String> platformList) throws Exception {
        return getPlatformSet(platformList, new HashSet<Platform>(ALL_PLATFORMS.values()));
    }

    public static Set<Platform> getPlatformSet(List<String> platformList, Set<Platform> allSet) throws Exception {
        if (platformList == null) {
            return allSet;
        }

        if (platformList.size() == 1) {
            String first = platformList.get(0);

            if ("all".equals(first)) {
                return allSet;
            }
            throw new Exception("The special platform 'all' can only be used if it is the only element in the platform list.");
        }

        Set<Platform> platformSet = new HashSet<Platform>();

        for (String platformName : platformList) {
            if (platformName.equals("all")) {
                throw new Exception("The special platform 'all' can only be used if it is the only element in a platform list.");
            }
            platformSet.add(getInstance(platformName));
        }

        return platformSet;
    }

    private Platform(String name, boolean isWindows, String binFileExtension) {
        this.name = name;

        this.isWindows = isWindows;

        this.binFileExtension = binFileExtension;
    }

    public String getInterpolationToken() {
        return isWindows ? "#" : "@";
    }

    public String getBinFileExtension() {
        return binFileExtension;
    }

    public String getBasedir() {
        return isWindows ? "\"%BASEDIR%\"" : "\"$BASEDIR\"";
    }

    public String getRepo() {
        return isWindows ? "\"%REPO%\"" : "\"$REPO\"";
    }

    public String getSeparator() {
        return isWindows ? "\\" : "/";
    }

    public String getPathSeparator() {
        return isWindows ? ";" : ":";
    }

    public String getCommentPrefix() {
        return isWindows ? "@REM " : "# ";
    }

    public String getNewLine() {
        return isWindows ? "\r\n" : "\n";
    }

    public String getClassPath(List<? extends ClasspathElement> classpath) throws Exception {

        StringBuilder classpathBuffer = new StringBuilder();

        for (ClasspathElement classpathElement : classpath) {
            if (classpathBuffer.length() > 0) {
                classpathBuffer.append(getPathSeparator());
            }

            if (classpathElement instanceof Directory) {
                Directory directory = (Directory) classpathElement;
                if (directory.getRelativePath().charAt(0) != '/') {
                    classpathBuffer.append(getBasedir()).append(getSeparator());
                }
            } else if (classpathElement instanceof Dependency) {
                classpathBuffer.append(getSeparator());
            } else {
                throw new Exception("Unknown classpath element type: " + classpathElement.getClass().getName());
            }

            classpathBuffer.append(StringUtils.replace(classpathElement.getRelativePath(), "/", getSeparator()));
        }

        return classpathBuffer.toString();
    }

    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        Platform platform = (Platform) o;

        return name.equals(platform.name);
    }

    public int hashCode() {
        return name.hashCode();
    }

    public String getName() {
        return name;
    }

    public boolean isShowConsoleWindow() {
        return showConsoleWindow;
    }

    public void setBinFileExtension(String binFileExtension) {
        if (binFileExtension == null) {
            this.binFileExtension = "";
        } else {
            this.binFileExtension = binFileExtension;
        }
    }
}
