// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiePlotImporterStyle.h"
#include "MiePlotImporter.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FMiePlotImporterStyle::StyleInstance = nullptr;

void FMiePlotImporterStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMiePlotImporterStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FMiePlotImporterStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MiePlotImporterStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FMiePlotImporterStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MiePlotImporterStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("AnisotropicVolumetrics")->GetBaseDir() / TEXT("Resources"));

	Style->Set("MiePlotImporter.ImportPhaseFunctionLUT", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FMiePlotImporterStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMiePlotImporterStyle::Get()
{
	return *StyleInstance;
}
