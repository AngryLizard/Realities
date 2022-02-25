// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include <functional>

#include "Realities/Base/TGOR_Object.h"
#include "TGOR_Content.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_UtilityWidget;
class UTGOR_GameInstance;
class UTGOR_Singleton;
class UTGOR_Mod;

////////////////////////////////////////////// STRUCTS //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_Display
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_Display();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content", meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Image", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface"))
		UObject* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		float Aspect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		FLinearColor CustomColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		bool UseCustomColor;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FContentDelegate, UTGOR_Content*, Content);

/**
* TGOR_Content is the baseclass for resources that need to be indexed
*/
UCLASS(Blueprintable)
class REALITIES_API UTGOR_Content : public UTGOR_Object
{
	GENERATED_BODY()

	friend class UTGOR_ContentManager;
	friend class UTGOR_Category;

public:

	UTGOR_Content();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Custom content links */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		TArray<TSubclassOf<UTGOR_Content>> InsertionClasses;

	/** Linked content instances */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		TArray<UTGOR_Content*> Insertions;

	/** Subordinate content classes */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		TArray<UTGOR_Content*> SubContent;
	
	/** Number of children in this subtree */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		int32 StorageSize;

	/** Index in content storage */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		int32 StorageIndex;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Adds a content to this sub-tree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		bool AddContent(UTGOR_Content* Content);

	/** Removes a content from this sub-tree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		bool RemoveContent(UTGOR_Content* Content);


	/** Populates content class storage */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void PopulateStorage(TArray<UTGOR_Content*>& ContentStorage, TMap<FString, int32>& ContentDictionary);

	/** Finds first occurence of a given type (WARNING: Also returns abstract classes) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Content* FindFirstOfType(TSubclassOf<UTGOR_Content> Type) const;

	/** Finds all occurences of a given type on first occurence layer (WARNING: Also returns abstract classes) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_Content*> FindAllOfType(TSubclassOf<UTGOR_Content> Type) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Adds an insertion to this sub-tree if defined in contentclass list */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void ConsiderInsertion(UTGOR_Content* Content);

	/** Adds an insertion to this sub-tree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void AddInsertion(UTGOR_Content* Content);

	/** Removes an insertion from this sub-tree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void RemoveInsertion(UTGOR_Content* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Gets called on initialisation of all resources */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		virtual void BuildResource(UWorld* World);

	/** Gets called after initialisation of all resources */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		virtual void PostBuildResource();

	/** Constructs default name */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void BuildDefaultName(FString ModName);

	/** Get default name */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		FString GetDefaultName() const;

	/** Get mod name */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		FString GetModIdentifier() const;

	/** Get index in the ContentManager storage */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		int32 GetStorageIndex() const;

	/** Get display struct */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		const FTGOR_Display& GetDisplay() const;

	/** Resets this content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void Reset();

	/** Get whether this content is of a given content type */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		bool IsOfType(TSubclassOf<UTGOR_Content> Type) const;

#ifdef WITH_EDITOR
	// Accessors mostly for EditorUtility
	void SetDefaultName(const FString& Name);

	void SetRawDisplayName(const FText& Name);
	void SetIsAbstract(bool IsAbstract);

	void AddInsertion(TSubclassOf<UTGOR_Content> Class);
	void RemoveInsertion(TSubclassOf<UTGOR_Content> Class);
#endif

//#ifdef WITH_EDITORONLY_DATA // This is broken <...<

	/** Widget used by editorutilities */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Content")
		TSubclassOf<UTGOR_UtilityWidget> UtilityWidget;

//#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Mod this content class comes from */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Content")
		UTGOR_Mod* Mod;

	/** Indicates that this resourceable is meant for categorisation, not initialisation */
	UPROPERTY(EditDefaultsOnly, Category = "TGOR Content")
		bool Abstract;

	/** Mod this resourceable was added from */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content")
		FString DefaultMod;

	/** Alias used in chat. (defaults to DisplayName if not empty, DfaultName otherwise)*/
	UPROPERTY(EditDefaultsOnly, Category = "TGOR Chat")
		TArray<FString> ChatAlias;

	/** Name this resourceable should be defined as (Classname if empty) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Content")
		FString DefaultName;

	/** Display associated with this resourceable */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Content")
		FTGOR_Display Display;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Content")
		TArray<FString> DefaultGroup;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Find whether this content is abstract */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		bool IsAbstract() const;

	/** Get chat aliases */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		const TArray<FString>& GetAliases() const;

	/** Find first possible chat alias */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FString GetFirstAlias() const;

	/** Calls a delegate on every content class including abstract */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", meta = (Keywords = "C++"))
		void ForAllContent(const FContentDelegate& Func);
		void ForAllContent(std::function<bool(UTGOR_Content*)> Func);

	/** Figure out whether this content contains a specific insertion */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", meta = (Keywords = "C++"))
		bool ContainsInsertion(UTGOR_Content* Content) const;
		template<typename T>
		bool ContainsI(T* Content) const
		{
			return(ContainsInsertion(Content));
		}

		/** Figure out whether this content contains a specific insertion type */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", meta = (Keywords = "C++"))
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
	UFUNCTION(BlueprintPure, Category = "TGOR Content", meta = (DeterminesOutputType = "Type", Keywords = "C++"))
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
	UFUNCTION(BlueprintPure, Category = "TGOR Content", meta = (DeterminesOutputType = "Type", Keywords = "C++"))
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

	/** Wrapper to get content from singleton */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", meta = (DeterminesOutputType = "Class", WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true", Keywords = "C++"))
		static UTGOR_Content* GetContentStatic(const UObject* WorldContextObject, TSubclassOf<UTGOR_Content> Class);

protected:

private:

};
