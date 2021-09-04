#pragma once


#include "EdgePoint.h"
#include "Event.h"
#include "Edge.h"

namespace Voronoi
{
	/* Region with value and id */
	class Cell : public Event
	{
		public:
			/* Create empty cell */
			Cell();

			/* Create cell with <id> on position <v> */
			Cell(Vector v, int id);

			virtual ~Cell();

			/* This cells center */
			EdgePoint center;

			/* Get ID of this cell */
			int getID();



			/* Get intersection ( y coordinate ) of sweep with parabola */
			float parabola(Vector sweep);

			/* pierce appended triangles (use different queryID for new query) */
			Triangle* pierce(Vector vector, int queryID);

			/* pierce triangles of neighbouring cells (use different queryID for new query) */
			Triangle* pierceNeighbours(Vector vector, int queryID);

			/* pierce all cells starting from this (use different queryID for new query) */
			Triangle* pierceAll(Vector vector, int queryID);


			/* Implement event's trigger, create entry in Parabola tree */
			void trigger(ParabolaTreeRoot* tree, CmpHeap<Event*>*& events) override;

			/* Implement event's trigger, return new Parabola tree */
			ParabolaTree* trigger() override;

			/* Implement event's center */
			Vector getCenter() override;


			/* Call <func> on each neighbouring cells */
			template<typename L>
			void forEachNeighbour(L func)
			{
				center.forEachEdge([&func](Edge* edge)->bool
				{
					return(func(edge->getOut()));
				});
			}
			

		protected:
			float _value;
			int _id;

		private:
			int _queryID;
	};
}