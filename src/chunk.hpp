#pragma once

#include "heightmap.hpp"

#include <any>

namespace TerraForma{

class Chunk{
  public:
    Chunk();
    ~Chunk();

    Chunk(Chunk&&) = default;
    Chunk& operator=(Chunk&&) = default;

    template<typename T>
    void set_data(T& data){
        this->arbitrary_data = data;
    }
    template<typename T>
    T& get_data(){
        return std::any_cast<T>(arbitrary_data);
    }
  private:
    HeightMap height_map;
    std::any arbitrary_data;
};

}