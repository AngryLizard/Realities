// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/TGOR_DisplayInstance.h"

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "RealitiesUGC/Mod/TGOR_Content.h"
#include "TGOR_CoreContent.generated.h"

class UTGOR_Category;

UCLASS(Blueprintable)
class CORESYSTEM_API UTGOR_CoreContent : public UTGOR_Content, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

	public:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////

		UTGOR_CoreContent();
		virtual void BuildResource() override;
		virtual void PostBuildResource() override;
		virtual void Reset() override;

		//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////


		////////////////////////////////////////////////////////////////////////////////////////////////////

		/** Get display struct */
		UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
			const FTGOR_Display& GetDisplay() const;

		/** Get chat aliases */
		UFUNCTION(BlueprintPure, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
			const TArray<FString>& GetAliases() const;

		/** Find first possible chat alias */
		UFUNCTION(BlueprintPure, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
			FString GetFirstAlias() const;

#ifdef WITH_EDITOR
		// Accessors mostly for EditorUtility
		void SetRawDisplayName(const FText& Name);
#endif

		////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

		/** Alias used in chat. (defaults to DisplayName if not empty, DefaultName otherwise) */
		UPROPERTY(EditDefaultsOnly, Category = "!TGOR Chat")
			TArray<FString> ChatAlias;

		/** Display used for ingame display */
		UPROPERTY(EditDefaultsOnly, Category = "!TGOR Chat")
			FTGOR_Display Display;


		////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Wrapper to get content from singleton */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", meta = (DeterminesOutputType = "Class", WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true", Keywords = "C++"))
		static UTGOR_Content* GetContentStatic(const UObject* WorldContextObject, TSubclassOf<UTGOR_Content> Class);


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Categories of this content */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Category>> CategoryInsertions;
	DECL_INSERTION(CategoryInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

protected:

private:

};
