// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryEditorCommands.h"

#define LOCTEXT_NAMESPACE "FElementusInventoryEditorModule"

void FElementusInventoryEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow,
	           "ElementusInventory",
	           "Open Elementus Inventory Window",
	           EUserInterfaceActionType::Button,
	           FInputChord());
}

#undef LOCTEXT_NAMESPACE
