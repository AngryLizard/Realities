#pragma once

#include <functional>
#include <string>

namespace Voronoi
{
	template <typename T>
	using Cmp = std::function<bool(T s, T t)>;

	template <typename T>
	using Print = std::function<std::string(T t)>;
	
	/* Ordinary binary search tree */
	template<typename T>
	class BinarySearchTree
	{
	public:
		/* Create Node with parent and content <t> */
		BinarySearchTree(T t, BinarySearchTree** parent);
		virtual ~BinarySearchTree();


		/* Call function <proc> on all elements in order */
		template<typename P>
		bool inorder(P proc)
		{
			// nodes might be removed during iteration
			BinarySearchTree<T>* left = _left;
			BinarySearchTree<T>* right = _right;
			T content = _content;

			if (left != nullptr)
				if (!left->inorder(proc))
					return(false);

			if (!proc(content))
				return(false);

			if (right != nullptr)
				if (!right->inorder(proc))
					return(false);
			return(true);
		}


		/* Get this node's content */
		T getContent();

		/* Remove and get the left most object in this subtree */
		T popLeftMost();

		/* Remove and get the right most object in this subtree */
		T popRightMost();



		/* Remove this node from this tree */
		void detach();

		/* Replace this node with <tree> */
		void overwrite(BinarySearchTree*& tree);


		/* Insert node in this subtree using compare function <slt>
		m returns true if successful */
		virtual bool insert(T t, Cmp<T> slt);

		/* Remove node from this subtree using compare function <slt>
		m returns true if successful */
		virtual bool remove(T t, Cmp<T> slt);

		/* print elements in this subtree */
		std::string print(Print<T> print, int tabs = 0);

	private:
		BinarySearchTree** _parent;
		BinarySearchTree* _right;
		BinarySearchTree* _left;
		
		T _content;
	};
}