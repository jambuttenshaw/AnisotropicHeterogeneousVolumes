#include "SPhaseFunctionWidget.h"
#include "Editor.h"

#include "MiePlotImportOptions.h"
#include "PhaseFunctionOperations.h"


void SPhaseFunctionWidget::Construct(const FArguments& InArgs)
{
	ImportOptions = InArgs._ImportOptions;
    pPhaseFunctionSamples = InArgs._PhaseFunctionSamples;

    RescaleAxes();
}


int32 SPhaseFunctionWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    if (pPhaseFunctionSamples == nullptr)
        return LayerId;

    const FTransform2D PointsTransform = GetPointsTransform(AllottedGeometry);
    TArray<FVector2D> Points;

    constexpr FLinearColor DarkGrey{ 0.08f, 0.08f, 0.08f };

    // Create circles to show that it is a polar plot
    {
        constexpr int32 NumPoints = 180;
        Points.Reserve(NumPoints);

        constexpr int32 NumRings = 4;
        for (int32 ring = 0; ring < NumRings; ring++)
        {
            float r = 0.5f * static_cast<float>(ring + 1) / static_cast<float>(NumRings);

	        for (int i = 0; i < NumPoints; i++)
	        {
                float Theta = static_cast<float>(i) / static_cast<float>(NumPoints - 1) * TWO_PI;

                FVector2D Offset{ 0.5f, 0.5f };
                Points.Add(PointsTransform.TransformPoint(Offset + FVector2D(r * FMath::Cos(Theta), r * FMath::Sin(Theta))));
	        }

            FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, DarkGrey);
            Points.Empty();
        }
    }

    // Create radial lines at equal intervals
    {
        constexpr int32 NumRadialLines = 12;
        // 2 points per line
        Points.Reserve(2 * NumRadialLines);

        for (int32 i = 0; i < NumRadialLines; i++)
        {
            float Theta = static_cast<float>(i) / static_cast<float>(NumRadialLines) * TWO_PI;

            FVector2D Offset{ 0.5f, 0.5f };

            Points.Add(PointsTransform.TransformPoint(Offset));
            Points.Add(PointsTransform.TransformPoint(Offset + FVector2D(0.5f * FMath::Cos(Theta), 0.5f * FMath::Sin(Theta))));
        }

        FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, DarkGrey);
        Points.Empty();
    }

    {
        constexpr int32 NumPoints = 1000;
        Points.Reserve(NumPoints);

        TArray<FLinearColor> Channels;
        if (ImportOptions->bConvertToMonochrome)
        {
            Channels.Add(FLinearColor::White);
        }
        else
        {
            Channels.Add(FLinearColor::Red);
            Channels.Add(FLinearColor::Green);
            Channels.Add(FLinearColor::Blue);
        }

        for (int32 channel = 0; channel < Channels.Num(); channel++)
        {
            for (int32 i = 0; i < NumPoints; i++)
            {
                float Theta = static_cast<float>(i) / static_cast<float>(NumPoints - 1) * TWO_PI;
                float CosTheta = FMath::Cos(Theta);

                // Sample phase function at CosTheta
                FVector4f Sample = FPhaseFunctionOperations::SamplePhaseFunction(*pPhaseFunctionSamples, CosTheta);


                // Normalize sample to fit in plot.
                // Plot with logarithmic scale
                float r = (FMath::LogX(10.0f, Sample[channel]) - LogMinSample[channel]) / (LogMaxSample[channel] - LogMinSample[channel]);

                // Scale r to fit within range [0,0.5]
                r *= 0.5f;

                // Convert from polar to cartesian
                float X = r * CosTheta;
                float Y = r * FMath::Sin(Theta);

                // To position points in the centre of the widget
                FVector2D Offset{ 0.5f, 0.5f };

                Points.Add(PointsTransform.TransformPoint(FVector2D(X, Y) + Offset));
            }

            FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, Channels[channel]);
            Points.Empty();
        }
    }

    return LayerId;
}


FVector2D SPhaseFunctionWidget::ComputeDesiredSize(float) const
{
    return FVector2D(300.0, 300.0);
}


FTransform2D SPhaseFunctionWidget::GetPointsTransform(const FGeometry& AllottedGeometry) const
{
    const double Margin = 0.05 * AllottedGeometry.GetLocalSize().GetMin();
    const FScale2D Scale((AllottedGeometry.GetLocalSize() - 2.0 * Margin) * FVector2D(1.0, -1.0));
    const FVector2D Translation(Margin, AllottedGeometry.GetLocalSize().Y - Margin);

    return FTransform2D(Scale, Translation);
}


void SPhaseFunctionWidget::RescaleAxes()
{
    FVector4f MinSample = FVector4f{ INFINITY, INFINITY, INFINITY };
    FVector4f MaxSample = -FVector4f{ INFINITY, INFINITY, INFINITY };
    for (auto& Sample : *pPhaseFunctionSamples)
    {
        MinSample.X = FMath::Min(Sample.X, MinSample.X);
        MaxSample.X = FMath::Max(Sample.X, MaxSample.X);

        MinSample.Y = FMath::Min(Sample.Y, MinSample.Y);
        MaxSample.Y = FMath::Max(Sample.Y, MaxSample.Y);

        MinSample.Z = FMath::Min(Sample.Z, MinSample.Z);
        MaxSample.Z = FMath::Max(Sample.Z, MaxSample.Z);
    }
    // Add a bit of padding
    MinSample *= 0.9f;
    MaxSample *= 1.1f;

    LogMinSample.X = FMath::LogX(10.0f, MinSample.X);
    LogMinSample.Y = FMath::LogX(10.0f, MinSample.Y);
    LogMinSample.Z = FMath::LogX(10.0f, MinSample.Z);

    LogMaxSample.X = FMath::LogX(10.0f, MaxSample.X);
    LogMaxSample.Y = FMath::LogX(10.0f, MaxSample.Y);
    LogMaxSample.Z = FMath::LogX(10.0f, MaxSample.Z);
}
