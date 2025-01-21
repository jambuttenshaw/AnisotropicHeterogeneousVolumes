// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

DECLARE_LOG_CATEGORY_EXTERN(LogMiePlotImporter, Log, All);

struct FMiePlotImportOptions;

class UDiscretePhaseFunction;


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
	static bool CreatePhaseFunctionLUT(const FString& FileName, const TArray<FVector4f>& PhaseFunctionSamples, UTexture2D** OutTexture);
	static bool CreatePhaseFunctionAsset(const FString& FileName, UDiscretePhaseFunction** OutPhaseFunction);

	static bool SaveAsset(UObject* Asset);

	// Helper functions
	static bool OpenFileDialogue(TArray<FString>& FilePaths,
		const FString& DialogTitle,
		const FString& DefaultPath,
		const FString& DefaultFile,
		const FString& FileType,
		bool IsMultiple);

	static void* GetWindowHandle();

	static bool ParseMiePlotData(const FString& Path, const FMiePlotImportOptions& ImportOptions, TArray<FVector4f>& OutPhaseFunctionSamples);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
