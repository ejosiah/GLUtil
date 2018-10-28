#pragma once

namespace ncl {
	namespace gl {
		struct Resolution {
			int width;
			int height;

			static const Resolution HD;
			static const Resolution FHD;
			static const Resolution QHD;
			static const Resolution UHD;
		};

		const Resolution Resolution::HD = Resolution{ 1280, 720 };
		const Resolution Resolution::FHD = Resolution{ 1920 , 1080 };
		const Resolution Resolution::QHD = Resolution{ 2560 , 1440 };
		const Resolution Resolution::UHD = Resolution{ 3840 , 2160 };
	}
}