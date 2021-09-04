// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartTransformQuery.h"

#include "CustomisationSystem/UI/TGOR_BodypartCustomisationWidget.h"
#include "CustomisationSystem/UI/TGOR_BodypartListWidget.h"
#include "CustomisationSystem/Actors/TGOR_PreviewActor.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_BodypartTransformQuery::UTGOR_BodypartTransformQuery()
	: Super()
{

}

void UTGOR_BodypartTransformQuery::TransformFromProbe(const FVector& WorldLocation, const FVector& WorldDirection)
{
	FVector ProbeLocation;
	FVector ProbeDirection;
	float Twist = 0.0f;
	float Scale = 0.0f;
	ParamsFromTransform(ProbeLocation, ProbeDirection, Twist, Scale);

	if (IsValid(CurrentPreview))
	{
		FVector Location;
		FVector Direction;
		if (!CurrentPreview->ProbePointerTransform(WorldLocation, WorldDirection, ProbeLocation, ProbeDirection, Location, Direction))
		{
			FTransform Transform = CurrentPreview->GetActorTransform();
			Location = Transform.InverseTransformPosition(ProbeLocation);
			Direction = Transform.InverseTransformVector(ProbeDirection);
		}

		TransformFromParams(Location, Direction, Twist, Scale);
	}
}

void UTGOR_BodypartTransformQuery::TransformFromParams(const FVector& Location, const FVector& Direction, float Twist, float Scale)
{
	FTransform Transform;
	Transform.SetScale3D(FVector(FMath::Exp(Scale)));
    const FQuat Alignment = FQuat::FindBetweenNormals(FVector::UpVector, Direction);
	Transform.SetRotation(Alignment * FQuat(FVector::UpVector, Twist * PI));
	Transform.SetLocation(Location);

	CommitTransform(Transform);
}

void UTGOR_BodypartTransformQuery::ParamsFromTransform(FVector& Location, FVector& Direction, float& Twist, float& Scale) const
{
	Location = CurrentTransform.GetLocation();
	Direction = CurrentTransform.TransformVectorNoScale(FVector::UpVector);
	Scale = FMath::Loge(CurrentTransform.GetScale3D().GetMax());

	const FQuat Alignment = FQuat::FindBetweenNormals(FVector::UpVector, Location);
	Twist = (Alignment.Inverse() * CurrentTransform.GetRotation()).GetAngle() / PI;
}

void UTGOR_BodypartTransformQuery::CommitTransform(const FTransform& Transform)
{
	CurrentTransform = Transform;

	DisplayTransform();
	if (IsValid(CallbackWidget))
	{
		CallbackWidget->TransformCallback(Transform);
	}
}

void UTGOR_BodypartTransformQuery::DisplayTransform()
{
	if (IsValid(CurrentPreview))
	{
		CurrentPreview->SetPointerTransform(CurrentTransform);
	}
}
void UTGOR_BodypartTransformQuery::SetupFromPreview(UTGOR_BodypartCustomisationWidget* CustomisationWidget, ATGOR_PreviewActor* Preview, const FTransform& Transform)
{
	AssignWorld(CustomisationWidget);
	CallbackWidget = CustomisationWidget;

	CurrentPreview = Preview;
	CurrentTransform = Transform;
}
