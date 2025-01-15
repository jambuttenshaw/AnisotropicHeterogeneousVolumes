// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "MiePlotImporterStyle.h"

class FMiePlotImporterCommands : public TCommands<FMiePlotImporterCommands>
{
public:

	FMiePlotImporterCommands()
		: TCommands<FMiePlotImporterCommands>(TEXT("MiePlotImporter"), NSLOCTEXT("Contexts", "MiePlotImporter", "MiePlotImporter Plugin"), NAME_None, FMiePlotImporterStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > ImportPhaseFunctionLUT;
};
