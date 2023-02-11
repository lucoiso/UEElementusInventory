// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include <Modules/ModuleInterface.h>

/**
 *
 */

class FElementusInventoryModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};