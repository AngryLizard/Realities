// The Gateway of Realities: Planes of Existence.


#include "TGOR_PlayerComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "PlayerSystem/Data/TGOR_UserData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UTGOR_PlayerComponent::UTGOR_PlayerComponent()
	: Super()
{
	MaxLabelDistance = 10000.0f;
}

void UTGOR_PlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetWidgetRender();
}

void UTGOR_PlayerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UTGOR_PlayerComponent, CurrentUser);
}

void UTGOR_PlayerComponent::SetWidgetRender()
{
	UWorld* World = GetWorld();
	UUserWidget* UserWidget = GetWidget();
	if (IsValid(World) && IsValid(UserWidget))
	{
		APlayerController* Controller = UserWidget->GetOwningPlayer();
		if (IsValid(Controller) && MaxLabelDistance > SMALL_NUMBER)
		{
			// Compute distance to local player camera
			const FVector CameraLocation = Controller->PlayerCameraManager->GetCameraLocation();
			const FVector NameLocation = GetComponentLocation();

			const float DistSquared = FVector::DistSquared(CameraLocation, NameLocation);
			const float Ratio = 1.0f - (DistSquared / (MaxLabelDistance * MaxLabelDistance));

			// Make sure label is in range
			if (Ratio < 1.0f)
			{
				FHitResult Hit;
				TArray<AActor*> Ignore;
				if (UKismetSystemLibrary::LineTraceSingle(this, NameLocation, CameraLocation, TraceType, false, Ignore, EDrawDebugTrace::None, Hit, true))
				{
					SetVisibility(true, true);
					UserWidget->SetRenderScale(FVector2D(Ratio, Ratio));
					return;
				}
			}
		}
	}
	SetVisibility(false, false);
}

void UTGOR_PlayerComponent::VisualiseUser(const FTGOR_UserProperties& User)
{

}

void UTGOR_PlayerComponent::ReplicateUser()
{
	VisualiseUser(CurrentUser);
}

void UTGOR_PlayerComponent::ResetUser()
{
	CurrentUser = FTGOR_UserProperties();

	ApplyUser(-1, CurrentUser);
}

void UTGOR_PlayerComponent::ApplyUser(int32 UserKey, const FTGOR_UserProperties& User)
{
	CurrentUser = User;
	VisualiseUser(CurrentUser);
	OnSetupUser.Broadcast(UserKey, CurrentUser);
}

void UTGOR_PlayerComponent::GrabUser(int32 UserKey)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetData<UTGOR_UserData>();
	if (IsValid(UserData) && UserData->HasUser(UserKey))
	{
		// Set user data
		const FTGOR_UserInstance& User = UserData->GetUser(UserKey);
		ApplyUser(UserKey, User.Properties);
	}
}