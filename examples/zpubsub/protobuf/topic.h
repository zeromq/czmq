#ifndef __TOPIC_H__
#define __TOPIC_H__

#include <czmq.h>
#include <google/protobuf/message.h>

namespace zpubsub
{
    class Topic
    {
    private:
        ::google::protobuf::Message* m_message;
        char* m_szPartition;

    public:
        Topic(::google::protobuf::Message* messageTemplate, const char* szPartition = NULL);
        Topic(const Topic& rhs);
        virtual ~Topic();

        void EnsurePartition(const char* szDefaultPartition);

        const char* GetTypeName() const;
        const char* GetPartition() const;

        ::google::protobuf::Message* GetMessage() const;
        void SetMessage(::google::protobuf::Message* message);

        byte* GetMessageData(size_t* size) const;
        void SetMessageData(byte* data, size_t size);
    };
}

#endif
