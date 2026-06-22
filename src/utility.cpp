#include "utility.hpp"

#include <glm/ext.hpp>

#include <stdio.h>

namespace TerraForma{

Random::Random(){
    this->engine = std::make_unique<std::mt19937_64>(time(0));
}
Random::Random(seed_t seed){
    this->engine = std::make_unique<std::mt19937_64>(seed);
}

u_int64_t Random::random_int(){
    this->number_count++;
    return (*this->engine)();
}

double Random::random_float_basic(){
    this->number_count++;
    return static_cast<double>( (*this->engine)() )/UINT64_MAX;
}

double Random::random_float(double max, double min){
    this->number_count++;
    return static_cast<double>( (*this->engine)() ) / (static_cast<double>(UINT64_MAX) / (max-min) ) + min;
}

glm::vec2 Random::direction(){
    auto buffer = this->random_float(glm::two_pi<double>());
    return {glm::cos(buffer), glm::sin(buffer)};
}



} // namespace TerraForma
