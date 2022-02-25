// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Components/TGOR_SceneComponent.h"
#include "Realities/Interfaces/TGOR_DimensionInterface.h"
#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "TGOR_DimensionLoaderComponent.generated.h"

class UTGOR_DimensionData;
class UTGOR_Dimension;
class ATGOR_Pawn;

UENUM(BlueprintType)
enum class ETGOR_DimensionLoaderLevel : uint8
{
	Loading,
	Loaded,
	Unloading,
	Idle
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDimensionLoadedDelegate, ETGOR_DimensionLoaderLevel, NewLevel);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_DimensionLoaderComponent : public UTGOR_SceneComponent, public ITGOR_DimensionInterface, public ITGOR_SaveInterface
{
	GENERATED_BODY()

public:
	UTGOR_DimensionLoaderComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Returns whether this dimension is ready to be entered */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool IsReady() const;

	/** Registers a given dimension with given unique suffix */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void DialDimension(TSubclassOf<UTGOR_Dimension> Dimension, const FString& Suffix);

	/** Registers a given dimension with given identifier */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void OpenDimension(UTGOR_Dimension* Dimension, const FString& Identifier);
	
	/** Whether a dimension is dialled */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool HasDial() const;

	/** Gets the linked dimension if loaded and ready  */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_DimensionData* GetLinkedDimension() const;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Dimension")
		FDimensionLoadedDelegate OnLoaderLevelChanged;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Loaded Dimension identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FName DimensionIdentifier;

	/** Sets dimension loader level and notifies listeners about changes */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void ChangeLoaderLevel(ETGOR_DimensionLoaderLevel Level);
	
	/** This loaders state */
	UPROPERTY(ReplicatedUsing = RepNotifyLoad, BlueprintReadOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		ETGOR_DimensionLoaderLevel LoaderLevel;

	UFUNCTION()
		void RepNotifyLoad();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Load dimension this portal is connected to for a given player (and host if not loaded yet). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void LoadForPlayer(ATGOR_OnlineController* Player);

};
