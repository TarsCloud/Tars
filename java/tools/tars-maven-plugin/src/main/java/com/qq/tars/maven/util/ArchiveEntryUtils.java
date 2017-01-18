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

import java.io.File;
import java.lang.reflect.Method;

import org.apache.maven.plugin.logging.Log;
import org.codehaus.plexus.archiver.ArchiverException;
import org.codehaus.plexus.util.Os;
import org.codehaus.plexus.util.cli.CommandLineException;
import org.codehaus.plexus.util.cli.CommandLineUtils;
import org.codehaus.plexus.util.cli.Commandline;

public final class ArchiveEntryUtils {

    public static boolean jvmFilePermAvailable = false;

    static {
        try {
            jvmFilePermAvailable = File.class.getMethod("setReadable", new Class[] { Boolean.TYPE }) != null;
        } catch (final Exception e) {
        }
    }

    private ArchiveEntryUtils() {
    }

    public static void chmod(final File file, final int mode, final Log logger, boolean useJvmChmod) throws ArchiverException {
        if (!Os.isFamily(Os.FAMILY_UNIX)) {
            return;
        }

        final String m = Integer.toOctalString(mode & 0xfff);

        if (useJvmChmod && !jvmFilePermAvailable) {
            logger.info("chmod it's not possible where your current jvm");
            useJvmChmod = false;
        }

        if (useJvmChmod && jvmFilePermAvailable) {
            applyPermissionsWithJvm(file, m, logger);
            return;
        }

        try {
            final Commandline commandline = new Commandline();

            commandline.setWorkingDirectory(file.getParentFile().getAbsolutePath());

            if (logger.isDebugEnabled()) {
                logger.debug(file + ": mode " + Integer.toOctalString(mode) + ", chmod " + m);
            }

            commandline.setExecutable("chmod");

            commandline.createArg().setValue(m);

            final String path = file.getAbsolutePath();

            commandline.createArg().setValue(path);

            final CommandLineUtils.StringStreamConsumer stderr = new CommandLineUtils.StringStreamConsumer();

            final CommandLineUtils.StringStreamConsumer stdout = new CommandLineUtils.StringStreamConsumer();

            final int exitCode = CommandLineUtils.executeCommandLine(commandline, stderr, stdout);

            if (exitCode != 0) {
                logger.warn("-------------------------------");
                logger.warn("Standard error:");
                logger.warn("-------------------------------");
                logger.warn(stderr.getOutput());
                logger.warn("-------------------------------");
                logger.warn("Standard output:");
                logger.warn("-------------------------------");
                logger.warn(stdout.getOutput());
                logger.warn("-------------------------------");

                throw new ArchiverException("chmod exit code was: " + exitCode);
            }
        } catch (final CommandLineException e) {
            throw new ArchiverException("Error while executing chmod.", e);
        }

    }

    public static void chmod(final File file, final int mode, final Log logger) throws ArchiverException {
        chmod(file, mode, logger, Boolean.getBoolean("useJvmChmod") && jvmFilePermAvailable);
    }

    public static void applyPermissionsWithJvm(final File file, final String mode, final Log logger) {
        final FilePermission filePermission = FilePermissionUtils.getFilePermissionFromMode(mode, logger);

        Method method;
        try {
            method = File.class.getMethod("setReadable", new Class[] { Boolean.TYPE, Boolean.TYPE });

            method.invoke(file, new Object[] { Boolean.valueOf(filePermission.isReadable()), Boolean.valueOf(filePermission.isOwnerOnlyReadable()) });

            method = File.class.getMethod("setExecutable", new Class[] { Boolean.TYPE, Boolean.TYPE });
            method.invoke(file, new Object[] { Boolean.valueOf(filePermission.isExecutable()), Boolean.valueOf(filePermission.isOwnerOnlyExecutable()) });

            method = File.class.getMethod("setWritable", new Class[] { Boolean.TYPE, Boolean.TYPE });
            method.invoke(file, new Object[] { Boolean.valueOf(filePermission.isWritable()), Boolean.valueOf(filePermission.isOwnerOnlyWritable()) });
        } catch (final Exception e) {
            logger.error("error calling dynamically file permissons with jvm " + e.getMessage(), e);
            throw new RuntimeException("error calling dynamically file permissons with jvm " + e.getMessage(), e);
        }
    }
}
