#include "perlin.hpp"

#include <cmath>

namespace TerraForma{

void Perlin::generate(u_int32_t vector_dimension, u_int32_t point_dimension){
    this->generate_directional_vectors(vector_dimension);
    this->generate_vertex_grid(point_dimension);

    const float divisor = static_cast<float>(point_dimension) / static_cast<float>(vector_dimension);
    for(u_int32_t i = 0; i < point_dimension; ++i)
        for(u_int32_t j = 0; j < point_dimension; ++j){
            this->vertices->operator[](i)[j].position.x = i;
            this->vertices->operator[](i)[j].position.z = j;
            this->vertices->operator[](i)[j].position.y = this->generate_point(glm::vec2{i,j}/=divisor);
        }

    // generating normals
    for(u_int32_t i = 0; i < point_dimension; ++i)
        for(u_int32_t j = 0; j < point_dimension; ++j){
        }    
}

void Perlin::generate_vertex_grid(u_int32_t new_dimension){
    if( this->vertices.has_value() )
        this->vertices.reset();

    this->vertices = VertexGrid(new_dimension);
}

void Perlin::generate_directional_vectors(u_int32_t new_dimension){
    if( !this->vectors.has_value() )
        this->vectors = std::vector<std::vector<glm::vec2>>(new_dimension);

    else
        this->vectors->resize(new_dimension);

    for(auto& it : *this->vectors){
        it.resize(new_dimension);
        for(auto& it2 : it)
            it2 = this->random.direction();
    }
}

float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// the parameter 'relative_vector_coordinate' determines which vector are to be used for perlin noise
float Perlin::generate_point(glm::vec2 relative_vector_coordinate){

    u_int32_t x0 = static_cast<u_int32_t>(std::floor(relative_vector_coordinate.x));
    u_int32_t z0 = static_cast<u_int32_t>(std::floor(relative_vector_coordinate.y));
        
    u_int32_t x1 = (x0 >= this->vectors->size() - 1) ? x0 : x0 + 1;
    u_int32_t z1 = (z0 >= this->vectors->size() - 1) ? z0 : z0 + 1;

    float tx = relative_vector_coordinate.x - static_cast<float>(x0);
    float tz = relative_vector_coordinate.y - static_cast<float>(z0);

    float dot00 = glm::dot(this->vectors->operator[](x0)[z0], {tx, tz});
    float dot10 = glm::dot(this->vectors->operator[](x1)[z0], {tx - 1.0f, tz});
    float dot01 = glm::dot(this->vectors->operator[](x0)[z1], {tx, tz - 1.0f});
    float dot11 = glm::dot(this->vectors->operator[](x1)[z1], {tx - 1.0f, tz - 1.0f});

    float i1 = glm::mix(dot00, dot10, fade(tx));
    float i2 = glm::mix(dot01, dot11, fade(tx));

    return glm::mix(i1, i2, fade(tz));
}


Perlin::Perlin(){
    
}

Perlin::~Perlin(){
    
}


}