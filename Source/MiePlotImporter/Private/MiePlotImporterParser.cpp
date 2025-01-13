// Fill out your copyright notice in the Description page of Project Settings.

#include "MiePlotImporter.h"
#include "MiePlotImportOptions.h"
#include "Misc/DefaultValueHelper.h"
#include "Sampling/VectorSetAnalysis.h"


bool FMiePlotImporterModule::ParseMiePlotData(const FString& Path, const FMiePlotImportOptions& ImportOptions, TArray<FVector4f>& OutPhaseFunctionSamples)
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
			if (ImportOptions.bConvertToMonochrome)
			{
				// average RGB components to form a monochrome phase value
				auto& sample = OutPhaseFunctionSamples.Last();
				float average = (sample.X + sample.Y + sample.Z) / 3.0f;
				OutPhaseFunctionSamples.Last() = FVector4f(average, average, average, 1.0f);
			}
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

	// Normalize phase function by numerically integrating over the surface of the sphere

	auto SamplePhaseLUT = [&](const FVector3f& Direction)-> FVector4f
		{
			const float cosTheta = FMath::Clamp(FVector3f::DotProduct(Direction, FVector3f(0.0f, 0.0f, 1.0f)), -1.0f, 1.0f);
			const float theta = FMath::Acos(cosTheta);

			float uv = theta / PI;
			float tc = uv * static_cast<float>(OutPhaseFunctionSamples.Num() - 1);

			return FMath::Lerp(
				OutPhaseFunctionSamples[FMath::FloorToInt(tc)],
				OutPhaseFunctionSamples[FMath::CeilToInt(tc)],
				FMath::Frac(tc)
			);
		};

	constexpr float UniformSpherePDF = 1.0f / (4.0f * PI);

	// Perform Monte Carlo integration
	constexpr uint32 NumSamples = 1'000'000;
	FVector4f Accumulator{ 0.0f, 0.0f, 0.0f };

	for (uint32 Sample = 0; Sample < NumSamples; Sample++)
	{
		FVector2D UV = { FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f) };

		// Map UV onto surface of sphere
		float CosTheta = 2.0f * UV.X - 1.0f;
		float Phi = 2.0f * PI * UV.Y;
		float SinTheta = CosTheta >= 1 ? 0 : FMath::Sqrt(1.0f - CosTheta * CosTheta);
		float SinPhi = FMath::Sin(Phi);
		float CosPhi = FMath::Cos(Phi);

		FVector4f PhaseSample = SamplePhaseLUT({ SinTheta * CosPhi, CosTheta, SinTheta * SinPhi });
		Accumulator += PhaseSample / UniformSpherePDF;
	}
	Accumulator /= static_cast<float>(NumSamples);

	// Normalize the data
	for (auto& PhaseSample : OutPhaseFunctionSamples)
	{
		PhaseSample /= Accumulator;
	}

	return true;
}
