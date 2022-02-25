// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Classes/Components/SplineComponent.h"

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Structs/TGOR_AutoThreshold.h"

#include "CoreMinimal.h"
#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "TGOR_PipelineComponent.generated.h"

class UTGOR_Pipeline;
class UTGOR_PipelineSocket;
class UTGOR_PilotComponent;

USTRUCT(BlueprintType)
struct FTGOR_PipelineAttachment
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Attached component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<UTGOR_PilotComponent> Component;

	/** Distance on pipeline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance;

	/** Speed on pipeline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Speed;

	/** Socket this attachment got added from */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_PipelineSocket* Socket;
};

template<>
struct TStructOpsTypeTraits<FTGOR_PipelineAttachment> : public TStructOpsTypeTraitsBase2<FTGOR_PipelineAttachment>
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT(BlueprintType)
struct FTGOR_PipelineAttachments
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;
	
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Pipeline")
		TMap<FTGOR_Serial, FTGOR_PipelineAttachment> Parts;
};

template<>
struct TStructOpsTypeTraits<FTGOR_PipelineAttachments> : public TStructOpsTypeTraitsBase2<FTGOR_PipelineAttachments>
{
	enum
	{
		WithNetSerializer = true
	};
};


USTRUCT(BlueprintType)
struct FTGOR_PipelineHitResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		FTGOR_Index Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float Depth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float Normal;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPipelineHitDelegate, UTGOR_PilotComponent*, Attachee);

/**
* TGOR_PipelineComponent allows custom parts to attach to
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PipelineComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_PipelineComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual USceneComponent* GetMovementBasis() const override;
	//virtual int32 GetIndexFromName(const FName& Name) const override;
	//virtual FName GetNameFromIndex(int32 Index) const override;
	virtual FTGOR_MovementDynamic GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const override;

	virtual void InflictPointImpact(const FVector& Point, const FVector& Impulse, int32 Index) override;
	virtual void InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime, int32 Index) override;

	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;

	virtual bool QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const override;
	virtual TArray<UTGOR_PilotComponent*> GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called when an attachee hit or left pipeline start */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Pipeline")
		FPipelineHitDelegate OnHitStart;

	/** Called when an attachee hit or left pipeline end */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Pipeline")
		FPipelineHitDelegate OnHitEnd;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get spline distance nearest to given location
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		float GetDistanceNearestTo(const FVector& Location) const; */

	/** Get spline distance at a given point */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		float GetDistanceAt(int32 SplinePoint) const;

	/** Get spline point at the end of the pipeline */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		int32 GetLastSplinePoint() const;

	/** Get spline displacement by attachments at given spline distance and falloff distance */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		float GetDisplacementAtDistance(float Distance, float Falloff) const;

	/** Get spline radius at given spline distance */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		float GetRadiusAtDistance(float Distance) const;

	/** Hit-tests along spline at a ball with given spline distance */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		bool IsOccupied(float Distance, float Radius, float Segment) const;

	/** Sweep by a given delta from distance.  */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		bool Sweep(float Distance, float Radius, float Segment, float Delta, FTGOR_Serial Ignore, FTGOR_PipelineHitResult& HitResult) const;

	/** Gets location in world space of an attachment with a distance offset */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		FTransform GetTransformAt(FTGOR_Serial Index, float Offset) const;

	/** Draw spline radius over the whole spline */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void DrawSplineRadius(float Interval);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Current transform of given socket in world space */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetTransformOfPipeline(UTGOR_PipelineSocket* Socket) const;

	/** Finds whether a shape can be attached to this pipeline at a given spline point */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		bool CanFitToPipeline(const FTGOR_MovementShape& Shape, UTGOR_PipelineSocket* Socket) const;

	/** Finds whether an object can be attached to this pipeline at a given spline point */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		bool CanAttachToPipeline(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket) const;

	/** Attaches an object to this pipeline if it can */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pipeline", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void AttachToPipeline(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket, ETGOR_ComputeEnumeration& Branches);

	/** Detaches an object from this pipeline */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void DetachFromPipeline(UTGOR_PilotComponent* Attachee);

	/** Detaches an object from this pipeline at given index */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void DetachFromPipelineAt(FTGOR_Serial Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Applies an impulse to all attachments */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyImpulse(const FVector& Impulse);

	/** Applies an impulse along the spline to all attachments */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyLineImpulse(float Impulse);

	/** Applies an impulse to a given attachment */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyImpulseTo(const FVector& Impulse, FTGOR_Serial Index);

	/** Applies an impulse along the spline to a given attachment */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyLineImpulseTo(float Impulse, FTGOR_Serial Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get spline mesh */
	UFUNCTION(BlueprintPure, Category = "TGOR Pipeline", Meta = (Keywords = "C++"))
		USplineComponent* GetSpline() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Attachments to this pipeline */
	UPROPERTY(ReplicatedUsing = RepNotifyAttachments, BlueprintReadOnly, Category = "TGOR Pipeline")
		FTGOR_PipelineAttachments Attachments;

	UFUNCTION()
		void RepNotifyAttachments();

	/** Attached spline components */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Pipeline")
		TWeakObjectPtr<USplineComponent> SplineComponent;

	/** Radius at scale 1.0 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float SplineRadius;

	/** Array of automatically loaded socket classes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Target")
		TArray<TSubclassOf<UTGOR_PipelineSocket>> SocketClasses;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Whether the pipe end is closed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		bool ClosedEnd;

	/** Whether the pipe start is open */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		bool ClosedStart;


	/** Speed all attachments start with from beginning/end */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float InitialSpeed;

	/** Speed all attachments are accelerated towards */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float StreamSpeed;

	/** Force towards stream speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float StreamAcceleration;

	/** Force against stream speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float StreamDamping;

	/** Amount of force applied by physical volume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float StreamPhysics;

	/** Applied external force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		FVector StreamExternal;


	/** Elasticity on pipeline collisions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline")
		float CollisionElasticity;

};

