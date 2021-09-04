#pragma once


#include "Edge.h"
#include "Cell.h"

namespace Voronoi
{
	Edge::Edge(Edge* edge)
		:	CmpHeap<Edge*>(this, nullptr),
			_direction(edge->_direction * -1),
			_start(edge->_start),
			_end(nullptr),
			_right(edge->_left),
			_left(edge->_right)
	{
		back = edge;
		edge->back = this;

		_left->center.append(this);
	}
	
	Edge::Edge(Vector start, Cell* left, Cell* right)
		:	CmpHeap<Edge*>(this, nullptr),
			back(nullptr),
			_end(nullptr),
			_right(right),
			_left(left),
			_owner(false)
	{
		_start = new EdgePoint(start);

		_direction = (_right->center.p - _left->center.p).orth();
		_left->center.append(this);

		_start->append(this);
	}
	
	Edge::~Edge()
	{
		// Prevent deletion of children
		truncate();
		// Back edge shares this edgepoint
		if (back != nullptr)
			back->_start = nullptr;
		// If not already deleted by back
		if(_start != nullptr)
			delete(_start);
		// Delete endpoint if not shared with anyone
		if (_owner) delete(_end);
	}

	bool Edge::slt(Edge* e, Edge* f)
	{
		return(e->_direction.cheapAngle() < f->_direction.cheapAngle());
	}

	Cell* Edge::getIn()
	{
		return(_left);
	}

	Cell* Edge::getOut()
	{
		return(_right);
	}

	EdgePoint* Edge::getEnd()
	{
		return(_end);
	}

	EdgePoint* Edge::getStart()
	{
		return(_start);
	}

	Vector Edge::getRay(float s)
	{
		return(_start->p + _direction * s);
	}

	bool Edge::isVertical()
	{
		return(abs(_direction.x) < 1e-5f);
	}

	float Edge::disparity(Vector sweep)
	{
		// Pierce parabola and bisector for y coordinates
		float parabola = _left->parabola(sweep);
		float bisector = projectV(sweep.x);
		float direction = sweep.x - _start->p.x;
		if (isnan(parabola)) return(direction);
		if (isnan(bisector)) return(direction);
		// Disparity is positive to the right
		return(_direction.x * (bisector - parabola));
	}

	float Edge::projectV(float x)
	{
		// Prevent infinite results
		if (_direction.x == 0) return(NAN);
		// Get t for line equation
		float t = (x - _start->p.x) / _direction.x;
		if (t < 0) return(NAN);
		return(_start->p.y + t * _direction.y);
	}

	float Edge::projectH(float y)
	{
		// Prevent infinite results
		if (_direction.y == 0) return(NAN);
		// Get t for line equation
		float t = (y - _start->p.y) / _direction.y;
		if (t < 0) return(NAN);
		return(_start->p.x + t * _direction.x);
	}

	void Edge::terminate(EdgePoint* point, bool owner)
	{
		_end = point;
		_owner = owner;

		_end->append(this);

		_direction = _end->p - _start->p;
	}


	Vector Edge::intersect(Vector a, Vector b)
	{
		Vector r = Vector();

		if (abs(_direction.x) > abs(_direction.y))
		{
			if (_direction.y == 0.0f)
			{
				r.y = _start->p.y;
				r.x = _direction.x > 0.0f ? b.x : a.x;
			}
			else
			{
				r.y = _direction.y > 0.0f ? b.y : a.y;
				r.x = projectH(r.y);
			}
		}
		else
		{
			if (_direction.x == 0.0f)
			{
				r.x = _start->p.x;
				r.y = _direction.y > 0.0f ? b.y : a.y;
			}
			else
			{
				r.x = _direction.x > 0.0f ? b.x : a.x;
				r.y = projectV(r.x);
			}
		}

		if(r.isNan()) 
			r = _start->p;
		return(r);
	}

	Vector Edge::intersect(Edge* edge)
	{
		Vector d = edge->_start->p - _start->p;
		float c = _direction.cross(edge->_direction);

		if (c == 0.0f) return(Vector(NAN, NAN));

		float t = d.cross(edge->_direction) / c;
		float s = d.cross(_direction) / c;
		// Make sure, intersection is in range
		if (t < 0.0f || s < 0.0f) return(Vector());
		if (t == 0.0f && s == 0.0f) return(Vector());
		if (_end != nullptr && t > 1.0f) return(Vector());
		if (edge->_end != nullptr && s > 1.0f) return(Vector());
		// Return intersection
		return(_start->p + _direction * t);
	}
}