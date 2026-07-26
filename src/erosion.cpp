#include "erosion.hpp"

#include "random.hpp"

#include <cmath>
#include <string>


namespace TerraForma::Erosion{

value_t get_stripe_value(myvec2 position, const StripeInfo& stripe_info){
    position -= stripe_info.pivot_point;

    myvec2 normalized{glm::normalize(stripe_info.rotation_vector)};
    double rotated_x = position.x * normalized.y - position.y * normalized.x;
    rotated_x += stripe_info.pivot_point.x;

    rotated_x = glm::sin((rotated_x * stripe_info.frequency) + stripe_info.frequency_offset);
    
    return (rotated_x * stripe_info.amplitude) + stripe_info.amplitude_offset;
}

HeightMap apply_erosion(HeightMap& base_terrain, value_t erosion_scale, value_t erosion_strength, u_int32_t octave_count){
    HeightMap new_terrain = base_terrain;
    new_terrain = apply_octave(new_terrain, erosion_scale, erosion_strength);
    return new_terrain;
}

value_t get_erosion_strength_from_slope(value_t gradient){
    if( gradient < 0 )
        gradient = -gradient;

    if(gradient > 1)
        return 1;

    if( gradient == 0 )
        return 0;

    // this function will apply an erosion of 0 at gradient 0, something near 1 at gradient 1
    // i multiply the square root of x with a function (0/0):gradient=0 that asymptotically approaches 1

    return std::sqrt(gradient) * ((value_t)0.5 - ( (1-(gradient*gradient*40)) / (2+(gradient*gradient*80)) ));
}

// 'cell_size' is to used as the dimension(amount of data points) of the square one voronoi point can spawn in
myvec2 find_closest_voronoi(index_t x, index_t y, value_t cell_size, std::vector<std::vector<myvec2>>& points){
    // Cell cordinates
    double xd = static_cast<double>(x); // d stands for double
    double yd = static_cast<double>(y); // in global space
    index_t x_idx = std::floor(xd/cell_size);  // idx stands for index; these variables will be used to index into voronoi points
    index_t y_idx = std::floor(yd/cell_size);  // index space

    std::optional<myvec2> ret{};
    value_t closest_distance = std::numeric_limits<float>::max();

    // The relative indexes -2 -1 0 1 2 are checked, because in edge cases a point might belong to the point two squares away
    for(index_t x1 = std::max(0L, x_idx-2); x1 <= x_idx+2 && x1 < points.size(); ++x1){
        for(index_t y1 = std::max(0L, y_idx-2); y1 <= y_idx+2 && y1 < points[x1].size(); ++y1){
            value_t distance_buffer = glm::pow(points[x1][y1].x-xd, 2) + glm::pow(points[x1][y1].y-yd, 2);

            if(distance_buffer < closest_distance){
                closest_distance = distance_buffer;
                ret = points[x1][y1];
            }

        }
    }
    
    if( !ret.has_value() ){
        throw std::logic_error("; scale: "+std::to_string(cell_size)+"; HeightMap cords: ("+std::to_string(x)
            +'|'+std::to_string(y)+"); relative voronoi: ("+std::to_string(xd)+'|'+std::to_string(yd)+"); no pivot point found");
    }
    //if( ret.has_value() && temp_i < 1000 && temp_i % 325 == 0 ){
    //    std::cout << "Index HeightMap: (" << x << '|' << y << "); relative voronoi: (" << xd << '|' << yd << "); voronoi: (" << x_idx << '|' << y_idx << ");" 
    //              << " Pivot: (" << ret->x << '|' << ret->y << ");" << std::endl;
    //}
    return *ret;
}

HeightMap apply_octave(HeightMap& terrain, value_t erosion_scale, value_t erosion_strength){

    // Generate Voronoi cells
    u_int32_t voronoi_dimension = ((float)terrain.get_dimension()/erosion_scale);
    std::cout << "Voronoi count: " << voronoi_dimension << "; Cell size: " << erosion_scale << ';' << std::endl;
    std::vector<std::vector<myvec2>> voronoi_points{voronoi_dimension};

    Random r;
    for(index_t x = 0; x < voronoi_points.size(); ++x){
        voronoi_points[x].resize(voronoi_dimension);
        for(index_t y = 0; y < voronoi_dimension; ++y){
            voronoi_points[x][y].x = erosion_scale*(x+0.05) + r.random_float(erosion_scale*0.9);
            voronoi_points[x][y].y = erosion_scale*(y+0.05) + r.random_float(erosion_scale*0.9);
        }
    }


    HeightMap new_terrain = terrain;

    // iterate through the points and figure out their stripe value
    for(index_t x = 0; x < terrain.get_dimension()-1; ++x){
        for(index_t y = 0; y < terrain.get_dimension()-1; ++y){
            auto cords = find_closest_voronoi(x,y, erosion_scale, voronoi_points);

            value_t buffer = get_stripe_value({x,y}, 
                {.pivot_point = cords,
                 .rotation_vector = { terrain[{x+1,y}]-terrain[{x,y}], terrain[{x,y+1}]-terrain[{x,y}] },
                 .frequency = 5/erosion_scale, .frequency_offset = (cords.x * 5 / erosion_scale)+glm::half_pi<value_t>(), .amplitude = 1}
            );

            new_terrain[{x,y}] += buffer*8*get_erosion_strength_from_slope(glm::length(myvec2{ terrain[{x+1,y}]-terrain[{x,y}], terrain[{x,y+1}]-terrain[{x,y}] })*12 );
        }
    }

    return new_terrain;
}


}