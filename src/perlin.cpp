#include "perlin.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <iostream>
#include <cmath>

namespace TerraForma{

HeightMap Perlin::generate(u_int32_t vector_dimension, u_int32_t point_dimension){
    auto vectors = this->generate_directional_vectors(vector_dimension);
    HeightMap data{static_cast<HeightMap::index_t>(point_dimension)};

    const float divisor = static_cast<float>(point_dimension) / static_cast<float>(vector_dimension);
    for(u_int32_t i = 0; i < point_dimension; ++i)
        for(u_int32_t j = 0; j < point_dimension; ++j)
            data[i][j] = this->generate_point(glm::vec2{i,j}/=divisor, vectors);

    return data;
}


Perlin::VectorGrid Perlin::generate_directional_vectors(u_int32_t new_dimension){
    VectorGrid vectors(new_dimension);

    for(auto& it : vectors){
        it.resize(new_dimension);
        for(auto& it2 : it)
            it2 = this->random_device->direction();
    }

    return vectors;
}

float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// the parameter 'relative_vector_coordinate' determines which vector are to be used for perlin noise
float Perlin::generate_point(glm::vec2 relative_vector_coordinate, VectorGrid& vectors){

    u_int32_t x0 = static_cast<u_int32_t>(std::floor(relative_vector_coordinate.x));
    u_int32_t z0 = static_cast<u_int32_t>(std::floor(relative_vector_coordinate.y));
        
    u_int32_t x1 = (x0 >= vectors.size() - 1) ? x0 : x0 + 1;
    u_int32_t z1 = (z0 >= vectors.size() - 1) ? z0 : z0 + 1;

    float tx = relative_vector_coordinate.x - static_cast<float>(x0);
    float tz = relative_vector_coordinate.y - static_cast<float>(z0);

    float dot00 = glm::dot(vectors[x0][z0], {tx, tz});
    float dot10 = glm::dot(vectors[x1][z0], {tx - 1.0f, tz});
    float dot01 = glm::dot(vectors[x0][z1], {tx, tz - 1.0f});
    float dot11 = glm::dot(vectors[x1][z1], {tx - 1.0f, tz - 1.0f});

    float i1 = glm::mix(dot00, dot10, fade(tx));
    float i2 = glm::mix(dot01, dot11, fade(tx));

    return glm::mix(i1, i2, fade(tz));
}

Perlin::Perlin(){
    this->random_device = std::make_unique<Random>();
}

}