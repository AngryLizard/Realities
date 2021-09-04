#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_MatterInstance.generated.h"



///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////


USTRUCT(BlueprintType)
struct FTGOR_MatterContainer
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTGOR_Segment* Segment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<UTGOR_Matter*, int32> Slots;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	int32 CountMatter(UTGOR_Matter* Matter) const;
	int32 RemoveMatter(UTGOR_Matter* Matter, int32 Quantity);
	int32 AddMatter(UTGOR_Matter* Matter, int32 Quantity);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void AddComposition(TMap<UTGOR_Matter*, int32>& Composition) const;
	void AddMissing(TMap<UTGOR_Matter*, int32>& Missing) const;
	void AddCapacity(TMap<UTGOR_Matter*, int32>& Capacity) const;
	void AddSupportedMatter(TSet<UTGOR_Matter*>& Supported) const;

};


USTRUCT(BlueprintType)
struct FTGOR_MatterContainers
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FTGOR_MatterContainer> Containers;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	int32 CountMatter(TSubclassOf<UTGOR_Segment> Filter, UTGOR_Matter* Matter) const;
	int32 RemoveMatter(TSubclassOf<UTGOR_Segment> Filter, UTGOR_Matter* Matter, int32 Quantity);
	int32 AddMatter(TSubclassOf<UTGOR_Segment> Filter, UTGOR_Matter* Matter, int32 Quantity);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void AddComposition(TSubclassOf<UTGOR_Segment> Filter, TMap<UTGOR_Matter*, int32>& Composition) const;
	void AddMissing(TSubclassOf<UTGOR_Segment> Filter, TMap<UTGOR_Matter*, int32>& Missing) const;
	void AddCapacity(TSubclassOf<UTGOR_Segment> Filter, TMap<UTGOR_Matter*, int32>& Capacity) const;
	void AddSupported(TSubclassOf<UTGOR_Segment> Filter, TSet<UTGOR_Matter*>& Supported) const;

};

template<>
struct TStructOpsTypeTraits<FTGOR_MatterContainers> : public TStructOpsTypeTraitsBase2<FTGOR_MatterContainers>
{
	enum { WithNetSerializer = true };
};


/**
 * 
 */
UCLASS()
class UTGOR_MatterBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get currently available matter. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		static TMap<UTGOR_Matter*, int32> GetComposition(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter);

	/** Get currently missing matter from full capacity */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		static TMap<UTGOR_Matter*, int32> GetMissing(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter);

	/** Get matter storage capacity of this container */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		static TMap<UTGOR_Matter*, int32> GetCapacity(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter);

	/** Gets by this container supported matter types */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		static TSet<UTGOR_Matter*> GetSupported(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter);
};

