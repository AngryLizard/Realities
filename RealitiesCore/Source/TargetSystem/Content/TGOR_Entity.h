// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "RealitiesUGC/Mod/TGOR_Insertions.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Entity.generated.h"

class UTGOR_Attribute;
class UTGOR_Target;

/**
 * Entity content, identifies objects with properties and attributes that can be interacted with.
 */
UCLASS(Blueprintable)
class TARGETSYSTEM_API UTGOR_Entity : public UTGOR_SpawnModule
{
	GENERATED_BODY()
		
public:
	UTGOR_Entity();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Attributes in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Attribute>> AttributeInsertions;
	DECL_INSERTION(AttributeInsertions);

	/** Sockets in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Target>> TargetInsertions;
	DECL_INSERTION(TargetInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
