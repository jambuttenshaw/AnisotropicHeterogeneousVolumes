// Fill out your copyright notice in the Description page of Project Settings.


#include "MiePlotImporter.h"

#include "Misc/Paths.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Interfaces/IMainFrameModule.h"


bool FMiePlotImporterModule::OpenFileDialogue(TArray<FString>& FilePaths, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileType, bool IsMultiple)
{
	if (void* WindowHandle = GetWindowHandle())
	{
		if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
		{
			bool bResult = DesktopPlatform->OpenFileDialog(
				WindowHandle,
				DialogTitle,
				DefaultPath,
				DefaultFile,
				FileType,
				(IsMultiple ? EFileDialogFlags::Type::Multiple : EFileDialogFlags::Type::None),
				FilePaths
			);

			if (bResult)
			{
				for (auto& FilePath : FilePaths)
				{
					FilePath = FPaths::ConvertRelativePathToFull(FilePath);
				}

				return true;
			}
		}
	}
	return false;
}


void* FMiePlotImporterModule::GetWindowHandle()
{
	if (GIsEditor)
	{
		IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
		TSharedPtr<SWindow> MainWindow = MainFrameModule.GetParentWindow();

		if (MainWindow.IsValid() && MainWindow->GetNativeWindow().IsValid())
		{
			return MainWindow->GetNativeWindow()->GetOSWindowHandle();
		}
	}
	else
	{
		// Unsure why this would ever be called in game...
		if (GEngine && GEngine->GameViewport)
		{
			return GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		}
	}

	return nullptr;
}
