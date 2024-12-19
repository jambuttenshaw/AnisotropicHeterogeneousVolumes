// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiePlotImporterCommands.h"

#define LOCTEXT_NAMESPACE "FMiePlotImporterModule"

void FMiePlotImporterCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MiePlotImporter", "Execute MiePlotImporter action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
