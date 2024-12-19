// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiePlotImporter.h"
#include "MiePlotImporterStyle.h"
#include "MiePlotImporterCommands.h"

#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName MiePlotImporterTabName("MiePlotImporter");

#define LOCTEXT_NAMESPACE "FMiePlotImporterModule"

void FMiePlotImporterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMiePlotImporterStyle::Initialize();
	FMiePlotImporterStyle::ReloadTextures();

	FMiePlotImporterCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMiePlotImporterCommands::Get().ImportPhaseFunctionLUT,
		FExecuteAction::CreateRaw(this, &FMiePlotImporterModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMiePlotImporterModule::RegisterMenus));
}

void FMiePlotImporterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMiePlotImporterStyle::Shutdown();

	FMiePlotImporterCommands::Unregister();
}

void FMiePlotImporterModule::PluginButtonClicked()
{
	Import();
}

void FMiePlotImporterModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMiePlotImporterCommands::Get().ImportPhaseFunctionLUT, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMiePlotImporterCommands::Get().ImportPhaseFunctionLUT));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}


void FMiePlotImporterModule::Import()
{
	// Get files to parse
	TArray<FString> Paths;
	if (!OpenFileDialogue(Paths, TEXT("Open MiePlot Data"), TEXT(""), TEXT(""), TEXT(".txt"), true))
	{
		// File import failed	
		return;
	}

	for (auto& Path : Paths)
	{
		// The array of samples to put into the LUT
		TArray<FVector> PhaseFunctionSamples;

		if (!ParseMiePlotData(Path, PhaseFunctionSamples))
		{
			// Error
			// Log error and continue to next file
			continue;
		}

		// Get destination file name

		// Create texture

		// Populate texture with sample data

	}

}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiePlotImporterModule, MiePlotImporter)