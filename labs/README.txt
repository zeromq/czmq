# Experimental Area

This directory is for classes that are works in progress, or experiments. The rules here are a little different. Experiments won't be packaged and shipped in tarballs. The API header for each class sits in this directory together with the source file. There is no visibility of experiments in the CZMQ project structure: neither in makefiles, nor documentation, nor czmq.h.

To use an experimental class in your code, you can either copy it wholesale, or you can include it directly (include the header file and include the source into a class in your project).

We will later find a better way to package experimental classes. Our goal at present is to have a space in the git repository without any effect on the formal CZMQ API.

All other rules, regarding process, patch quality, and code style apply. Ugly or poorly-argued code risks being deleted.

Prerequisites: Install czmq!

Step 1: ./configure --enable-labs=yes
Step 2: make
Step 3: make install

Now you've installed a library called libczmq_labs and can link it with -lczmq_labs.
