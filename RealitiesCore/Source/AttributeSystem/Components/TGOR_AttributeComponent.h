// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
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
class ATTRIBUTESYSTEM_API UTGOR_AttributeComponent : public UTGOR_Component
{
	GENERATED_BODY()

public:	
	UTGOR_AttributeComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently cached attribute values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<UTGOR_Attribute*, float> AttributeValues;

	/** Handles for aim suspension */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Attributes")
		TMap<UTGOR_Modifier*, FTGOR_AttributeHandle> Handles;

	/** Recompute values from handles. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		void RecomputeValues();

	/** Add values to cache (AttributeValues). */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		void AddValues(const TMap<UTGOR_Attribute*, float>& Values);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Register values to go active. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		void RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Modifier* Modifier);

	/** Unregister manually. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		void UnregisterHandle(UTGOR_Modifier* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get attributes of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		void UpdateAttributes(UPARAM(ref) TMap<UTGOR_Attribute*, float>& Attributes) const;

	/** Update attribute interfaces of given actor and their components */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		void AttributeUpdate(AActor* Actor);

	/** Gets current attribute state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		TArray<FTGOR_AttributeOutput> GetAttributes() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	
};