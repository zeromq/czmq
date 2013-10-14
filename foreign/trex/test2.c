//  Alternative text example, by Pieter Hintjens

#include <czmq.h>
#undef _UNICODE
#define TREX_API    //  Empty, trex not exported to API
#include "trex.h"
#include "trex.c"

int main (void)
{
    const char *begin,*end;
    const char *error = NULL;
    TRex *trex = trex_compile ("([0-9]+)\\-([0-9]+)\\-([0-9]+)", &error);
    if (trex) {
        if (trex_search (trex, "123-456-789", &begin, &end)) {
            int matches = trex_getsubexpcount (trex);
            int index;
            //  First match is always the whole expression
            for (index = 1; index < matches; index++) {
                TRexMatch match;
                trex_getsubexp (trex, index, &match);
                printf ("[%d] %.*s\n", index, match.len, match.begin);
            }
            printf ("I: %d sub matches\n", matches - 1);
        }
        else
            printf ("W: no match\n");

        trex_free (trex);
    }
    else
        printf ("E: compilation error [%s]\n", error? error: "undefined");
    return 0;
}
