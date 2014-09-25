#include "subscriber.h"

namespace zpubsub
{
    Subscriber::Subscriber (const Topic &topic, const char *szDefaultPartition, sample_fn *callback)
    {
        assert (szDefaultPartition);
        assert (callback);

        m_key = GenerateKey (topic, szDefaultPartition);

        m_topic = new Topic (topic);
        m_topic->EnsurePartition (szDefaultPartition);

        m_callback = callback;
    }

    Subscriber::~Subscriber ()
    {
        delete[] m_key;
        delete m_topic;
    }

    const char *
    Subscriber::GetKey () const
    {
        return m_key;
    }

    void
    Subscriber::Callback (byte *data, size_t size) const
    {
        m_topic->SetMessageData (data, size);
        (m_callback) (*m_topic);
    }

    char *
    Subscriber::GenerateKey (const Topic &topic, const char *szDefaultPartition)
    {
        const char *type_name = topic.GetTypeName ();
        const char *partition = GetPartition (topic, szDefaultPartition);

        assert (type_name);
        assert (partition);

        size_t size = strlen (type_name) + strlen (partition) + 2;
        char *key = new char[size];
        snprintf (key, size, "%s:%s", partition, type_name);

        return key;
    }

    const char *
    Subscriber::GetPartition (const Topic &topic, const char *szDefaultPartition)
    {
        const char *partition = topic.GetPartition ();

        if (partition == NULL || strlen (partition) == 0)
            partition = szDefaultPartition;

        return partition;
    }
}
