#pragma once

#include "Array.cpp"

namespace Voronoi
{
	template <typename T>
	using Cmp = std::function<bool(T s, T t)>;

	template <typename T>
	using Print = std::function<std::string(T t)>;

	/*
	Min/Max heap
	*/
	template<typename T>
	class CmpHeap
	{
		public:
			/* Create node with <t> with parent <parent> */
			CmpHeap(T t, CmpHeap** parent);

			virtual ~CmpHeap();


			/* Return this node's content */
			T getContent();

			/* True if part of a heap */
			bool isAttached();

			/* Attach to a heap */
			void attach(CmpHeap** parent);

			/* Detach from heap */
			void detach();



			/* Detach children */
			void truncate();

			/* Update position in this tree */
			void sink(Cmp<T> slt);

			/* Settle <h> in this subtree */
			void defer(CmpHeap* h, Cmp<T> cmp);



			/* Add <h> to this subtree, swap to uphold compare condition */
			virtual void insert(CmpHeap* h, Cmp<T> cmp);

			/* Remove from tree and put new element at the top */
			virtual T extract(Cmp<T> slt);

			/* Print this tree */
			std::string print(Print<T> print, int tabs = 0);
		
		protected:
			T _content;

			CmpHeap** _parent;
			CmpHeap* _left;
			CmpHeap* _right;
	};
}
