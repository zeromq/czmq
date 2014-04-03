/*
    makecert

    Certificate generator for ZeroMQ CURVE security. Produces two files:
    
    * mycert.txt - public certificate
    * mycert.txt_secret - secret certificate
    
    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <czmq.h>

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
        zcert_set_meta (cert, name, value);
    return 0;
}

int main (void) 
{
    puts ("Creating new CURVE certificate");

    zcert_t *cert = zcert_new ();
    if (s_get_meta (cert, "Enter your full name:", "name")
    ||  s_get_meta (cert, "Enter your email address:", "email")
    ||  s_get_meta (cert, "Enter your organization:", "organization"))
        return -1;
        
    char *timestr = zclock_timestr ();
    zcert_set_meta (cert, "created-by", "CZMQ makecert");
    zcert_set_meta (cert, "date-created", timestr);
    free (timestr);
    zcert_dump (cert);
    zcert_save (cert, "mycert.txt");
    puts ("I: CURVE certificate created in mycert.txt and mycert.txt_secret");
    zcert_destroy (&cert);

    return 0;
}
