#pragma once
#include "Intersection.h"
#include "Cell.h"

namespace Voronoi
{
	/* Parabolatree supporting a scanline algorithm, stands for a region/interval between two parabolas */
	class ParabolaTree
	{
		public:
			/* Create node from cell */
			ParabolaTree(Cell* cell);

			/* Create node from neighbour */
			ParabolaTree(ParabolaTree* left, Edge* edge);
			virtual ~ParabolaTree();



			/* Node to the right */
			ParabolaTree* neighbour;

			/* Intersection to the right */
			Intersection intersection;



			/* Edge of this region */
			Edge* getEdge();

			/* Get position of intersection with nearest parabola */
			Vector getAnchor(Cell* left, Cell* right);



			/* Replace <child> with <tree> */
			void replace(ParabolaTree* child, ParabolaTree* tree);

			/* Update neighbour pointers, update and add intersections */
			void link(ParabolaTree* left, ParabolaTree* right, CmpHeap<Event*>*& events);

			/* Update intersection, intersect if necessary and relink regions */
			void update(Intersection* meet, CmpHeap<Event*>*& events);

			/* Remove region from this tree (intersection happened) */
			void detach(ParabolaTree* tree, CmpHeap<Event*>*& events);

			/* Insert new region, updating and adding intersection */
			void insert(ParabolaTree* tree, CmpHeap<Event*>*& events);
			
		private:
			ParabolaTree* _parent;
			ParabolaTree* _right;
			ParabolaTree* _left;

			Edge* _edge;
			Cell* _cell;
	};


	/* Topmost element of Parabolatree for administration reasons */
	class ParabolaTreeRoot
	{
		public:
			ParabolaTreeRoot();
			virtual ~ParabolaTreeRoot();

			void insert(ParabolaTree* tree, CmpHeap<Event*>*& events);

		private:
			ParabolaTree* _tree;
	};
}