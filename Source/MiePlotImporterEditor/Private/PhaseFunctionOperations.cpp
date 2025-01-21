#include "PhaseFunctionOperations.h"
#include "MiePlotImportOptions.h"


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


FVector4f FPhaseFunctionOperations::SamplePhaseFunction(const TArray<FVector4f>& PhaseFunctionSamples, float CosTheta)
{
	const float Theta = FMath::Acos(CosTheta);
	float uv = Theta / PI;
	float tc = uv * static_cast<float>(PhaseFunctionSamples.Num() - 1);

	return FMath::Lerp(
		PhaseFunctionSamples[FMath::FloorToInt(tc)],
		PhaseFunctionSamples[FMath::CeilToInt(tc)],
		FMath::Frac(tc)
	);
}

template<typename T>
static T MCIntegration(const std::function<T(float)>& Func)
{
	constexpr float InvUniformSpherePDF = (4.0f * PI);

	// Perform Monte Carlo integration
	constexpr uint32 NumSamples = 1'000'000;
	T Integral{ };

	for (uint32 Sample = 0; Sample < NumSamples; Sample++)
	{
		FVector2D UV = { FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f) };

		// Map UV onto surface of sphere
		// Phase functions only depend on theta
		float CosTheta = 2.0f * UV.X - 1.0f;

		Integral += Func(CosTheta);
	}
	Integral *= InvUniformSpherePDF / static_cast<float>(NumSamples);
	return Integral;
}


void FPhaseFunctionOperations::GetMagnitude(const TArray<FVector4f>& PhaseFunctionSamples, FVector4f& OutMagnitude)
{
	std::function func = [&](float CosTheta) { return SamplePhaseFunction(PhaseFunctionSamples, CosTheta); };
	OutMagnitude = MCIntegration(func);
}


void FPhaseFunctionOperations::Normalize(TArray<FVector4f>& PhaseFunctionSamples)
{
	// Normalize phase function by numerically integrating over the surface of the sphere
	FVector4f Magnitude;
	GetMagnitude(PhaseFunctionSamples, Magnitude);

	// Normalize the data
	for (auto& PhaseSample : PhaseFunctionSamples)
	{
		PhaseSample /= Magnitude;
	}
}


void FPhaseFunctionOperations::Clamp(TArray<FVector4f>& PhaseFunctionSamples, float Min, float Max)
{
	for (auto& PhaseSample : PhaseFunctionSamples)
	{
		PhaseSample.X = FMath::Clamp(PhaseSample.X, Min, Max);
		PhaseSample.Y = FMath::Clamp(PhaseSample.Y, Min, Max);
		PhaseSample.Z = FMath::Clamp(PhaseSample.Z, Min, Max);
	}
}


void FPhaseFunctionOperations::ConvertToMonochrome(TArray<FVector4f>& PhaseFunctionSamples)
{
	for (auto& PhaseSample : PhaseFunctionSamples)
	{
		float Average = (PhaseSample.X + PhaseSample.Y + PhaseSample.Z) / 3.0f;
		PhaseSample.X = Average;
		PhaseSample.Y = Average;
		PhaseSample.Z = Average;
	}
}



void FPhaseFunctionOperations::ExtractZonalHarmonics(const TArray<FVector4f>& PhaseFunctionSamples, FVector2f& OutZonalHarmonics)
{
	std::function func = [&](float CosTheta)
		{
			FVector4f PhaseSampleRGB = SamplePhaseFunction(PhaseFunctionSamples, CosTheta);
			float PhaseSample = (PhaseSampleRGB.X + PhaseSampleRGB.Y + PhaseSampleRGB.Z) / 3.0f;

			return FVector2f{
				PhaseSample * SHBasis(0, 0, CosTheta, 0.0f),
				PhaseSample * SHBasis(1, 0, CosTheta, 0.0f)
			};
		};
	OutZonalHarmonics = MCIntegration(func);
}


void FPhaseFunctionOperations::ApplyImportOptions(TArray<FVector4f>& PhaseFunctionSamples, const FMiePlotImportOptions& ImportOptions)
{
	Normalize(PhaseFunctionSamples);
	if (ImportOptions.bConvertToMonochrome)
	{
		ConvertToMonochrome(PhaseFunctionSamples);
	}
	if (ImportOptions.bClampPhaseSamples)
	{
		Clamp(PhaseFunctionSamples, ImportOptions.PhaseSampleClampMin, ImportOptions.PhaseSampleClampMax);
	}
	if (ImportOptions.bReNormalize)
	{
		Normalize(PhaseFunctionSamples);
	}
}
