/*
    zmakecert [filename]

    Certificate generator for ZeroMQ CURVE security. Produces two files:
    
    * {filename} - public certificate
    * {filename}_secret - secret certificate
    
    Default filename is mycert.txt.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "czmq_classes.h"

//  Get and store one header in certificate

static int
s_get_meta (zcert_t *cert, char *prompt, char *name)
{
    printf ("%s ", prompt);
    char value [256];
    if (fgets (value, 256, stdin) == NULL)
        return -1;
        
    if (strlen (value) && value [strlen (value) - 1] == '\n')
        value [strlen (value) - 1] = 0;
    if (*value)
        zcert_set_meta (cert, name, "%s", value);
    return 0;
}

int main (int argc, char *argv [])
{
    int argn = 1;
    char *filename = "mycert.txt";
    if (argn < argc)
        filename = argv [argn++];

    zsys_info ("Creating new CURVE certificate in %s", filename);

    zcert_t *cert = zcert_new ();
    if (s_get_meta (cert, "Enter your full name:", "name")
    ||  s_get_meta (cert, "Enter your email address:", "email")
    ||  s_get_meta (cert, "Enter your organization:", "organization"))
        return -1;
        
    char *timestr = zclock_timestr ();
    zcert_set_meta (cert, "created-by", "CZMQ zmakecert");
    zcert_set_meta (cert, "date-created", "%s", timestr);
    free (timestr);
    zcert_dump (cert);
    zcert_save (cert, filename);
    zsys_info ("CURVE certificate created in %s and %s_secret", filename, filename);
    zcert_destroy (&cert);

    return 0;
}
