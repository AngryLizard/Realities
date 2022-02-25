#pragma once

#include <iostream>
#include <functional>

template <typename T>
using Print = std::function<std::string(T t)>;

namespace Voronoi
{
	/* Continuous array */
	template<typename T>
	class Array
	{
	public:
		/* Create array from C-Array of size <s>, deallocates C-Array */
		Array(T* t, int s);

		/* Create empty array with reserved size <reserved> */
		Array(int reserved = 16);
		virtual ~Array();
		


		/* Get number of elements in this array */
		int getLength();

		/* Change reserved array size*/
		void adapt(int size);



		/* Access element by index */
		T& operator[](int i);

		/* Set exact size (use adapt instead) */
		void reallocate(int size);



		/* Print elements using print function */
		std::string print(Print<T> print);

		/* Removes <size> elements from the end of this array */
		void trunc(int size);

		/* Removes an element <t> from this array */
		void remove(T t);

		/* Add <t> to the end of this array */
		void push(T t);

		/* Remove top element of this array */
		T pop();

	private:
		T* _members;
		int _reserved;
		int _length;
		int _margin;
	};
}