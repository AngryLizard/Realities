// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Classes/Components/SplineComponent.h"

#include "DimensionSystem/TGOR_PilotInstance.h"

#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "TGOR_PipelineComponent.generated.h"

class UTGOR_Pipeline;
class UTGOR_PipelineSocket;
class UTGOR_PilotComponent;
class UTGOR_PipelineSocketTask;

USTRUCT(BlueprintType)
struct SOCKETSYSTEM_API FTGOR_PipelineHitResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		FTGOR_Index Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float Depth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float Normal;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPipelineHitDelegate, UTGOR_PilotComponent*, Attachee);

/**
* TGOR_PipelineComponent allows custom parts to attach to
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class SOCKETSYSTEM_API UTGOR_PipelineComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()

	friend class UTGOR_PipelineSocket;
	friend class UTGOR_PipelineSocketTask;
	
public:
	UTGOR_PipelineComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void InflictPointImpact(const FVector& Point, const FVector& Impulse) override;
	virtual void InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	/** Called when an attachee hit or left pipeline start */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Pipeline")
		FPipelineHitDelegate OnHitStart;

	/** Called when an attachee hit or left pipeline end */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Pipeline")
		FPipelineHitDelegate OnHitEnd;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get spline distance nearest to given location
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		float GetDistanceNearestTo(const FVector& Location) const; */

	/** Get spline distance at a given point */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		float GetDistanceAt(int32 SplinePoint) const;

	/** Get spline danymic at a given distance and speed */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		FTGOR_MovementDynamic GetPipelineDynamic(float Distance, float Speed, bool Forward) const;

	/** Get spline point at the end of the pipeline */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		int32 GetLastSplinePoint() const;

	/** Get spline displacement by attachments at given spline distance and falloff distance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		float GetDisplacementAtDistance(float Distance, float Falloff) const;

	/** Get spline radius at given spline distance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		float GetRadiusAtDistance(float Distance) const;

	/** Check whether spline is a closed loop */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		bool IsClosedLoop() const;

	/** Hit-tests along spline at a ball with given spline distance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		bool IsOccupied(float Distance, float Radius, float Segment) const;

	/** Sweep by a given delta from distance.  */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		bool Sweep(float Distance, float Radius, float Segment, float Delta, FTGOR_Serial Ignore, FTGOR_PipelineHitResult& HitResult) const;

	/** Draw spline radius over the whole spline */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		void DrawSplineRadius(float Interval);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get view location at a given distance, direction and distance offset */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		FVector GetViewLocation(float Distance, float Direction, float Offset) const;

	/** Get acceleration in spline space (along the spline) applied at a given point with a given speed */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		float GetAccelerationAtDistance(float Distance, float Speed, float Direction) const;

	/** Get distance to a closed pipeline end of a given point on the spline. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		float GetDistanceFromEnd(float Distance, float Threshold) const;

	/** Get distance to a closed pipeline start of a given point on the spline. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		float GetDistanceFromStart(float Distance, float Threshold) const;

	/** Collides with pipeline end, returns true if we reflected */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		bool CollideWithEnd(UTGOR_PilotComponent* Attachee);

	/** Collides with pipeline start, returns true if we reflected */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pipeline|Internal", Meta = (Keywords = "C++"))
		bool CollideWithStart(UTGOR_PilotComponent* Attachee);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Current transform of given socket in world space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetTransformOfPipeline(UTGOR_PipelineSocket* Socket) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Finds whether a shape can be attached to this pipeline at a given spline point */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		bool CanFitToPipeline(const FTGOR_MovementShape& Shape, UTGOR_PipelineSocket* Socket) const;

	/** Finds whether an object can be attached to this pipeline at a given spline point */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		bool CanAttachToPipeline(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Parents a component to this pipeline at a given socket. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool ParentPipelineSocket(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket);

	/** Check whether we can parent to a given pipeline socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParentPipelineSocket(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Attachments to this component */
	UPROPERTY()
		TArray<TWeakObjectPtr<UTGOR_PipelineSocketTask>> Attachments;

	/** Add a child to attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterAttachToSocket(UTGOR_PipelineSocketTask* Attachee);

	/** Remove a child from attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterDetachFromSocket(UTGOR_PipelineSocketTask* Attachee);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Applies an impulse to all attachments */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyImpulse(const FVector& Impulse);

	/** Applies an impulse along the spline to all attachments */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyLineImpulse(float Impulse);

	/** Applies an impulse to a given attachment */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyImpulseTo(const FVector& Impulse, FTGOR_Serial Index);

	/** Applies an impulse along the spline to a given attachment */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		void ApplyLineImpulseTo(float Impulse, FTGOR_Serial Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get spline mesh */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pipeline", Meta = (Keywords = "C++"))
		USplineComponent* GetSpline() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Attached spline components */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Pipeline")
		TWeakObjectPtr<USplineComponent> SplineComponent;

	/** Radius at scale 1.0 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float SplineRadius;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Whether the pipe end is closed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		bool ClosedEnd;

	/** Whether the pipe start is open */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		bool ClosedStart;

	/** Whether attachments move in forward direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		bool StreamForward;

	/** Speed all attachments start with from beginning/end */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float InitialSpeed;

	/** Speed all attachments are accelerated towards */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float StreamSpeed;

	/** Force towards stream speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float StreamAcceleration;

	/** Force against stream speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float StreamDamping;

	/** Amount of force applied by physical volume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float StreamPhysics;

	/** Applied external force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		FVector StreamExternal;


	/** Elasticity on pipeline collisions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Pipeline")
		float CollisionElasticity;

};

