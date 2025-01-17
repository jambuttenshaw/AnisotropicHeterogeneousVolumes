// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DiscretePhaseFunction.h"

#include "AnisotropicCommon.h"
#include "Components/HeterogeneousVolumeComponent.h"

#include "AnisotropicHeterogeneousVolumeComponent.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class ANISOTROPICVOLUMETRICS_API UAnisotropicHeterogeneousVolumeComponent : public UHeterogeneousVolumeComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Lighting)
	EPhaseFunctionMethod PhaseFunctionMethod;

	UPROPERTY(EditAnywhere, Category = Lighting, meta = (EditCondition = "PhaseFunctionMethod == EPhaseFunctionMethod::Discrete"))
	TObjectPtr<UDiscretePhaseFunction> DiscretePhaseFunction;

	UPROPERTY(EditAnywhere, Category = Lighting,
		meta = (EditCondition = "PhaseFunctionMethod == EPhaseFunctionMethod::HenyeyGreenstein", ClampMin = "-0.999", ClampMax = "0.999"))
	float HGAnisotropy;

public:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

};
