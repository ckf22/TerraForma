#pragma once

#include "random.hpp"
#include "heightmap.hpp"

#include <memory>

namespace TerraForma{
struct Perlin{
    Perlin();
    typedef std::vector<std::vector<glm::vec2>> VectorGrid;

    HeightMap generate(u_int32_t vector_dimension, u_int32_t point_dimension);
    HeightMap generate(VectorGrid& vectors, u_int32_t vector_dimension, u_int32_t point_dimension);

    float generate_point(glm::vec2 relative_vector_coordinate, VectorGrid& vectors);
    VectorGrid generate_directional_vectors(u_int32_t new_dimension);

    std::unique_ptr<Random> random_device;
};

}