#pragma 

#define GLM_SWIZZLE 
#include <glm/glm.hpp>

namespace ncl {
	namespace sampling {
		// Hash without Sine
// Creative Commons Attribution-ShareAlike 4.0 International Public License
// Created by David Hoskins.

//----------------------------------------------------------------------------------------
//  1 out, 1 in...
		inline float hash11(float p)
		{
			p = glm::fract(p * 0.1031f);
			p *= p + 33.33;
			p *= p + p;
			return glm::fract(p);
		}

		//----------------------------------------------------------------------------------------
		//  1 out, 2 in...
		float hash12(glm::vec2 p)
		{
			glm::vec3 p3 = glm::fract(glm::vec3(p.xyx) * 0.1031f);
			p3 += glm::dot(p3, p3.yzx + glm::vec3(33.33));
			return glm::fract((p3.x + p3.y) * p3.z);
		}

		//----------------------------------------------------------------------------------------
		//  1 out, 3 in...
		float hash13(glm::vec3 p3)
		{
			p3 = glm::fract(p3 * 0.1031f);
			p3 += glm::dot(p3, p3.yzx + glm::vec3(33.33));
			return glm::fract((p3.x + p3.y) * p3.z);
		}

		//----------------------------------------------------------------------------------------
		//  2 out, 1 in...
		glm::vec2 hash21(float p)
		{
			glm::vec3 p3 = glm::fract(glm::vec3(p) * glm::vec3(0.1031, .1030, .0973));
			p3 += glm::dot(p3, p3.yzx + glm::vec3(33.33));
			return glm::fract((p3.xx + p3.yz) * p3.zy);

		}

		//----------------------------------------------------------------------------------------
		///  2 out, 2 in...
		glm::vec2 hash22(glm::vec2 p)
		{
			glm::vec3 p3 = glm::fract(glm::vec3(p.xyx) * glm::vec3(0.1031, .1030, .0973));
			p3 += glm::dot(p3, p3.yzx + glm::vec3(33.33));
			return glm::fract((p3.xx + p3.yz) * p3.zy);

		}

		//----------------------------------------------------------------------------------------
		///  2 out, 3 in...
		glm::vec2 hash23(glm::vec3 p3)
		{
			p3 = glm::fract(p3 * glm::vec3(0.1031, .1030, .0973));
			p3 += glm::dot(p3, p3.yzx + glm::vec3(33.33));
			return glm::fract((p3.xx + p3.yz) * p3.zy);
		}

		//----------------------------------------------------------------------------------------
		//  3 out, 1 in...
		glm::vec3 hash31(float p)
		{
			glm::vec3 p3 = glm::fract(glm::vec3(p) * glm::vec3(0.1031, .1030, .0973));
			p3 += glm::dot(p3, p3.yzx + glm::vec3(33.33));
			return glm::fract((p3.xxy + p3.yzz) * p3.zyx);
		}


		//----------------------------------------------------------------------------------------
		///  3 out, 2 in...
		glm::vec3 hash32(glm::vec2 p)
		{
			glm::vec3 p3 = glm::fract(glm::vec3(p.xyx) * glm::vec3(0.1031, .1030, .0973));
			p3 += glm::dot(p3, p3.yxz + glm::vec3(33.33));
			return glm::fract((p3.xxy + p3.yzz) * p3.zyx);
		}

		//----------------------------------------------------------------------------------------
		///  3 out, 3 in...
		glm::vec3 hash33(glm::vec3 p3)
		{
			p3 = glm::fract(p3 * glm::vec3(0.1031, .1030, .0973));
			p3 += glm::dot(p3, p3.yxz + glm::vec3(33.33));
			return glm::fract((p3.xxy + p3.yxx) * p3.zyx);

		}

		//----------------------------------------------------------------------------------------
		// 4 out, 1 in...
		glm::vec4 hash41(float p)
		{
			glm::vec4 p4 = glm::fract(glm::vec4(p) * glm::vec4(0.1031, .1030, .0973, .1099));
			p4 += glm::dot(p4, p4.wzxy + glm::vec4(33.33));
			return glm::fract((p4.xxyz + p4.yzzw) * p4.zywx);

		}

		//----------------------------------------------------------------------------------------
		// 4 out, 2 in...
		glm::vec4 hash42(glm::vec2 p)
		{
			glm::vec4 p4 = glm::fract(glm::vec4(p.xyxy) * glm::vec4(0.1031, .1030, .0973, .1099));
			p4 += glm::dot(p4, p4.wzxy + glm::vec4(33.33));
			return glm::fract((p4.xxyz + p4.yzzw) * p4.zywx);

		}

		//----------------------------------------------------------------------------------------
		// 4 out, 3 in...
		glm::vec4 hash43(glm::vec3 p)
		{
			glm::vec4 p4 = glm::fract(glm::vec4(p.xyzx) * glm::vec4(0.1031, .1030, .0973, .1099));
			p4 += glm::dot(p4, p4.wzxy + glm::vec4(33.33));
			return glm::fract((p4.xxyz + p4.yzzw) * p4.zywx);
		}

		//----------------------------------------------------------------------------------------
		// 4 out, 4 in...
		glm::vec4 hash44(glm::vec4 p4)
		{
			p4 = glm::fract(p4 * glm::vec4(0.1031, .1030, .0973, .1099));
			p4 += glm::dot(p4, p4.wzxy + glm::vec4(33.33));
			return glm::fract((p4.xxyz + p4.yzzw) * p4.zywx);
		}
	}
}