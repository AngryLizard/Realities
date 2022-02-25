#pragma once


#include "CmpHeap.h"


namespace Voronoi
{
	template<typename T>
	CmpHeap<T>::CmpHeap(T t, CmpHeap** parent)
			: _content(t),
			_parent(parent),
			_left(nullptr),
			_right(nullptr)
		{
		}

	template<typename T>
	CmpHeap<T>::~CmpHeap()
	{
		// Delete children
		if (_left != nullptr) 
			delete(_left);
		if (_right != nullptr) 
			delete(_right);
	}

	template<typename T>
	T CmpHeap<T>::getContent()
	{
		return(_content);
	}

	template<typename T>
	bool CmpHeap<T>::isAttached()
	{
		return(_parent != nullptr);
	}

	template<typename T>
	void CmpHeap<T>::attach(CmpHeap<T>** parent)
	{
		*parent = this;
		_parent = parent;
	}

	template<typename T>
	void CmpHeap<T>::detach()
	{
		if(_parent != nullptr)
		*_parent = nullptr;
		_parent = nullptr;
	}

	template<typename T>
	void CmpHeap<T>::sink(Cmp<T> slt)
	{
		//Remove from tree
		CmpHeap<T>** parent = _parent;
		extract(slt);

		// Insert at parent
		(*parent)->insert(this, slt);
	}

	template<typename T>
	void CmpHeap<T>::defer(CmpHeap* h, Cmp<T> slt)
	{
		if (_left == nullptr)
		{
			h->_parent = &_left;
			_left = h;
		}
		else if (_right == nullptr)
		{
			h->_parent = &_right;
			_right = h;
		}
		else if (rand() < 0x3FFF)
			_left->insert(h, slt);
		else
			_right->insert(h, slt);
	}

	template<typename T>
	void CmpHeap<T>::insert(CmpHeap<T>* h, Cmp<T> slt)
	{
		if (h == nullptr) return;
		
		if (slt(h->_content, _content))
		{
			h->_parent = _parent;
			*_parent = h;
			h->insert(this, slt);
		}
		else
			defer(h, slt);
	}

	template<typename T>
	void CmpHeap<T>::truncate()
	{
		_left = nullptr;
		_right = nullptr;
	}

	template<typename T>
	T CmpHeap<T>::extract(Cmp<T> slt)
	{
		if (!isAttached()) 
			return(_content);

		if (_left != nullptr)
		{
			*_parent = _left;
			_left->_parent = _parent;
			_left->insert(_right, slt);
		}
		else if (_right != nullptr)
		{
			*_parent = _right;
			_right->_parent = _parent;
		}
		else
			*_parent = nullptr;

		truncate();
		_parent = nullptr;
		return(_content);
	}

	template<typename T>
	std::string CmpHeap<T>::print(Print<T> print, int tabs)
	{
		std::string text = "";

		if(_left != nullptr)
			text.append(_left->print(print, tabs + 1));

		text.append("\n");
		for (int i = 0; i < tabs; i++) text.append("    ");
		text.append(print(_content));

		if (_right != nullptr)
			text.append(_right->print(print, tabs + 1));

		return(text);
	}
}
