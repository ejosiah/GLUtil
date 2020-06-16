const int BSDF_REFLECTION = 1 << 0;
const int BSDF_TRANSMISSION = 1 << 1;
const int BSDF_DIFFUSE = 1 << 2;
const int BSDF_GLOSSY = 1 << 3;
const int BSDF_SPECULAR = 1 << 4;
const int BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION;

const int SPECIULAR_REFLECT = BSDF_REFLECTION | BSDF_SPECULAR | (1 << 5);
const int SPECULAR_TRANSMISSION = BSDF_TRANSMISSION | BSDF_SPECULAR | (1 << 6);
const int LAMBERTIAN_REFLECT = BSDF_REFLECTION | BSDF_DIFFUSE | (1 << 7);
const int OREN_NAYAR = BSDF_REFLECTION | BSDF_DIFFUSE | (1 << 8);
const int FRESNEL_SPECULAR = BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR | (1 << 9);

const int FRESNEL_NOOP = 1 << 0;
const int FRESNEL_DIELECTRIC = 1 << 1;
const int FRESNEL_CONDOCTOR = 1 << 2;

bool matchesFlag(int expectedTypes, int actualTypes) {
	return (expectedTypes & actualTypes) > 0;
}

bool isBsdf(int expectedBsdf, int actualBsdf) {
	return (expectedBsdf & actualBsdf) == expectedBsdf;
}

float f0(float etaI, float etaT) {
	float num = etaT - etaI;
	float denum = etaT + etaI;
	float f0 = pow(num / denum, 2);
	return f0;
}

float fresnelSchlick(float cosTheta, float F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float fresnelDielectric(float cosThetaI, float etaI, float etaT) {
	cosThetaI = clamp(cosThetaI, -1.0, 1.0);
	// Potentially swap indices of refraction
	bool entering = cosThetaI > 0.f;
	if (!entering) {
		swap(etaI, etaT);
		cosThetaI = abs(cosThetaI);
	}

	// Compute _cosThetaT_ using Snell's law
	float sinThetaI = sqrt(max(0, 1 - cosThetaI * cosThetaI));
	float sinThetaT = etaI / etaT * sinThetaI;

	// Handle total internal reflection
	if (sinThetaT >= 1) {
		return 1;
	}
	float cosThetaT = sqrt(max(0, 1 - sinThetaT * sinThetaT));
	float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
		((etaT * cosThetaI) + (etaI * cosThetaT));
	float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
		((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl * Rparl + Rperp * Rperp) / 2;
}

float fresnel(float cosTheta, float etaI, float etaT) {
	//return fresnelSchlick(cosTheta, f0(etaI, etaT));
	return fresnelDielectric(cosTheta, etaI, etaT);
}