#include "utility.hpp"

#include <glm/ext.hpp>

#include <stdio.h>
#include <stdexcept>

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


VertexGrid::VertexGrid(u_int32_t _dimension_length) : dimension_length{_dimension_length} {
    this->data.resize(glm::pow(this->dimension_length,2));
}

VertexGrid::VertexGrid(const std::vector<Vertex>& _data) : data{std::move(_data)} {
    auto root = glm::sqrt(_data.size());
    if( static_cast<std::size_t>(root) == root )
        this->dimension_length = static_cast<u_int32_t>(root);
    else{
        this->dimension_length = static_cast<u_int32_t>(root)+1;
        this->data.resize(glm::pow(this->dimension_length,2));
    }
}

VertexGrid::VertexGrid(const std::vector<Vertex>& _data, u_int32_t _dimension_length) : data{std::move(_data)}, dimension_length{_dimension_length} {
    if( glm::pow(_dimension_length,2) >= this->data.size() )
        throw std::out_of_range("");

    if( glm::pow(_dimension_length,2) != this->data.size() )
        this->data.resize( glm::pow(_dimension_length,2) );

    this->dimension_length = _dimension_length;
}

VertexGrid::~VertexGrid(){}

Vertex* VertexGrid::operator[](u_int32_t row){
    return &this->data[row*this->dimension_length];
}

Vertex& VertexGrid::operator[](GridCoordinate position){
    return this->data[(position.x*this->dimension_length)+position.y];
}


} // namespace TerraForma
