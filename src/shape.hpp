/*

    This is the gcode generator from image that uses genetic algorithm for optimization of path
    Copyright (C) 2019  Tadeusz Puźniakowski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.


*/

#ifndef __SHAPE_TP_HPP___
#define  __SHAPE_TP_HPP___

#include "distance_t.hpp"
#include <vector>

namespace tp {
namespace shape {
    using namespace raspigcd;

    // X, Y, Z, F
    using shape_t = std::vector < generic_position_t <double, 4> >;

    // width, height, data
    using image_t = std::tuple<unsigned,unsigned,std::vector<short int> >;

    /**
     * create new image with given resolution. Black and white
     * */
    image_t new_image(int width, int height, unsigned char c = 0);

    void save_image(const image_t &img, const std::string &fname );
    image_t load_image( const std::string &fname );

    void draw_circle(image_t &img, 
    const int x, const int y, const double r, 
    const unsigned char color);

    /**
     * draws shape on image.
     * 
     * It takes x,y, and depth
     * 
     * the deeper the depth (negative values) the darker it is
     * */
    image_t draw_shape(const shape_t shape, const double radius, double depth, const int margin);
}
}

#endif
