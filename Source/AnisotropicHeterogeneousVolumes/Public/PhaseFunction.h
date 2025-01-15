#pragma once


#include "CoreMinimal.h"

#include "PhaseFunction.generated.h"


/*
 *	A data asset that encapsulates all data required to use custom discrete phase functions for anisotropic heterogeneous volume rendering
 */
UCLASS(BlueprintType)
class ANISOTROPICHETEROGENEOUSVOLUMES_API UPhaseFunction : public UObject
{
	GENERATED_BODY()
private:
	UPhaseFunction(const FObjectInitializer&);

public:
	/*
	 *	1D Look-up table of phase function values. This is used to evaluate the phase function for direct illumination
	 */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture> LUT;

	/*
	 *	Two-band zonal harmonic representation of the same phase function to use with indirect illumination.
	 *	Lumen only provides two-band spherical harmonics, therefore there is no point calculating and storing higher-order representations of the LUT.
	 */
	UPROPERTY(EditAnywhere)
	FVector2f ZonalHarmonics;
};
