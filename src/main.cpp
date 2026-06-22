#include "utility.hpp"

#include <iostream>

int main(int argc, char ** argv){
    TerraForma::Random a{};
    for(int i = 0; i < 10; ++i){
        std::cout << a.random_float(2,-5) << std::endl;
    }
    return 0;
}