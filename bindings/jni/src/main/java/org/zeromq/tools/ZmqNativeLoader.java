package org.zeromq.tools;

import java.util.Set;
import java.util.HashSet;
import org.scijava.nativelib.NativeLoader;

public class ZmqNativeLoader {

    private static final Set<String> loadedLibraries = new HashSet<>();

    public static void loadLibrary(String libname) {
        if (!loadedLibraries.contains(libname)) {
            try {
                if (System.getProperty("java.vm.vendor").contains("Android")) {
                    System.loadLibrary(libname);
                } else {
                    NativeLoader.loadLibrary(libname);
                }
            } catch (Exception e) {
                System.err.println("[WARN] " + e.getMessage() +" from jar. Assuming it is installed on the system.");
            }
            loadedLibraries.add(libname);
        }
    }

}
