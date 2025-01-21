#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"


class SPhaseFunctionWidget;
struct FMiePlotImportOptions;


class SMiePlotImportWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMiePlotImportWindow) {}
		SLATE_ARGUMENT(FMiePlotImportOptions*, ImportOptions)
		SLATE_ARGUMENT(TArray<FVector4f>*, pPhaseFunctionSamples)
		SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
		SLATE_ARGUMENT(FText, FullPath)
		SLATE_ARGUMENT(float, MaxWindowHeight)
		SLATE_ARGUMENT(float, MaxWindowWidth)
	SLATE_END_ARGS()

public:
	virtual bool SupportsKeyboardFocus() const override { return true; }
	void Construct(const FArguments& InArgs);

	FReply OnImport();
	FReply OnCancel();

	bool ShouldImport() const;

	// Whether all assets in the import should use these import options
	bool ShouldApplyImportOptionsToAllAssets() const;

private:
	EActiveTimerReturnType SetFocusPostConstruct(double InCurrentTime, float InDeltaTime);
	bool CanImport() const;

	void OnCheckedStateChanged(ECheckBoxState State);

	TOptional<float> GetClampMax() const;
	void SetClampMax(float val, ETextCommit::Type);

	void OnAnyImportOptionsChanged();

private:
	FMiePlotImportOptions* ImportOptions = nullptr;
	TArray<FVector4f>* pPhaseFunctionSamples = nullptr;		// Pointer to the original, un-modified phase function samples
	TArray<FVector4f> PhaseFunctionSamplesPreview;	// A preview of the phase function samples after they have gone through the import pipeline
	FVector4f PhaseFunctionMagnitude;

	TSharedPtr<SCheckBox> ConvertToMonochromeCheckBox;
	TSharedPtr<SCheckBox> ClampCheckBox;
	TSharedPtr<SCheckBox> ReNormalizeCheckBox;

	TSharedPtr<SNumericEntryBox<float>> ClampMaxEntryBox;

	TSharedPtr<STextBlock> MagnitudeRLabel;
	TSharedPtr<STextBlock> MagnitudeGLabel;
	TSharedPtr<STextBlock> MagnitudeBLabel;

	TSharedPtr<SCheckBox> ApplyToAllCheckBox;
	TSharedPtr<SButton> ImportButton;

	TSharedPtr<SPhaseFunctionWidget> PhaseFunctionWidget;

	TWeakPtr<SWindow> WidgetWindow;

	FNumberFormattingOptions NumberFormattingOptions;

	bool bShouldImport = false;
};