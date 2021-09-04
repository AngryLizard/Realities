// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include <functional>

#define RESOURCE_TABLE(A, N)	CTGOR_ContentTable<std::wstring, A*, N>
#define DICTIONARY(A, N)		CTGOR_ContentTable<std::wstring, A, N>
#define HASH_TABLE(A, N)		CTGOR_ContentTable<int32, A, N>

template<typename K, typename R>
using TableLambda = std::function<bool(const K& Key, R& Entry, int32 Index)>;

template<typename K, typename R>
using ConstTableLambda = std::function<bool(const K& Key, const R& Entry, int32 Index)>;

/**
 * Hashtable
 *
 * Operates in O(n),
 * T(n) for n collisions
 * T(1) for no collision
 * Preserves indexes.
 *
 */

template<typename K, typename R, int N>
class REALITIESUTILITY_API CTGOR_ContentTable
{
private:
	struct CTGOR_Resource
	{
		K Key;
		R Ref;
		bool Del;
	};

	// Actual table length
	static const int Len = ((2 << N) - 1);

	// Dynamic list
	CTGOR_Resource* Elements[Len];
	R None;

public:

	// Deallocate all members
	void clear()
	{
		for (int i = 0; i < Len; i++)
			if (Elements[i] != nullptr)
				delete(Elements[i]);
		memset(Elements, 0, Len * sizeof(CTGOR_Resource*));
	}

	// NullObject gets returned on empty query
	CTGOR_ContentTable(R NullObject)
	{
		None = NullObject;
		memset(Elements, 0, Len * sizeof(CTGOR_Resource*));
	}

	virtual ~CTGOR_ContentTable()
	{
		clear();
	}


	// Primitive buffer implementation. Allows iteration through all elements.
	bool forEach(TableLambda<K, R> Func)
	{
		int Pointer = 0;
		while (++Pointer < Len)
		{
			CTGOR_Resource*& Node = Elements[Pointer];
			// Key not empty nor deleted
			if (Node != nullptr && !Node->Del)
			{
				if (!Func(Node->Key, Node->Ref, Pointer))
				{
					return(false);
				}
			}
		}
		return(true);
	}

	// Primitive buffer implementation. Allows iteration through all elements.
	bool forEachConst(ConstTableLambda<K, R> Func) const
	{
		int Pointer = 0;
		while (++Pointer < Len)
		{
			CTGOR_Resource*const& Node = Elements[Pointer];
			// Key not empty nor deleted
			if (Node != nullptr && !Node->Del)
			{
				if (!Func(Node->Key, Node->Ref, Pointer))
				{
					return(false);
				}
			}
		}
		return(true);
	}


	// get Hash number of key.
	int32 getHash(K Key) const
	{
		return(std::hash<K>()(Key) % Len);
	}

	// get the index of an element.
	int32 find(K Key) const
	{
		int32 i = getHash(Key);
		int32 z = (i - 1) % Len;
		while (z != i)
		{
			CTGOR_Resource* Node = Elements[i];
			// Key not found
			if (Node == nullptr) return(-1);
			// Active key found
			else if (Node->Key == Key && !Node->Del) return(i);
			i = (i + 1) % Len;
		}
		return(-1);
	}

	// get element
	R get(K Key) const
	{
		int32 i = find(Key);
		if (i == -1) return(None);
		return(Elements[i]->Ref);
	}


	// get element by index
	R getByIndex(int Index) const
	{
		// Index in range
		if (Index < 0 || Index > Len) return(None);
		CTGOR_Resource* Node = Elements[Index];
		// Node empty or deleted
		if (Node == nullptr || Node->Del) return(None);
		return(Node->Ref);
	}

	// get pointer to element
	R& getReference(K Key) const
	{
		int32 i = find(Key);
		if (i == -1) return(nullptr);
		return(Elements[i]->Ref);
	}

	// get pointer to element by index
	R& getReferenceByIndex(int Index) const
	{
		// Index in range
		if (Index < 0 || Index > Len) return(nullptr);
		CTGOR_Resource*& Node = Elements[Index];
		// Node deleted
		if (Node->Del) return(nullptr);
		return(Node->Ref);
	}

	// get key at index
	K getKey(int Index) const
	{
		// Index in range
		if (Index < 0 || Index > Len) return((K)NULL);
		CTGOR_Resource* Node = Elements[Index];
		// Node empty or deleted
		if (Node == nullptr || Node->Del) return((K)NULL);
		return(Node->Key);
	}



	// Insert an element
	int32 insert(K Key, R Ref)
	{
		int32 i = getHash(Key);
		int32 z = (i - 1) % Len;
		while (z != i)
		{
			CTGOR_Resource*& Node = Elements[i];
			// Check if node is empty
			if (Node == nullptr)
			{
				Node = new CTGOR_Resource({ Key, Ref, false });
				return(i);
			}
			// Check if deletion mark is set
			else if (Node->Del)
			{
				int32 index = find(Key);
				// Key doesn't exist yet
				if (index == -1)
				{
					*Node = { Key, Ref, false };
					return(i);
				}
				// Key already exists, replace
				else
				{
					Elements[index]->Ref = Ref; return(index);
				}
			}
			// If key already exists, replace data
			else if (Node->Key == Key)
			{
				*Node = { Key, Ref, false };
				return(i);
			}
			i = (i + 1) % Len;
		}
		return(-1);
	}

	// remove element
	R remove(K Key)
	{
		int32 i = find(Key);
		if (i == -1) return(None);
		CTGOR_Resource*& Node = Elements[i];
		Node->Del = true;
		// Copy, because could be replaced
		return(Node->Ref);
	}
};