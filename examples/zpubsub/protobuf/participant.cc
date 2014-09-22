#include "participant.h"

namespace zpubsub
{
	const char *Participant::cm_szDefaultPartition = "<default>";
	
	static TopicDescriptor *
	s_filter_to_topic_descriptor (zpubsub_filter_t *filter)
	{
		assert (filter);
		
		TopicDescriptor *td = new TopicDescriptor;
		td->set_magicnumber (filter->magic_number);
		td->set_majorversion (filter->major_version);
		td->set_minorversion (filter->minor_version);
		td->set_topicname (filter->topic);
		td->set_partition (filter->partition);
		
		return td;
	}
	
	static zpubsub_filter_t *
	s_topic_descriptor_to_filter (TopicDescriptor *td)
	{
		assert (td);
		
		zpubsub_filter_t *filter = (zpubsub_filter_t *) zmalloc (sizeof (zpubsub_filter_t));
		filter->magic_number = td->magicnumber ();
		filter->major_version = td->majorversion ();
		filter->minor_version = td->minorversion ();
		filter->topic = strdup (td->topicname ().c_str ());
		filter->partition = strdup (td->partition ().c_str ());
		
		return filter;
	}
	
	static int
	s_serialize_filter (zpubsub_filter_t *filter, byte *data, int size)
	{
		assert (filter);
		
		TopicDescriptor *td = s_filter_to_topic_descriptor (filter);
		int required_size = td->ByteSize ();
		
		if (size >= required_size)
		{
			assert (data);
			
			td->SerializeToArray (data, size);
		}
		
		delete td;
		return required_size;
	}
	
	static int
	s_deserialize_filter (byte *data, int size, zpubsub_filter_t **filter)
	{
		assert (data);
		assert (size > 0);
		assert (filter);
		
		TopicDescriptor td;
		td.ParseFromArray (data, size);		
		*filter = s_topic_descriptor_to_filter (&td);
		
		return 0;
	}
	
	static int
	s_serialize_message (const char *topic, const char *partition, void* message, byte *data, int size)
	{
		assert (topic);
		assert (partition);
		assert (message);
		
		::google::protobuf::Message *msg = (::google::protobuf::Message *) message;		
		int required_size = msg->ByteSize ();
		
		if (size >= required_size)
		{
			assert (data);
			
			msg->SerializeToArray (data, size);
		}
			
		return required_size;
	}
	
	static int
	s_deserialize_message (const char *topic, const char *partition, void *args, byte *data, int size, void** message)
	{
		assert (topic);
		assert (partition);
		assert (args);
		assert (data);
		assert (size > 0);
		assert (message);
		
		Topic *sample = new Topic (*((Topic *) args));
		sample->SetMessageData (data, size);
		*message = sample->GetMessage ();
		
		return 0;
	}
	
	Participant::Participant (int iDomain, const char *szDefaultPartition)
	{		
		m_szDefaultPartition = szDefaultPartition? strdup (szDefaultPartition): strdup (cm_szDefaultPartition);
		m_pubSub = zpubsub_new (iDomain, m_szDefaultPartition, s_serialize_filter, s_deserialize_filter);
	}
	
	Participant::~Participant ()
	{
		zpubsub_destroy (&m_pubSub);
	}
	
	void Participant::Subscribe (Topic *topic, zpubsub_sample_fn *callback)
	{
		topic->EnsurePartition (m_szDefaultPartition);
		zpubsub_subscribe (m_pubSub, (char *) topic->GetTypeName (), (char *) topic->GetPartition (), (void *) topic, s_deserialize_message, callback);
	}
	
	void Participant::Unsubscribe (Topic *topic)
	{
		topic->EnsurePartition (m_szDefaultPartition);
		zpubsub_unsubscribe (m_pubSub, (char *) topic->GetTypeName (), (char *) topic->GetPartition ());
	}

	void Participant::Publish (Topic *sample) const
	{
		assert(sample);
		
		sample->EnsurePartition (m_szDefaultPartition);
		zpubsub_publish (m_pubSub, (char *) sample->GetTypeName (), (char *) sample->GetPartition (), sample->GetMessage(), s_serialize_message);
	}	
}
