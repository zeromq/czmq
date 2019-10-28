package org.zeromq.tools;

import org.scijava.nativelib.NativeLibraryUtil;
import org.scijava.nativelib.NativeLoader;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public class ZmqNativeLoader {

    private static final Set<String> loadedLibraries = new HashSet<>();

    private static final Set<String> potentialDlls = new HashSet<>();

    static {
        if (System.getProperty("os.name").toLowerCase().contains("windows")) {
            // Collect potential DLLs to be match when loading the libraries

            // 1. Lookup the java.library.path
            String javaLibraryPath = System.getProperty("java.library.path");
            if (javaLibraryPath != null) {
                final String[] paths = javaLibraryPath.split(File.pathSeparator);
                for (String path : paths) {
                    File folder = new File(path);
                    if (folder.exists()) {
                        for (final File fileEntry : folder.listFiles()) {
                            if (fileEntry.isFile() && fileEntry.getName().indexOf(".dll") != -1) {
                                potentialDlls.add(fileEntry.getName().substring(0, fileEntry.getName().indexOf(".")));
                            }
                        }
                    }
                }
            }

            // 2. Lookup the jar files
            try {
                String path = "natives/" + NativeLibraryUtil.getArchitecture().name().toLowerCase() + "/";
                URL url = ZmqNativeLoader.class.getClassLoader().getResource(path);
                if (url != null && url.getProtocol().equals("jar")) {
                    String jarPath = url.getPath().substring(5, url.getPath().indexOf("!")); // Strip out only the JAR file
                    JarFile jar = new JarFile(URLDecoder.decode(jarPath, "UTF-8"));
                    Enumeration<JarEntry> entries = jar.entries(); // Gives ALL entries in jar
                    while (entries.hasMoreElements()) {
                        String name = entries.nextElement().getName();
                        // Filter DLLs according to the path
                        if (name.startsWith(path) && name.indexOf(".dll") != -1) {
                            potentialDlls.add(name.substring(path.length(), name.length() - 4));
                        }
                    }
                }
            } catch (IOException e) {
                // Ignore!
            }
        }
    }

    public static void loadLibraries(Map<String, Boolean> libraries) {
        for (String libraryName : libraries.keySet()) {
            boolean libraryLoaded = false;
            if (System.getProperty("os.name").toLowerCase().contains("windows")) {
                // Windows DLL names are not as standardized as dynamic library names are under linux or osx.
                // Therefore we inspect potential DLLs to find a match.

                // Find all other libraries that contains this library's name
                Set<String> conflictingLibrariesNames = new HashSet<>();
                for (String otherLibraryName : libraries.keySet()) {
                    if (otherLibraryName.contains(libraryName) && !otherLibraryName.equals(libraryName)) {
                        conflictingLibrariesNames.add(otherLibraryName);
                    }
                }
                // Build regexes to exclude any prefixes and suffixes of libraries that contain this library's name
                String regexForbiddenPrefix = "";
                String regexForbiddenSuffix = "";
                if (conflictingLibrariesNames.size() > 0) {
                    for (String conflictingLibraryName : conflictingLibrariesNames) {
                        String prefix = conflictingLibraryName.substring(0, conflictingLibraryName.indexOf(libraryName));
                        if (!"".equals(prefix)) {
                            regexForbiddenPrefix += prefix + "|";
                        }
                    }
                    if (!"".equals(regexForbiddenPrefix)) {
                        regexForbiddenPrefix = regexForbiddenPrefix.substring(0, regexForbiddenPrefix.length() - 1);
                        regexForbiddenPrefix = "(?<!" + regexForbiddenPrefix + ")";
                    }

                    for (String conflictingLibraryName : conflictingLibrariesNames) {
                        String suffix = conflictingLibraryName.substring(conflictingLibraryName.indexOf(libraryName) + libraryName.length());
                        if (!"".equals(suffix)) {
                            regexForbiddenSuffix += suffix + "|";
                        }
                    }
                    if (!"".equals(regexForbiddenSuffix)) {
                        regexForbiddenSuffix = regexForbiddenSuffix.substring(0, regexForbiddenSuffix.length() - 1);
                        regexForbiddenSuffix = "(?!" + regexForbiddenSuffix + ")";
                    }
                }

                for (String fileName : potentialDlls) {
                    String regex = "^(?:lib)?.*" + regexForbiddenPrefix + libraryName + regexForbiddenSuffix + ".*$";
                    if (fileName.matches(regex)) {
                        loadLibrary(fileName, libraries.get(libraryName));
                        libraryLoaded = true;
                    }
                }
            }
            if (!libraryLoaded) {
                loadLibrary(libraryName, libraries.get(libraryName));
            }
        }
    }

    public static void loadLibrary(String libname, boolean optional) {
        if (!loadedLibraries.contains(libname)) {
            try {
                NativeLoader.loadLibrary(libname);
            } catch (IOException e) {
                if (optional) {
                    System.err
                        .println("[WARN] " + e.getMessage() + " from jar. Assuming it is installed on the system.");
                } else {
                    System.err.println("[FATAL] " + e.getMessage() + " from jar. Dependency is required!");
                    System.exit(-1);
                }
            }
            loadedLibraries.add(libname);
        }
    }

}
