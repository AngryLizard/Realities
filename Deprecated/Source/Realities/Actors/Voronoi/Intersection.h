#pragma once
#include "Event.h"
#include "Edge.h"

namespace Voronoi
{
	/* Intersection event between two edges (between two parabolas) */
	class Intersection : public Event
	{
	public:
		Intersection();
		virtual ~Intersection();



		/* Region (edge) to the left */
		ParabolaTree* left;

		/* Region (edge) to the right */
		ParabolaTree* right;

		/* Calculate future intersection between left and right Region (edge) */
		bool intersect();

		/* Actually intersedt left and right Region (edge) */
		Edge* connect();



		/* Trigger intersection */
		void trigger(ParabolaTreeRoot* tree, CmpHeap<Event*>*& events) override;

		/* Return left Region (edge) */
		ParabolaTree* trigger() override;

		/* Return sweep location */
		Vector getCenter() override;

	private:
		Vector _intersection;
		float _sweep;
	};
}