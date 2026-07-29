#pragma once

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>

#include <vector>
#include <iostream>

namespace TerraForma{

class HeightMap{
  public:
    typedef double height_t;
    typedef std::size_t index_t;

    // 'SubRow' is used to represent a row of data owned by 'HeightMap'
    class SubRow{
        friend class HeightMap;
      public:
        SubRow(height_t * _data_, index_t _length);
        ~SubRow();
        height_t& operator[](index_t index);
        void * data();
      private:
        index_t length;
        height_t * data_;
    };
    friend class SubRow;


  public:
    HeightMap(std::vector<height_t>& _data_, bool _automatic_resizes_ = false);
    HeightMap(std::vector<height_t>& _data_, index_t _dimension, bool _automatic_resizes_ = false);
    HeightMap(index_t _dimension, bool _automatic_resizes_ = false);
    HeightMap(bool _automatic_resizes_ = false);
    HeightMap(const HeightMap&);
    HeightMap(HeightMap&&);
    ~HeightMap();

    HeightMap& operator=(std::vector<height_t>& _data_);
    HeightMap& operator=(std::vector<height_t>&& _data_);
    HeightMap& operator=(const HeightMap& _data_);
    HeightMap& operator=(HeightMap&& _data_);

    SubRow operator[](index_t row);
    height_t& operator[](glm::vec<2, index_t> position);

    index_t get_dimension() const { return this->dimension; }
    void resize(index_t new_dimension);
    void * data_ptr();
    std::vector<height_t>& get_raw_vector();
  
    void print() {
      for(index_t i = 0; i < this->data_.size(); ++i){
        if(i != 0 && i % this->dimension == 0)
          std::cout << '\n';
        std::cout << this->data_[i] << ' ';
      }
      std::cout << std::endl;
    }
    void generate_image(std::string filename, double summand, double factor);
    void write_to_file(std::string filename, double horizontal_scale = 1.0, double vertical_scale = 1.0);
  private:
    void update_dimension();

    bool automatic_resizes_ = false;
    index_t dimension = 0; // length of one row
    std::vector<height_t> data_; // list of rows
};

}
