#pragma once


#include "BinarySearchTree.h"

namespace Voronoi
{
	template<typename T>
	BinarySearchTree<T>::BinarySearchTree(T t, BinarySearchTree** parent)
		:	_parent(parent),
			_right(nullptr),
			_left(nullptr),
			_content(t)
	{
	}

	template<typename T>
	BinarySearchTree<T>::~BinarySearchTree()
	{
		delete(_right);
		delete(_left);
	}

	template<typename T>
	T BinarySearchTree<T>::getContent()
	{
		return(_content);
	}

	template<typename T>
	T BinarySearchTree<T>::popLeftMost()
	{
		if (_left == nullptr)
		{
			T t = _content;
			detach();
			return(t);
		}
		return(_left->popLeftMost());
	}

	template<typename T>
	T BinarySearchTree<T>::popRightMost()
	{
		if (_right == nullptr)
		{
			T t = _content;
			detach();
			return(t);
		}
		return(_right->popRightMost());
	}

	template<typename T>
	bool BinarySearchTree<T>::insert(T t, Cmp<T> slt)
	{
		// Make sure node doesn't already exist
		if (t == _content) return(true);

		if (slt(t, _content))
		{
			if (_left != nullptr) 
				return(_left->insert(t, slt));
			_left = new BinarySearchTree<T>(t, &_left);
		}
		else
		{
			if (_right != nullptr) 
				return(_right->insert(t, slt));
			_right = new BinarySearchTree<T>(t, &_right);
		}

		return(true);
	}

	template<typename T>
	void BinarySearchTree<T>::overwrite(BinarySearchTree<T>*& tree)
	{
		// Set parent
		*_parent = tree;
		// Set child's parent
		if(tree != nullptr) 
			tree->_parent = _parent;
		// Reset this
		tree = nullptr;
		delete(this);
	}

	template<typename T>
	void BinarySearchTree<T>::detach()
	{
		// Overwrite if leaf
		if (_left == nullptr)
			overwrite(_right);
		else if (_right == nullptr)
			overwrite(_left);
		else if (rand() < 0x3fff)
		{
			_content = _right->popLeftMost();
		}
		else
		{
			_content = _left->popRightMost();
		}
	}

	template<typename T>
	bool BinarySearchTree<T>::remove(T t, Cmp<T> slt)
	{
		// Search tree until found
		if (_content == t)
		{
			detach();
			return(true);
		}
		else if (slt(t, _content))
		{
			if (_left != nullptr) 
				 return(_left->remove(t, slt));
		}
		else
		{
			if (_right != nullptr)
				return(_right->remove(t, slt));
		}

		return(false);
	}

	template<typename T>
	std::string BinarySearchTree<T>::print(Print<T> print, int tabs)
	{
		std::string text = "";

		for (int i = 0; i < tabs + 1; i++) text.append("    ");
		if (_left != nullptr) text.append(_left->print(print, tabs + 1));

		text.append("\n");
		for (int i = 0; i < tabs; i++) text.append("    ");

		text.append(print(_content));

		for (int i = 0; i < tabs + 1; i++) text.append("    ");
		if (_right != nullptr) text.append(_right->print(print, tabs + 1));

		return(text);
	}
}