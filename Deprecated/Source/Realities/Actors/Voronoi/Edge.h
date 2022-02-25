#pragma once

#include "EdgePoint.h"

namespace Voronoi
{
	class Cell;

	/* Edge between two cells */
	class Edge : public CmpHeap<Edge*>
	{
	public:
		/* Create counter edge to <edge> */
		Edge(Edge* edge);

		/* Create edge at <start> right between <left> and <right> */
		Edge(Vector start, Cell* left, Cell* right);
		virtual ~Edge();

		/* Compare instruction for BinarySearchTree */
		static bool slt(Edge* e, Edge* f);
		


		/* Counter edge */
		Edge* back;
		
		/* Get cell to the left */
		Cell* getIn();

		/* Get cell to the right */
		Cell* getOut();



		/* Get endpoint (can be nullptr) */
		EdgePoint* getEnd();

		/* Get startpoint */
		EdgePoint* getStart();



		/* Get endpoint with distance <s> from start */
		Vector getRay(float s);

		/* Returns true if this edge is (near) vertical */
		bool isVertical();



		/* Project to vertical line at <x> and return y coordinate */
		float projectV(float x);

		/* Project to horizontal line at <y> and return x coordinate */
		float projectH(float y);

		/* Terminate edge with <point>, true whether or not this edge is owner of <point> */
		void terminate(EdgePoint* point, bool owner);

		/* Get difference between parabola from left and this edge at <sweep> */
		float disparity(Vector sweep);


		/* Intersect with box with corner <a> and <b> and return intersection */
		Vector intersect(Vector a, Vector b);

		/* Intersect with edge and return intersection */
		Vector intersect(Edge* edge);

		
	private:
		Vector _direction;
		EdgePoint* _start;
		EdgePoint* _end;
		Cell* _right;
		Cell* _left;
		bool _owner;
	};
}