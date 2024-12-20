// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

DECLARE_LOG_CATEGORY_EXTERN(LogMiePlotImporter, Log, All);


class FMiePlotImporterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();

private:
	// Main import functionality
	static void Import();

	// Helper functions
	static bool OpenFileDialogue(TArray<FString>& FilePaths,
		const FString& DialogTitle,
		const FString& DefaultPath,
		const FString& DefaultFile,
		const FString& FileType,
		bool IsMultiple);

	static void* GetWindowHandle();

	static bool ParseMiePlotData(const FString& Path, TArray<FVector4f>& OutPhaseFunctionSamples);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
