// The Gateway of Realities: Planes of Existence.

#pragma once
#include "../TGOR_AttributeInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Attribute.generated.h"


/**
 *
 */
UENUM(BlueprintType)
enum class ETGOR_AttributeAccumulation : uint8
{
	Add,
	Mul,
	Max
};

/**
 * 
 */
UCLASS(Blueprintable)
class ATTRIBUTESYSTEM_API UTGOR_Attribute : public UTGOR_SpawnModule
{
	GENERATED_BODY()
	
public:
	UTGOR_Attribute();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Accumulation rule for this attribute (if multiple attributes are defined) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_AttributeAccumulation Accumulation;

	/** Starting value of this attribute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DefaultValue;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get attribute from given instance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Attribute", Meta = (Keywords = "C++"))
		static float GetInstanceAttribute(const FTGOR_AttributeInstance& Instance, UTGOR_Attribute* Attribute, float Default);

	/** Modify value via defined accumulation */
	UFUNCTION(BlueprintPure, Category = "!TGOR Attribute", Meta = (Keywords = "C++"))
		float ModifyValue(float Value, float Modifier) const;
};
