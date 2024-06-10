#include "InteractComponent.h"

#include "GASTraining/GASTrainingCharacter.h"
#include "GASTraining/Components/InventoryComponent/Interface/PickupTriggerInterface.h"
#include "Camera/CameraComponent.h"
#include "GASTraining/Components/InventoryComponent/InventoryComponent.h"
#include "GASTraining/Components/InventoryComponent/ItemInstance.h"
#include "GASTraining/Components/InventoryComponent/MainActorCustom/ActorDataAssetCustom.h"
#include "GASTraining/RandomStructs/FClientCameraTraceParams.h"

UInteractComponent::UInteractComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;
	
	InteractCameraDistance = 500;
	DropCameraDistance = 50;
}

void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// Making sure it's attached with a proper Owner
	if(GetOwner() == nullptr) { return; }
	
	// Currently working with the CHARACTER only --> Could add more for the AI with a bool / enum ?
	AGASTrainingCharacter* PlayerCharacter = Cast<AGASTrainingCharacter>(GetOwner());
	if(PlayerCharacter != nullptr)
	{
		// Handle interaction for Player Character
		PlayerCharacter->OnInteract.BindUObject(this, &UInteractComponent::OnInteract);
		PlayerCharacter->OnDropped.BindUObject(this, &UInteractComponent::OnDropped);
	}
}


//==========================================================================
// Interacting Functionalities
void UInteractComponent::OnInteract()
{
	GetClientCameraParams(InteractCameraDistance, [this](const FClientCameraTraceParams CameraParams)
	{
		OnServerInteract(CameraParams);
	});
}

void UInteractComponent::OnServerInteract_Implementation(const FClientCameraTraceParams& ClientCameraParams)
{
	FHitResult OutHit;
	
	// Does line trace hit something?
	if (GetWorld()->LineTraceSingleByChannel(OutHit, ClientCameraParams.StartLocation, ClientCameraParams.EndLocation, ECC_Visibility))
	{
		
		// Making sure the Actor hit implements the interface!
		if(OutHit.GetActor()->GetClass()->ImplementsInterface(UPickupTriggerInterface::StaticClass()))
		{
			// Access the functions in the interface
			IPickupTriggerInterface* PickupTriggerInterface = Cast<IPickupTriggerInterface>(OutHit.GetActor());
			if(PickupTriggerInterface != nullptr)
			{
				
				// All defaults values are setup here!!!
				FItem NewItem;
				NewItem.ItemDataAsset = PickupTriggerInterface->GetDataAssetOfActor();
				NewItem.ValueChanging = 5;
					
				
				
				// Adding the newly created item to the Inventory component Array
				UInventoryComponent* Inventory = GetOwner()->GetComponentByClass<UInventoryComponent>();
				if(Inventory != nullptr)
				{
					Inventory->AddItem(NewItem);
                    
                    					
					// Destroy the actor in the world, since it's stored
					OutHit.GetActor()->Destroy();
				}
			}
		}
	}
}
// End of Interacting Functionalities
//===========================================================================




//==========================================================================
// Dropping Functionalities
void UInteractComponent::OnDropped()
{
	// TODO: it shouldn't check for an empty array, it should check to see if the current slot has an ITEM...
	if(GetInventoryComponent()->ItemsArray.IsEmpty()) { return; }

	
	GetClientCameraParams(DropCameraDistance, [this](FClientCameraTraceParams CameraParams)
	{
		OnServerDropped(CameraParams);
	});
}

void UInteractComponent::OnServerDropped_Implementation(const FClientCameraTraceParams& ClientCameraParams)
{
	// Safety
	if(GetWorld() == nullptr) { return; }
		
	UInventoryComponent* Inventory = GetInventoryComponent();
	AActorDataAssetCustom* ItemSpawned = Cast<AActorDataAssetCustom>(GetWorld()->SpawnActor<AActor>(AActorDataAssetCustom::StaticClass(), ClientCameraParams.EndLocation, FRotator()));
	if(ItemSpawned && Inventory != nullptr)
	{
		// Delete the Actor from the Array (since it's spawned)
		Inventory->DropItem(1);
		// TODO: Currently dropping the item from the last slot, but it should be from the current slot

		
		// TODO: Currently taking the last item out of the array, later, it should be the current one (the one you are holding)
		
		ItemSpawned->CurrentDataAsset = Inventory->ItemsArray.Last().ItemDataAsset;
		ItemSpawned->OnActorUpdate();
	}
}

// End of Dropping Functionalities
//===========================================================================



void UInteractComponent::GetClientCameraParams(const int32 Distance, const std::function<void(const FClientCameraTraceParams&)>& Callback)
{
	UCameraComponent* Camera = GetOwner()->GetComponentByClass<UCameraComponent>();
	
	// Making sure there's a correct and valid Owner
	if(GetOwner() && GetWorld() && Camera != nullptr)
	{
		// Since Camera is a client only component, we can get the location on the client ONLY
		FClientCameraTraceParams ClientCameraParams;
		ClientCameraParams.StartLocation = Camera->GetComponentLocation();
		ClientCameraParams.EndLocation = ClientCameraParams.StartLocation + Camera->GetForwardVector() * Distance;
		
		Callback(ClientCameraParams);
	}
}

[[nodiscard]] UInventoryComponent* UInteractComponent::GetInventoryComponent()
{
	return GetOwner()->FindComponentByClass<UInventoryComponent>();
}
