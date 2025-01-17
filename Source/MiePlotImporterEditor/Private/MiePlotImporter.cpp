// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiePlotImporter.h"

#include "MiePlotImporterStyle.h"
#include "MiePlotImporterCommands.h"

#include "MiePlotImportOptions.h"

#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "TextureCompiler.h"

#include "MiePlotImportWindow.h"
#include "DiscretePhaseFunction.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Interfaces/IMainFrameModule.h"


DEFINE_LOG_CATEGORY(LogMiePlotImporter);

#define LOCTEXT_NAMESPACE "FMiePlotImporterModule"


static bool ShowMiePlotImportWindow(const FString& Filename, bool& bApplyForAllAssets, FMiePlotImportOptions* ImportOptions)
{
	TSharedPtr<SWindow> ParentWindow;

	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
		ParentWindow = MainFrame.GetParentWindow();
	}

	// Compute centered window position based on max window size, which include when all categories are expanded
	constexpr float ImportWindowWidth = 450.0f;
	constexpr float ImportWindowHeight = 750.0f;
	FVector2D ImportWindowSize = FVector2D(ImportWindowWidth, ImportWindowHeight); // Max window size it can get based on current slate


	FSlateRect WorkAreaRect = FSlateApplicationBase::Get().GetPreferredWorkArea();
	FVector2D DisplayTopLeft(WorkAreaRect.Left, WorkAreaRect.Top);
	FVector2D DisplaySize(WorkAreaRect.Right - WorkAreaRect.Left, WorkAreaRect.Bottom - WorkAreaRect.Top);

	float ScaleFactor = FPlatformApplicationMisc::GetDPIScaleFactorAtPoint(DisplayTopLeft.X, DisplayTopLeft.Y);
	ImportWindowSize *= ScaleFactor;

	FVector2D WindowPosition = (DisplayTopLeft + (DisplaySize - ImportWindowSize) / 2.0f) / ScaleFactor;

	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("MiePlotImportOptionsTitle", "MiePlot Import Options"))
		.SizingRule(ESizingRule::Autosized)
		.AutoCenter(EAutoCenter::None)
		.ClientSize(ImportWindowSize)
		.ScreenPosition(WindowPosition);

	TSharedPtr<SMiePlotImportWindow> MiePlotImportWindow;
	Window->SetContent(
		SAssignNew(MiePlotImportWindow, SMiePlotImportWindow)
		.ImportOptions(ImportOptions)
		.WidgetWindow(Window)
		.FullPath(FText::FromString(Filename))
		.MaxWindowHeight(ImportWindowWidth)
		.MaxWindowWidth(ImportWindowHeight)
	);

	FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);

	bApplyForAllAssets = MiePlotImportWindow->ShouldApplyImportOptionsToAllAssets();

	return MiePlotImportWindow->ShouldImport();
}

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
		return;
	}

	bool bApplyImportOptionsToAll = false;
	FMiePlotImportOptions ImportOptions;

	for (auto& Path : Paths)
	{
		if (!bApplyImportOptionsToAll)
		{
			if (!ShowMiePlotImportWindow(Path, bApplyImportOptionsToAll , &ImportOptions))
			{
				continue;
			}
		}

		FString FileName = FPaths::GetBaseFilename(Path);

		// The array of samples to put into the LUT
		TArray<FVector4f> PhaseFunctionSamples;

		if (!ParseMiePlotData(Path, ImportOptions, PhaseFunctionSamples))
		{
			// Error
			// Log error and continue to next file
			UE_LOG(LogMiePlotImporter, Error, TEXT("Failed to parse '%s'"), *FileName);
			continue;
		}

		UTexture2D* Texture;
		if (!CreatePhaseFunctionLUT(FileName, PhaseFunctionSamples, &Texture))
		{
			continue;
		}
		
		UDiscretePhaseFunction* PhaseFunction;
		if (!CreatePhaseFunctionAsset(FileName, &PhaseFunction))
		{
			continue;
		}

		PhaseFunction->LUT = Texture;
		ExtractZonalHarmonics(PhaseFunctionSamples, PhaseFunction->ZonalHarmonics);

		SaveAsset(Texture);
		SaveAsset(PhaseFunction);
	}
}

bool FMiePlotImporterModule::CreatePhaseFunctionLUT(const FString& FileName, const TArray<FVector4f>& PhaseFunctionSamples, UTexture2D** OutTexture)
{
	// Dimensions of textures
	const int32 Width = PhaseFunctionSamples.Num();
	constexpr int32 Height = 1;
	constexpr EPixelFormat PixelFormat = EPixelFormat::PF_A32B32G32R32F;
	constexpr ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_RGBA32F;

	// Create Texture Package
	FString AssetName, PackageName;
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName("/Game/MiePlot/", FileName, PackageName, AssetName);

	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogMiePlotImporter, Error, TEXT("Failed to create package '%s'"), *PackageName);
		return false;
	}

	// Create the Texture
	UTexture2D* Texture = NewObject<UTexture2D>(Package, UTexture2D::StaticClass(), FName(*AssetName), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	Texture->AddToRoot();

	// Texture Settings
	FTexturePlatformData* PlatformData = new FTexturePlatformData;
	PlatformData->SizeX = Width;
	PlatformData->SizeY = Height;
	PlatformData->PixelFormat = PixelFormat;

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new FTexture2DMipMap;
	Mip->SizeX = Width;
	Mip->SizeY = Height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);

	// 4 floats per element
	const int64 DataSize = Width * Height * sizeof(float) * 4;
	float* TextureData = reinterpret_cast<float*>(Mip->BulkData.Realloc(DataSize));
	FMemory::Memcpy(TextureData, PhaseFunctionSamples.GetData(), DataSize);

	Mip->BulkData.Unlock();
	PlatformData->Mips.Add(Mip);
	Texture->SetPlatformData(PlatformData);

	Texture->SRGB = 0;
	Texture->CompressionSettings = TC_HDR;
	Texture->AddressX = TA_Clamp;
	Texture->AddressY = TA_Clamp;
	Texture->Source.Init(Width, Height, 1, 1, SourceFormat, reinterpret_cast<const uint8*>(TextureData));

	// Updating Texture & mark it as unsaved
	Texture->UpdateResource();

	(void)Package->MarkPackageDirty();

	FAssetRegistryModule::AssetCreated(Texture);
	Package->FullyLoad();
	FTextureCompilingManager::Get().FinishCompilation({ Texture });

	*OutTexture = Texture;
	return true;
}


bool FMiePlotImporterModule::CreatePhaseFunctionAsset(const FString& FileName, UDiscretePhaseFunction** OutPhaseFunction)
{
	FString FileNameWithSuffix = FileName + TEXT("_PhaseFunction");

	// Create Texture Package
	FString AssetName, PackageName;
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName("/Game/MiePlot/", FileNameWithSuffix, PackageName, AssetName);

	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogMiePlotImporter, Error, TEXT("Failed to create package '%s'"), *PackageName);
		return false;
	}

	// Create the Texture
	UDiscretePhaseFunction* PhaseFunction = NewObject<UDiscretePhaseFunction>(Package, UDiscretePhaseFunction::StaticClass(), FName(*AssetName), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	PhaseFunction->AddToRoot();

	(void)Package->MarkPackageDirty();

	FAssetRegistryModule::AssetCreated(PhaseFunction);
	Package->FullyLoad();

	*OutPhaseFunction = PhaseFunction;
	return true;
}

bool FMiePlotImporterModule::SaveAsset(UObject* Asset)
{
	UPackage* Package = Asset->GetPackage();
	FString PackageName = Package->GetName();

	FSavePackageArgs Args;
	Args.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	FSavePackageResultStruct SaveResult = UPackage::Save(
		Package, Asset, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()), Args);
	if (!SaveResult.IsSuccessful())
	{
		UE_LOG(LogMiePlotImporter, Error, TEXT("Failed to save package '%s'"), *PackageName);
		return false;
	}

	return true;
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiePlotImporterModule, MiePlotImporter)