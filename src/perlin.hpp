#pragma once

#include "utility.hpp"

#include <optional>

namespace TerraForma{

typedef std::vector<std::vector<glm::vec2>> VectorGrid;

class Perlin{
  public:
    Perlin();
    ~Perlin();

    Vertex& operator[](GridCoordinate cord){ return vertices->operator[](cord); }
    Vertex& operator[](glm::vec2 vector_cord){ 
      vector_cord /= vectors->size(); vector_cord *= vertices->get_dimension_length(); return vertices->operator[](vector_cord); 
    }

    void generate(u_int32_t vector_dimension, u_int32_t point_dimension);
    float generate_point(glm::vec2 relative_vector_coordinate);

    void generate_vertex_grid(u_int32_t new_dimension);
    void generate_directional_vectors(u_int32_t new_dimension);
  private:
    Random random{};
    std::optional<VectorGrid> vectors;
    std::optional<VertexGrid> vertices;
};

}