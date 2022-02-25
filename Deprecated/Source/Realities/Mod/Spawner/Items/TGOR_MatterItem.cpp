#include "TGOR_MatterItem.h"

#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Content/TGOR_Content.h"

UTGOR_MatterItem::UTGOR_MatterItem()
	: Super()
{
	Modules.Modules.Add("Storage", UTGOR_MatterModule::StaticClass());
}

void UTGOR_MatterItem::BuildStorage(UTGOR_ItemStorage* Storage)
{
	Super::BuildStorage(Storage);

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_MatterModule* Module = Storage->GetModule<UTGOR_MatterModule>();
	if (IsValid(Module))
	{
		// Generate composition
		for (auto& Pair : MatterStorage)
		{
			UTGOR_Matter* Content = ContentManager->GetTFromType<UTGOR_Matter>(Pair.Key); // *this breaks if Matter isn't included in item.h, see header for more.
			Module->AddStorageCapacity(Content, Pair.Value);
		}
	}
}
