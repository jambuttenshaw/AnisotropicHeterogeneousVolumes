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
		FMiePlotImporterCommands::Get().PluginAction,
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
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FMiePlotImporterModule::PluginButtonClicked()")),
							FText::FromString(TEXT("MiePlotImporter.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FMiePlotImporterModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMiePlotImporterCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMiePlotImporterCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiePlotImporterModule, MiePlotImporter)