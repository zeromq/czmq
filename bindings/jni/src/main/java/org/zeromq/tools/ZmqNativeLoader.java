package org.zeromq.tools;

import java.io.IOException;
import java.util.Set;
import java.util.HashSet;
import org.scijava.nativelib.NativeLoader;

public class ZmqNativeLoader {

    private static final Set<String> loadedLibraries = new HashSet<>();

    public static void loadLibrary(String libname, boolean optional) {
        if (!loadedLibraries.contains(libname)) {
            try {
                NativeLoader.loadLibrary(libname);
            } catch (IOException e) {
                if (optional) {
                    System.err.println("[WARN] " + e.getMessage() + " from jar. Assuming it is installed on the system.");
                } else {
                    System.exit(-1);
                }
            }
            loadedLibraries.add(libname);
        }
    }

}
