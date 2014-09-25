#include "participant.h"

namespace zpubsub
{
    const char *Participant::cm_szDefaultPartition = "<default>";

    static void
    s_sample_fn (const char *topic, const char *partition, void *args, byte *sample, size_t size)
    {
        assert (topic);
        assert (partition);
        assert (args);
        assert (sample);

        Subscriber *sub = (Subscriber *) args;
        sub->Callback (sample, size);
    }

    static void
    s_free_subscriber (void **subscriber)
    {
        assert (subscriber);

        Subscriber * sub = (Subscriber *) *subscriber;
        if (sub) {
            delete sub;
            *subscriber = NULL;
        }
    }

    Participant::Participant (int iDomain, const char *szDefaultPartition)
    {
        m_szDefaultPartition = szDefaultPartition? strdup (szDefaultPartition): strdup (cm_szDefaultPartition);
        m_pubSub = zpubsub_new (iDomain, m_szDefaultPartition);
        m_subscribers = zhash_new ();
        zhash_set_destructor (m_subscribers, s_free_subscriber);
    }

    Participant::~Participant ()
    {
        zpubsub_destroy (&m_pubSub);
        zhash_destroy (&m_subscribers);
    }

    void Participant::Subscribe (const Topic &topic, sample_fn *callback)
    {
        char *key = Subscriber::GenerateKey (topic, m_szDefaultPartition);

        if (zhash_lookup (m_subscribers, key) == NULL) {
            Subscriber *sub = new Subscriber (topic, m_szDefaultPartition, callback);
            zpubsub_subscribe (m_pubSub, topic.GetTypeName (), topic.GetPartition (), sub, s_sample_fn);
            zhash_update (m_subscribers, key, sub);
        }

        delete[] key;
    }

    void Participant::Unsubscribe (const Topic &topic)
    {
        char *key = Subscriber::GenerateKey (topic, m_szDefaultPartition);

        if (zhash_lookup (m_subscribers, key)) {
            zpubsub_unsubscribe (m_pubSub, topic.GetTypeName (), topic.GetPartition ());
            zhash_delete (m_subscribers, key);
        }

        delete[] key;
    }

    void Participant::Publish (const Topic &sample) const
    {
        const char *topic = sample.GetTypeName ();
        const char *partition = Subscriber::GetPartition (sample, m_szDefaultPartition);

        assert (topic);
        assert (partition);

        size_t size;
        byte *data = sample.GetMessageData (&size);
        assert (data);

        zpubsub_publish (m_pubSub, topic, partition, data, size);
        delete[] data;
    }
}
