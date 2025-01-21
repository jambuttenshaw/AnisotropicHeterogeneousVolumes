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
	static void Clamp(TArray<FVector4f>& PhaseFunctionSamples, float Min, float Max);
	static void ConvertToMonochrome(TArray<FVector4f>& PhaseFunctionSamples);

	static void ExtractZonalHarmonics(const TArray<FVector4f>& PhaseFunctionSamples, FVector2f& OutZonalHarmonics);


	static void ApplyImportOptions(TArray<FVector4f>& PhaseFunctionSamples, const FMiePlotImportOptions& ImportOptions);

};

