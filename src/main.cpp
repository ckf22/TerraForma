#include "perlin.hpp"
#include "erosion.hpp"

#include <iostream>

int main(int argc, char ** argv){
    TerraForma::Perlin perlin{};
    TerraForma::HeightMap data = perlin.generate(8, 1500);
    data.generate_image("test.bmp", 1, 128);

    //std::cout << TerraForma::Erosion::get_stripe_value({1.57,2}, {.pivot_point = glm::vec2{1.57,2}, .rotation_vector = glm::vec2{.10,1}}) << std::endl;
    //
}