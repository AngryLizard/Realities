// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../TGOR_DimensionInstance.h"

#include "CoreSystem/Components/TGOR_SceneComponent.h"
#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "../Interfaces/TGOR_DimensionInterface.h"
#include "TGOR_DimensionLoaderComponent.generated.h"

class UTGOR_DimensionData;
class UTGOR_Dimension;

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
class DIMENSIONSYSTEM_API UTGOR_DimensionLoaderComponent : public UTGOR_SceneComponent, public ITGOR_DimensionInterface, public ITGOR_SaveInterface
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
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool IsReady() const;

	/** Registers a given dimension with given unique suffix */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void DialDimension(TSubclassOf<UTGOR_Dimension> Dimension, const FString& Suffix);

	/** Registers a given dimension with given identifier */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void OpenDimension(UTGOR_Dimension* Dimension, const FString& Identifier);
	
	/** Whether a dimension is dialled */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool HasDial() const;

	/** Gets the linked dimension if loaded and ready  */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_DimensionData* GetLinkedDimension() const;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Dimension")
		FDimensionLoadedDelegate OnLoaderStateChange;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Loaded Dimension identifier */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FName DimensionIdentifier;

	/* Loaded Dimension connections */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FTGOR_ConnectionCollection DimensionConnections;

	/* Which connections to load on loaded dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		TArray<FTGOR_ConnectionSelection> ConnectionRequests;

	/** Sets dimension loader level and notifies listeners about changes */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		virtual void ChangeLoaderLevel(ETGOR_DimensionLoaderLevel Level);
	
	/** This loaders state */
	UPROPERTY(ReplicatedUsing = RepNotifyLoad, BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		ETGOR_DimensionLoaderLevel LoaderLevel;

	UFUNCTION()
		void RepNotifyLoad();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get dimension loaded by this loader */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FName GetLoadedIdentifier() const;


	/** Get a connection */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_ConnectionComponent* GetConnection(const FName& ConnectionName) const;

	/** Return connections of given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (DeterminesOutputType = "Class", Keywords = "C++"))
		UTGOR_ConnectionComponent* GetConnectionOfType(const FName& ConnectionName, TSubclassOf<UTGOR_ConnectionComponent> Class) const;

	template<typename T> TArray<T*> GetCOfType(const FName& ConnectionName, TSubclassOf<T> Type) const
	{
		return Cast<T>(GetConnectionOfType(ConnectionName, Type));
	}
	template<typename T> TArray<T*> GetCOfType(const FName& ConnectionName) const
	{
		return GetCListOfType<T>(T::StaticClass());
	}


};
