#pragma once

#include <glm/glm.hpp>

namespace ncl {


    inline float signNotZero(float k) {
        return (k >= 0.0) ? 1.0 : -1.0;
    }


    inline glm::vec2 signNotZero(glm::vec2 v) {
        return glm::vec2(signNotZero(v.x), signNotZero(v.y));
    }


    /** Assumes that v is a unit vector. The result is an octahedral vector on the [-1, +1] square. */
    inline glm::vec2 octEncode(glm::vec3 v) {
        float l1norm = glm::abs(v.x) + glm::abs(v.y) + glm::abs(v.z);
        glm::vec2 result = v.xy * (1.0f / l1norm);
        if (v.z < 0.0f) {
            result = (1.0f - glm::abs(glm::vec2(result.yx))) * signNotZero(result.xy);
        }
        return result;
    }


    /** Returns a unit vector. Argument o is an octahedral vector packed via octEncode,
        on the [-1, +1] square*/
    inline glm::vec3 octDecode(glm::vec2 o) {
        glm::vec3 v = glm::vec3(o.x, o.y, 1.0f - glm::abs(o.x) - glm::abs(o.y));
        if (v.z < 0.0f) {
            v.xy = (1.0f - glm::abs(glm::vec2(v.yx))) * signNotZero(v.xy);
        }
        return normalize(v);
    }
}