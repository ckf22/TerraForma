#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <memory>
#include <random>
#include <vector>

namespace TerraForma{

class Random{
  public:
    typedef u_int64_t seed_t;

    Random();
    Random(seed_t seed);

    u_int64_t random_int();
    double random_float(double max = 1, double min = 0);
    double random_float_basic();
    glm::vec2 direction();

    seed_t get_seed() const noexcept { return seed; }
    u_int64_t get_generated_count() const noexcept { return number_count; }
  private:
    seed_t seed;
    u_int64_t number_count = 0;
    std::unique_ptr<std::mt19937_64> engine;
};

struct Vertex{
    glm::vec3 position;
    glm::vec3 color{.9f};
    glm::vec3 normal{0,-1,0};
};

typedef glm::vec<2, u_int32_t> GridCoordinate;

class VertexGrid{
  public:
    VertexGrid(u_int32_t _dimension_length);
    VertexGrid(const std::vector<Vertex>& _data);
    VertexGrid(const std::vector<Vertex>& _data, u_int32_t _dimension_length);
    ~VertexGrid();

    u_int32_t get_dimension_length() const noexcept { return dimension_length; }
    const std::vector<Vertex>& operator()() const noexcept { return data; };
    Vertex* operator[](u_int32_t row);
    Vertex& operator[](GridCoordinate position);
  private:
    u_int32_t dimension_length;
    std::vector<Vertex> data;
};

}