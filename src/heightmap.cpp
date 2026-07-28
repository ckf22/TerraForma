#include "heightmap.hpp"

#include "libs/easy-bmp.hpp"


#include <stdexcept>
#include <string>
#include <string.h>
#include <cmath>
#include <algorithm>

namespace TerraForma{

HeightMap::SubRow::SubRow(height_t * _data_, index_t _length) : data_{_data_}, length{_length} {}

HeightMap::SubRow::~SubRow(){
    this->data_ = nullptr;
}

HeightMap::height_t& HeightMap::SubRow::operator[](index_t index){
    if( index < 0 || index >= this->length )
        throw std::out_of_range(std::string("Index ")+std::to_string(index)+std::string(" out of bounds. Length: ")+std::to_string(this->length));

    return this->data_[index];
}

void * HeightMap::SubRow::data(){
    return (void*)this->data_;
}


HeightMap::HeightMap(const HeightMap& other){
    this->data_ = other.data_;
    this->dimension = other.dimension;
}

HeightMap::HeightMap(HeightMap&& other){
    this->data_ = other.data_;
    this->dimension = other.dimension;
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
            throw std::out_of_range("");
    }
    this->data_.resize(this->dimension*this->dimension);
}

HeightMap::HeightMap(index_t _dimension, bool _automatic_resizes_) : dimension{_dimension}, automatic_resizes_{_automatic_resizes_} {
    this->data_.resize(this->dimension*this->dimension);
}

HeightMap::HeightMap(bool _automatic_resizes_) : automatic_resizes_{_automatic_resizes_} {
    this->data_ = {};
    this->dimension = 0;
}

HeightMap::~HeightMap(){}

HeightMap& HeightMap::operator=(std::vector<height_t>& _data_){
    if( this->automatic_resizes_ == false && _data_.size() > this->dimension*this->dimension )
        throw std::out_of_range("New Data's Size exceeds limits and 'automatic_resizes' is false");

    this->data_ = _data_;
    
    if( _data_.size() > this->dimension*this->dimension || this->automatic_resizes_ == true )
        this->update_dimension();        

    this->data_.resize(this->dimension*this->dimension);

    return *this;
}

HeightMap& HeightMap::operator=(std::vector<height_t>&& _data_){
    if( this->automatic_resizes_ == false && _data_.size() > this->dimension*this->dimension )
        throw std::out_of_range("New Data's Size exceeds limits and 'automatic_resizes' is false");

    this->data_ = std::move(_data_);

    if( _data_.size() > this->dimension*this->dimension || this->automatic_resizes_ == true )
        this->update_dimension();        

    this->data_.resize(this->dimension*this->dimension);

    return *this;
}

HeightMap& HeightMap::operator=(const HeightMap& _data_){
    this->data_ = _data_.data_;
    this->dimension = _data_.dimension;

    return *this;
}

HeightMap& HeightMap::operator=(HeightMap&& _data_){
    this->data_ = _data_.data_;
    this->dimension = _data_.dimension;

    return *this;
}

HeightMap::SubRow HeightMap::operator[](index_t row){
    return SubRow(&this->data_[row*this->dimension], this->dimension);
}

HeightMap::height_t& HeightMap::operator[](glm::vec<2, index_t> position){
    if( position.x >= this->dimension || position.y >= this->dimension )
        throw std::out_of_range("Index out of bounds");

    return this->data_[ (position.x * this->dimension) + position.y ];
}

void HeightMap::resize(index_t new_dimension){
    if( new_dimension == this->dimension )
        throw std::logic_error("Cannot resize to previous dimensions");

    if(new_dimension < this->dimension){
        std::vector<height_t> new_data(new_dimension*new_dimension);
        for(index_t i = 0, j = 0; i < this->data_.size() && j < new_data.size(); i += this->dimension){
            memcpy(&new_data[j], &this->data_[i], sizeof(height_t)*new_dimension);
            j += new_dimension;
        }
        this->data_ = std::move(new_data);
    }
    else{
        std::vector<height_t> new_data(new_dimension*new_dimension);
        for(index_t i = 0, j = 0; i < this->data_.size() && j < new_data.size(); i += this->dimension){
            memcpy(&new_data[j], &this->data_[i], sizeof(height_t)*this->dimension);
            j += new_dimension;
        }
        this->data_ = std::move(new_data);
    }

    this->dimension = new_dimension;
}

void * HeightMap::data_ptr(){
    return this->data_.data();
}

std::vector<HeightMap::height_t>& HeightMap::get_raw_vector(){
    return this->data_;
}

void HeightMap::update_dimension(){
    double root = sqrt(this->data_.size());
    if( root == std::floor(root) )
        this->dimension = root;
    else
        this->dimension = std::floor(root)+1;
}

void HeightMap::generate_image(std::string filename, double summand, double factor){
    double max = 0; double min = std::numeric_limits<double>::max();
    for(int x = 0; x < this->dimension; ++x){
        for(int y = 0; y < this->dimension; ++y){
            auto buffer = this->operator[]({x,y});
            if(buffer > max)
                max = buffer;
            if(buffer < min)
                min = buffer;
        }
    }
    summand = -min;
    factor = static_cast<double>(256)/(max-min);


    EasyBMP::Image image{static_cast<int64_t>(this->dimension), static_cast<int64_t>(this->dimension)};
    for(int x = 0; x < this->dimension; ++x){
        auto arr = this->operator[](x);
        for(int y = 0; y < this->dimension; ++y){
            auto buffer = arr[y];
            buffer += summand; buffer *= factor;
            image.SetPixel(x,y, EasyBMP::RGBColor(buffer, buffer, buffer));
        }
    }
    image.Write(filename);

}

}
/*  // Used to test the stripe function
#include "erosion.hpp"
void debug(){
    EasyBMP::Image stripes{1000,1000};
    for(double x = 0; x < stripes.w(); ++x){
        for(double y = 0; y < stripes.h(); ++y){
            double v = TerraForma::Erosion::get_stripe_value({x/40, y/40}, {.rotation_vector = {1,1}});
            v += 1; v *= 128;
            stripes.SetPixel(x,y, EasyBMP::RGBColor(v,v,v));
        }
    }
    stripes.Write("stripes.bmp");

}
*/

// I'm testing the voronoi point generation
#include "random.hpp"
void TerraForma::debug(){
    u_int32_t voronoi_dimension = 10; double erosion_scale = 100;
    std::cout << "Vornoi dimension: " << voronoi_dimension << std::endl;
    std::vector<std::vector<glm::vec<2,double>>> voronoi_points{voronoi_dimension};

    TerraForma::Random r;
    for(double x = 0; x < voronoi_points.size(); ++x){
        voronoi_points[x].resize(voronoi_dimension);
        for(double y = 0; y < voronoi_dimension; ++y){
            voronoi_points[x][y].x = erosion_scale*(x+0.2) + r.random_float(erosion_scale*0.6);
            voronoi_points[x][y].y = erosion_scale*(y+0.2) + r.random_float(erosion_scale*0.6);
        }
    }

    EasyBMP::Image image{voronoi_dimension*erosion_scale, voronoi_dimension*erosion_scale};

    for(int i = 1; i < voronoi_dimension; ++i){
        image.DrawLine(i*erosion_scale, 0, i*erosion_scale, voronoi_dimension*erosion_scale-1, EasyBMP::RGBColor(150, 40, 40));
        image.DrawLine(0, i*erosion_scale, voronoi_dimension*erosion_scale-1, i*erosion_scale, EasyBMP::RGBColor(150, 40, 40));
    }  

    for(auto& it : voronoi_points){
        for(auto& it2 : it){
            image.DrawCircle(it2.x, it2.y, erosion_scale/10, EasyBMP::RGBColor(40,200,40), true);
        }
    }
    image.Write("test2.bmp");
}