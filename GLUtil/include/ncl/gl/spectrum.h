#pragma once

namespace ncl {
	namespace gl {

		static const int sampledLambdaStart = 400;
		static const int sampledLambdaEnd = 700;
		static const int nSpectralSamples = 60;

		static const int nCIESamples = 471;
		extern const float CIE_X[nCIESamples];
		extern const float CIE_Y[nCIESamples];
		extern const float CIE_Z[nCIESamples];
		extern const float CIE_lambda[nCIESamples];
		static const float CIE_Y_integral = 106.856895;
	}
}