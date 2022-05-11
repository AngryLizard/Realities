// The Gateway of Realities: Planes of Existence.


#include "TGOR_PreviewActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "EngineUtils.h"

#include "CoreSystem/Components/TGOR_SceneComponent.h"

#include "CustomisationSystem/UserData/TGOR_BodypartUserData.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"

#include "DimensionSystem/Components/TGOR_IdentityComponent.h"

#define LOCTEXT_NAMESPACE "TGOR_PreviewActor"

ATGOR_PreviewActor::ATGOR_PreviewActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CaptureTexture(nullptr),
	CaptureMaterial(nullptr),
    BlendSpeed(10.0f),
    DistanceRatio(2.0f)
{
    PrimaryActorTick.bCanEverTick = true;

	UTGOR_SceneComponent* Root = ObjectInitializer.CreateDefaultSubobject<UTGOR_SceneComponent>(this, FName(TEXT("Root")));
	SetRootComponent(Root);

    BodypartComponent = CreateDefaultSubobject<UTGOR_CustomisationComponent>(TEXT("BodypartComponent"));
    BodypartComponent->SetupAttachment(Root);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(Root);

	CaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent2D"));
	CaptureComponent2D->SetupAttachment(SpringArmComponent);

	CaptureComponent2D->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	CaptureComponent2D->bCaptureEveryFrame = false;
	CaptureComponent2D->bCaptureOnMovement = false;
	CaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	CaptureComponent2D->ShowOnlyActors.Emplace(this);
}

void ATGOR_PreviewActor::BeginPlay()
{
	Super::BeginPlay();
    
    InitialiseRenderTarget();
}

void ATGOR_PreviewActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TickRenderTarget(DeltaTime);
}

void ATGOR_PreviewActor::InitialiseRenderTarget()
{
    CaptureTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 2048, 1024, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), false);
    CaptureMaterial = UMaterialInstanceDynamic::Create(UIMaterial, this);

    CaptureComponent2D->TextureTarget = CaptureTexture;
    if (IsValid(CaptureMaterial))
    {
        FEditorScriptExecutionGuard ScriptGuard;
        UpdateTexture(CaptureMaterial, CaptureTexture);
    }

    TargetLocation = SpringArmComponent->GetRelativeLocation();
    TargetArmLength = SpringArmComponent->TargetArmLength * DistanceRatio;
}

void ATGOR_PreviewActor::TickRenderTarget(float DeltaTime)
{
    const float Delta = FMath::Min(1.0f, BlendSpeed * DeltaTime);

    const FVector& Relative = SpringArmComponent->GetRelativeLocation();
    SpringArmComponent->SetRelativeLocation(Relative + (TargetLocation + FVector(0.0f, 0.0f, TargetArmLength * TargetOffset) - Relative) * Delta);

    const float ArmLength = SpringArmComponent->TargetArmLength;
    SpringArmComponent->TargetArmLength = ArmLength + (TargetArmLength * DistanceRatio - ArmLength) * Delta;
}


void ATGOR_PreviewActor::OffsetCamera(const FVector2D& Delta)
{
    SpringArmComponent->AddRelativeRotation(FRotator(0.0f, Delta.X, 0.0f));
    TargetOffset += Delta.Y;
    TargetOffset = FMath::Clamp(TargetOffset, -1.0f, 1.0f);

}

void ATGOR_PreviewActor::Focus(int32 NodeIndex)
{
    FVector Center;
    float Extend;
    if (BodypartComponent->GetBounds(NodeIndex, false, Center, Extend))
    {
        const FTransform& Transform = GetActorTransform();
        TargetLocation = Transform.InverseTransformPosition(Center);
        TargetArmLength = Extend;
        //TargetOffset = 0.0f;
    }
}


bool ATGOR_PreviewActor::ProbeMesh(USkinnedMeshComponent* ProbeTarget, const FVector& InLocation, const FVector& InDirection, FVector& OutLocation, FVector& OutDirection, FVector& OutCenter, FVector& OutNormal)
{
    float HitDistance = SpringArmComponent->TargetArmLength * 2;
    OutDirection = -InDirection;
    OutLocation = InLocation + InDirection * HitDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.bTraceComplex = true;
    if (ProbeTarget->LineTraceComponent(Hit, InLocation, OutLocation, Params))
    {
        HitDistance = Hit.Distance;
        OutLocation = Hit.ImpactPoint;
        OutDirection = Hit.ImpactNormal;

        OutCenter = FVector::ZeroVector;
        OutNormal = FVector::UpVector;

        if (IsValid(ProbeTarget->SkeletalMesh))
        {
            UTGOR_BodypartUserData* UserData = ProbeTarget->SkeletalMesh->GetAssetUserData<UTGOR_BodypartUserData>();
            if (IsValid(UserData) && UserData->RenderSections.IsValidIndex(Hit.ElementIndex))
            {
                const FTGOR_RenderGeometrySection& Section = UserData->RenderSections[Hit.ElementIndex];

                const int32* TrianglePtr = Section.Mapping.Find(Hit.FaceIndex);
                if (TrianglePtr)
                {
                    const FTGOR_MergeUVTri& Tri = Section.Triangles[*TrianglePtr];
                    OutCenter = FVector(Tri.V0.Pos + Tri.V1.Pos + Tri.V2.Pos) / 3;
                    OutNormal = FVector(Tri.V0.Normal() + Tri.V1.Normal() + Tri.V2.Normal()).GetSafeNormal();
                    return true;
                }
            }
        }
    }
    return false;
}


// Source: https://forums.unrealengine.com/development-discussion/c-gameplay-programming/117296-deproject-screen-to-world-and-scenecapture2d
bool ATGOR_PreviewActor::DeprojectFromCaptureSpace(const FVector2D& CaptureCoords, FVector& WorldLocation, FVector& WorldDirection) const
{
    if (!IsValid(CaptureComponent2D) || !IsValid(CaptureComponent2D->TextureTarget))
    {
        return false;
    }



    const FTransform& Transform = CaptureComponent2D->GetComponentToWorld();
    FMatrix ViewMatrix = Transform.ToInverseMatrixWithScale();

    // swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
    ViewMatrix = ViewMatrix * FMatrix(
        FPlane(0, 0, 1, 0),
        FPlane(1, 0, 0, 0),
        FPlane(0, 1, 0, 0),
        FPlane(0, 0, 0, 1));

    const float FOV = CaptureComponent2D->FOVAngle * (float)PI / 360.0f;

    FIntPoint CaptureSize(CaptureComponent2D->TextureTarget->GetSurfaceWidth(), CaptureComponent2D->TextureTarget->GetSurfaceHeight());
    const FVector2D TexCoords = CaptureCoords * CaptureSize;

    float XAxisMultiplier;
    float YAxisMultiplier;

    if (CaptureSize.X > CaptureSize.Y)
    {
        // if the viewport is wider than it is tall
        XAxisMultiplier = 1.0f;
        YAxisMultiplier = CaptureSize.X / (float)CaptureSize.Y;
    }
    else
    {
        // if the viewport is taller than it is wide
        XAxisMultiplier = CaptureSize.Y / (float)CaptureSize.X;
        YAxisMultiplier = 1.0f;
    }

    const FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(FOV, FOV, XAxisMultiplier, YAxisMultiplier, GNearClippingPlane, GNearClippingPlane);

    const FIntRect ViewRect = FIntRect(0, 0, CaptureSize.X, CaptureSize.Y);

    FSceneView::DeprojectScreenToWorld(TexCoords, ViewRect, ViewMatrix.InverseFast(), ProjectionMatrix.Inverse(), WorldLocation, WorldDirection);
    return true;
}

bool ATGOR_PreviewActor::ProjectToCaptureSpace(const FVector& WorldLocation, FVector2D& CaptureCoords) const
{
    if (!IsValid(CaptureComponent2D) || !IsValid(CaptureComponent2D->TextureTarget))
    {
        return false;
    }

    const float FOV = CaptureComponent2D->FOVAngle * (float)PI / 360.0f;

    FIntPoint CaptureSize(CaptureComponent2D->TextureTarget->GetSurfaceWidth(), CaptureComponent2D->TextureTarget->GetSurfaceHeight());

    float XAxisMultiplier;
    float YAxisMultiplier;

    if (CaptureSize.X > CaptureSize.Y)
    {
        // if the viewport is wider than it is tall
        XAxisMultiplier = 1.0f;
        YAxisMultiplier = CaptureSize.X / (float)CaptureSize.Y;
    }
    else
    {
        // if the viewport is taller than it is wide
        XAxisMultiplier = CaptureSize.Y / (float)CaptureSize.X;
        YAxisMultiplier = 1.0f;
    }

    const FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(FOV, FOV, XAxisMultiplier, YAxisMultiplier, GNearClippingPlane, GNearClippingPlane);

    const FIntRect ViewRect = FIntRect(0, 0, CaptureSize.X, CaptureSize.Y);

    FSceneView::ProjectWorldToScreen(WorldLocation, ViewRect, ProjectionMatrix, CaptureCoords);
    CaptureCoords /= CaptureSize;
    return true;
}

UMaterialInstanceDynamic* ATGOR_PreviewActor::Capture()
{
	CaptureComponent2D->CaptureScene();
	return CaptureMaterial;
}



#undef LOCTEXT_NAMESPACE

