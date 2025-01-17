#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"


struct FMiePlotImportOptions;


class SMiePlotImportWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMiePlotImportWindow) {}
		SLATE_ARGUMENT(FMiePlotImportOptions*, ImportOptions)
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

	void UpdateImportOptions();

	TOptional<float> GetClampSamplesMin() const;
	void SetClampSamplesMin(float val, ETextCommit::Type);

	TOptional<float> GetClampSamplesMax() const;
	void SetClampSamplesMax(float val, ETextCommit::Type);

private:
	FMiePlotImportOptions* ImportOptions = nullptr;

	TSharedPtr<SCheckBox> ConvertToMonochromeCheckBox;
	TSharedPtr<SCheckBox> ClampSamplesCheckBox;

	TSharedPtr<SNumericEntryBox<float>> ClampSamplesMinEntryBox;
	TSharedPtr<SNumericEntryBox<float>> ClampSamplesMaxEntryBox;

	TSharedPtr<SCheckBox> ApplyToAllCheckBox;
	TSharedPtr<SButton> ImportButton;

	TWeakPtr<SWindow> WidgetWindow;

	bool bShouldImport = false;
};