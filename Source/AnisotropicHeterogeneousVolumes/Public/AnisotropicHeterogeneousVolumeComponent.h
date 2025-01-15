// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PhaseFunction.h"

#include "Components/HeterogeneousVolumeComponent.h"
#include "AnisotropicHeterogeneousVolumeComponent.generated.h"


UENUM()
enum EHeterogeneousVolumePhaseFunctionMethod : int
{
	PhaseFunctionMethod_Isotropic = 0,
	PhaseFunctionMethod_Discrete = 1,
	PhaseFunctionMethod_HenyeyGreenstein = 2
};


/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class ANISOTROPICHETEROGENEOUSVOLUMES_API UAnisotropicHeterogeneousVolumeComponent : public UHeterogeneousVolumeComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Lighting)
	TEnumAsByte<enum EHeterogeneousVolumePhaseFunctionMethod> PhaseFunctionMethod;

	UPROPERTY(EditAnywhere, Category = Lighting, meta = (EditCondition = "PhaseFunctionMethod == PhaseFunctionMethod_Discrete"))
	TObjectPtr<UPhaseFunction> DiscretePhaseFunction;

	UPROPERTY(EditAnywhere, Category = Lighting,
		meta = (EditCondition = "PhaseFunctionMethod == PhaseFunctionMethod_HenyeyGreenstein", ClampMin = "-0.999", ClampMax = "0.999"))
	float HGAnisotropy;

public:

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

};
