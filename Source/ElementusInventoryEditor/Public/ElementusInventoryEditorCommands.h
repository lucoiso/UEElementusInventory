// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ElementusInventoryEditorStyle.h"

class FElementusInventoryEditorCommands final : public TCommands<FElementusInventoryEditorCommands>
{
public:
	FElementusInventoryEditorCommands()
		: TCommands<FElementusInventoryEditorCommands>(TEXT("ElementusInventory"),
		                                               NSLOCTEXT("Contexts",
		                                                         "ElementusInventory",
		                                                         "ElementusInventoryEditor Plugin"),
		                                               NAME_None,
		                                               FElementusInventoryEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
