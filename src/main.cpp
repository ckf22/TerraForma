#include "perlin.hpp"
#include "erosion.hpp"

#include <iostream>

int main(int argc, char ** argv){
    try{
        TerraForma::Perlin perlin{};
        TerraForma::HeightMap data{perlin.generate(4, 1000)};
        data = TerraForma::Erosion::apply_erosion(
            data, {.horizontal_scale = 1, .terrain_feature_size = 250, .erosion_strength = 0.4,
                 .erosion_scale = 3, .stripe_frequency_modifier = 6, .octave_count = 6}
        );
        data.generate_image("test.bmp", 1.32, 90);
        data.write_to_file("test.obj", 0.01, 2);
        //TerraForma::debug();
    } catch(std::exception& e){
        std::cout << e.what() << std::endl; 
    }

    //std::cout << TerraForma::Erosion::get_stripe_value({1.57,2}, {.pivot_point = glm::vec2{1.57,2}, .rotation_vector = glm::vec2{.10,1}}) << std::endl;
    //
}