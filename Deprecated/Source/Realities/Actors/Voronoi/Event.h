#pragma once
#include "EdgePoint.h"

namespace Voronoi
{
	class ParabolaTree;
	class ParabolaTreeRoot;

	/* Triggerable event to be added to a heap */
	class Event : public CmpHeap<Event*>
	{
		public:
			Event();
			virtual ~Event();

			/* Default compare instruction by y coordinate */
			static bool slt(Event* s, Event* t);

			/* Trigger overrideable to implement behaviour */
			virtual void trigger(ParabolaTreeRoot* tree, CmpHeap<Event*>*& events) = 0;
			virtual ParabolaTree* trigger() = 0;

			/* Coordinates of this event */
			virtual Vector getCenter() = 0;
	};
}