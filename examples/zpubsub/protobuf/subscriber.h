#ifndef __SUBSCRIBER_H__
#define __SUBSCRIBER_H__

#include "topic.h"

namespace zpubsub
{
    typedef void (sample_fn) (const Topic& sample);

    class Subscriber
    {
    private:
        Topic *m_topic;
        char *m_key;
        sample_fn *m_callback;

    public:
        Subscriber (const Topic &topic, const char *szDefaultPartition, sample_fn *callback);
        virtual ~Subscriber ();

        const char *GetKey () const;
        void Callback (byte *data, size_t size) const;

        static char *GenerateKey (const Topic &topic, const char *szDefaultPartition);
        static const char *GetPartition (const Topic &topic, const char *szDefaultPartition);
    };
}

#endif
