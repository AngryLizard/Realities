#pragma once
#include "Cell.h"

namespace Voronoi
{
	/* Region to store search */
	struct Region
	{
		int id;
		float lerp;
	};

	/* Actor to search in a triangulation effectively */
	class SearchActor
	{
	public:
		/* Global query ID to make sure every query gets called only once per cell (no loops) */
		static int QUERY_ID;

		SearchActor();
		virtual ~SearchActor();
		
		/* Invalidate search */
		void reset();

		/* Set to first triangle in cell */
		void reset(Cell* cell);


		/* Checks if any cell got assigned */
		bool isValid();
		

		/* Get found ID */
		int getID();

		/* Get found triangle ID */
		int getSubID();

		/* Get found triangle's surroundings IDs */
		void getSurroundings(int& opposite, int& prev, int& next);

		/* Get found triangle's surroundings IDs */
		void getLerps(float& prev, float& next, float& meet);


		/* Update new position */
		void update(Vector vector);

	protected:
		Triangle * _triangle;
		int _sub;
		int _id;

	private:
	};
}