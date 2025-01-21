#include "MiePlotImportWindow.h"

#include "SPrimaryButton.h"

#include "MiePlotImportOptions.h"
#include "PhaseFunctionOperations.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "SPhaseFunctionWidget.h"


#define LOCTEXT_NAMESPACE "SMiePlotImportWindow"


void SMiePlotImportWindow::Construct(const FArguments& InArgs)
{
	ImportOptions = InArgs._ImportOptions;
	pPhaseFunctionSamples = InArgs._pPhaseFunctionSamples;
	WidgetWindow = InArgs._WidgetWindow;

	NumberFormattingOptions
		.SetMinimumIntegralDigits(1)
		.SetMaximumIntegralDigits(1)
		.SetMinimumFractionalDigits(3)
		.SetMaximumFractionalDigits(3);

	// Create copy of the phase function samples to preview
	PhaseFunctionSamplesPreview = *pPhaseFunctionSamples;
	FPhaseFunctionOperations::ApplyImportOptions(PhaseFunctionSamplesPreview, *ImportOptions);

	FPhaseFunctionOperations::GetMagnitude(PhaseFunctionSamplesPreview, PhaseFunctionMagnitude);

	TSharedPtr<SBox> InspectorBox;

	this->ChildSlot
		[
			SNew(SBox)
			.MaxDesiredHeight(InArgs._MaxWindowHeight)
			.MaxDesiredWidth(InArgs._MaxWindowWidth)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SNew(SBorder)
						.Padding(FMargin(3))
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(STextBlock)
										.Text(LOCTEXT("Import_CurrentFileTitle", "Current Asset: "))
								]
								+ SHorizontalBox::Slot()
								.Padding(5, 0, 0, 0)
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
										.Text(InArgs._FullPath)
										.ToolTipText(InArgs._FullPath)
								]
						]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						.AutoWidth()
						.Padding(2)
						[
							SNew(SBorder)
								.Padding(FMargin(3))
								.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
								[
									SAssignNew(PhaseFunctionWidget, SPhaseFunctionWidget)
									.ImportOptions(ImportOptions)
									.PhaseFunctionSamples(&PhaseFunctionSamplesPreview)
								]
						]
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Right)
						.AutoWidth()
						.Padding(2)
						[
							SAssignNew(InspectorBox, SBox)
								//.MinDesiredHeight(200.0f)
								//.MaxDesiredHeight(650.0f)
								//.WidthOverride(400.0f)
						]
					
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Right)
					.AutoWidth()
					.Padding(2)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("UseSettingsForAll_Label", "Apply for all assets"))
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Right)
					.AutoWidth()
					.Padding(2)
					[
						SAssignNew(ApplyToAllCheckBox, SCheckBox)
						.IsChecked(false)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(2)
					+ SUniformGridPanel::Slot(1, 0)
					[
						SAssignNew(ImportButton, SPrimaryButton)
						.Text(LOCTEXT("MiePlotImportWindow_Import", "Import"))
						.IsEnabled(this, &SMiePlotImportWindow::CanImport)
						.OnClicked(this, &SMiePlotImportWindow::OnImport)
					]
					+ SUniformGridPanel::Slot(2, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("MiePlotImportWindow_Cancel", "Cancel"))
						.OnClicked(this, &SMiePlotImportWindow::OnCancel)
					]
				]
				
			]
		];

	InspectorBox->SetContent(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.FillWidth(1.0f)
			.Padding(2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ConvertToMonochromeCheckBoxLabel", "Convert To Monochrome"))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(2)
			[
				SAssignNew(ConvertToMonochromeCheckBox, SCheckBox)
				.IsChecked(ImportOptions->bConvertToMonochrome)
				.OnCheckStateChanged(this, &SMiePlotImportWindow::OnCheckedStateChanged)
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(2)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.FillWidth(1.0f)
				.Padding(2)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ClampSamplesCheckBoxLabel", "Clamp Phase Samples"))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.AutoWidth()
				.Padding(2)
				[
					SAssignNew(ClampCheckBox, SCheckBox)
					.IsChecked(ImportOptions->bClamp)
					.OnCheckStateChanged(this, &SMiePlotImportWindow::OnCheckedStateChanged)
				]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(2)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.FillWidth(1.0f)
				.Padding(2)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ClampSamplesMaxEntryBoxLabel", "Clamp Max"))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.AutoWidth()
				.Padding(2)
				[
					SAssignNew(ClampMaxEntryBox, SNumericEntryBox<float>)
					.Value(this, &SMiePlotImportWindow::GetClampMax)
					.OnValueCommitted(this, &SMiePlotImportWindow::SetClampMax)
				]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.FillWidth(1.0f)
			.Padding(2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ReNormalizeCheckBoxLabel", "Re-Normalize"))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(2)
			[
				SAssignNew(ReNormalizeCheckBox, SCheckBox)
				.IsChecked(ImportOptions->bReNormalize)
				.OnCheckStateChanged(this, &SMiePlotImportWindow::OnCheckedStateChanged)
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.VAlign(VAlign_Bottom)
		.Padding(2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.FillWidth(1.0f)
			.Padding(2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("MagnitudeLabel", "Magnitude"))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(2)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.Padding(2)
				[
					SAssignNew(MagnitudeRLabel, STextBlock)
					.Text(FText::AsNumber(PhaseFunctionMagnitude.X, &NumberFormattingOptions))
				]
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.Padding(2)
				[
					SAssignNew(MagnitudeGLabel, STextBlock)
					.Text(FText::AsNumber(PhaseFunctionMagnitude.Y, &NumberFormattingOptions))
				]
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.Padding(2)
				[
					SAssignNew(MagnitudeBLabel, STextBlock)
					.Text(FText::AsNumber(PhaseFunctionMagnitude.Z, &NumberFormattingOptions))
				]
			]
		]
	);

	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SMiePlotImportWindow::SetFocusPostConstruct));
}

FReply SMiePlotImportWindow::OnImport()
{
	bShouldImport = true;
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}

FReply SMiePlotImportWindow::OnCancel()
{
	bShouldImport = false;
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}

bool SMiePlotImportWindow::ShouldImport() const
{
	return bShouldImport;
}

bool SMiePlotImportWindow::ShouldApplyImportOptionsToAllAssets() const
{
	return ApplyToAllCheckBox->IsChecked();
}


EActiveTimerReturnType SMiePlotImportWindow::SetFocusPostConstruct(double InCurrentTime, float InDeltaTime)
{
	if (ImportButton.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(ImportButton, EFocusCause::SetDirectly);
	}

	return EActiveTimerReturnType::Stop;
}

bool SMiePlotImportWindow::CanImport() const
{
	return true;
}

void SMiePlotImportWindow::OnCheckedStateChanged(ECheckBoxState State)
{
	OnAnyImportOptionsChanged();
}

void SMiePlotImportWindow::OnAnyImportOptionsChanged()
{
	ImportOptions->bConvertToMonochrome = ConvertToMonochromeCheckBox->IsChecked();
	ImportOptions->bClamp = ClampCheckBox->IsChecked();
	ImportOptions->bReNormalize = ReNormalizeCheckBox->IsChecked();

	// Re-process samples
	PhaseFunctionSamplesPreview = *pPhaseFunctionSamples;
	FPhaseFunctionOperations::ApplyImportOptions(PhaseFunctionSamplesPreview, *ImportOptions);

	PhaseFunctionWidget->RescaleAxes();

	FPhaseFunctionOperations::GetMagnitude(PhaseFunctionSamplesPreview, PhaseFunctionMagnitude);
	MagnitudeRLabel->SetText(FText::AsNumber(PhaseFunctionMagnitude.X, &NumberFormattingOptions));
	MagnitudeGLabel->SetText(FText::AsNumber(PhaseFunctionMagnitude.Y, &NumberFormattingOptions));
	MagnitudeBLabel->SetText(FText::AsNumber(PhaseFunctionMagnitude.Z, &NumberFormattingOptions));
}


TOptional<float> SMiePlotImportWindow::GetClampMax() const
{
	return ImportOptions->ClampMax;
}

void SMiePlotImportWindow::SetClampMax(float val, ETextCommit::Type)
{
	ImportOptions->ClampMax = val;
	OnAnyImportOptionsChanged();
}
