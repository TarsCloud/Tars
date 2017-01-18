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
package com.qq.tars.maven.util;

import java.util.ArrayList;
import java.util.List;

import org.apache.maven.plugin.logging.Log;
import org.codehaus.plexus.util.StringUtils;

public class FilePermissionUtils {

    private FilePermissionUtils() {
    }

    public static FilePermission getFilePermissionFromMode(String mode, Log logger) {
        if (StringUtils.isBlank(mode)) {
            throw new IllegalArgumentException(" file mode cannot be empty");
        }
        if (mode.length() != 3 && mode.length() != 4) {
            throw new IllegalArgumentException(" file mode must be 3 or 4 characters");
        }

        List modes = new ArrayList(mode.length());
        for (int i = 0, size = mode.length(); i < size; i++) {
            modes.add(String.valueOf(mode.charAt(i)));
        }

        boolean executable = false, ownerOnlyExecutable = true, ownerOnlyReadable = true, readable = false, ownerOnlyWritable = true, writable = false;

        // handle user perm
        try {
            int userMode = Integer.valueOf((String) modes.get(mode.length() == 4 ? 1 : 0)).intValue();
            switch (userMode) {
                case 0:
                    break;
                case 1:
                    executable = true;
                    break;
                case 2:
                    writable = true;
                    break;
                case 3:
                    writable = true;
                    executable = true;
                    break;
                case 4:
                    readable = true;
                    break;
                case 5:
                    readable = true;
                    executable = true;
                    break;
                case 6:
                    readable = true;
                    writable = true;
                    break;
                case 7:
                    writable = true;
                    readable = true;
                    executable = true;
                    break;
                default:
                    logger.warn("ignore file mode " + userMode);
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException(" file mode must contains only number " + mode);
        }

        try {
            int allMode = Integer.valueOf((String) modes.get(mode.length() == 4 ? 3 : 2)).intValue();
            switch (allMode) {
                case 0:
                    break;
                case 1:
                    executable = true;
                    ownerOnlyExecutable = false;
                    break;
                case 2:
                    writable = true;
                    ownerOnlyWritable = false;
                    break;
                case 3:
                    writable = true;
                    executable = true;
                    ownerOnlyExecutable = false;
                    ownerOnlyWritable = false;
                    break;
                case 4:
                    readable = true;
                    ownerOnlyReadable = false;
                    break;
                case 5:
                    readable = true;
                    executable = true;
                    ownerOnlyReadable = false;
                    ownerOnlyExecutable = false;
                    break;
                case 6:
                    readable = true;
                    ownerOnlyReadable = false;
                    writable = true;
                    ownerOnlyWritable = false;
                    break;
                case 7:
                    writable = true;
                    readable = true;
                    executable = true;
                    ownerOnlyReadable = false;
                    ownerOnlyExecutable = false;
                    ownerOnlyWritable = false;
                    break;
                default:
                    logger.warn("ignore file mode " + allMode);
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException(" file mode must contains only number " + mode);
        }
        return new FilePermission(executable, ownerOnlyExecutable, ownerOnlyReadable, readable, ownerOnlyWritable, writable);
    }
}
