#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <czmq.h>
#include "../../../labs/zpubsub.h"
#include "topic.h"
#include "subscriber.h"

namespace zpubsub
{
    class Participant
    {
    private:
        zpubsub_t *m_pubSub;
        zhash_t *m_subscribers;
        const char *m_szDefaultPartition;
        static const char *cm_szDefaultPartition;

    public:
        Participant (int iDomain, const char *szDefaultPartition = NULL);
        ~Participant ();

        void Subscribe (const Topic &topic, sample_fn *callback);
        void Unsubscribe (const Topic &topic);

        void Publish (const Topic &sample) const;
    };
}

#endif
