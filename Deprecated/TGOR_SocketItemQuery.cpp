// The Gateway of Realities: Planes of Existence.


#include "TGOR_SocketItemQuery.h"
#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Components/System/TGOR_ActionComponent.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/UI/Inventory/TGOR_InventoryTabMenu.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Unlocks/Spawner/Items/TGOR_Item.h"

#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

UTGOR_SocketItemQuery::UTGOR_SocketItemQuery()
	: Super()
{
}

bool UTGOR_SocketItemQuery::FilterItem(UTGOR_ItemStorage* Storage) const
{
	if (!Super::FilterItem(Storage)) return false;
	if (Movement.IsValid() && IsValid(Storage) && IsValid(Socket))
	{
		UTGOR_Item* Content = Storage->GetItem();
		FTGOR_MovementShape Shape;
		Shape.Radius = Content->GetRadius();
		Shape.Height = Content->GetHeight();
		return Socket->CanFitToSocket(Movement.Get(), Shape);
	}
	return true;
}

void UTGOR_SocketItemQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, int32 ActionIdentifier, const FTGOR_ItemRestriction& Filter)
{
	if (IsValid(ActionComponent))
	{
		UTGOR_AimComponent* AimComponent = ActionComponent->GetOwnerComponent<UTGOR_AimComponent>();
		if (IsValid(AimComponent))
		{
			const FTGOR_AimInstance Aim = AimComponent->GetCurrentAim();
			Movement = Cast<UTGOR_PilotComponent>(Aim.Component);
			Socket = Cast<UTGOR_Socket>(Aim.Target);
		}
	}

	Super::AssignComponent(ActionComponent, ActionIdentifier, Filter);
}

