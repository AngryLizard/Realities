#pragma once


#include "Array.h"

namespace Voronoi
{
	template<typename T>
	Array<T>::Array(T* t, int s)
		:	_members(t),
			_margin(s),
			_length(s)
	{
		reallocate(s);
	}

	template<typename T>
	Array<T>::Array(int reserved)
		:	_members(nullptr),
			_length(0),
			_margin(reserved)
	{
		reallocate(reserved);
	}

	template<typename T>
	Array<T>::~Array()
	{
		if (_members != nullptr)
			delete[](_members);
	}

	template<typename T>
	void Array<T>::reallocate(int size)
	{
		T* members = new T[_reserved = __max(size, _margin)];
		memset(members, 0, sizeof(T) * _reserved);
		memcpy(members, _members, sizeof(T) * _length);
		if(_members != nullptr)
			delete[](_members);
		_members = members;
	}

	template<typename T>
	int Array<T>::getLength()
	{
		return(_length);
	}
	
	template<typename T>
	void Array<T>::adapt(int size)
	{
		while (_reserved < size)
			_reserved = _reserved << 1;
		reallocate(_reserved);
	}

	template<typename T>
	T& Array<T>::operator[](int i)
	{
		adapt(i + 1);
		_length = __max(_length, i + 1);
		return(_members[i]);
	}

	template<typename T>
	std::string Array<T>::print(Print<T> print)
	{
		std::string text = "";
		for (int i = 0; i < _length; i++)
			text = text.append(print(_members[i]).append(", "));
		return(text);
	}

	template<typename T>
	void Array<T>::push(T t)
	{
		if (_length == _reserved)
			reallocate(_reserved << 1);
		_members[_length++] = t;
	}

	template<typename T>
	void Array<T>::trunc(int size)
	{
		_length -= size;
		if (_length < _reserved >> 1)
			reallocate(_length);
	}

	template<typename T>
	void Array<T>::remove(T t)
	{
		int j = 0;
		for (int i = 0; i < _length; i++)
			if (_members[i] != t)
				_members[j++] = _members[i];
		
		trunc(_length - j);
	}

	template<typename T>
	T Array<T>::pop()
	{
		T t = _members[_length - 1];
		trunc(1);
		return(t);
	}
}