#pragma once

#include "CoreMinimal.h"


struct FMiePlotImportOptions
{
	bool bConvertToMonochrome = false;

	bool bClamp = false;
	float ClampMax = 1.0f;

	bool bReNormalize = false;
};
