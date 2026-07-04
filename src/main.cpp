#include "perlin.hpp"

#include <iostream>

int main(int argc, char ** argv){
    TerraForma::Perlin p{};
    p.generate(10, 125);

    for(float f = 0; f < 3; f += .15f)
        std::cout << p[glm::vec2{f, f/4}].position.y << std::endl;

    return 0;
}