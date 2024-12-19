// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiePlotImporterCommands.h"

#define LOCTEXT_NAMESPACE "FMiePlotImporterModule"

void FMiePlotImporterCommands::RegisterCommands()
{
	UI_COMMAND(ImportPhaseFunctionLUT, "MiePlotImporter", "Import MiePlot Phase Function as a LUT", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
