// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiePlotImporter.h"

#include "MiePlotImporterStyle.h"
#include "MiePlotImporterCommands.h"


#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "TextureCompiler.h"


static const FName MiePlotImporterTabName("MiePlotImporter");

DEFINE_LOG_CATEGORY(LogMiePlotImporter);

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
		// File open failed
		UE_LOG(LogMiePlotImporter, Error, TEXT("Failed to open file(s) to import."));
		return;
	}

	for (auto& Path : Paths)
	{
		FString FileName = FPaths::GetBaseFilename(Path);

		// The array of samples to put into the LUT
		TArray<FVector4f> PhaseFunctionSamples;

		if (!ParseMiePlotData(Path, PhaseFunctionSamples))
		{
			// Error
			// Log error and continue to next file
			UE_LOG(LogMiePlotImporter, Error, TEXT("Failed to parse '%s'"), *FileName);
			continue;
		}

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
			continue;
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
		Texture->Source.Init(Width, Height, 1, 1, SourceFormat, reinterpret_cast<const uint8*>(TextureData));

		// Updating Texture & mark it as unsaved
		Texture->UpdateResource();

		(void)Package->MarkPackageDirty();

		FAssetRegistryModule::AssetCreated(Texture);
		Package->FullyLoad();
		FTextureCompilingManager::Get().FinishCompilation({ Texture });

		// Save asset
		FSavePackageArgs Args;
		Args.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
		FSavePackageResultStruct SaveResult = UPackage::Save(
			Package, Texture, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()), Args);
		if (!SaveResult.IsSuccessful())
		{
			UE_LOG(LogMiePlotImporter, Error, TEXT("Failed to save package '%s'"), *PackageName);
			continue;
		}
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiePlotImporterModule, MiePlotImporter)