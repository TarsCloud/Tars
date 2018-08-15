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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.regex.Pattern;

import org.apache.commons.io.FileUtils;
import org.apache.maven.artifact.Artifact;
import org.apache.maven.artifact.DefaultArtifact;
import org.apache.maven.artifact.factory.ArtifactFactory;
import org.apache.maven.artifact.metadata.ArtifactMetadataSource;
import org.apache.maven.artifact.repository.ArtifactRepository;
import org.apache.maven.artifact.repository.ArtifactRepositoryFactory;
import org.apache.maven.artifact.repository.layout.ArtifactRepositoryLayout;
import org.apache.maven.artifact.resolver.ArtifactCollector;
import org.apache.maven.artifact.resolver.ArtifactNotFoundException;
import org.apache.maven.artifact.resolver.ArtifactResolutionException;
import org.apache.maven.artifact.resolver.ArtifactResolver;
import org.apache.maven.artifact.resolver.filter.ArtifactFilter;
import org.apache.maven.artifact.resolver.filter.ScopeArtifactFilter;
import org.apache.maven.execution.MavenSession;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugin.MojoFailureException;
import org.apache.maven.plugins.annotations.Component;
import org.apache.maven.plugins.annotations.Execute;
import org.apache.maven.plugins.annotations.LifecyclePhase;
import org.apache.maven.plugins.annotations.Mojo;
import org.apache.maven.plugins.annotations.Parameter;
import org.apache.maven.plugins.annotations.ResolutionScope;
import org.apache.maven.project.MavenProject;
import org.apache.maven.project.MavenProjectBuilder;
import org.apache.maven.shared.dependency.tree.DependencyNode;
import org.apache.maven.shared.dependency.tree.DependencyTreeBuilder;
import org.apache.maven.shared.mapping.MappingUtils;
import org.codehaus.plexus.PlexusConstants;
import org.codehaus.plexus.PlexusContainer;
import org.codehaus.plexus.archiver.ArchiverException;
import org.codehaus.plexus.archiver.UnArchiver;
import org.codehaus.plexus.archiver.manager.ArchiverManager;
import org.codehaus.plexus.archiver.manager.NoSuchArchiverException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;
import org.codehaus.plexus.context.Context;
import org.codehaus.plexus.context.ContextException;
import org.codehaus.plexus.interpolation.InterpolationException;
import org.codehaus.plexus.personality.plexus.lifecycle.phase.Contextualizable;
import org.codehaus.plexus.util.IOUtil;
import org.codehaus.plexus.util.InterpolationFilterReader;

import com.qq.tars.maven.model.Dependency;
import com.qq.tars.maven.script.Platform;
import com.qq.tars.maven.util.ArchiveEntryUtils;
import com.qq.tars.maven.util.XMLConfigElement;
import com.qq.tars.maven.util.XMLConfigFile;

@Mojo(name = "build", defaultPhase = LifecyclePhase.PACKAGE, requiresDependencyResolution = ResolutionScope.RUNTIME, threadSafe = true)
@Execute(phase = LifecyclePhase.PACKAGE)
public class TarsBuildMojo extends AbstractMojo implements Contextualizable {

    @Parameter(defaultValue = "${project}")
    private MavenProject project;

    @Parameter(defaultValue = "${session}")
    private MavenSession session;

    @Parameter(alias = "", defaultValue = "${project.build.directory}/${project.build.finalName}")
    private File war;

    @Parameter
    protected String outputFileNameMapping;

    @Parameter(defaultValue = "default")
    protected String repositoryLayout;

    @Parameter(defaultValue = "false")
    protected boolean useTimestampInSnapshotFileName;

    @Parameter(defaultValue = "${localRepository}", readonly = true, required = true)
    protected ArtifactRepository localRepository;

    @Parameter(defaultValue = "${project.remoteArtifactRepositories}", readonly = true, required = true)
    protected List remoteRepositories;

    @Component
    private ArtifactFactory artifactFactory;
    @Component
    protected ArtifactResolver artifactResolver;
    @Component
    protected ArtifactRepositoryFactory artifactRepositoryFactory;
    @Component
    protected ArtifactMetadataSource artifactMetadataSource;
    @Component
    protected MavenProjectBuilder mavenProjectBuilder;
    @Component
    protected DependencyTreeBuilder dependencyTreeBuilder;
    @Component
    protected ArtifactCollector artifactCollector;
    @Component
    protected ArchiverManager archiverManager;

    protected PlexusContainer container;

    @Parameter
    private String platformName;

    @Parameter
    private String tarsGroupId;
    @Parameter
    private String tarsArtifactId;
    @Parameter
    private String tarsVersion;

    private static final String TARS_PROTOCOL = "tars";
    private static final String TUP_PROTOCOL = "tup";
    private static final String MAIN_CLASS = "com.qq.tars.server.startup.Main";

    // -----------------------------------------------------------------------
    // Plexus Implementation
    // -----------------------------------------------------------------------
    /**
     * {@inheritDoc}
     */
    public void contextualize(Context context) throws ContextException {
        container = (PlexusContainer) context.get(PlexusConstants.PLEXUS_KEY);
    }

    protected static final String SCOPE_DEFAULT_PATTERN = "^((?!test).)*$";
    private String includedScope = SCOPE_DEFAULT_PATTERN;

    private List<Dependency> dependencyList = new ArrayList<Dependency>();

    private Pattern pattern;

    protected boolean includedScope(final String scope) {
        if (pattern == null) {
            pattern = Pattern.compile(includedScope, Pattern.CASE_INSENSITIVE);
        }
        return scope != null ? pattern.matcher(scope).matches() : Boolean.TRUE;
    }

    public void execute() throws MojoExecutionException, MojoFailureException {
        getLog().info("Start run build...");
        try {
            if (platformName == null) {
                String osName = System.getProperty("os.name");
                if (osName.toLowerCase().contains(Platform.WINDOWS_NAME)) {
                    platformName = Platform.WINDOWS_NAME;
                } else {
                    platformName = Platform.UNIX_NAME;
                }
            }

            File targetDir = new File(project.getBuild().getDirectory());
            File tarsDir = new File(targetDir, "tars");
            if (tarsDir.exists()) {
                FileUtils.forceDelete(tarsDir);
            }
            tarsDir.mkdir();

            File binDir = new File(tarsDir, "bin");
            binDir.mkdir();

            File confDir = new File(tarsDir, "conf");
            confDir.mkdir();

            File dataDir = new File(tarsDir, "data");
            dataDir.mkdir();

            File binAppsDir = new File(binDir, "apps" + File.separator + "ROOT");
            binAppsDir.mkdir();
            FileUtils.copyDirectory(war, binAppsDir);

            File binConfDir = new File(binDir, "conf");
            binConfDir.mkdir();

            File binLogDir = new File(binDir, "log");
            binLogDir.mkdir();

            File libDir = new File(binDir, "lib");
            libDir.mkdir();

            String app = getApp();
            String server = getServer();

            if (app == null || server == null) {
                throw new MojoExecutionException("Failed to build app server is null.");
            }

            String configFileName = String.format("%s.%s.%s", app, server, "config.conf");
            File configFile = new File(confDir, configFileName);
            getLog().info(String.format("Create config file %s ", configFile.getCanonicalPath()));
            createConfigScript(binDir, dataDir, binLogDir, configFile);

            installLibDependency(libDir);

            getLog().info(String.format("Create platform %s start script...", platformName));
            Platform platform = Platform.getInstance(platformName);
            String binscript = createBinScript(platform, libDir, binDir, configFile);
            getLog().info("Start Script in " + binscript);
        } catch (Exception e) {
            getLog().error(e);
            throw new MojoExecutionException("Failed to tars build", e);
        }
    }

    private void installLibDependency(File libDir) throws Exception {
        ArtifactRepository artifactRepository = getArtifactRepository(libDir.getAbsolutePath());

        getLog().info("Install tars artifact...");
        Set<Artifact> projectDependency = project.getDependencyArtifacts();
        Artifact tarsArtifact = null;
        for (Artifact artifact : projectDependency) {
            if (artifact.getArtifactId().contains("tars")) {
                getLog().info(String.format("Use tars artifact %s-%s-%s", artifact.getGroupId(), artifact.getArtifactId(), artifact.getVersion()));
                tarsArtifact = artifact;
                break;
            }
        }

        if (tarsArtifact == null) {
            tarsArtifact = artifactFactory.createArtifact(tarsGroupId, tarsArtifactId, tarsVersion, Artifact.SCOPE_COMPILE, "jar");
        }
        artifactResolver.resolve(tarsArtifact, remoteRepositories, localRepository);
        installArtifact(tarsArtifact, libDir, artifactRepository, useTimestampInSnapshotFileName);

        MavenProject coreProject = mavenProjectBuilder.buildFromRepository(tarsArtifact, remoteRepositories, localRepository);
        final ArtifactFilter artifactFilter = new ScopeArtifactFilter(DefaultArtifact.SCOPE_COMPILE) {

            @Override
            public boolean include(Artifact artifact) {
                return includedScope(artifact.getScope());
            }
        };

        final Set<Artifact> artifacts = new HashSet<Artifact>();
        final DependencyNode rootNode = dependencyTreeBuilder.buildDependencyTree(coreProject, localRepository, artifactFactory, artifactMetadataSource, artifactFilter, artifactCollector);

        for (final Iterator<?> iterator = rootNode.getChildren().iterator(); iterator.hasNext();) {
            final DependencyNode child = (DependencyNode) iterator.next();
            collect(child, artifacts);
        }
        if (artifacts == null || artifacts.isEmpty()) {
            throw new MojoExecutionException("Unable to install tars artifact dependency.");
        }
        for (final Artifact artifact : artifacts) {
            try {
                artifactResolver.resolve(artifact, remoteRepositories, localRepository);
            } catch (final ArtifactResolutionException e) {
                throw new MojoExecutionException("Unable to resolve " + artifact, e);
            } catch (final ArtifactNotFoundException e) {
                throw new MojoExecutionException("Unable to resolve " + artifact, e);
            }
            installArtifact(artifact, libDir, artifactRepository, useTimestampInSnapshotFileName);
        }
    }

    /**
     * extract specified artifact. 
     *  
     * @param destination destination folder 
     * @param artifactFile 
     * @throws NoSuchArchiverException 
     * @throws ArchiverException 
     * @throws MojoExecutionException 
     */
    private void unArchiveArtifact(final File destination, final File artifactFile) throws MojoExecutionException, IOException {
        try {
            final UnArchiver unArchiver = archiverManager.getUnArchiver(artifactFile);
            unArchiver.setSourceFile(artifactFile);
            unArchiver.setDestDirectory(destination);
            unArchiver.setOverwrite(true);
            unArchiver.extract();
        } catch (final NoSuchArchiverException e) {
            throw new MojoExecutionException("Unable to unarchive " + artifactFile.getName(), e);
        } catch (final ArchiverException e) {
            throw new MojoExecutionException("Unable to unarchive " + artifactFile.getName(), e);
        }
    }

    private void collect(final DependencyNode root, final Set<Artifact> artifacts) {
        Stack<DependencyNode> stack = new Stack<DependencyNode>();
        stack.push(root);
        while (!stack.isEmpty()) {
            DependencyNode node = stack.pop();
            if (node.getState() == DependencyNode.INCLUDED) {
                final Artifact artifact = node.getArtifact();
                if (includedScope(artifact.getScope())) {
                    getLog().info("Adding Artefact: " + artifact.toString());
                    artifacts.add(artifact);
                    // check children 
                    if (!node.getChildren().isEmpty()) {
                        stack.addAll(node.getChildren());
                    }
                }
            }
        }
    }

    private ArtifactRepository getArtifactRepository(final String outputDirectory) throws ArtifactNotFoundException, MojoFailureException {
        return artifactRepositoryFactory.createDeploymentArtifactRepository("tars", "file://" + outputDirectory, getArtifactRepositoryLayout(), false);
    }

    protected ArtifactRepositoryLayout getArtifactRepositoryLayout() throws MojoFailureException {
        try {
            ArtifactRepositoryLayout artifactRepositoryLayout = null;
            artifactRepositoryLayout = (ArtifactRepositoryLayout) container.lookup("org.apache.maven.artifact." + "repository.layout.ArtifactRepositoryLayout", repositoryLayout);
            if (artifactRepositoryLayout == null) {
                throw new MojoFailureException("Unknown repository layout '" + repositoryLayout + "'.");
            }
            return artifactRepositoryLayout;
        } catch (ComponentLookupException e) {
            throw new MojoFailureException("Unable to lookup the repository layout component '" + repositoryLayout + "': " + e.getMessage());
        }
    }

    protected void installArtifact(Artifact artifact, File libDir, ArtifactRepository artifactRepository,
                                   boolean useTimestampInSnapshotFileName) throws MojoExecutionException {
        if (artifact != null && artifact.getFile() != null) {
            try {
                File source = artifact.getFile();

                File destination = libDir; //

                if (!source.isDirectory()) {
                    if (outputFileNameMapping != null) {
                        String fileName = MappingUtils.evaluateFileNameMapping(outputFileNameMapping, artifact);
                        destination = new File(destination, fileName);
                    } else {
                        destination = new File(destination, source.getName());
                    }

                    FileUtils.copyFile(source, destination);
                    Dependency dependency = new Dependency(artifact.getGroupId(), artifact.getArtifactId(), artifact.getVersion(), artifact.getClassifier());
                    dependency.setRelativePath(destination.getCanonicalPath());
                    dependencyList.add(dependency);
                }
                getLog().info("Installing artifact " + source.getPath() + " to " + destination);
            } catch (IOException e) {
                throw new MojoExecutionException("Failed to copy artifact.", e);
            } catch (InterpolationException e) {
                throw new MojoExecutionException("Failed to map file name.", e);
            }
        }
    }

    private void createConfigScript(File binDir, File dataDir, File binLogDir, File configFile) throws Exception {
        InputStream in = null;
        FileWriter out = null;
        try {

            File servicesXMLFile = new File(war, "WEB-INF" + File.separator + "servants.xml");
            if (!servicesXMLFile.exists()) {
                throw new MojoExecutionException("failed to find WEB-INF/servants.xml, " + " servants will be disabled");
            }

            XMLConfigFile cfg = new XMLConfigFile();
            cfg.parse(new FileInputStream(servicesXMLFile));
            XMLConfigElement root = cfg.getRootElement();
            ArrayList<XMLConfigElement> elements = root.getChildList();

            in = getConfigTemplate();
            InputStreamReader reader = new InputStreamReader(getConfigTemplate());

            Map<Object, Object> context = new HashMap<Object, Object>();

            context.put("LOCALIP", getLocalIp());
            context.put("APP", getApp());
            context.put("SERVER", getServer());
            context.put("BASEPATH", binDir.getCanonicalPath());
            context.put("DATAPATH", dataDir.getCanonicalFile());
            context.put("LOGPATH", binLogDir.getCanonicalPath());
            context.put("JVMPARAMS", getJvmParams());
            context.put("MAINCLASS", MAIN_CLASS);

            context.put("CONFIG", configFile.getAbsolutePath());

            StringBuilder suf = new StringBuilder();
            for (XMLConfigElement element : elements) {
                if ("servant".equals(element.getName())) {
                    if (suf.length() > 0) {
                        suf.append("\n");
                    }
                    Servant servant = new Servant();
                    servant.setName(element.getStringAttribute("name"));
                    servant.setProtocol(element.getStringAttribute("protocol", TARS_PROTOCOL));
                    servant.setPort(element.getStringAttribute("port"));
                    suf.append(readServantScript(servant));
                    if (servant.getProtocol() != null && isJceOrWup(servant.getProtocol())) {
                        context.put("ADMINPORT", servant.getPort());
                    }
                }
            }

            context.put("SERVANTADAPTER", suf.toString());

            InterpolationFilterReader interpolationFilterReader = new InterpolationFilterReader(reader, context, "$", "$");

            out = new FileWriter(configFile);
            IOUtil.copy(interpolationFilterReader, out);
        } catch (FileNotFoundException e) {
            throw new MojoExecutionException("Failed to get template for config file.", e);
        } catch (IOException e) {
            throw new MojoExecutionException("Failed to write config file.", e);
        } finally {
            IOUtil.close(out);
            IOUtil.close(in);
        }
    }

    private String getLocalIp() throws UnknownHostException {
        return System.getProperty("localIp", InetAddress.getLocalHost().getHostAddress());
    }

    private String getApp() {
        return System.getProperty("app", null);
    }

    private String getServer() {
        return System.getProperty("server", null);
    }

    private boolean isPackage() {
        return Boolean.parseBoolean(System.getProperty("package", "false"));
    }

    private String getJvmParams() {
        return System.getProperty("jvmParams", "-Xms1024m -Xmx1024m");
    }

    private boolean isJceOrWup(String protocol) {
        return TARS_PROTOCOL.equalsIgnoreCase(protocol) || TUP_PROTOCOL.equalsIgnoreCase(protocol);
    }

    private String readServantScript(Servant servant) throws Exception {
        InputStream in = null;
        try {
            in = getServantTemplate();
            Map<Object, Object> context = new HashMap<Object, Object>();
            context.put("LOCALIP", getLocalIp());
            context.put("APP", getApp());
            context.put("SERVER", getServer());
            context.put("SERVANTNAME", servant.getName());
            context.put("PORT", servant.getPort());
            context.put("PROTOCOL", servant.getProtocol() == null || isJceOrWup(servant.getProtocol()) ? "tars" : "no_tars");

            InterpolationFilterReader interpolationFilterReader = new InterpolationFilterReader(new InputStreamReader(in), context, "$", "$");
            return IOUtil.toString(interpolationFilterReader);
        } catch (FileNotFoundException e) {
            throw new MojoExecutionException("Failed to get template for servant file.", e);
        } catch (IOException e) {
            throw new MojoExecutionException("Failed to write servant file.", e);
        } finally {
            IOUtil.close(in);
        }
    }

    private String createBinScript(Platform platform, File libDir, File binDir, File configFile) throws Exception {

        InputStream in = null;

        FileWriter out = null;
        File binFile;

        try {
            in = getScriptTemplate(platform.getName());

            Map<Object, Object> context = new HashMap<Object, Object>();
            context.put("JVMPARAMS", getJvmParams());
            context.put("MAINCLASS", MAIN_CLASS);
            context.put("CONFIG", configFile.getAbsolutePath());
            context.put("CLASSPATH", platform.getClassPath(dependencyList));
            if (platform.isShowConsoleWindow()) {
                context.put("JAVA_BINARY", "java");
                context.put("UNIX_BACKGROUND", "");
            } else {
                context.put("JAVA_BINARY", "start /min javaw");
                context.put("UNIX_BACKGROUND", " &");
            }

            String interpolationToken = platform.getInterpolationToken();
            InterpolationFilterReader interpolationFilterReader = new InterpolationFilterReader(new InputStreamReader(in), context, interpolationToken, interpolationToken);

            String programName = "tars_start";

            FileUtils.forceMkdir(binDir);
            binFile = new File(binDir, programName + platform.getBinFileExtension());
            if (Platform.UNIX_NAME.equals(platform.getName())) {
                if (binFile.exists()) {
                    try {
                        ArchiveEntryUtils.chmod(binFile, 0777, getLog(), true);
                    } catch (ArchiverException ae) {
                        throw new MojoExecutionException("Failed to change permission for bin file.", ae);
                    }
                }
            }

            out = new FileWriter(binFile);
            getLog().debug("Writing shell file for platform '" + platform.getName() + "' to '" + binFile.getAbsolutePath() + "'.");

            IOUtil.copy(interpolationFilterReader, out);
        } catch (FileNotFoundException e) {
            throw new MojoExecutionException("Failed to get template for bin file.", e);
        } catch (IOException e) {
            throw new MojoExecutionException("Failed to write bin file.", e);
        } finally {
            IOUtil.close(out);
            IOUtil.close(in);
        }

        if (Platform.UNIX_NAME.equals(platform.getName())) {
            try {
                ArchiveEntryUtils.chmod(binFile, 0777, getLog(), true);
            } catch (ArchiverException ae) {
                throw new MojoExecutionException("Failed to change permission for bin file.", ae);
            }
        }
        return binFile.getCanonicalPath();
    }

    private InputStream getScriptTemplate(String platformName) throws MojoExecutionException {
        InputStream is = null;
        try {
            is = Platform.class.getResourceAsStream(platformName + "BinTemplate");
            if (is == null) {
                throw new MojoExecutionException("Unable to load internal template resource: " + platformName + "BinTemplate");
            }
        } catch (Exception e) {
            throw new MojoExecutionException("Unable to load external template file", e);
        }
        return is;
    }

    private InputStream getConfigTemplate() throws MojoExecutionException {
        InputStream is = null;
        try {
            is = Platform.class.getResourceAsStream("configTemplate");
            if (is == null) {
                throw new MojoExecutionException("Unable to load internal template resource: configTemplate");
            }
        } catch (Exception e) {
            throw new MojoExecutionException("Unable to load external template file", e);
        }
        return is;
    }

    private InputStream getServantTemplate() throws MojoExecutionException {
        InputStream is = null;
        try {
            is = Platform.class.getResourceAsStream("servantTemplate");
            if (is == null) {
                throw new MojoExecutionException("Unable to load internal template resource: configTemplate");
            }
        } catch (Exception e) {
            throw new MojoExecutionException("Unable to load external template file", e);
        }
        return is;
    }
}
