// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "TGOR_Insertions.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Content;

#define DECL_INSERTION(I) CTGOR_Insertion<decltype(I),false> Instanced_##I
#define DECL_INSERTION_REQUIRED(I) CTGOR_Insertion<decltype(I),true> Instanced_##I
#define MOV_INSERTION(I) Success = Instanced_##I.MoveInsertion(I, Insertion, Action) && Success;

UENUM(BlueprintType)
enum class ETGOR_InsertionActionEnumeration : uint8
{
	Add,
	Remove,
	Validate
};

template<typename S, bool B, typename = void>
struct CTGOR_Insertion
{};

template<typename S, bool B>
struct CTGOR_Insertion<TSubclassOf<S>, B>
{
	S* Collection;

	bool Contains(const S* Content) const
	{
		return Collection == Content;
	}

	S* GetOfType(TSubclassOf<S> Type) const
	{
		if (*Type)
		{
			if (IsValid(Collection) && Collection->IsA(Type))
			{
				return Collection;
			}
		}
		return nullptr;
	}

	template<typename T>
	S* GetOfType() const
	{
		return GetOfType(T::StaticClass());
	}

	bool MoveInsertion(const TSubclassOf<S>& Insertions, UTGOR_Content* Content, ETGOR_InsertionActionEnumeration Action)
	{
		if (Action == ETGOR_InsertionActionEnumeration::Validate)
		{
			if (B && !Collection)
			{
				ERRET(FString::Printf(TEXT("Couldn't find %s in single insertion."), *S::StaticClass()->GetName()), Error, false);
			}
			return true;
		}

		S* C = Cast<S>(Content);
		if (IsValid(C))
		{
			if (Action == ETGOR_InsertionActionEnumeration::Add)
			{
				if (*Insertions && C->IsA(Insertions))
				{
					Collection = C;
					return true;
				}
			}
			if (Action == ETGOR_InsertionActionEnumeration::Remove)
			{
				if (Collection == C)
				{
					Collection = nullptr;
					return true;
				}
			}
		}
		return false;
	}
};

template<typename S, bool B>
struct CTGOR_Insertion<TArray<TSubclassOf<S>>, B>
{
	TArray<S*> Collection;

	bool Contains(const S* Content) const
	{
		return Collection.Contains(Content);
	}

	bool ContainsOfType(TSubclassOf<S> Type) const
	{
		if (*Type)
		{
			for (S* Content : Collection)
			{
				if (IsValid(Content) && Content->IsA(Type))
				{
					return true;
				}
			}
		}
		return false;
	}

	template<typename T>
	S* GetOfType(TSubclassOf<T> Type) const
	{
		if (*Type)
		{
			for (S* Content : Collection)
			{
				if (IsValid(Content) && Content->IsA(Type))
				{
					return Cast<T>(Content);
				}
			}
		}
		return nullptr;
	}

	template<typename T>
	S* GetOfType() const
	{
		return GetOfType<T>(T::StaticClass());
	}

	template<typename T>
	TArray<T*> GetListOfType(TSubclassOf<T> Type) const
	{
		TArray<T*> Contents;
		if (*Type)
		{
			for (S* Content : Collection)
			{
				if (IsValid(Content) && Content->IsA(Type))
				{
					Contents.Add(Cast<T>(Content));
				}
			}
		}
		return Contents;
	}

	template<typename T>
	TArray<T*> GetListOfType() const
	{
		return GetListOfType<T>(T::StaticClass());
	}

	bool MoveInsertion(const TArray<TSubclassOf<S>>& Insertions, UTGOR_Content* Content, ETGOR_InsertionActionEnumeration Action)
	{
		if (Action == ETGOR_InsertionActionEnumeration::Validate)
		{
			if (B && Collection.Num() == 0)
			{
				ERRET(FString::Printf(TEXT("Couldn't find any %s in array insertion."), *S::StaticClass()->GetName()), Error, false);
			}
			return true;
		}

		S* C = Cast<S>(Content);
		if (IsValid(C) && !Collection.Contains(C))
		{
			if (Action == ETGOR_InsertionActionEnumeration::Add)
			{
				int32 Backlog = 0;
				for (const auto& Insertion : Insertions)
				{
					// Keep track of the location we'd want to insert the new item to keep insertion order
					while (Collection.IsValidIndex(Backlog) && IsValid(Collection[Backlog]) && Collection[Backlog]->IsA(Insertion))
					{
						Backlog += 1;
					}

					if (*Insertion && C->IsA(Insertion))
					{
						Collection.Insert(C, Backlog);
						return true;
					}
				}
			}
			if (Action == ETGOR_InsertionActionEnumeration::Remove)
			{
				if (Collection.Contains(C))
				{
					Collection.Remove(C);
					return true;
				}
			}
		}
		return false;
	}
};

template<typename S, bool B>
struct CTGOR_Insertion<TSet<TSubclassOf<S>>, B>
{
	TSet<S*> Collection;

	bool Contains(const S* Content) const
	{
		return Collection.Contains(Content);
	}

	bool ContainsOfType(TSubclassOf<S> Type) const
	{
		if (*Type)
		{
			for (S* Content : Collection)
			{
				if (IsValid(Content) && Content->IsA(Type))
				{
					return true;
				}
			}
		}
		return false;
	}

	template<typename T>
	S* GetOfType(TSubclassOf<T> Type) const
	{
		if (*Type)
		{
			for (S* Content : Collection)
			{
				if (IsValid(Content) && Content->IsA(Type))
				{
					return Cast<T>(Content);
				}
			}
		}
		return nullptr;
	}

	template<typename T>
	S* GetOfType() const
	{
		return GetOfType<T>(T::StaticClass());
	}

	template<typename T>
	TSet<T*> GetListOfType(TSubclassOf<T> Type) const
	{
		TSet<T*> Contents;
		if (*Type)
		{
			for (S* Content : Collection)
			{
				if (IsValid(Content) && Content->IsA(Type))
				{
					Contents.Add(Cast<T>(Content));
				}
			}
		}
		return Contents;
	}

	template<typename T>
	TSet<T*> GetListOfType() const
	{
		return GetListOfType<T>(T::StaticClass());
	}

	bool MoveInsertion(const TSet<TSubclassOf<S>>& Insertions, UTGOR_Content* Content, ETGOR_InsertionActionEnumeration Action)
	{
		if (Action == ETGOR_InsertionActionEnumeration::Validate)
		{
			if (B && Collection.Num() == 0)
			{
				ERRET(FString::Printf(TEXT("Couldn't find any %s in set insertion."), *S::StaticClass()->GetName()), Error, false);
			}
			return true;
		}

		S* C = Cast<S>(Content);
		if (IsValid(C))
		{
			if (Action == ETGOR_InsertionActionEnumeration::Add)
			{
				for (const auto& Insertion : Insertions)
				{
					if (*Insertion && C->IsA(Insertion))
					{
						Collection.Add(C);
						return true;
					}
				}
			}
			if (Action == ETGOR_InsertionActionEnumeration::Remove)
			{
				if (Collection.Contains(C))
				{
					Collection.Remove(C);
					return true;
				}
			}
		}
		return false;
	}
};

template<typename S, typename R, bool B>
struct CTGOR_Insertion<TMap<TSubclassOf<S>, R>, B>
{
	TMap<S*, R> Collection;

	bool Contains(const S* Content) const
	{
		return Collection.Contains(Content);
	}

	template<typename T>
	TPair<T*, R> GetOfType(TSubclassOf<T> Type) const
	{
		if (*Type)
		{
			for (const auto& Content : Collection)
			{
				if (IsValid(Content.Key) && Content.Key->IsA(Type))
				{
					return Content;
				}
			}
		}
		return TPair<T*, R>(nullptr, R());
	}

	template<typename T>
	TPair<T*, R> GetOfType() const
	{
		return GetOfType<T>(T::StaticClass());
	}

	template<typename T>
	TArray<TPair<T*, R>> GetListOfType(TSubclassOf<T> Type) const
	{
		TArray<TPair<T*, R>> Contents;
		if (*Type)
		{
			for (const auto& Content : Collection)
			{
				if (IsValid(Content.Key) && Content.Key->IsA(Type))
				{
					Contents.Add(TPair<T*, R>(Cast<T>(Content.Key), Content.Value));
				}
			}
		}
		return Contents;
	}

	template<typename T>
	TArray<TPair<T*, R>> GetListOfType() const
	{
		return GetListOfType<T>(T::StaticClass());
	}

	bool MoveInsertion(const TMap<TSubclassOf<S>, R>& Insertions, UTGOR_Content* Content, ETGOR_InsertionActionEnumeration Action)
	{
		if (Action == ETGOR_InsertionActionEnumeration::Validate)
		{
			if (B && Collection.Num() == 0)
			{
				ERRET(FString::Printf(TEXT("Couldn't find any %s in map insertion."), *S::StaticClass()->GetName()), Error, false);
			}
			return true;
		}

		S* C = Cast<S>(Content);
		if (IsValid(C))
		{
			if (Action == ETGOR_InsertionActionEnumeration::Add)
			{
				for (const auto& Insertion : Insertions)
				{
					if (*Insertion.Key && C->IsA(Insertion.Key))
					{
						Collection.Add(C, Insertion.Value);
						return true;
					}
				}
			}
			if (Action == ETGOR_InsertionActionEnumeration::Remove)
			{
				if (Collection.Contains(C))
				{
					Collection.Remove(C);
					return true;
				}
			}
		}
		return false;
	}
};

