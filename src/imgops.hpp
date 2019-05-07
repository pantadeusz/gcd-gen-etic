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

#ifndef __TP__IMGOPS__HPP___
#define __TP__IMGOPS__HPP___

#include "shape.hpp"

#include <list>

namespace tp {
namespace shape {

    /**
 * takes image, and finds edges. z is based on the color.
 * Black is the most bottom. White means the surface of the material.
 * */
    std::list<shape_t> image_to_shapes(image_t& image, int tool_d_px=10);

    inline std::list<shape_t> smoothen_shape_simple(const std::list<shape_t>& shapes)
    {
        std::list<shape_t> ret;
        for (auto& shape : shapes) {
            if (shape.size() > 2) {
                shape_t ns;
                ns.push_back(shape[0]);
                for (unsigned i = 1; i < shape.size()-1; i++) {
                    ns.push_back((shape[i-1]+shape[i]+shape[i+1])*(1.0/3.0));
                }
                ns.push_back(shape.back());
                ret.push_back(ns);
            }else {
                ret.push_back(shape);
            }
         }
         return ret;
    }

    std::vector<std::string> shapes_to_gcd(const std::list<shape_t>& shapes,
        const raspigcd::distance_t& g0feedrate,
        const raspigcd::distance_t& g1feedrate,
        const double z = 5.0);

} // namespace shape
} // namespace tp

#endif