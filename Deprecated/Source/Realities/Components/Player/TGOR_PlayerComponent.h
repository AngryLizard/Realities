// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Instances/Player/TGOR_UserInstance.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Components/WidgetComponent.h"
#include "TGOR_PlayerComponent.generated.h"


//////////////////////////////////////////// DELEGATES /////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSetupUserDelegate, int32, UserKey, const FTGOR_UserProperties&, User);

/**
* TGOR_PlayerComponent adds functionality to manage users
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PlayerComponent : public UWidgetComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_PlayerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "TGOR Online")
		FSetupUserDelegate OnSetupUser;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max distance at which this name is still visible */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Online")
		float MaxLabelDistance;

	/** Trace type to figure out whether to display a player name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Online", Meta = (Keywords = "C++", ClampMin = "1"))
		TEnumAsByte<ETraceTypeQuery> TraceType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set widget visibility and scale */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void SetWidgetRender();

	/** Visualise user on widget */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void VisualiseUser(const FTGOR_UserProperties& User);

	UFUNCTION()
		void ReplicateUser();

	/** Resets user information */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void ResetUser();

	/** Grabs userproperties and applies user */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void GrabUser(int32 UserKey);

	/** Sets userproperties and visualises them (call on server only), UserKey = -1 for NPC */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void ApplyUser(int32 UserKey, const FTGOR_UserProperties& User);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current user information */
	UPROPERTY(ReplicatedUsing = ReplicateUser, BlueprintReadWrite, Category = "TGOR Online")
		FTGOR_UserProperties CurrentUser;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
