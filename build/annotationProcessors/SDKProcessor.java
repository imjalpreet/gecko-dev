/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.annotationProcessors;

import com.android.tools.lint.checks.ApiLookup;
import com.android.tools.lint.LintCliClient;

import org.mozilla.gecko.annotationProcessors.classloader.AnnotatableEntity;
import org.mozilla.gecko.annotationProcessors.classloader.ClassWithOptions;
import org.mozilla.gecko.annotationProcessors.classloader.IterableJarLoadingURLClassLoader;
import org.mozilla.gecko.annotationProcessors.utils.GeneratableElementIterator;
import org.mozilla.gecko.annotationProcessors.utils.Utils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Iterator;
import java.util.Properties;
import java.util.Scanner;
import java.util.Vector;
import java.net.URL;
import java.net.URLClassLoader;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

public class SDKProcessor {
    public static final String GENERATED_COMMENT =
            "// GENERATED CODE\n" +
            "// Generated by the Java program at /build/annotationProcessors at compile time\n" +
            "// from annotations on Java methods. To update, change the annotations on the\n" +
            "// corresponding Javamethods and rerun the build. Manually updating this file\n" +
            "// will cause your build to fail.\n" +
            "\n";

    private static ApiLookup sApiLookup;
    private static int sMaxSdkVersion;

    public static void main(String[] args) throws Exception {
        // We expect a list of jars on the commandline. If missing, whinge about it.
        if (args.length < 5) {
            System.err.println("Usage: java SDKProcessor sdkjar classlistfile outdir fileprefix max-sdk-version");
            System.exit(1);
        }

        System.out.println("Processing platform bindings...");

        String sdkJar = args[0];
        Vector classes = getClassList(args[1]);
        String outdir = args[2];
        String generatedFilePrefix = args[3];
        sMaxSdkVersion = Integer.parseInt(args[4]);

        LintCliClient lintClient = new LintCliClient();
        sApiLookup = ApiLookup.get(lintClient);

        // Start the clock!
        long s = System.currentTimeMillis();

        // Get an iterator over the classes in the jar files given...
        // Iterator<ClassWithOptions> jarClassIterator = IterableJarLoadingURLClassLoader.getIteratorOverJars(args);

        StringBuilder headerFile = new StringBuilder(GENERATED_COMMENT);
        headerFile.append(
                "#ifndef " + generatedFilePrefix + "_h__\n" +
                "#define " + generatedFilePrefix + "_h__\n" +
                "\n" +
                "#include \"mozilla/jni/Refs.h\"\n" +
                "\n" +
                "namespace mozilla {\n" +
                "namespace widget {\n" +
                "namespace sdk {\n" +
                "\n");

        StringBuilder implementationFile = new StringBuilder(GENERATED_COMMENT);
        implementationFile.append(
                "#include \"" + generatedFilePrefix + ".h\"\n" +
                "#include \"mozilla/jni/Accessors.h\"\n" +
                "\n" +
                "namespace mozilla {\n" +
                "namespace widget {\n" +
                "namespace sdk {\n" +
                "\n");

        // Used to track the calls to the various class-specific initialisation functions.
        ClassLoader loader = null;
        try {
            loader = URLClassLoader.newInstance(new URL[] { new URL("file://" + sdkJar) },
                                                SDKProcessor.class.getClassLoader());
        } catch (Exception e) {
            throw new RuntimeException(e.toString());
        }

        for (Iterator<String> i = classes.iterator(); i.hasNext(); ) {
            String className = i.next();
            System.out.println("Looking up: " + className);

            generateClass(Class.forName(className, true, loader),
                          implementationFile,
                          headerFile);
        }

        implementationFile.append(
                "} /* sdk */\n" +
                "} /* widget */\n" +
                "} /* mozilla */\n");

        headerFile.append(
                "} /* sdk */\n" +
                "} /* widget */\n" +
                "} /* mozilla */\n" +
                "#endif\n");

        writeOutputFiles(outdir, generatedFilePrefix, headerFile, implementationFile);
        long e = System.currentTimeMillis();
        System.out.println("SDK processing complete in " + (e - s) + "ms");
    }

    private static Member[] sortAndFilterMembers(Member[] members) {
        Arrays.sort(members, new Comparator<Member>() {
            @Override
            public int compare(Member a, Member b) {
                return a.getName().compareTo(b.getName());
            }
        });

        ArrayList<Member> list = new ArrayList<>();
        for (Member m : members) {
            int version = 0;

            if (m instanceof Method || m instanceof Constructor) {
                version = sApiLookup.getCallVersion(
                        Utils.getClassDescriptor(m.getDeclaringClass()),
                        m.getName(),
                        Utils.getSignature(m));
            } else if (m instanceof Field) {
                version = sApiLookup.getFieldVersion(
                        Utils.getClassDescriptor(m.getDeclaringClass()), m.getName());
            } else {
                throw new IllegalArgumentException("expected member to be Method, Constructor, or Field");
            }

            if (version > sMaxSdkVersion) {
                System.out.println("Skipping " + m.getDeclaringClass().getName() + "." + m.getName() +
                    ", version " + version + " > " + sMaxSdkVersion);
                continue;
            }

            list.add(m);
        }

        return list.toArray(new Member[list.size()]);
    }

    private static void generateClass(Class<?> clazz,
                                      StringBuilder implementationFile,
                                      StringBuilder headerFile) {
        String generatedName = clazz.getSimpleName();

        CodeGenerator generator = new CodeGenerator(new ClassWithOptions(clazz, generatedName));

        generator.generateMembers(sortAndFilterMembers(clazz.getDeclaredConstructors()));
        generator.generateMembers(sortAndFilterMembers(clazz.getDeclaredMethods()));
        generator.generateMembers(sortAndFilterMembers(clazz.getDeclaredFields()));

        headerFile.append(generator.getHeaderFileContents());
        implementationFile.append(generator.getWrapperFileContents());
    }

    private static Vector<String> getClassList(String path) {
        Scanner scanner = null;
        try {
            scanner = new Scanner(new FileInputStream(path));

            Vector lines = new Vector();
            while (scanner.hasNextLine()) {
                lines.add(scanner.nextLine());
            }
            return lines;
        } catch (Exception e) {
            System.out.println(e.toString());
            return null;
        } finally {
            if (scanner != null) {
                scanner.close();
            }
        }
    }

    private static void writeOutputFiles(String aOutputDir, String aPrefix, StringBuilder aHeaderFile,
                                         StringBuilder aImplementationFile) {
        FileOutputStream implStream = null;
        try {
            implStream = new FileOutputStream(new File(aOutputDir, aPrefix + ".cpp"));
            implStream.write(aImplementationFile.toString().getBytes());
        } catch (IOException e) {
            System.err.println("Unable to write " + aOutputDir + ". Perhaps a permissions issue?");
            e.printStackTrace(System.err);
        } finally {
            if (implStream != null) {
                try {
                    implStream.close();
                } catch (IOException e) {
                    System.err.println("Unable to close implStream due to "+e);
                    e.printStackTrace(System.err);
                }
            }
        }

        FileOutputStream headerStream = null;
        try {
            headerStream = new FileOutputStream(new File(aOutputDir, aPrefix + ".h"));
            headerStream.write(aHeaderFile.toString().getBytes());
        } catch (IOException e) {
            System.err.println("Unable to write " + aOutputDir + ". Perhaps a permissions issue?");
            e.printStackTrace(System.err);
        } finally {
            if (headerStream != null) {
                try {
                    headerStream.close();
                } catch (IOException e) {
                    System.err.println("Unable to close headerStream due to "+e);
                    e.printStackTrace(System.err);
                }
            }
        }
    }
}
