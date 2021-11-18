// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_AttributeComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Attribute;
class UTGOR_Modifier;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_AttributeHandle
{
	GENERATED_USTRUCT_BODY()

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Attributes")
		TScriptInterface<ITGOR_RegisterInterface> Register;
};

USTRUCT(BlueprintType)
struct FTGOR_AttributeOutput
{
	GENERATED_USTRUCT_BODY()

	/** Attribute content. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Attributes")
		UTGOR_Attribute* Attribute = nullptr;

	/** Actual attribute value. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Attributes")
		float Value = 0.0f;

	/** Which content classes have contributed to this attribute. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Attributes")
		TMap<UTGOR_Modifier*, float> Contributions;
};

/**
* TGOR_AttrbuteComponent distributes attributes to attribute interfaces
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ATTRIBUTESYSTEM_API UTGOR_AttributeComponent : public UTGOR_Component, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:	
	UTGOR_AttributeComponent();

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Recompute values from handles. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		float ComputeAttributeValue(TSubclassOf<UTGOR_Attribute> AttributeType, float DefaultValue) const;

	template<typename T>
	float ComputeAValue() const
	{
		return ComputeValues(T::StaticClass());
	}

	/** List all active attributes. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		TArray<FTGOR_AttributeOutput> ComputeActiveAttributes() const;

	/** Get attribute value of given actor, or default if not supported. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		static float GetAttributeValue(AActor* Actor, TSubclassOf<UTGOR_Attribute> AttributeType, float DefaultValue);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Compute currently active modifiers. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		TArray<UTGOR_Modifier*> QueryActiveModifiers() const;
};