// The Gateway of Realities: Planes of Existence.


#include "TGOR_SocketTabQuery.h"
#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/UI/Inventory/TGOR_InventoryTabMenu.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_SocketTabQuery::UTGOR_SocketTabQuery()
	: Super()
{
}

bool UTGOR_SocketTabQuery::FilterItem(UTGOR_ItemStorage* Storage) const
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

void UTGOR_SocketTabQuery::AssignTab(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_InventoryTabMenu> TabMenu)
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

	Super::AssignTab(ActionComponent, TabMenu);
}

