#! /usr/bin/perl
#
#   Checks for socket options missing from model, based on current libzmq
#   sitting in ../../libzmq/doc/zmq_[g|s]etsockopt.txt.

#   Hoover up socket options defined in our model
if (open (INPUT, "sockopts.xml")) {
    while (<INPUT>) {
        if (/<option name = \"([^\"]+)\"/) {
            $option = $1;
        }
        if (/mode = \"([^\"]+)\"/) {
            #   Store option and mode r/w/rw
            $options {$option} = $1;
        }
    }
}

if (open (INPUT, "../../libzmq/doc/zmq_setsockopt.txt")) {
    while (<INPUT>) {
        if (/^ZMQ_([^:]+):/) {
            $option = lc $1;
            if (!$options {$option}) {
                print "Missing option: $1 (write)\n";
            }
            elsif ($options {$option} ne "w"
                && $options {$option} ne "rw") {
                print "Not writeable: $1\n";
            }
        }
    }
}

if (open (INPUT, "../../libzmq/doc/zmq_getsockopt.txt")) {
    while (<INPUT>) {
        if (/^ZMQ_([^:]+):/) {
            $option = lc $1;
            if (!$options {$option}) {
                print "Missing option: $1 (read)\n";
            }
            elsif ($options {$option} ne "r"
                && $options {$option} ne "rw") {
                print "Not readable  : $1\n";
            }
        }
    }
}
