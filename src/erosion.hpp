#pragma once

#include "heightmap.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <optional>

namespace TerraForma{

namespace Erosion{
    typedef double value_t;
    typedef glm::vec<2, value_t> myvec2;
    typedef long index_t;
    struct StripeInfo{
        static constexpr myvec2 unrotated_stripe_direction{0,1};
        // the stripe will be rotated around this point
        myvec2 pivot_point{0,0};

        // the stripes will be aligned to this vector
        // if this remains uninitialised the first (x) coordinate will be used as input to the sin function
        myvec2 rotation_vector = unrotated_stripe_direction;

        value_t frequency = 1;
        value_t frequency_offset = 0;
        // the sine output will be multiplied with this variable
        value_t amplitude = 1;
        value_t amplitude_offset = 0;
    };

    value_t get_stripe_value(myvec2 position, const StripeInfo& stripe_info = StripeInfo{});

    // 'erosion_scale' assumes the distance between two neighbouring values in the heightmap to be 1
    HeightMap apply_erosion(HeightMap& base_terrain, value_t erosion_scale, value_t erosion_strength, u_int32_t octave_count);

    HeightMap apply_octave(HeightMap& terrain, value_t erosion_scale, value_t erosion_strength);

}

}