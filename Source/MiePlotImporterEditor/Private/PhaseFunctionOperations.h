#pragma once

#include "CoreMinimal.h"


struct FMiePlotImportOptions;

class FPhaseFunctionOperations
{
	FPhaseFunctionOperations() = delete;
public:

	static FVector4f SamplePhaseFunction(const TArray<FVector4f>& PhaseFunctionSamples, float CosTheta);

	static void GetMagnitude(const TArray<FVector4f>& PhaseFunctionSamples, FVector4f& OutMagnitude);
	static void Normalize(TArray<FVector4f>& PhaseFunctionSamples);
	static void Clamp(TArray<FVector4f>& PhaseFunctionSamples, float Max);
	static void ConvertToMonochrome(TArray<FVector4f>& PhaseFunctionSamples);

	static void ExtractZonalHarmonics(const TArray<FVector4f>& PhaseFunctionSamples, FVector2f& OutZonalHarmonics);


	static void ApplyImportOptions(TArray<FVector4f>& PhaseFunctionSamples, const FMiePlotImportOptions& ImportOptions);

	// For creating mip chains of LUTs where each subsequent mip is more isotropic
	// This is preferable to creating a 2D LUT because it uses substantially less data
	static void GenerateNextMip(const TArray<FVector4f>& InPhaseFunctionSamples, TArray<FVector4f>& OutPhaseFunctionSamples, int32 KernelWidthInTexels);

};

