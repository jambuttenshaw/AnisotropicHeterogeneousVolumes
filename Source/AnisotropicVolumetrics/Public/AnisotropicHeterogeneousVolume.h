#pragma once

#include "CoreMinimal.h"
#include "AnisotropicHeterogeneousVolumeComponent.h"
#include "AnisotropicHeterogeneousVolume.generated.h"

UCLASS(showcategories = (Movement, Rendering, Transformation, DataLayers, "Input|MouseInput", "Input|TouchInput"), ClassGroup = Fog, hidecategories = (Info, Object, Input))
class ANISOTROPICVOLUMETRICS_API AAnisotropicHeterogeneousVolume : public AInfo
{
	GENERATED_BODY()

private:
	AAnisotropicHeterogeneousVolume(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Volume, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnisotropicHeterogeneousVolumeComponent> AnisotropicHeterogeneousVolumeComponent;

#if WITH_EDITOR
	virtual bool ActorTypeSupportsDataLayer()  const override { return true; }
#endif

};
