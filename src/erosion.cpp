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

// 'erosion_scale' and 'erosion_strength' will be divided by 'octave_denominator' after each octave
HeightMap apply_erosion(HeightMap& base_terrain, value_t erosion_scale, value_t erosion_strength, value_t octave_denominator, u_int32_t octave_count){
    HeightMap new_terrain = base_terrain;
    for(u_int32_t i = 0; i < octave_count; ++i){
        std::cout << "Octave " << i << ": Erosion Scale: " << erosion_scale << "; Erosion Strength: " << erosion_strength << ";" << std::endl;
        new_terrain = apply_octave(new_terrain, erosion_scale, erosion_strength);

        erosion_strength /= octave_denominator;
        erosion_scale /= octave_denominator;
    }
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

    //return std::sqrt(gradient) * ((value_t)0.5 - ( (1-(gradient*gradient*40)) / (2+(gradient*gradient*80)) ));
    return (double)1-glm::pow(1-gradient,2);
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
    return *ret;
}

inline constexpr const value_t get_point_influence(const value_t distance_squared, const value_t influence_radius) noexcept {
    return glm::max(   glm::pow(  1 - ( distance_squared / glm::pow(influence_radius,2) ),  3  ),   (value_t)0   );
}

value_t get_blended_result(index_t x, index_t y, myvec2 gradient, value_t frequency, value_t frequency_offset, value_t cell_size, value_t frequency_modifier, std::vector<std::vector<myvec2>>& points){
    // Cell cordinates
    double xd = static_cast<double>(x); // d stands for double
    double yd = static_cast<double>(y); // in global space
    index_t x_idx = std::floor(xd/cell_size);  // idx stands for index; these variables will be used to index into voronoi points
    index_t y_idx = std::floor(yd/cell_size);  // index space


    value_t result = 0;

    // i use the square root of the distance to multiply the stripe value with,
    // this keeps the sum of the applied factors, for the result to be divided by
    value_t total_applied_multipliers = 0;

    // The relative indexes -2 -1 0 1 2 are checked, because in edge cases a point might belong to the point two squares away
    for(index_t x1 = std::max(0L, x_idx-2); x1 <= x_idx+2 && x1 < points.size(); ++x1){
        for(index_t y1 = std::max(0L, y_idx-2); y1 <= y_idx+2 && y1 < points[x1].size(); ++y1){
            value_t distance_squared = glm::pow(points[x1][y1].x-xd,2)+glm::pow(points[x1][y1].y-yd,2);
            myvec2 voronoi = points[x1][y1];

            // 'cell_size * 2' is the distance a neighbouring voronoi point could be away
            value_t point_influence = get_point_influence(distance_squared, cell_size*2);

            total_applied_multipliers += point_influence;

            result += get_stripe_value({xd,yd}, {.pivot_point = voronoi, .rotation_vector = gradient, 
                .frequency = frequency, .frequency_offset = (voronoi.x * frequency_modifier / cell_size)+glm::half_pi<value_t>(), .amplitude = 1}) * point_influence; 
        }
    }

    return result / total_applied_multipliers;
}


myvec2 determine_gradient_vector(index_t x, index_t y, HeightMap& heightmap){
    if( x < 0 || y < 0 || x >= heightmap.get_dimension() || y >= heightmap.get_dimension() || heightmap.get_dimension() < 2)
        throw std::out_of_range("Illegal HeightMap coordinates");

    myvec2 ret{};
    if( x > 0 )
        ret.x = heightmap[{x,y}]-heightmap[{x-1,y}];
    else
        ret.x = heightmap[{x+1,y}]-heightmap[{x,y}];

    if( y > 0 )
        ret.y = heightmap[{x,y}]-heightmap[{x,y-1}];
    else
        ret.y = heightmap[{x,y+1}]-heightmap[{x,y}];

    return ret;
}


HeightMap apply_octave(HeightMap& terrain, value_t erosion_scale, value_t erosion_strength){

    // Generate Voronoi cells
    u_int32_t voronoi_dimension = ((float)terrain.get_dimension()/erosion_scale);
    std::vector<std::vector<myvec2>> voronoi_points{voronoi_dimension};

    Random r;
    value_t spawn_area = 0.9;
    for(index_t x = 0; x < voronoi_points.size(); ++x){
        voronoi_points[x].resize(voronoi_dimension);
        for(index_t y = 0; y < voronoi_dimension; ++y){
            voronoi_points[x][y].x = erosion_scale*(x+(1-spawn_area)/2) + r.random_float(erosion_scale*spawn_area);
            voronoi_points[x][y].y = erosion_scale*(y+(1-spawn_area)/2) + r.random_float(erosion_scale*spawn_area);
        }
    }


    HeightMap new_terrain = terrain;

    // iterate through the points and figure out their stripe value
    for(index_t x = 0; x < terrain.get_dimension()-1; ++x){
        for(index_t y = 0; y < terrain.get_dimension()-1; ++y){
            auto cords = find_closest_voronoi(x,y, erosion_scale, voronoi_points);

            new_terrain[{x,y}] += get_blended_result(
                x,y,
                determine_gradient_vector(x,y, terrain),
                5/erosion_scale, (cords.x * 5 / erosion_scale)+glm::half_pi<value_t>(),
                erosion_scale, 5,
                voronoi_points
            ) * erosion_strength;
        }
    }

    return new_terrain;
}


}