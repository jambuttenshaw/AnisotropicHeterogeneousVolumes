#include "MiePlotImporter.h"


static unsigned long long Factorial[] = {
	1,
	1,
	2,
	6,
	24,
	120,
	720,
	5040,
	40'320,
	362'880,
	3'628'800,
	39'916'800,
	479'001'600,
	6'227'020'800,
	87'178'291'200,
	1'307'674'368'000,
	20'922'789'888'000,
	355'687'428'096'000,
	6'402'373'705'728'000,
	121'645'100'408'832'000,
	2'432'902'008'176'640'000,
};

static float EvalP(int l, int m, float x)
{
	float pmm = 1.0f;
	if (m > 0) {
		float somx2 = sqrt((1.0f - x) * (1.0f + x));
			float fact = 1.0f;
		for (int i = 1; i <= m; i++) {
			pmm *= (-fact) * somx2;
			fact += 2.0f;
		}
	}
	if (l == m) return pmm;
	float pmmp1 = x * (2.0f * m + 1.0f) * pmm;
	if (l == m + 1) return pmmp1;
	float pll = 0.0f;
	for (int ll = m + 2; ll <= l; ++ll) {
		pll = ((2.0f * ll - 1.0f) * x * pmmp1 - (ll + m - 1.0f) * pmm) / (ll - m);
		pmm = pmmp1;
		pmmp1 = pll;
	}
	return pll;
}

static float EvalK(int l, int m)
{
	float temp = ((2.0f * l + 1.0f) * Factorial[l - m]) / (4.0f * PI * Factorial[l + m]);
	return FMath::Sqrt(temp);
}

static float SHBasis(int l, int m, float cosTheta, float phi)
{
	const float Sqrt2 = FMath::Sqrt(2.0f);
	if (m == 0)
		return EvalK(l, 0) * EvalP(l, m, cosTheta);
	else if (m > 0)
		return Sqrt2 * EvalK(l, m) * FMath::Cos(m * phi) * EvalP(l, m, cosTheta);
	else
		return Sqrt2 * EvalK(l, -m) * FMath::Sin(-m * phi) * EvalP(l, -m, cosTheta);
}


bool FMiePlotImporterModule::ExtractZonalHarmonics(const TArray<FVector4f>& PhaseFunctionSamples, FVector2f& OutZonalHarmonics)
{
	auto SamplePhaseLUT = [&](float cosTheta)-> FVector4f
		{
			const float theta = FMath::Acos(cosTheta);

			float uv = theta / PI;
			float tc = uv * static_cast<float>(PhaseFunctionSamples.Num() - 1);

			return FMath::Lerp(
				PhaseFunctionSamples[FMath::FloorToInt(tc)],
				PhaseFunctionSamples[FMath::CeilToInt(tc)],
				FMath::Frac(tc)
			);
		};

	constexpr float InvUniformSpherePDF = (4.0f * PI);

	// Perform Monte Carlo integration
	constexpr uint32 NumSamples = 1'000'000;
	FVector2f Coefficients{ 0.0f, 0.0f };

	for (uint32 Sample = 0; Sample < NumSamples; Sample++)
	{
		FVector2D UV = { FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f) };

		// Map UV onto surface of sphere
		float CosTheta = 2.0f * UV.X - 1.0f;
		float Phi = 2.0f * PI * UV.Y;

		// Phase only depends on theta
		FVector4f PhaseSampleRGB = SamplePhaseLUT(CosTheta);
		float PhaseSample = (PhaseSampleRGB.X + PhaseSampleRGB.Y + PhaseSampleRGB.Z) / 3.0f;

		Coefficients.X += (PhaseSample * SHBasis(0, 0, CosTheta, Phi));
		Coefficients.Y += (PhaseSample * SHBasis(1, 0, CosTheta, Phi));
	}
	Coefficients *= InvUniformSpherePDF / static_cast<float>(NumSamples);

	OutZonalHarmonics = Coefficients;
	return true;
}
