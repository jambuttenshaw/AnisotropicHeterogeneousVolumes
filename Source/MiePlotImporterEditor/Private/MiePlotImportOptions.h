#pragma once

#include "CoreMinimal.h"


struct FMiePlotImportOptions
{
	bool bConvertToMonochrome = false;

	bool bClampPhaseSamples = false;
	float PhaseSampleClampMin = 0.0f;
	float PhaseSampleClampMax = 1.0f;
};
