// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "Management/ElementusInventoryData.h"

const FElementusItemInfo FElementusItemInfo::EmptyItemInfo(FPrimaryElementusItemId(), -1);

UElementusItemData::UElementusItemData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}