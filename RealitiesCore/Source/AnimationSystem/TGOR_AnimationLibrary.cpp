// The Gateway of Realities: Planes of Existence.

#include "TGOR_AnimationLibrary.h"

#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "AnimationSystem/Components/TGOR_EllipsoidComponent.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"


UTGOR_AnimationLibrary::UTGOR_AnimationLibrary(class FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_AttachComponent* UTGOR_AnimationLibrary::GetAttachConeComponent(AActor* Owner, TSubclassOf<UTGOR_AttachCone> Type)
{
	if (IsValid(Owner))
	{
		TArray<UTGOR_AttachComponent*> Components;
		Owner->GetComponents(Components);
		for (UTGOR_AttachComponent* Component : Components)
		{
			if (Component->SpawnAttachCone->IsChildOf(Type))
			{
				return Component;
			}
		}
	}
	return nullptr;
}

void UTGOR_AnimationLibrary::GetAttachConeComponents(TArray<UTGOR_AttachComponent*>& Cones, AActor* Owner, const TSet<TSubclassOf<UTGOR_AttachCone>>& Types)
{
	Cones.Reset();

	if (IsValid(Owner))
	{
		TArray<UTGOR_AttachComponent*> Components;
		Owner->GetComponents(Components);
		for (UTGOR_AttachComponent* Component : Components)
		{
			for (TSubclassOf<UTGOR_AttachCone> Type : Types)
			{
				if (Component->SpawnAttachCone->IsChildOf(Type))
				{
					Cones.Emplace(Component);
					break;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_EllipsoidComponent* UTGOR_AnimationLibrary::GetEllipsoidComponent(AActor* Owner, TSubclassOf<UTGOR_Ellipsoid> Type)
{
	if (IsValid(Owner))
	{
		TArray<UTGOR_EllipsoidComponent*> Components;
		Owner->GetComponents(Components);
		for (UTGOR_EllipsoidComponent* Component : Components)
		{
			if (Component->SpawnEllipsoid->IsChildOf(Type))
			{
				return Component;
			}
		}
	}
	return nullptr;
}

void UTGOR_AnimationLibrary::GetEllipsoidComponents(TArray<UTGOR_EllipsoidComponent*>& Ellipsoids, AActor* Owner, const TSet<TSubclassOf<UTGOR_Ellipsoid>>& Types)
{
	Ellipsoids.Reset();

	if (IsValid(Owner))
	{
		TArray<UTGOR_EllipsoidComponent*> Components;
		Owner->GetComponents(Components);
		for (UTGOR_EllipsoidComponent* Component : Components)
		{
			for (TSubclassOf<UTGOR_Ellipsoid> Type : Types)
			{
				if (Component->SpawnEllipsoid->IsChildOf(Type))
				{
					Ellipsoids.Emplace(Component);
					break;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_HandleComponent* UTGOR_AnimationLibrary::GetHandleComponent(AActor* Owner, TSubclassOf<UTGOR_Primitive> Type, bool EnforceCone)
{
	if (IsValid(Owner))
	{
		TArray<UTGOR_HandleComponent*> Components;
		Owner->GetComponents(Components);
		for (UTGOR_HandleComponent* Component : Components)
		{
			if (!EnforceCone || IsValid(Component->MovementCone))
			{
				if (Component->SpawnPrimitive->IsChildOf(Type))
				{
					return Component;
				}
			}
		}
	}
	return nullptr;
}

void UTGOR_AnimationLibrary::GetHandleComponents(TArray<UTGOR_HandleComponent*>& Handles, AActor* Owner, const TSet<TSubclassOf<UTGOR_Primitive>>& Types, bool EnforceCone)
{
	Handles.Reset();

	if (IsValid(Owner))
	{
		TArray<UTGOR_HandleComponent*> Components;
		Owner->GetComponents(Components);
		for (UTGOR_HandleComponent* Component : Components)
		{
			if (!EnforceCone || IsValid(Component->MovementCone))
			{
				for (TSubclassOf<UTGOR_Primitive> Type : Types)
				{
					if (Component->SpawnPrimitive->IsChildOf(Type))
					{
						Handles.Emplace(Component);
						break;
					}
				}
			}
		}
	}
}