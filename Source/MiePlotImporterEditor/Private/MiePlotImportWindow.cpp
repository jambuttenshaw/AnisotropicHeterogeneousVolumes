#include "MiePlotImportWindow.h"

#include "SPrimaryButton.h"

#include "MiePlotImportOptions.h"
#include "Widgets/Layout/SUniformGridPanel.h"


#define LOCTEXT_NAMESPACE "SMiePlotImportWindow"


void SMiePlotImportWindow::Construct(const FArguments& InArgs)
{
	ImportOptions = InArgs._ImportOptions;
	WidgetWindow = InArgs._WidgetWindow;

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
					SAssignNew(InspectorBox, SBox)
					.MinDesiredHeight(200.0f)
					.MaxDesiredHeight(650.0f)
					.WidthOverride(400.0f)
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
					.Text(LOCTEXT("ClampSamplesCheckBoxLabel", "Clamp Phase Samples"))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.AutoWidth()
				.Padding(2)
				[
					SAssignNew(ClampSamplesCheckBox, SCheckBox)
					.IsChecked(ImportOptions->bClampPhaseSamples)
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
					.Text(LOCTEXT("ClampSamplesMinEntryBoxLabel", "Clamp Phase Samples Min"))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.AutoWidth()
				.Padding(2)
				[
					SAssignNew(ClampSamplesMinEntryBox, SNumericEntryBox<float>)
					.Value(this, &SMiePlotImportWindow::GetClampSamplesMin)
					.OnValueCommitted(this, &SMiePlotImportWindow::SetClampSamplesMin)
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
					.Text(LOCTEXT("ClampSamplesMaxEntryBoxLabel", "Clamp Phase Samples Max"))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.AutoWidth()
				.Padding(2)
				[
					SAssignNew(ClampSamplesMaxEntryBox, SNumericEntryBox<float>)
					.Value(this, &SMiePlotImportWindow::GetClampSamplesMax)
					.OnValueCommitted(this, &SMiePlotImportWindow::SetClampSamplesMax)
				]
		]
	);

	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SMiePlotImportWindow::SetFocusPostConstruct));
}

FReply SMiePlotImportWindow::OnImport()
{
	bShouldImport = true;
	UpdateImportOptions();

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


void SMiePlotImportWindow::UpdateImportOptions()
{
	ImportOptions->bConvertToMonochrome = ConvertToMonochromeCheckBox->IsChecked();
	ImportOptions->bClampPhaseSamples = ClampSamplesCheckBox->IsChecked();
}


TOptional<float> SMiePlotImportWindow::GetClampSamplesMin() const
{
	return ImportOptions->PhaseSampleClampMin;
}

TOptional<float> SMiePlotImportWindow::GetClampSamplesMax() const
{
	return ImportOptions->PhaseSampleClampMax;
}

void SMiePlotImportWindow::SetClampSamplesMin(float val, ETextCommit::Type)
{
	ImportOptions->PhaseSampleClampMin = val;
}

void SMiePlotImportWindow::SetClampSamplesMax(float val, ETextCommit::Type)
{
	ImportOptions->PhaseSampleClampMax = val;
}
