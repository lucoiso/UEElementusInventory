// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "Actors/ElementusInventoryPackage.h"
#include "Components/ElementusInventoryComponent.h"
#include "Management/ElementusInventorySettings.h"
#include "Management/ElementusInventoryFunctions.h"
#include "Management/ElementusInventoryData.h"
#include "LogElementusInventory.h"
#include <Net/UnrealNetwork.h>
#include <Net/Core/PushModel/PushModel.h>

#ifdef UE_INLINE_GENERATED_CPP_BY_NAME
#include UE_INLINE_GENERATED_CPP_BY_NAME(ElementusInventoryPackage)
#endif

AElementusInventoryPackage::AElementusInventoryPackage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bNetStartup = false;
    bNetLoadOnClient = false;
    bReplicates = true;

    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    PackageInventory = CreateDefaultSubobject<UElementusInventoryComponent>(TEXT("PackageInventory"));
    PackageInventory->SetIsReplicated(true);

    if (const UElementusInventorySettings* const Settings = UElementusInventorySettings::Get())
    {
        bDestroyWhenInventoryIsEmpty = Settings->bDestroyWhenInventoryIsEmpty;
    }
}

void AElementusInventoryPackage::BeginPlay()
{
    Super::BeginPlay();

    SetDestroyOnEmpty(bDestroyWhenInventoryIsEmpty);

    if (bDestroyWhenInventoryIsEmpty && UElementusInventoryFunctions::HasEmptyParam(PackageInventory->GetItemsArray()))
    {
        Destroy();
    }
}

void AElementusInventoryPackage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams SharedParams;
    SharedParams.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(AElementusInventoryPackage, PackageInventory, SharedParams);
}

void AElementusInventoryPackage::PutItemIntoPackage(const TArray<FElementusItemInfo> ItemInfo, UElementusInventoryComponent* FromInventory)
{
    UElementusInventoryFunctions::TradeElementusItem(ItemInfo, FromInventory, PackageInventory);
    MARK_PROPERTY_DIRTY_FROM_NAME(AElementusInventoryPackage, PackageInventory, this);
}

void AElementusInventoryPackage::GetItemFromPackage(const TArray<FElementusItemInfo> ItemInfo, UElementusInventoryComponent* ToInventory)
{
    UElementusInventoryFunctions::TradeElementusItem(ItemInfo, PackageInventory, ToInventory);
    MARK_PROPERTY_DIRTY_FROM_NAME(AElementusInventoryPackage, PackageInventory, this);
}

void AElementusInventoryPackage::SetDestroyOnEmpty(const bool bDestroy)
{
    if (bDestroyWhenInventoryIsEmpty == bDestroy)
    {
        return;
    }

    bDestroyWhenInventoryIsEmpty = bDestroy;
    FElementusInventoryEmpty Delegate = PackageInventory->OnInventoryEmpty;

    if (const bool bIsAlreadyBound = Delegate.IsAlreadyBound(this, &AElementusInventoryPackage::BeginPackageDestruction); bDestroy && !bIsAlreadyBound)
    {
        Delegate.AddDynamic(this, &AElementusInventoryPackage::BeginPackageDestruction);
    }
    else if (!bDestroy && bIsAlreadyBound)
    {
        Delegate.RemoveDynamic(this, &AElementusInventoryPackage::BeginPackageDestruction);
    }
}

bool AElementusInventoryPackage::GetDestroyOnEmpty() const
{
    return bDestroyWhenInventoryIsEmpty;
}

void AElementusInventoryPackage::BeginPackageDestruction_Implementation()
{
    // Check if this option is still active before the destruction
    if (bDestroyWhenInventoryIsEmpty)
    {
        Destroy();
    }
    else
    {
        UE_LOG(LogElementusInventory_Internal, Warning, TEXT("ElementusInventory - %s: Package %s was not destroyed because the " "option 'bDestroyWhenInventoryIsEmpty' was disabled"), *FString(__func__), *GetName());
    }
}