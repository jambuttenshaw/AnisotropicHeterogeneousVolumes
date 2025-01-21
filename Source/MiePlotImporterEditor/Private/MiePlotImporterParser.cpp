// Fill out your copyright notice in the Description page of Project Settings.

#include "MiePlotImporter.h"
#include "Misc/DefaultValueHelper.h"


bool FMiePlotImporterModule::ParseMiePlotData(const FString& Path, TArray<FVector4f>& OutPhaseFunctionSamples)
{
	// Load and parse data
	TArray<FString> LoadedFile;
	if (!FFileHelper::LoadFileToStringArray(LoadedFile, *Path))
	{
		return false;
	}

	// The current line in the file
	int32 Cursor = 0;

	// TODO: Parse header for number of channels and other validation

	// Skip through file until reaching data table
	while (!LoadedFile[++Cursor].StartsWith("Angle"))
	{
		if (Cursor == LoadedFile.Num())
		{
			// Malformed data file
			return false;
		}
	}

	// Cursor points at table header - Optionally parse header


	// Parse data body
	// Format is:
	// Angle   Wavelength   Perpendicular   Parallel

	uint32 LineCounter = 0;
	OutPhaseFunctionSamples.Reserve(2000);

	// Parse individual lines
	while (++Cursor < LoadedFile.Num() && LoadedFile[Cursor][0] != '*')
	{
		// Create copy of line that we can process
		FString Line = LoadedFile[Cursor];
		Line.ConvertTabsToSpacesInline(1);

		// Gets the next float from input string
		// Floats should be separated by spaces
		auto ParseNextFloat = [](FString& Str, float& OutValue) -> bool
			{
				// Remove any leading spaces
				while (Str.RemoveFromStart(" ")) {}

				// Find the next space in the string
				int32 Index = Str.Len();
				// OK if no space is found - it will just use the entire string
				Str.FindChar(' ', Index);

				if (!FDefaultValueHelper::ParseFloat(Str.LeftChop(Str.Len() - Index), OutValue))
				{
					return false;
				}
				Str.RightChopInline(Index);

				return true;
			};


		float Angle, Wavelength, PerpendicularPhase, ParallelPhase;
		bool Success = true;

		Success &= ParseNextFloat(Line, Angle);
		Success &= ParseNextFloat(Line, Wavelength);
		Success &= ParseNextFloat(Line, PerpendicularPhase);
		Success &= ParseNextFloat(Line, ParallelPhase);

		if (!Success)
		{
			// Failed to parse line
			return false;
		}

		float UnPolarizedPhase = 0.5f * (PerpendicularPhase + ParallelPhase);

		// 3 components per sample - R then G then B across 3 lines
		// Create a new sample every third line
		switch (LineCounter % 3)
		{
		case 0:
			OutPhaseFunctionSamples.Emplace(UnPolarizedPhase, 0.0f, 0.0f, 0.0f);
			break;
		case 1:
			OutPhaseFunctionSamples.Last().Y = UnPolarizedPhase;
			break;
		case 2:
			OutPhaseFunctionSamples.Last().Z = UnPolarizedPhase;
			break;
		default:
			// Maths has broken
			break;
		}

		LineCounter++;
	}

	if (LineCounter % 3 != 0)
	{
		// Not a multiple of 3 lines in the data file - that is wrong
		return false;
	}

	return true;
}
