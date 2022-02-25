#pragma once
#include <string>

namespace Voronoi
{
	/* 2D Vector */
	class Vector
	{
	public:
		Vector();
		Vector(float x, float y);
		virtual ~Vector();

		float x;
		float y;

		/* true if x and y are equal */
		bool operator==(Vector vector);

		/* true if x and y are not equal */
		bool operator!=(Vector vector);

		/* vector subtraction */
		Vector operator-(Vector vector);

		/* vector addition */
		Vector operator+(Vector vector);

		/* Scalar product */
		Vector operator*(float f);

		/* Scalar division */
		Vector operator/(float f);

		/* Cross product */
		float cross(Vector vector);

		/* Dot product */
		float dot(Vector vector);

		/* Round to the hundredth */
		Vector round();

		/* Return Vector orthogonal to this one */
		Vector orth();

		/* Get unit vector */
		Vector norm();

		/* Get vector size */
		float size();

		/* True if any x or y are NaN */
		bool isNan();

		/* Very cheap angle function in [0, 4[ */
		float cheapAngle();

		std::string print();

	private:

	};
}