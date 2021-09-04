#pragma once


#include "Triangle.h"
#include "Cell.h"

namespace Voronoi
{

	Triangle::Triangle(int id)
		:	_id(id),
			_cell(nullptr),
			_prev(nullptr),
			_next(nullptr)
	{
	}


	Triangle::~Triangle()
	{
	}

	bool Triangle::cmp(Triangle* a, Triangle* b, EdgePoint* c)
	{
		return((a->_m - c->p).cheapAngle() < (b->_m - c->p).cheapAngle());
	}


	bool Triangle::inside(Vector vector)
	{
		_s = _fact * (_sO + _sX * vector.x + _sY * vector.y);
		_t = _fact * (_tO + _tX * vector.x + _tY * vector.y);
		_r = 1.0f - (_s + _t);
		
		return((_s >= 0.0f) && (_t >= 0.0f) && (_r >= 0.0f));
	}

	Cell* Triangle::getCell()
	{
		return(_cell);
	}

	int Triangle::getID()
	{
		return(_id);
	}


	float Triangle::getR()
	{
		return(_r);
	}

	float Triangle::getS()
	{
		return(_s);
	}

	float Triangle::getT()
	{
		return(_t);
	}

	void Triangle::unlink()
	{
		_cell->center.trunc(this);
		_prev->trunc(this);
		_next->trunc(this);
	}

	void Triangle::link(Cell* cell, EdgePoint* prev, EdgePoint* next)
	{
		_cell = cell;
		_prev = prev;
		_next = next;

		Vector p0 = _cell->center.p;
		Vector p1 = prev->p;
		Vector p2 = next->p;

		_m = (p0 + p1 + p2) / 3;

		_cell->center.append(this);
		_prev->append(this);
		_next->append(this);

		_sX = p2.y - p0.y;
		_tX = p0.y - p1.y;

		_sY = p0.x - p2.x;
		_tY = p1.x - p0.x;

		_sO = p0.y * p2.x - p0.x * p2.y;
		_tO = p0.x * p1.y - p0.y * p1.x;

		_fact = 1.0f / (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
	}
}