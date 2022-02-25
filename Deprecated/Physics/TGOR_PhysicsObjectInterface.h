// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "TGOR_PhysicsInterface.h"
#include "TGOR_PhysicsObjectInterface.generated.h"


USTRUCT(BlueprintType)
struct FTGOR_InertiaTensor
{
	GENERATED_BODY()

public:
	FTGOR_InertiaTensor();
	FTGOR_InertiaTensor(const FVector& Diag);
	FTGOR_InertiaTensor(const FVector& InX, const FVector& InY, const FVector& InZ);
	FTGOR_InertiaTensor(const FTGOR_InertiaTensor& In);

	FVector GetColumnX() const;
	FVector GetColumnY() const;
	FVector GetColumnZ() const;
	FVector operator*(const FVector& Vector) const;
	FVector operator^(const FVector& Vector) const;
	FTGOR_InertiaTensor Scale(const FVector& Scale) const;
	FTGOR_InertiaTensor InvScale(const FVector& Scale) const;
	FTGOR_InertiaTensor operator*(const FTGOR_InertiaTensor& Tensor) const; //AB
	FTGOR_InertiaTensor operator^(const FTGOR_InertiaTensor& Tensor) const; //A'B
	FTGOR_InertiaTensor operator&(const FTGOR_InertiaTensor& Tensor) const; //AB'
	FTGOR_InertiaTensor operator|(const FTGOR_InertiaTensor& Tensor) const; //A'B'
	FTGOR_InertiaTensor operator+(const FTGOR_InertiaTensor& Tensor) const;
	FTGOR_InertiaTensor operator+=(const FTGOR_InertiaTensor& Tensor);
	bool operator==(const FTGOR_InertiaTensor& Tensor) const;
	FTGOR_InertiaTensor Inverse() const;
	FString ToString() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		FVector X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		FVector Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		FVector Z;

};




// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_PhysicsObjectInterface : public UTGOR_PhysicsInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REALITIES_API ITGOR_PhysicsObjectInterface : public ITGOR_PhysicsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Moves body, returns correction vector and makes sure not to go faster than MaxSpeed per iteration by returning a rest timestep */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual FVector ApplyMove(float DeltaTime, const FVector& External, const TArray<AActor*>& Ignore);


	/** Sets physics parent */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void SetParent(TScriptInterface<ITGOR_PhysicsObjectInterface> ParentInterface);

	
	/** Computes inverse inertia tensor in parent space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual FTGOR_InertiaTensor ComputeInertiaTensor();

	/** Forces recomputation of inertia */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void InvalidateInertiaTensor();


	/** Gets current offset to parent in parent space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual FVector GetOffset() const;

	/** Moves current offset to parent space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void MoveOffset(const FVector& Delta);

	/** Gets current location in world space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual FVector GetLocation() const;

	/** Moves by a given delta */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		virtual void Move(const FVector& Delta);

};
