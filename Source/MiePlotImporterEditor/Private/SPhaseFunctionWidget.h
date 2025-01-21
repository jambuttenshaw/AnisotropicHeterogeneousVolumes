#pragma once

#include "CoreMinimal.h"

#include "Widgets/SLeafWidget.h"


struct FMiePlotImportOptions;


class SPhaseFunctionWidget : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SPhaseFunctionWidget) {}
		SLATE_ARGUMENT(FMiePlotImportOptions*, ImportOptions)
		SLATE_ARGUMENT(TArray<FVector4f>*, PhaseFunctionSamples)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;

	// Since we are pointing to somebody else's phase samples,
	// we need to be informed when the samples have changed so that we can
	// make sure the axes are scaled correctly
	void RescaleAxes();

private:
	FTransform2D GetPointsTransform(const FGeometry& AllottedGeometry) const;

private:
	FMiePlotImportOptions* ImportOptions = nullptr;
	TArray<FVector4f>* pPhaseFunctionSamples = nullptr;

	FVector4f LogMinSample;
	FVector4f LogMaxSample;
};
