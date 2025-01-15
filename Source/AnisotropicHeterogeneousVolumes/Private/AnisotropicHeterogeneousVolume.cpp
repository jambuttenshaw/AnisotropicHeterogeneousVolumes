#include "AnisotropicHeterogeneousVolume.h"

#include "Components/BillboardComponent.h"


AAnisotropicHeterogeneousVolume::AAnisotropicHeterogeneousVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AnisotropicHeterogeneousVolumeComponent = CreateDefaultSubobject<UAnisotropicHeterogeneousVolumeComponent>(TEXT("AnisotropicHeterogeneousVolumeComponent"));
	RootComponent = AnisotropicHeterogeneousVolumeComponent;

#if WITH_EDITORONLY_DATA

	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			FName ID_HeterogeneousVolume;
			FText NAME_HeterogeneousVolume;
			FConstructorStatics()
				: ID_HeterogeneousVolume(TEXT("Fog"))
				, NAME_HeterogeneousVolume(NSLOCTEXT("SpriteCategory", "Fog", "Fog"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;
		if (GetSpriteComponent())
		{
			GetSpriteComponent()->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			GetSpriteComponent()->bHiddenInGame = true;
			GetSpriteComponent()->bIsScreenSizeScaled = true;
			GetSpriteComponent()->SpriteInfo.Category = ConstructorStatics.ID_HeterogeneousVolume;
			GetSpriteComponent()->SpriteInfo.DisplayName = ConstructorStatics.NAME_HeterogeneousVolume;
			GetSpriteComponent()->SetupAttachment(AnisotropicHeterogeneousVolumeComponent);
			GetSpriteComponent()->bReceivesDecals = false;
		}
	}
#endif // WITH_EDITORONLY_DATA

	PrimaryActorTick.bCanEverTick = true;
	SetHidden(false);
}
