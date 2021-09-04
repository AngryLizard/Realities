#pragma once

#include "BinarySearchTree.cpp"
#include "CmpHeap.cpp"

#include "Triangle.h"
#include "Vector.h"

namespace Voronoi
{
	class Edge;
	class Triangle;

	/*
	Point of an edge or center of a Cell
	*/
	class EdgePoint : public CmpHeap<EdgePoint*>
	{
		public:
			/* Create point at <vector> */
			EdgePoint(Vector vector);

			virtual ~EdgePoint();
			

			/* Position */
			Vector p;


			/* Get number of connected edges */
			int getRank();

			/* Connect edge */
			void append(Edge* edge);
			
			/* Call <func> on all connected edges */
			template<typename L>
			void forEachEdge(L func)
			{
				if (_region != nullptr)
					_region->inorder(func);
			}


			/* Appends a triangle to this cell */
			void append(Triangle* joint);

			/* Removes a triangle to this cell */
			void trunc(Triangle* joint);
			
			/* Call <func> on each triangle in this triangulation */
			template<typename L>
			void forEachJoint(L func)
			{
				if (_joints != nullptr)
					_joints->inorder(func);
			}

		private:
			BinarySearchTree<Triangle*>* _joints;
			BinarySearchTree<Edge*>* _region;

			int _rank;
	};
}
