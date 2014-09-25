#include <stdio.h>
#include "topic.h"

namespace zpubsub
{
    Topic::Topic (::google::protobuf::Message *messageTemplate, const char *szPartition)
    {
        assert (messageTemplate != NULL);

        m_message = NULL;
        m_szPartition = NULL;

        if (szPartition != NULL)
            m_szPartition = strdup (szPartition);

        SetMessage (messageTemplate);
    }

    Topic::Topic (const Topic& rhs)
    {
        assert (rhs.m_message);

        m_message = NULL;
        m_szPartition = NULL;

        if (rhs.m_szPartition != NULL)
            m_szPartition = strdup (rhs.m_szPartition);

        SetMessage (rhs.m_message->New ());
    }

    Topic::~Topic ()
    {
        if (m_message != NULL)
            delete m_message;

        if (m_szPartition != NULL)
            free (m_szPartition);
    }

    const char *Topic::GetPartition () const
    {
        return m_szPartition;
    }

    const char* Topic::GetTypeName () const
    {
        assert (m_message);

        return m_message->GetTypeName ().c_str ();
    }

    void Topic::EnsurePartition (const char *szDefaultPartition)
    {
        assert (szDefaultPartition != NULL);

        if (m_szPartition == NULL)
            m_szPartition = strdup (szDefaultPartition);
    }

    ::google::protobuf::Message *Topic::GetMessage () const
    {
        return m_message;
    }

    void Topic::SetMessage (::google::protobuf::Message *message)
    {
        assert (message != NULL);

        if (m_message != NULL)
            delete m_message;

        m_message = message;
    }

    byte *Topic::GetMessageData (size_t *size) const
    {
        assert (size != NULL);
        *size = 0;

        if (m_message == NULL)
            return NULL;

        *size = m_message->ByteSize ();
        byte* data = new byte[*size];
        m_message->SerializeToArray (data, *size);

        return data;
    }

    void Topic::SetMessageData (byte *data, size_t size)
    {
        assert (data != NULL);
        assert (size > 0);
        assert (m_message != NULL);

        m_message->ParseFromArray (data, size);
    }
}
