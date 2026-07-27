#include "perlin.hpp"
#include "erosion.hpp"

#include <iostream>

int main(int argc, char ** argv){
    try{
        TerraForma::Perlin perlin{};
        TerraForma::HeightMap data{perlin.generate(4, 800)};
        data = TerraForma::Erosion::apply_erosion(data, 70, 0.35, 2, 4);
        data.generate_image("test.bmp", 1, 128);
        //TerraForma::debug();
    } catch(std::exception& e){
        std::cout << e.what() << std::endl; 
    }

    //std::cout << TerraForma::Erosion::get_stripe_value({1.57,2}, {.pivot_point = glm::vec2{1.57,2}, .rotation_vector = glm::vec2{.10,1}}) << std::endl;
    //
}