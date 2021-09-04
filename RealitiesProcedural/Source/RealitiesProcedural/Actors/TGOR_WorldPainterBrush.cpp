// The Gateway of Realities: Planes of Existence.

#include "TGOR_WorldPainterBrush.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/ArrowComponent.h"
#include "UObject/ConstructorHelpers.h"

ATGOR_WorldPainterBrush::ATGOR_WorldPainterBrush(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	Size(0.1f)
{
	DynamicMaterial = nullptr;
	bIsEditorOnlyActor = true;

#if WITH_EDITORONLY_DATA
	RootComponent = CreateEditorOnlyDefaultSubobject<USceneComponent>(TEXT("Root"));
	PreviewComponent = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UStaticMesh> WPBrushMeshObject;

			FConstructorStatics()
				: WPBrushMeshObject(TEXT("/RealitiesProcedural/Resources/SM_WorldPainter_BrushMesh.SM_WorldPainter_BrushMesh"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		PreviewComponent->SetRelativeScale3D_Direct(FVector(1.f, 1.f, 1.f));
		PreviewComponent->SetStaticMesh(ConstructorStatics.WPBrushMeshObject.Get());
		PreviewComponent->SetupAttachment(RootComponent);
	}

#endif // WITH_EDITORONLY_DATA

	PrimaryActorTick.bCanEverTick = false;
	bAllowTickBeforeBeginPlay = true;
	bReplicates = false;
	NetUpdateFrequency = 10.0f;
	SetHidden(true);
	SetReplicatingMovement(false);
	SetCanBeDamaged(false);
}

void ATGOR_WorldPainterBrush::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UMaterialInterface* Material = GetPreviewComponent()->GetMaterial(0);
	if (IsValid(Material) && !IsValid(DynamicMaterial))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	}

	UpdateBillboardMaterial();
}


void ATGOR_WorldPainterBrush::Bake()
{
	ATGOR_WorldPainterLayer* PainterLayer = GetPainterLayer();
	if (PainterLayer)
	{
		PainterLayer->Bake();
	}
}

void ATGOR_WorldPainterBrush::UpdateBillboardMaterial()
{
	if (IsValid(DynamicMaterial))
	{
		const int32 Num = Brush.Slices.Num();
		for (int32 Index = 0; Index < Num; Index++)
		{
			DynamicMaterial->SetVectorParameterValue(*FString::Printf(TEXT("Color%d"), Index), Brush.Slices[Index]);
		}

		ATGOR_WorldPainterLayer* PainterLayer = GetPainterLayer();
		if (IsValid(PainterLayer))
		{
			DynamicMaterial->SetScalarParameterValue("CanvasID", PainterLayer->MaterialID);
			DynamicMaterial->SetScalarParameterValue("SliceCount", PainterLayer->Bias.Slices.Num());
		}

		GetPreviewComponent()->SetMaterial(0, DynamicMaterial);
	}
}

ATGOR_WorldPainterLayer* ATGOR_WorldPainterBrush::GetPainterLayer() const
{
	return Cast<ATGOR_WorldPainterLayer>(GetAttachParentActor());
}

#if WITH_EDITOR

void ATGOR_WorldPainterBrush::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
		if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterBrush, Size))
		{
			UpdateBillboardMaterial();
			ATGOR_WorldPainterLayer* PainterLayer = GetPainterLayer();
			if (PainterLayer)
			{
				PainterLayer->OnBrushUpdate(false);
			}
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ATGOR_WorldPainterBrush::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterBrush, Brush))
	{
		UpdateBillboardMaterial();
		ATGOR_WorldPainterLayer* PainterLayer = GetPainterLayer();
		if (PainterLayer)
		{
			PainterLayer->OnBrushUpdate(false);
		}
	}
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

void ATGOR_WorldPainterBrush::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	PreviewComponent->SetRelativeScale3D_Direct(FVector(1.f, 1.f, 1.f));
	PreviewComponent->SetWorldRotation(FQuat::Identity);

	ATGOR_WorldPainterLayer* PainterLayer = GetPainterLayer();
	if (PainterLayer)
	{
		PainterLayer->OnBrushUpdate(bFinished);
	}
}

#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
/** Returns PreviewComponent subobject **/
UStaticMeshComponent* ATGOR_WorldPainterBrush::GetPreviewComponent() const { return PreviewComponent; }
#endif
