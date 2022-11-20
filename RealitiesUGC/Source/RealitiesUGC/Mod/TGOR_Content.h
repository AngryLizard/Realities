// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "TGOR_ModInstance.h"
#include "TGOR_Insertions.h"

#include <functional>

#include "RealitiesUtility/Utility/TGOR_WorldObject.h"
#include "TGOR_Content.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_UtilityWidget;
class UTGOR_GameInstance;
class UTGOR_Singleton;
class UTGOR_Mod;

////////////////////////////////////////////// STRUCTS //////////////////////////////////////////////////////

DECLARE_DYNAMIC_DELEGATE_OneParam(FContentDelegate, UTGOR_Content*, Content);

/**
* TGOR_Content is the baseclass for resources that need to be serialisable
*/
UCLASS(Blueprintable)
class REALITIESUGC_API UTGOR_Content : public UTGOR_WorldObject
{
	GENERATED_BODY()

	friend class UTGOR_ContentManager;
	friend class UTGOR_Category;

public:

	UTGOR_Content();

	/** There is at most one object created in UGCBaseGameInstance with defaultname as stable name. */
	virtual bool IsNameStableForNetworking() const { return true; };
	virtual bool IsSupportedForNetworking() const { return true; };

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Linked content instances */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Content")
		TArray<UTGOR_Content*> Insertions;

	/** Subordinate content classes */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Content")
		TArray<UTGOR_Content*> SubContent;
	
	/** Number of children in this subtree */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Content")
		int32 StorageSize;

	/** Index in content storage */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Content")
		int32 StorageIndex;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Adds a content to this sub-tree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		bool AddContent(UTGOR_Content* Content);

	/** Removes a content from this sub-tree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		bool RemoveContent(UTGOR_Content* Content);


	/** Populates content class storage */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		void PopulateStorage(TArray<UTGOR_Content*>& ContentStorage, TMap<FString, int32>& ContentDictionary);

	/** Finds first occurence of a given type (WARNING: Also returns abstract classes) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* FindFirstOfType(TSubclassOf<UTGOR_Content> Type) const;

	/** Finds all occurences of a given type on first occurence layer (WARNING: Also returns abstract classes) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_Content*> FindAllOfType(TSubclassOf<UTGOR_Content> Type) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	///** Adds an insertion to this sub-tree if defined in contentclass list */
	//UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
	//	void ConsiderInsertion(UTGOR_Content* Content);

	///** Adds an insertion to this sub-tree */
	//UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
	//	void AddInsertion(UTGOR_Content* Content);

	///** Removes an insertion from this sub-tree */
	//UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
	//	void RemoveInsertion(UTGOR_Content* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Gets called on initialisation of all resources */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		virtual void BuildResource();

	/** Gets called after initialisation of all resources */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		virtual void PostBuildResource();

	/** Validation after initialisation of all resources to check whether all content properties are valid */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		bool Validate();
		virtual bool Validate_Implementation();

	/** Constructs default name */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		void BuildDefaultName(FString ModName);
	
	/** Get default name */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		FString GetDefaultName() const;

	/** Get default name */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		FText GetDefaultDescription() const;

	/** Get default icon */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		UObject* GetDefaultIcon() const;

	/** Get mod name */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		FString GetModIdentifier() const;

	/** Get index in the ContentManager storage */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		int32 GetStorageIndex() const;

	/** Resets insertion list and content hierarchy of this content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content|Internal", Meta = (Keywords = "C++"))
		virtual void Reset();

	/** Get whether this content is of a given content type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		bool IsOfType(TSubclassOf<UTGOR_Content> Type) const;

#if WITH_EDITOR
	// Accessors mostly for EditorUtility
	void SetDefaultName(const FString& Name);
	void SetIsAbstract(bool IsAbstract);

	void AddInsertion(TSubclassOf<UTGOR_Content> Class);
	void RemoveInsertion(TSubclassOf<UTGOR_Content> Class);
#endif

//#ifdef WITH_EDITORONLY_DATA // This is broken <...<

	/** Widget used by editorutilities */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Content")
		TSubclassOf<UTGOR_UtilityWidget> UtilityWidget;

//#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Mod this content class comes from */
	UPROPERTY()
		UTGOR_Mod* Mod;

	/** Mod this resourceable was added from */
	UPROPERTY()
		FString DefaultMod;

	/** Indicates that this resourceable is meant for categorisation, not initialisation */
	UPROPERTY(EditDefaultsOnly, Category = "!TGOR Content")
		bool Abstract;

	/** Name this resourceable should be defined as (Classname if empty) */
	UPROPERTY(EditDefaultsOnly, Category = "!TGOR Content")
		FString DefaultName;

	/** Description about this content class */
	UPROPERTY(EditAnywhere, Category = "!TGOR Content")
		FText DefaultDescription;

	/** Icon of this content class to be shown in content management */
	UPROPERTY(EditAnywhere, Category = "!TGOR Display", meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Image", AllowedClasses = "/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface"))
		UObject* DefaultIcon;

	UPROPERTY(EditDefaultsOnly, Category = "!TGOR Content")
		TArray<FString> DefaultGroup;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Find whether this content is abstract */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		bool IsAbstract() const;

	/** Calls a delegate on every content class including abstract */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", meta = (Keywords = "C++"))
		void ForAllContent(const FContentDelegate& Func);
		void ForAllContent(std::function<bool(UTGOR_Content*)> Func);

	/** Figure out whether this content contains a specific insertion */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", meta = (Keywords = "C++"))
		bool ContainsInsertion(const UTGOR_Content* Content) const;
		template<typename T>
		bool ContainsI(const T* Content) const
		{
			return(ContainsInsertion(Content));
		}

		/** Figure out whether this content contains a specific insertion type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", meta = (Keywords = "C++"))
		bool ContainsInsertionFromType(TSubclassOf<UTGOR_Content> Type) const;
		template<typename T>
		bool ContainsIFromType(TSubclassOf<T> Type) const
		{
			return(ContainsInsertionFromType(Type));
		}
		template<typename T>
		bool ContainsIFromType() const
		{
			return(ContainsIFromType<T>(T::StaticClass()));
		}

	/** Gets first found inserted content of given class */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* GetInsertionFromType(TSubclassOf<UTGOR_Content> Type) const;
		template<typename T>
		T* GetIFromType(TSubclassOf<T> Type) const
		{
			return(Cast<T>(GetInsertionFromType(Type)));
		}
		template<typename T>
		T* GetIFromType() const
		{
			return(GetIFromType<T>(T::StaticClass()));
		}

	/** Gets all found inserted content of given class */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_Content*> GetInsertionListFromType(TSubclassOf<UTGOR_Content> Type) const;
		template<typename T>
		TArray<T*> GetIListFromType(TSubclassOf<T> Type) const
		{
			TArray<T*> Out;
			TArray<UTGOR_Content*> List = GetInsertionListFromType(Type);
			for (UTGOR_Content* Content : List)
			{
				Out.Emplace(Cast<T>(Content));
			}
			return(Out);
		}
		template<typename T>
		TArray<T*> GetIListFromType() const
		{
			return(GetIListFromType<T>(T::StaticClass()));
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Insertion to this content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) {}

	/** Insertion to this content and all sub-contents */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		void RecursiveMoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action);



};
