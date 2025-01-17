#pragma once

#include "CoreMinimal.h"

#include "AnisotropicCommon.generated.h"


UENUM()
enum class EPhaseFunctionMethod : uint8
{
	Isotropic = 0,
	Discrete = 1,
	HenyeyGreenstein = 2
};
