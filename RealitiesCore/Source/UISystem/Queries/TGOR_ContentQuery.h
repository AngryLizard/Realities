// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/TGOR_DisplayInstance.h"

#include "TGOR_SelectionQuery.h"
#include "TGOR_ContentQuery.generated.h"

/**
 * 
 */
UCLASS()
class UISYSTEM_API UTGOR_ContentQuery : public UTGOR_SelectionQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ContentQuery();
	virtual TArray<FTGOR_Display> QueryDisplays() const override;
	virtual void CallIndex(int32 Index) override;
	virtual void Commit() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Content")
		bool Required;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Content")
		FTGOR_Display NoneDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Content")
		FTGOR_Display EmptyDisplay;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Query a list of content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		virtual TArray<UTGOR_CoreContent*> QueryContent() const;

	/** Query a list of content override */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		TArray<UTGOR_CoreContent*> OnQueryContent() const;

	/** Call content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		virtual void CallContent(int32 Index);

	/** Call content override */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		void OnCallContent(int32 Index);

	/** Call none */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		virtual void CallNone();

	/** Call none override */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		void OnCallNone();

	/** Commit content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		virtual void CommitContent(int32 Index);

	/** Commit content override */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		void OnCommitContent(int32 Index);

	/** Commit none */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		virtual void CommitNone();

	/** Commit none override */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		void OnCommitNone();

protected:

	template<typename T>
	TArray<UTGOR_CoreContent*> MigrateContentArray(const TArray<T*>& Contents) const
	{
		TArray<UTGOR_CoreContent*> Out;
		for (T* t : Contents)
		{
			Out.Add(t);
		}
		return(Out);
	}
};
