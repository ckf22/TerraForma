#include "perlin.hpp"

#include <iostream>

int main(int argc, char ** argv){
    TerraForma::Perlin perlin{};
    auto data = perlin.generate(10, 125);

    for(float f = 0; f < 20; f += 1)
        std::cout << data[f][f/2] << std::endl;

    return 0;
}