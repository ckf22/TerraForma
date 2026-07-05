#include "heightmap.hpp"

#include <stdexcept>
#include <string>
#include <cmath>
#include <algorithm>

namespace TerraForma{

HeightMap::SubRow::SubRow(height_t * _data_, index_t _length) : data_{_data_}, length{_length} {}

HeightMap::SubRow::~SubRow(){
    this->data_ = nullptr;
}

HeightMap::height_t& HeightMap::SubRow::operator[](index_t index){
    if( index < 0 || index >= this->length )
        throw std::overflow_error(std::string("Index ")+std::to_string(index)+std::string(" out of bounds. Length: ")+std::to_string(this->length));

    return this->data_[index];
}

void * HeightMap::SubRow::data(){
    return (void*)this->data_;
}


HeightMap::HeightMap(std::vector<height_t>& _data_, bool _automatic_resizes_) : data_{_data_}, automatic_resizes_{_automatic_resizes_} {
    this->update_dimension();
    this->data_.resize(this->dimension*this->dimension);
}

HeightMap::HeightMap(std::vector<height_t>& _data_, index_t _dimension, bool _automatic_resizes_) 
 : data_{_data_}, dimension{_dimension}, automatic_resizes_{_automatic_resizes_} {
    if( _data_.size() > _dimension * _dimension ){
        if( _automatic_resizes_ )
            this->update_dimension();
        else
            throw std::overflow_error("");
    }
    this->data_.resize(this->dimension*this->dimension);
}

HeightMap::HeightMap(index_t _dimension, bool _automatic_resizes_) : dimension{_dimension}, automatic_resizes_{_automatic_resizes_} {
    this->data_.resize(this->dimension*this->dimension);
}

HeightMap::HeightMap(bool _automatic_resizes_) : automatic_resizes_{_automatic_resizes_} {
    this->data_ = {};
}

HeightMap::~HeightMap(){}

HeightMap& HeightMap::operator=(std::vector<height_t>& _data_){
    if( !this->automatic_resizes_ && _data_.size() > this->dimension*this->dimension )
        throw std::overflow_error("New Data's Size exceeds limits and 'automatic_resizes' is false");

    this->data_ = _data_;
    
    if( _data_.size() > this->dimension*this->dimension )
        this->update_dimension();        

    this->data_.resize(this->dimension*this->dimension);

    return *this;
}

HeightMap& HeightMap::operator=(std::vector<height_t>&& _data_){
    return *this;
}

HeightMap& HeightMap::operator=(const HeightMap& _data_){
    this->data_ = _data_.data_;
    this->dimension = _data_.dimension;

    return *this;
}

HeightMap& HeightMap::operator=(HeightMap&& _data_){
    this->data_ = std::move(_data_.data_);
    this->dimension = _data_.dimension;

    return *this;
}

HeightMap::SubRow HeightMap::operator[](index_t row){
    return SubRow(&this->data_[row*this->dimension], this->dimension);
}

HeightMap::height_t& HeightMap::operator[](glm::vec<2, index_t> position){
    if( position.x >= this->dimension || position.y >= this->dimension )
        throw std::overflow_error("Index out of bounds");

    return this->data_[ (position.x * this->dimension) + position.y ];
}

void HeightMap::resize(index_t new_dimension){
}

void * HeightMap::data(){
    return this->data_.data();
}

void HeightMap::update_dimension(){
    double root = sqrt(this->data_.size());
    if( root == std::floor(root) )
        this->dimension = root;
    else
        this->dimension = std::floor(root)+1;
}

}