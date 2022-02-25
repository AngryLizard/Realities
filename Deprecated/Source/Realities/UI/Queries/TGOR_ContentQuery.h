// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Queries/TGOR_SelectionQuery.h"
#include "TGOR_ContentQuery.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ContentQuery : public UTGOR_SelectionQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ContentQuery();
	virtual TArray<FTGOR_Display> QueryDisplays() const override;
	virtual void CallIndex(int32 Index) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Content")
		bool Required;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Content")
		FTGOR_Display NoneDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Content")
		FTGOR_Display EmptyDisplay;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Query a list of content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		virtual TArray<UTGOR_Content*> QueryContent() const;

	/** Query a list of content override */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Content", Meta = (Keywords = "C++"))
		TArray<UTGOR_Content*> OnQueryContent() const;

	/** Call content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		virtual void CallContent(int32 Index);

	/** Call content override */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void OnCallContent(int32 Index);

	/** Call none */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		virtual void CallNone();

	/** Call none override */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void OnCallNone();

protected:

	template<typename T>
	TArray<UTGOR_Content*> MigrateContentArray(const TArray<T*>& Contents) const
	{
		TArray<UTGOR_Content*> Out;
		for (T* t : Contents)
		{
			Out.Add(t);
		}
		return(Out);
	}
};
