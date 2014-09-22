#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <czmq.h>
#include "topic.h"

namespace zpubsub
{	
	class Participant
	{
	private:
		zpubsub_t *m_pubSub;
		const char *m_szDefaultPartition;
		static const char *cm_szDefaultPartition;
	
	public:
		Participant (int iDomain, const char *szDefaultPartition = NULL);
		~Participant ();
		
		void Subscribe (Topic *topic, zpubsub_sample_fn *callback);
		void Unsubscribe (Topic *topic);
		
		void Publish (Topic *sample) const;
	};
}

#endif
