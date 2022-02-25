// TGOR (C) // CHECKED //
#pragma once


#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/System/TGOR_ModInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "Runtime/GameplayTags/Classes/GameplayTagContainer.h"

#include "Realities/Base/TGOR_Object.h"
#include "TGOR_ContentManager.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Content;
class UTGOR_Mod;


/**
 * TGOR_ContentManager stores TGOR_Contents and indexes them inside hashtables
 */
UCLASS()
class REALITIES_API UTGOR_ContentManager : public UTGOR_Object
{
	GENERATED_BODY()

		friend class UTGOR_Category;

public:

	UTGOR_ContentManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	// Currently active filter tags
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		FGameplayTagContainer FilterTags;

	// True if a mod has been loaded
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		bool ModLoaded;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Enrich resources with content from mod. Return value has to be stored to prevent garbage collection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void ApplyModEntries(UTGOR_Mod* Mod, const TArray<FTGOR_ContentLink>& Entries);

	/** Enrich resources with content from mod. Return value has to be stored to prevent garbage collection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void ApplyModInsertions(const TArray<FTGOR_ContentInsertion>& Insertions);

	/** Enrich resources with content from mod. Return value has to be stored to prevent garbage collection */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void ApplyMod(UTGOR_Mod* Mod, const TArray<FTGOR_ContentLink>& Assets);

	/** Link all content classes with insertions */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void ApplyInsertions();

	/** Calls postload on all resources */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void PostLoad();

	/** Resets the content storage */
	UFUNCTION(BlueprintCallable, Category = "TGOR Table", Meta = (Keywords = "C++"))
		void Reset();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Returns first content matching type (WARNING: Including Abstract) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* FindFirstOfType(TSubclassOf<UTGOR_Content> Type) const;

	/** Returns all content matching type on first occurence level (WARNING: Including Abstract) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_Content*> FindAllOfType(TSubclassOf<UTGOR_Content> Type) const;

	/** Returns first content matching type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* GetContentFromType(TSubclassOf<UTGOR_Content> Type) const;
		template<typename T>
		T* GetTFromType(TSubclassOf<T> Type) const
		{ 
			return(Cast<T>(GetContentFromType(Type)));
		}
		template<typename T>
		T* GetTFromType() const
		{
			return(GetTFromType<T>(T::StaticClass()));
		}

	/** Returns all content matching type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_Content*> GetContentListFromType(TSubclassOf<UTGOR_Content> Type) const;
		template<typename T>
		TArray<T*> GetTListFromType(TSubclassOf<T> Type) const
		{
			TArray<T*> Out;
			TArray<UTGOR_Content*> List = GetContentListFromType(*Type);
			for (UTGOR_Content* Content : List)
			{
				Out.Emplace(Cast<T>(Content));
			}
			return(Out);
		}
		template<typename T>
		TArray<T*> GetTListFromType() const
		{
			return(GetTListFromType<T>(T::StaticClass()));
		}

	/** Returns content at given index relative to a content type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* GetContentFromIndex(TSubclassOf<UTGOR_Content> Type, int32 Index) const;
		template<typename T>
		T* GetTFromIndex(TSubclassOf<T> Type, int32 Index) const
		{
			return(Cast<T>(GetContentFromIndex(Type, Index)));
		}
		template<typename T>
		T* GetTFromIndex(int32 Index) const
		{
			return(GetTFromIndex<T>(T::StaticClass(), Index));
		}

	/** Returns index of given content relative to a content type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		int32 GetContentIndex(TSubclassOf<UTGOR_Content> Type, UTGOR_Content* Content) const;
		template<typename T>
		int32 GetTIndex(TSubclassOf<T> Type, UTGOR_Content* Content) const
		{
			return(GetContentIndex(Type, Content));
		}
		template<typename T>
		int32 GetTIndex(UTGOR_Content* Content) const
		{
			return(GetTIndex<T>(T::StaticClass(), Content));
		}

	/** Returns number of content classes relative to a content type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		int32 GetContentCapacity(TSubclassOf<UTGOR_Content> Type) const;
		template<typename T>
		int32 GetTCapacity(TSubclassOf<T> Type) const
		{
			return(GetContentCapacity(Type));
		}
		template<typename T>
		int32 GetTCapacity() const
		{
			return(GetTCapacity<T>(T::StaticClass()));
		}

	/** Returns content with given DefaultName (Type does nothing for now) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* GetContentFromName(TSubclassOf<UTGOR_Content> Type, const FString& Name) const;
		template<typename T>
		T* GetTFromName(TSubclassOf<T> Type, const FString& Name) const
		{
			return(Cast<T>(GetContentFromName(Type, Name)));
		}
		template<typename T>
		T* GetTFromName(const FString& Name) const
		{
			return(GetTFromName<T>(T::StaticClass(), Name));
		}

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Root Content class of the content tree */
	UPROPERTY(Instanced, BlueprintReadOnly, Category = "TGOR Content")
		UTGOR_Content* ContentRoot;

	/** All loaded non-abstract content classes */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		TArray<UTGOR_Content*> ContentStorage;

	/** All content classes associated with name indexing ContentStorage */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		TMap<FString, int32> ContentDictionary;

	/** All loaded content insertions */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		TArray<FTGOR_ContentInsertion> ContentInsertions;

	////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	/** Returns content root */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		UTGOR_Content* GetContentRoot() const;

};
