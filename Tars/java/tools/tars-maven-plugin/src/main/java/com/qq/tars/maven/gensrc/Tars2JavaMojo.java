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
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.atomic.AtomicInteger;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.Token;
import org.antlr.runtime.tree.CommonTree;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugin.MojoFailureException;
import org.apache.maven.plugins.annotations.Mojo;
import org.apache.maven.plugins.annotations.Parameter;

import com.qq.tars.maven.parse.TarsLexer;
import com.qq.tars.maven.parse.TarsParser;
import com.qq.tars.maven.parse.ast.TarsConst;
import com.qq.tars.maven.parse.ast.TarsCustomType;
import com.qq.tars.maven.parse.ast.TarsEnum;
import com.qq.tars.maven.parse.ast.TarsInterface;
import com.qq.tars.maven.parse.ast.TarsKey;
import com.qq.tars.maven.parse.ast.TarsMapType;
import com.qq.tars.maven.parse.ast.TarsNamespace;
import com.qq.tars.maven.parse.ast.TarsOperation;
import com.qq.tars.maven.parse.ast.TarsParam;
import com.qq.tars.maven.parse.ast.TarsPrimitiveType;
import com.qq.tars.maven.parse.ast.TarsPrimitiveType.PrimitiveType;
import com.qq.tars.maven.parse.ast.TarsRoot;
import com.qq.tars.maven.parse.ast.TarsStruct;
import com.qq.tars.maven.parse.ast.TarsStructMember;
import com.qq.tars.maven.parse.ast.TarsType;
import com.qq.tars.maven.parse.ast.TarsVectorType;

@Mojo(name = "tars2java", threadSafe = true)
public class Tars2JavaMojo extends AbstractMojo {

    @Parameter(required = true)
    private Tars2JavaConfig tars2JavaConfig;

    private AtomicInteger var = new AtomicInteger(0);

    public void execute() throws MojoExecutionException, MojoFailureException {
        // 1. check configurations
        if (!tars2JavaConfig.packagePrefixName.endsWith(".")) {
            tars2JavaConfig.packagePrefixName += ".";
        }

        if (tars2JavaConfig.tarsFiles.length == 0) {
            getLog().error("configuration tarsFiles is missing...");
            return;
        }

        Map<String, List<TarsNamespace>> nsMap = new HashMap<String, List<TarsNamespace>>();

        // 2. parse tars files
        for (String tarsFile : tars2JavaConfig.tarsFiles) {
            try {
                getLog().info("Parse " + tarsFile + " ...");
                TarsLexer tarsLexer = new TarsLexer(new ANTLRFileStream(tarsFile, tars2JavaConfig.tarsFileCharset));
                CommonTokenStream tokens = new CommonTokenStream(tarsLexer);
                TarsParser tarsParser = new TarsParser(tokens);
                TarsRoot root = (TarsRoot) tarsParser.start().getTree();
                root.setTokenStream(tokens);
                for (TarsNamespace ns : root.namespaceList()) {
                    List<TarsNamespace> list = nsMap.get(ns.namespace());
                    if (list == null) {
                        list = new ArrayList<TarsNamespace>();
                        nsMap.put(ns.namespace(), list);
                    }
                    list.add(ns);
                }
            } catch (Throwable th) {
                getLog().error("Parse " + tarsFile + " Error!", th);
            }
        }

        // 3. generate java files.
        for (Entry<String, List<TarsNamespace>> entry : nsMap.entrySet()) {
            try {
                getLog().info("generate code for namespace : " + entry.getKey() + " ...");
                genJava(entry.getKey(), entry.getValue(), nsMap);
            } catch (Throwable th) {
                getLog().error("generate code for namespace : " + entry.getKey() + " Error!", th);
            }
        }
    }

    private String getDoc(CommonTree ast, String prefix) {
        CommonTokenStream ts = ((TarsRoot) ast.getAncestor(TarsParser.TARS_ROOT)).getTokenStream();
        Token t = ts.get(ast.getTokenStartIndex() - 1);
        if (t != null && t.getChannel() == Token.HIDDEN_CHANNEL) {
            return prefix + t.getText().replaceAll("\n\\s*\\*", "\n" + prefix + " *");
        } else {
            return "";
        }
    }

    private void genJava(String nsName, List<TarsNamespace> namespaces, Map<String, List<TarsNamespace>> nsMap) throws Exception {
        getLog().info("module " + nsName + " >>");

        String packageName = packageName(tars2JavaConfig.packagePrefixName, nsName);
        String dirPath = tars2JavaConfig.srcPath + '/' + packageName.replace('.', '/') + "/";
        new File(dirPath).mkdirs();

        // generate Const
        List<TarsConst> allConst = new ArrayList<TarsConst>();
        for (TarsNamespace namespace : namespaces) {
            allConst.addAll(namespace.constList());
        }
        if (!allConst.isEmpty()) {
            genConst(dirPath, packageName, nsName, allConst);
        }

        // generate Enum
        for (TarsNamespace ns : namespaces) {
            for (TarsEnum e : ns.enumList()) {
                genEnum(dirPath, packageName, ns.namespace(), e);
            }
        }

        // generate Struct
        for (TarsNamespace ns : namespaces) {
            for (TarsStruct s : ns.structList()) {
                genStruct(dirPath, packageName, ns.namespace(), s, ns.keyMap().get(s.structName()), nsMap);
            }
        }

        // generate interface Prx
        for (TarsNamespace ns : namespaces) {
            for (TarsInterface tarsInterface : ns.interfaceList()) {
                if (tars2JavaConfig.servant) {
                    genServant(dirPath, packageName, ns.namespace(), tarsInterface, nsMap);
                } else {
                    genPrx(dirPath, packageName, ns.namespace(), tarsInterface, nsMap);
                    genPrxCallback(dirPath, packageName, ns.namespace(), tarsInterface, nsMap);
                }
            }
        }
        getLog().info("module " + nsName + " <<");
    }

    private void printHead(PrintWriter out) {
        out.println("// **********************************************************************");
        out.println("// This file was generated by a TARS parser!");
        out.println("// TARS version 1.0.1.");
        out.println("// **********************************************************************");
        out.println();
    }

    private void printDoc(PrintWriter out, String doc) {
        if (doc != null && doc.startsWith("/**") && doc.endsWith("*/")) {
            out.println(doc.substring(0, doc.length()));
        }
    }

    private void genConst(String dirPath, String packageName, String namespace, List<TarsConst> constList) throws Exception {
        String constClass = firstUpStr(namespace) + "Const";
        PrintWriter out = new PrintWriter(dirPath + constClass + ".java", tars2JavaConfig.charset);

        printHead(out);
        out.println("package " + packageName + ";");
        out.println();
        out.println("public interface " + constClass + " {");
        for (TarsConst cn : constList) {
            out.println(getDoc(cn, "\t"));
            out.println("\tpublic static final " + type(cn.constType(), null) + " " + cn.constName() + " = " + cn.constValue() + ";");
        }
        out.println("}");
        out.close();

        getLog().info("generate Const " + constClass);
    }

    private void genEnum(String dirPath, String packageName, String namespace, TarsEnum e) throws Exception {
        String enumClass = e.enumName();
        PrintWriter out = new PrintWriter(dirPath + enumClass + ".java", tars2JavaConfig.charset);
        printHead(out);
        out.println("package " + packageName + ";");
        out.println();
        printDoc(out, getDoc(e, ""));
        out.println("public enum " + enumClass + " {");
        out.println();
        for (int i = 0; i < e.enumMemberList().size(); ++i) {
            String item = e.enumMemberList().get(i);
            String value = i < e.enumValueList().size() ? e.enumValueList().get(i) : String.valueOf(i);
            out.print("\t" + item + "(" + value + ")");
            if (i < e.enumMemberList().size() - 1) {
                out.println(",");
            } else {
                out.println(";");
            }
        }
        out.println();
        out.println("\tprivate final int value;");
        out.println();
        out.println("\tprivate " + e.enumName() + "(int value) {");
        out.println("\t\tthis.value = value;");
        out.println("\t}");
        out.println();
        out.println("\tpublic int value() {");
        out.println("\t\treturn this.value;");
        out.println("\t}");
        out.println();
        out.println("\t@Override");
        out.println("\tpublic String toString() {");
        out.println("\t\treturn this.name() + \":\" + this.value;");
        out.println("\t}");
        out.println();

        out.println("\tpublic static " + e.enumName() + " convert(int value) {");
        out.println("\t\tfor(" + e.enumName() + " v : values()) {");
        out.println("\t\t\tif(v.value() == value) {");
        out.println("\t\t\t\treturn v;");
        out.println("\t\t\t}");
        out.println("\t\t}");
        out.println("\t\treturn null;");
        out.println("\t}");
        out.println("}");
        out.close();

        getLog().info("generate Enum " + enumClass);
    }

    private void genStruct(String dirPath, String packageName, String namespace, TarsStruct struct, TarsKey key,
                           Map<String, List<TarsNamespace>> nsMap) throws Exception {
        String structClass = struct.structName();
        PrintWriter out = new PrintWriter(dirPath + structClass + ".java", tars2JavaConfig.charset);
        printHead(out);
        out.println("package " + packageName + ";");
        out.println();

        out.println("import com.qq.tars.protocol.util.*;");
        out.println("import com.qq.tars.protocol.annotation.*;");
        out.println("import com.qq.tars.protocol.tars.*;");
        out.println("import com.qq.tars.protocol.tars.annotation.*;");
        out.println();
        printDoc(out, getDoc(struct, ""));
        out.println("@TarsStruct");
        if (key == null) {
            out.println("public class " + struct.structName() + " {");
        } else {
            out.println("public class " + struct.structName() + " implements Comparable<" + struct.structName() + "> {");
        }
        out.println();

        // 定义成员变量
        for (TarsStructMember m : struct.memberList()) {
            out.println("\t@TarsStructProperty(order = " + m.tag() + ", isRequire = " + m.isRequire() + ")");
            out.println("\tpublic " + type(m.memberType(), nsMap) + " " + m.memberName() + " = " + (m.defaultValue() == null ? typeInit(m.memberType(), nsMap, false) : m.defaultValue()) + ";");
        }
        out.println();

        // 生成 getter setter
        for (TarsStructMember m : struct.memberList()) {
            out.println("\tpublic " + type(m.memberType(), nsMap) + " " + fieldGetter(m.memberName(), m.memberType()) + "() {");
            out.println("\t\treturn " + m.memberName() + ";");
            out.println("\t}");
            out.println();

            out.println("\tpublic void " + fieldSetter(m.memberName(), m.memberType()) + "(" + type(m.memberType(), nsMap) + " " + m.memberName() + ") {");
            out.println("\t\tthis." + m.memberName() + " = " + m.memberName() + ";");
            out.println("\t}");
            out.println();
        }

        // 生成 constructor
        out.println("\tpublic " + struct.structName() + "() {");
        out.println("\t}");
        out.println();
        out.print("\tpublic " + struct.structName() + "(");
        for (int i = 0; i < struct.memberList().size(); ++i) {
            TarsStructMember m = struct.memberList().get(i);
            out.print(type(m.memberType(), nsMap) + " " + m.memberName());
            if (i < struct.memberList().size() - 1) {
                out.print(", ");
            }
        }
        out.println(") {");
        for (TarsStructMember m : struct.memberList()) {
            out.println("\t\tthis." + m.memberName() + " = " + m.memberName() + ";");
        }
        out.println("\t}");
        out.println();

        // compareTo()
        if (key != null) {
            out.println("\t@Override");
            out.println("\tpublic int compareTo(" + struct.structName() + " o) {");
            out.println("\t\tint c = 0;");
            for (String k : key.keyList()) {
                out.println("\t\tif((c = TarsUtil.compareTo(" + k + ", o." + k + ")) != 0 ) {");
                out.println("\t\t\treturn c;");
                out.println("\t\t}");
            }
            out.println("\t\treturn 0;");
            out.println("\t}");
            out.println();
        }

        // hashCode()
        if (key != null) {
            out.println();
            out.println("\t@Override");
            out.println("\tpublic int hashCode() {");
            out.println("\t\tfinal int prime = 31;");
            out.println("\t\tint result = 1;");
            for (int i = 0; i < key.keyList().size(); ++i) {
                String k = key.keyList().get(i);
                out.println("\t\tresult = prime * result + TarsUtil.hashCode(" + k + ");");
            }
            out.println("\t\treturn result;");
            out.println("\t}");
            out.println();
        } else {
            out.println("\t@Override");
            out.println("\tpublic int hashCode() {");
            out.println("\t\tfinal int prime = 31;");
            out.println("\t\tint result = 1;");
            for (TarsStructMember m : struct.memberList()) {
                out.println("\t\tresult = prime * result + TarsUtil.hashCode(" + m.memberName() + ");");
            }
            out.println("\t\treturn result;");
            out.println("\t}");
            out.println();
        }

        // equals()
        out.println("\t@Override");
        out.println("\tpublic boolean equals(Object obj) {");

        out.println("\t\tif (this == obj) {");
        out.println("\t\t\treturn true;");
        out.println("\t\t}");
        out.println("\t\tif (obj == null) {");
        out.println("\t\t\treturn false;");
        out.println("\t\t}");
        out.println("\t\tif (!(obj instanceof " + struct.structName() + ")) {");
        out.println("\t\t\treturn false;");
        out.println("\t\t}");
        out.println("\t\t" + struct.structName() + " other = (" + struct.structName() + ") obj;");
        out.println("\t\treturn (");

        if (key != null) {
            for (int i = 0; i < key.keyList().size(); ++i) {
                String k = key.keyList().get(i);
                out.println("\t\t\tTarsUtil.equals(" + k + ", other." + k + ") " + (i < key.keyList().size() - 1 ? "&&" : ""));
            }
        } else {
            for (int i = 0; i < struct.memberList().size(); ++i) {
                String k = struct.memberList().get(i).memberName();
                out.println("\t\t\tTarsUtil.equals(" + k + ", other." + k + ") " + (i < struct.memberList().size() - 1 ? "&&" : ""));
            }
        }
        out.println("\t\t);");
        out.println("\t}");
        out.println();

        //writeTo
        out.println("\tpublic void writeTo(TarsOutputStream _os) {");
        for (TarsStructMember m : struct.memberList()) {
            if (!m.isRequire()) {
                if (m.memberType().isPrimitive() || isEnum(m.memberType(), nsMap)) {
                    TarsPrimitiveType primitiveType = m.memberType().asPrimitive();
                    if (primitiveType != null && primitiveType.primitiveType().equals(PrimitiveType.STRING)) {
                        out.println("\t\tif (null != " + m.memberName() + ") {");
                        out.println("\t\t\t_os.write(" + m.memberName() + ", " + m.tag() + ");");
                        out.println("\t\t}");
                    } else {
                        out.println("\t\t_os.write(" + m.memberName() + ", " + m.tag() + ");");
                    }
                } else {
                    out.println("\t\tif (null != " + m.memberName() + ") {");
                    out.println("\t\t\t_os.write(" + m.memberName() + ", " + m.tag() + ");");
                    out.println("\t\t}");
                }
            } else {
                out.println("\t\t_os.write(" + m.memberName() + ", " + m.tag() + ");");
            }
        }
        out.println("\t}");
        out.println();

        //cache var
        for (TarsStructMember m : struct.memberList()) {
            boolean isenum = isEnum(m.memberType(), nsMap);
            if ((!isenum && m.memberType().isCustom()) || m.memberType().isMap() || (m.memberType().isVector())) {
                String memberName = "cache_" + m.memberName();
                out.println("\tstatic " + type(m.memberType(), true, nsMap) + " " + memberName + ";");
                out.println("\tstatic { ");
                genCacheVar(memberName, true, m.memberType(), nsMap, out);
                out.println("\t}");
            }
        }
        out.println();

        //readFrom
        out.println("\tpublic void readFrom(TarsInputStream _is) {");
        for (TarsStructMember m : struct.memberList()) {
            String type = null;
            boolean isenum = isEnum(m.memberType(), nsMap);
            if ((!isenum && m.memberType().isCustom()) || m.memberType().isMap() || (m.memberType().isVector())) {
                type = type(m.memberType(), nsMap);
                out.println("\t\tthis." + m.memberName() + " = " + (type == null ? "" : "(" + type + ") ") + "_is.read(cache_" + m.memberName() + ", " + m.tag() + ", " + m.isRequire() + ");");
            } else {
                if (m.memberType().isPrimitive()) {
                    TarsPrimitiveType primitiveType = m.memberType().asPrimitive();
                    if (primitiveType.primitiveType().equals(PrimitiveType.STRING)) {
                        out.println("\t\tthis." + m.memberName() + " = " + "_is.readString(" + m.tag() + ", " + m.isRequire() + ");");
                    } else {
                        out.println("\t\tthis." + m.memberName() + " = " + (type == null ? "" : "(" + type + ") ") + "_is.read(" + m.memberName() + ", " + m.tag() + ", " + m.isRequire() + ");");
                    }
                } else {
                    out.println("\t\tthis." + m.memberName() + " = " + (type == null ? "" : "(" + type + ") ") + "_is.read(" + m.memberName() + ", " + m.tag() + ", " + m.isRequire() + ");");
                }
            }
        }
        out.println("\t}");
        out.println();

        out.println("}");
        out.close();

        getLog().info("generate Struct " + structClass);
    }

    private void genCacheVar(String memberName, boolean hasDeclare, TarsType type,
                             Map<String, List<TarsNamespace>> nsMap, PrintWriter out) {
        if (type.isCustom() && !isEnum(type, nsMap)) {
            out.println("\t\t" + (hasDeclare ? memberName : (type(type, true, nsMap) + " " + memberName)) + " = new " + type(type, true, nsMap) + "();");
        } else if (type.isMap()) {
            TarsMapType mapType = type.asMap();
            out.println("\t\t" + (hasDeclare ? memberName : (type(type, true, nsMap) + " " + memberName)) + " = new java.util.HashMap<" + type(mapType.keyType(), true, nsMap) + ", " + type(mapType.valueType(), true, nsMap) + ">();");

            String varkey = "var_" + var.incrementAndGet();
            String varval = "var_" + var.incrementAndGet();

            genCacheVar(varkey, false, mapType.keyType(), nsMap, out);
            genCacheVar(varval, false, mapType.valueType(), nsMap, out);

            out.println("\t\t" + memberName + ".put(" + varkey + " ," + varval + ");");

        } else if (type.isVector()) {
            TarsVectorType v = type.asVector();
            String varType = "var_" + var.incrementAndGet();
            if (v.isByteArray()) {
                out.println("\t\t" + (hasDeclare ? memberName : (type(type, false, nsMap) + " " + memberName)) + " = new " + type(v.subType(), false, nsMap) + "[1];");
                genCacheVar(varType, false, v.subType(), nsMap, out);
                out.println("\t\t" + memberName + "[0] = " + varType + ";");
            } else {
                out.println("\t\t" + (hasDeclare ? memberName : (type(type, true, nsMap) + " " + memberName)) + " = new java.util.ArrayList<" + type(v.subType(), true, nsMap) + ">();");
                genCacheVar(varType, false, v.subType(), nsMap, out);
                out.println("\t\t" + memberName + ".add(" + varType + ");");
            }
        } else if (type.isPrimitive()) {
            out.println("\t\t" + (hasDeclare ? memberName : (type(type, false, nsMap) + " " + memberName)) + " = " + typeInit(type, nsMap, true) + ";");
        }
    }

    public void genPrx(String dirPath, String packageName, String namespace, TarsInterface _interface,
                       Map<String, List<TarsNamespace>> nsMap) throws Exception {
        String prxClass = _interface.interfaceName() + "Prx";

        PrintWriter out = new PrintWriter(dirPath + prxClass + ".java", tars2JavaConfig.charset);

        printHead(out);
        // 1. print package and imports.
        out.println("package " + packageName + ";");
        out.println();

        out.println("import com.qq.tars.protocol.annotation.*;");
        out.println("import com.qq.tars.protocol.tars.annotation.*;");
        out.println("import com.qq.tars.common.support.Holder;");

        out.println();

        // 2. print comments and class line.
        printDoc(out, getDoc(_interface, ""));
        out.println("@Servant");
        out.println("public interface " + prxClass + " {");
        // out.println();

        // 4. print tars methods and prototypes
        for (TarsOperation op : _interface.operationList()) {
            // 2 print sync method without context
            out.println(getDoc(op, "\t"));
            out.println("\tpublic " + type(op.retType(), nsMap) + " " + op.oprationName() + "(" + opertaionParams(null, op.paramList(), null, true, nsMap) + ");");

            // 3 print sync method with context
            out.println(getDoc(op, "\t"));
            out.println("\tpublic " + type(op.retType(), nsMap) + " " + op.oprationName() + "(" + opertaionParams(null, op.paramList(), Arrays.asList("@TarsContext java.util.Map<String, String> ctx"), true, nsMap) + ");");

            // 4 print async method without context
            out.println(getDoc(op, "\t"));
            out.println("\tpublic void async_" + op.oprationName() + "(" + opertaionParams(Arrays.asList("@TarsCallback " + prxClass + "Callback callback"), op.paramList(), null, false, nsMap) + ");");

            // 5 print async method with context
            out.println(getDoc(op, "\t"));
            out.println("\tpublic void async_" + op.oprationName() + "(" + opertaionParams(Arrays.asList("@TarsCallback " + prxClass + "Callback callback"), op.paramList(), Arrays.asList("@TarsContext java.util.Map<String, String> ctx"), false, nsMap) + ");");
        }

        out.println("}");
        out.close();

        getLog().info("generate Prx " + prxClass);
    }

    public void genServant(String dirPath, String packageName, String namespace, TarsInterface _interface,
                           Map<String, List<TarsNamespace>> nsMap) throws Exception {
        String prxClass = _interface.interfaceName() + "Servant";

        PrintWriter out = new PrintWriter(dirPath + prxClass + ".java", tars2JavaConfig.charset);

        printHead(out);
        // 1. print package and imports.
        out.println("package " + packageName + ";");
        out.println();

        out.println("import com.qq.tars.protocol.annotation.*;");
        out.println("import com.qq.tars.protocol.tars.annotation.*;");
        out.println("import com.qq.tars.common.support.Holder;");
        out.println();

        // 2. print comments and class line.
        printDoc(out, getDoc(_interface, ""));
        out.println("@Servant");
        out.println("public interface " + prxClass + " {");
        // out.println();

        // 4. print tars methods and prototypes
        for (TarsOperation op : _interface.operationList()) {
            // 2 print sync method without context
            out.println(getDoc(op, "\t"));
            out.println("\tpublic " + type(op.retType(), nsMap) + " " + op.oprationName() + "(" + opertaionParams(null, op.paramList(), null, true, nsMap) + ");");
        }

        out.println("}");
        out.close();

        getLog().info("generate Servant " + prxClass);
    }

    public void genPrxCallback(String dirPath, String packageName, String namespace, TarsInterface tarsInterface,
                               Map<String, List<TarsNamespace>> nsMap) throws Exception {
        String prxClass = tarsInterface.interfaceName() + "PrxCallback";

        PrintWriter out = new PrintWriter(dirPath + prxClass + ".java", tars2JavaConfig.charset);
        printHead(out);
        // 1. print package and imports.
        out.println("package " + packageName + ";");
        out.println();
        out.println("import com.qq.tars.rpc.protocol.tars.support.TarsAbstractCallback;");
        out.println();

        // 2. print comments and class line.
        printDoc(out, getDoc(tarsInterface, ""));
        out.println("public abstract class " + prxClass + " extends TarsAbstractCallback {");
        out.println();

        // 4. print tars methods and prototypes
        for (TarsOperation op : tarsInterface.operationList()) {
            String type = type(op.retType(), false, nsMap);
            if ("void".equals(type) || "Void".equals(type)) {
                out.println("\tpublic abstract void callback_" + op.oprationName() + "(" + opertaionCallBackParams(null, op.paramList(), null, nsMap) + ");");
            } else {
                out.println("\tpublic abstract void callback_" + op.oprationName() + "(" + opertaionCallBackParams(Arrays.asList(type + " ret"), op.paramList(), null, nsMap) + ");");
            }

            out.println();
        }

        out.println("}");
        out.close();

        getLog().info("generate Prx " + prxClass);
    }

    private String type(TarsType jt, Map<String, List<TarsNamespace>> nsMap) {
        return type(jt, false, nsMap);
    }

    private String type(TarsType jt, boolean usePrimitiveWrapper, Map<String, List<TarsNamespace>> nsMap) {
        if (jt.isPrimitive()) {
            TarsPrimitiveType p = jt.asPrimitive();
            if (!usePrimitiveWrapper) {
                switch (p.primitiveType()) {
                    case VOID:
                        return "void";
                    case BOOL:
                        return "boolean";
                    case BYTE:
                        return "byte";
                    case SHORT:
                        return "short";
                    case INT:
                        return "int";
                    case LONG:
                        return "long";
                    case FLOAT:
                        return "float";
                    case DOUBLE:
                        return "double";
                    case STRING:
                        return "String";
                    default:
                        return "";
                }
            } else {
                switch (p.primitiveType()) {
                    case VOID:
                        return "Void";
                    case BOOL:
                        return "java.lang.Boolean";
                    case BYTE:
                        return "java.lang.Byte";
                    case SHORT:
                        return "java.lang.Short";
                    case INT:
                        return "java.lang.Integer";
                    case LONG:
                        return "java.lang.Long";
                    case FLOAT:
                        return "java.lang.Float";
                    case DOUBLE:
                        return "java.lang.Double";
                    case STRING:
                        return "String";
                    default:
                        return "";
                }
            }
        } else if (jt.isVector()) {
            TarsVectorType v = jt.asVector();
            if (v.isByteArray()) {
                return "byte[]";
            } else {
                return "java.util.List<" + type(v.subType(), true, nsMap) + ">";
            }
        } else if (jt.isMap()) {
            TarsMapType m = jt.asMap();
            return "java.util.Map<" + type(m.keyType(), true, nsMap) + ", " + type(m.valueType(), true, nsMap) + ">";
        } else if (jt.isCustom()) {
            TarsCustomType ct = jt.asCustom();

            boolean isEnum = nsMap != null ? isEnum(jt, nsMap) : false;
            if (isEnum) {
                if (!usePrimitiveWrapper)
                    return "int";
                else
                    return "java.lang.Integer";
            }
            if (ct.namespace() == null) {
                return ct.typeName();
            } else {
                return packageName(tars2JavaConfig.packagePrefixName, ct.namespace()) + "." + ct.typeName();
            }
        } else {
            return "";
        }
    }

    private boolean isEnum(TarsType jt, Map<String, List<TarsNamespace>> nsMap) {
        List<TarsEnum> allEnum = new ArrayList<TarsEnum>();
        for (Entry<String, List<TarsNamespace>> entry : nsMap.entrySet()) {
            for (TarsNamespace namespace : entry.getValue()) {
                allEnum.addAll(namespace.enumList());
            }
        }

        if (!allEnum.isEmpty()) {
            for (TarsEnum tarsEnum : allEnum) {
                if (jt.typeName().equals(tarsEnum.enumName())) {
                    return true;
                }
            }
        }
        return false;
    }

    private String typeInit(TarsType jt, Map<String, List<TarsNamespace>> nsMap, boolean useDefault) {
        if (!useDefault) {
            if (jt.isPrimitive()) {
                TarsPrimitiveType p = jt.asPrimitive();
                switch (p.primitiveType()) {
                    case VOID:
                        return "";
                    case BOOL:
                        return "false";
                    case BYTE:
                        return "(byte)0";
                    case SHORT:
                        return "(short)0";
                    case INT:
                        return "0";
                    case LONG:
                        return "0L";
                    case FLOAT:
                        return "0F";
                    case DOUBLE:
                        return "0D";
                    case STRING:
                        return "\"\"";
                    default:
                        return "";
                }
            } else {
                return nsMap != null && isEnum(jt, nsMap) ? "0" : "null";
            }
        } else {
            if (jt.isPrimitive()) {
                TarsPrimitiveType p = jt.asPrimitive();
                switch (p.primitiveType()) {
                    case VOID:
                        return "";
                    case BOOL:
                        return "false";
                    case BYTE:
                        return "(byte)0";
                    case SHORT:
                        return "(short)0";
                    case INT:
                        return "0";
                    case LONG:
                        return "0L";
                    case FLOAT:
                        return "0F";
                    case DOUBLE:
                        return "0D";
                    case STRING:
                        return "\"\"";
                    default:
                        return "";
                }
            } else if (jt.isVector()) {
                TarsVectorType v = jt.asVector();
                if (v.isByteArray()) {
                    return "new byte[1]";
                } else {
                    return "new java.util.ArrayList<" + type(v.subType(), true, nsMap) + ">()";
                }
            } else if (jt.isMap()) {
                TarsMapType m = jt.asMap();
                return "new java.util.HashMap<" + type(m.keyType(), true, nsMap) + ", " + type(m.valueType(), true, nsMap) + ">()";
            } else if (jt.isCustom()) {
                TarsCustomType ct = jt.asCustom();

                boolean isEnum = nsMap != null ? isEnum(jt, nsMap) : false;
                if (isEnum) {
                    return "0";
                }
                if (ct.namespace() == null) {
                    return "new " + ct.typeName() + "()";
                } else {
                    return "new " + packageName(tars2JavaConfig.packagePrefixName, ct.namespace()) + "." + ct.typeName() + "()";
                }
            } else {
                return "";
            }
        }
    }

    public String opertaionCallBackParams(List<String> beforeParams, List<TarsParam> paramList,
                                          List<String> afterParams, Map<String, List<TarsNamespace>> nsMap) {
        StringBuilder sb = new StringBuilder();
        boolean isFirst = true;
        if (beforeParams != null) {
            for (String other : beforeParams) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    sb.append(", ");
                }
                sb.append(other);
            }
        }
        for (TarsParam p : paramList) {
            if (!p.isOut()) {
                continue;
            }
            sb.append(isFirst ? "" : ", ");
            sb.append(type(p.paramType(), nsMap)).append(" ").append(p.paramName());
            if (isFirst) {
                isFirst = false;
            }
        }

        if (afterParams != null) {
            for (String other : afterParams) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    sb.append(", ");
                }
                sb.append(other);
            }
        }
        return sb.toString();
    }

    public String opertaionParams(List<String> beforeParams, List<TarsParam> paramList, List<String> afterParams,
                                  boolean isSync, Map<String, List<TarsNamespace>> nsMap) {
        StringBuilder sb = new StringBuilder();
        boolean isFirst = true;
        if (beforeParams != null) {
            for (String other : beforeParams) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    sb.append(", ");
                }
                sb.append(other);
            }
        }
        for (TarsParam p : paramList) {
            if (!isSync && p.isOut()) {
                continue;
            }
            if (p.isOut()) {
                sb.append(isFirst ? "" : ", ");
                sb.append("@TarsHolder Holder<").append(type(p.paramType(), true, nsMap)).append("> ").append(p.paramName());
            } else {
                sb.append(isFirst ? "" : ", ");
                sb.append(type(p.paramType(), nsMap)).append(" ").append(p.paramName());
            }
            if (isFirst) {
                isFirst = false;
            }
        }

        if (afterParams != null) {
            for (String other : afterParams) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    sb.append(", ");
                }
                sb.append(other);
            }
        }
        return sb.toString();
    }

    private static String firstUpStr(String str) {
        if (str == null || str.length() < 1) {
            return str;
        }
        return str.substring(0, 1).toUpperCase() + str.substring(1);
    }

    private static String packageName(String packagePrefixName, String namespace) {
        return packagePrefixName + namespace.toLowerCase();
    }

    public static String fieldGetter(String fieldName, TarsType type) {
        if (type.isPrimitive() && (type.asPrimitive()).primitiveType() == TarsPrimitiveType.PrimitiveType.BOOL && fieldName.startsWith("is")) {
            return fieldName;
        } else {
            return "get" + firstUpStr(fieldName);
        }
    }

    public static String fieldSetter(String fieldName, TarsType type) {
        if (type.isPrimitive() && (type.asPrimitive()).primitiveType() == TarsPrimitiveType.PrimitiveType.BOOL && fieldName.startsWith("is")) {
            return "set" + fieldName.substring(2);
        } else {
            return "set" + firstUpStr(fieldName);
        }
    }
}
