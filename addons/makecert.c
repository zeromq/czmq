/*
    makecert
    
    Certificate generator for ZeroMQ CURVE security. Produces two files:
    
    * mycert.txt - public certificate
    * mycert.txt_secret - secret certificate
    
    Copyright (C) 2013 iMatix Corporation 

    Licensed under MIT/X11.
    
    Permission is hereby granted, free of charge, to any person obtaining 
    a copy of this software and associated documentation files (the 
    "Software"), to deal in the Software without restriction, including 
    without limitation the rights to use, copy, modify, merge, publish, 
    distribute, sublicense, and/or sell copies of the Software, and to 
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <czmq.h>

//  Get and store one header in certificate

static int
s_get_header (zcert_t *cert, char *prompt, char *name)
{
    printf ("%s ", prompt);
    char value [256];
    if (fgets (value, 256, stdin) == NULL)
        return -1;
        
    if (strlen (value) && value [strlen (value) - 1] == '\n')
        value [strlen (value) - 1] = 0;
    if (*value)
        zcert_set_header (cert, name, value);
    return 0;
}

int main (void) 
{
    puts ("Creating new CURVE certificate");

    zcert_t *cert = zcert_new ();
    if (s_get_header (cert, "Enter your full name:", "name")
    ||  s_get_header (cert, "Enter your email address:", "email")
    ||  s_get_header (cert, "Enter your organization:", "organization"))
        return -1;
        
    char *timestr = zclock_timestr ();
    zcert_set_header (cert, "created-by", "CZMQ makecert");
    zcert_set_header (cert, "date-created", timestr);
    free (timestr);
    zcert_dump (cert);
    zcert_save (cert, "mycert.txt");
    puts ("I: CURVE certificate created in mycert.txt and mycert.txt_secret");
    zcert_destroy (&cert);

    return 0;
}
