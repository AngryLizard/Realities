#pragma once


#include "Vector.h"


namespace Voronoi
{
	Vector::Vector()
		: x(NAN), y(NAN)
	{
	}

	Vector::Vector(float x, float y)
		: x(x), y(y)
	{
	}

	Vector::~Vector()
	{
	}

	bool Vector::operator==(Vector vector)
	{
		return(x == vector.x && y == vector.y);
	}

	bool Vector::operator!=(Vector vector)
	{
		return(x != vector.x || y != vector.y);
	}

	Vector Vector::operator-(Vector vector)
	{
		return(Vector(x - vector.x, y - vector.y));
	}

	Vector Vector::operator+(Vector vector)
	{
		return(Vector(x + vector.x, y + vector.y));
	}

	Vector Vector::operator*(float f)
	{
		return(Vector(x * f, y * f));
	}

	Vector Vector::operator/(float f)
	{
		return(Vector(x / f, y / f));
	}

	float Vector::cross(Vector vector)
	{
		return(x * vector.y - y * vector.x);
	}

	float Vector::dot(Vector vector)
	{
		return(x * vector.x + y * vector.y);
	}

	Vector Vector::round()
	{
		return(Vector(floor(x * 100 + 0.5f) / 100, floor(y * 100 + 0.5f) / 100));
	}

	Vector Vector::orth()
	{
		return(Vector(y, -x));
	}

	Vector Vector::norm()
	{
		float s = size();
		return(*this / s);
	}

	float Vector::size()
	{
		return(sqrtf(x * x + y * y));
	}

	bool Vector::isNan()
	{
		return(isnan(x) || isnan(y));
	}

	float Vector::cheapAngle()
	{
		float m = y / (x + y);
		float n = x / (x - y);

		if (y >= 0) return (x >= 0 ? m : 1 + n);
		else		return (x < 0 ? 2 + m : 3 + n);
	}


	std::string Vector::print()
	{
		std::string text = "";

		text.append("(");
		text.append(std::to_string(x));
		text.append("|");
		text.append(std::to_string(y));
		text.append(")");

		return(text);
	}
}