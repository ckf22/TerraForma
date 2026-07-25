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
    ~Random();

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

}