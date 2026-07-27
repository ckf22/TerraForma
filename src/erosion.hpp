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


    struct ErosionInfo{
        // distance between two neighbouring points in the HeightMap
        value_t horizontal_scale = 1;

        // the horizontal scale will be applied to this variale before usage
        value_t terrain_feature_size = 200;

        // The final values of each octave will be multiplied by this value before being added to the heightmap
        value_t erosion_strength = 0.4;

        // Amount of voronoi cells within the dimensions of a terrain feature
        value_t erosion_scale = 5;

        // this will be divided by the voronoi cell size to be multiplied with the final input to the sine function
        // and will therefore determine the stripe thickness and density
        value_t stripe_frequency_modifier = 5;

        // this determines the dimensions of a square centered in the middle of the respective cell
        value_t voronoi_point_spawn_area = 0.9;

    
        // Amount of LOD
        index_t octave_count = 1;

        // After each octave, the cell size and the strength of the erosion will be multiplied by this factor
        value_t octave_factor = 0.5;
    };

    // 'erosion_scale' assumes the distance between two neighbouring values in the heightmap to be 1
    HeightMap apply_erosion(HeightMap& base_terrain, ErosionInfo info = ErosionInfo{});

    HeightMap apply_octave(HeightMap& terrain, value_t erosion_scale, value_t erosion_strength, value_t frequency_factor, value_t voronoi_spawn_area);

}

}