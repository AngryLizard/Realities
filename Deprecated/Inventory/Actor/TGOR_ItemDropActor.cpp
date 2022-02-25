// The Gateway of Realities: Planes of Existence.


#include "TGOR_ItemDropActor.h"
#include "Components/Dimension/TGOR_SaveComponent.h"
#include "Components/Inventory/TGOR_SingleItemComponent.h"
#include "Base/TGOR_Singleton.h"
#include "Mod/Items/TGOR_Item.h"

// Sets default values
ATGOR_ItemDropActor::ATGOR_ItemDropActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	DestroyOnEmpty = true;
	CurrentItem = nullptr;

	Interactable = ObjectInitializer.CreateDefaultSubobject<UTGOR_InteractableComponent>(this, FName(TEXT("Interactable")));
	RootComponent = Interactable;
		
	SaveComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SaveComponent>(this, FName(TEXT("SaveComponent")));

	Container = ObjectInitializer.CreateDefaultSubobject<UTGOR_SingleItemComponent>(this, FName(TEXT("Container")));
	Container->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ATGOR_ItemDropActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATGOR_ItemDropActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Update item if content changed
	//const FTGOR_ItemInstance_OLD& Instance = Container->ItemInstance;
	//if (CurrentItem != Instance.Content || Instance.Quantity < ITEM_EPS)
	//{
	//	UpdateItem(); 
	//}
}


void ATGOR_ItemDropActor::Combine(ATGOR_ItemDropActor* Drop)
{
	//SINGLETON_CHK
	//UTGOR_ContentManager* ContentManager = Singleton->ContentManager;

	//UTGOR_Item* Ours = Container->ItemInstance.Content;
	//UTGOR_Item* Theirs = Drop->Container->ItemInstance.Content;

	//if (!IsValid(Ours) || !IsValid(Theirs)) return;
	//
	//if (!Ours->CanBeMerged(Container->ItemInstance, Drop->Container->ItemInstance)) return;
	//Ours->Merge(Drop->Container->ItemInstance, Container->ItemInstance, 0);
	//Container->AssignItem(Container->ItemInstance);

	//if (!IsValid(Drop->Container->ItemInstance.Content))
	//	Drop->Container->AssignItem(Drop->Container->ItemInstance);
	//else
	//	Drop->Interactable->Despawn(this);
}


void ATGOR_ItemDropActor::UpdateItem()
{
	//const FTGOR_ItemInstance& Instance = Container->ItemInstance;
	//if (!IsValid(Instance.Content) || Instance.Quantity < ITEM_EPS)
	//{
	//	CurrentItem = nullptr;
	//	
	//	if (DestroyOnEmpty)
	//	{
	//		//Interactable->Despawn(this); 
	//	}
	//	return;
	//};
		
	if (IsValid(EnsureItem()))
	{
		UpdateDisplay(_item->Display);
	}
}

UTGOR_Item* ATGOR_ItemDropActor::EnsureItem()
{
	//const FTGOR_ItemInstance& Instance = Container->ItemInstance;
	//if (CurrentItem == Instance.Content)
	//{
	//	return(_item);
	//}

	//SINGLETON_RETCHK(nullptr);
	//UTGOR_ContentManager* ContentManager = Singleton->ContentManager;

	//CurrentItem = Instance.Content;
	return(CurrentItem);
}