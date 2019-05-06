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

#include "imgops.hpp"

namespace tp {

namespace shape {


    std::list<shape_t> image_to_shapes(image_t& image)
    {
        std::list<shape_t> shapes = {};
        static std::vector<std::array<int,2>> directions_to_check = {
            { -1, 0},
            { -1, -1},
            { 0, -1},
            { 1, -1},
            { 1, 0},
            { 1, 1},
            { 0, 1},
            { -1, 1}
        };
        static std::vector<std::array<int,2>> directions_to_move = {
            { -1, 0},
            { 0, -1},
            { 1, 0},
            { 0, 1},
        };
        auto& [width, height, image_data] = image;
        if ((width == 0) || (height == 0)) return {};
        image_t image_consumed = new_image(width, height, 255);
        auto& [ic_width, ic_height, ic_image_data] = image_consumed;

        auto get_max_around = [&](auto x0, auto y0) {
            short max_found = 0;
            for (int x = x0-1; x < (x0+2); x++) {
                for (int y = y0-1; y < (y0+2); y++) {
                    max_found = std::max(max_found, image_data.at(y*width+x));
                }
            }
            return max_found;
        };
        
        auto next_edge_point = [&](int x, int y) -> std::array<int,2> {
            for (auto & d: directions_to_move) {
                int nx = x + d[0];
                int ny = y + d[1];
                if ((get_max_around(nx,ny) > image_data.at(ny*width+nx)) && (ic_image_data.at(ny*width+nx) == 255)) {
                    if ((nx < (width-1)) && (nx > 0) && (ny < (height -1)) && (ny > 0))
                    {
                        return {nx,ny};
                    }
                }
            }
            return {x,y};
        };

        auto get_shape = [&](int x0, int y0) {
            shape_t shape;
            int x = x0, y = y0;
            do {
                shape.push_back({(double)x, (double)y,(double)image_data[y * width + x],0.0});
                ic_image_data.at(y * width + x) = 0;
                auto ncpos = next_edge_point(x,y);
                if ((ncpos[0] == x) && (ncpos[1] == y)) return shape;
                x = ncpos[0];
                y = ncpos[1];
                if (!((x < (width-1)) && (x > 0) && (y < (height -1)) && (y > 0))) return shape;
            } while (true);
        };

        for (unsigned int y = 1; y < height - 1; y++) {
            for (unsigned int x = 1; x < width - 1; x++) {
                if ((get_max_around(x,y) > image_data.at(y * width + x)) && (ic_image_data.at(y*width+x) == 255)) {
                    shapes.push_back(get_shape(x, y));
                } 
            }
        }
        return shapes;
    }

    std::string position_to_g(int g, const generic_position_t<double, 4>& p)
    {
        using namespace std;
        return string("G") + to_string(g) + "X" + to_string(p[0]) + "Y" + to_string(p[1]) + "Z" + to_string(p[2])
            + ((p[3] > 0.0) ? ("F" + to_string(p[3])) : "");
    }

    std::vector<generic_position_t<double, 4>> shapes_to_gcd_jmp_to(
        generic_position_t<double, 4> p0,
        generic_position_t<double, 4> p1,
        double z)
    {
        return {
            { p0[0], p0[1], z, p1[3] },
            { p1[0], p1[1], z, p1[3] },
            { p1[0], p1[1], p1[2], p1[3] }
        };
    }
    std::vector<std::string> shapes_to_gcd(const std::list<shape_t>& shapes,
        const raspigcd::distance_t& g0feedrate,
        const raspigcd::distance_t& g1feedrate,
        double z)
    {
        std::vector<std::string> ret;
        generic_position_t<double, 4> current_pos = { 0, 0, 0, 0 };
        for (const auto shape : shapes) {
            if (shape.size() > 0) {
                for (auto e : shapes_to_gcd_jmp_to(current_pos, shape.at(0), z)) {
                    ret.push_back(position_to_g(0, e));
                }
                for (auto e : std::vector<distance_t>(shape.begin() + 1, shape.end())) {
                    ret.push_back(position_to_g(1, e));
                }
                current_pos = shape.back();
            }
        }
        if (ret.size() > 0) {
            for (auto e : shapes_to_gcd_jmp_to(current_pos, { 0, 0, 0, 0 }, z)) {
                ret.push_back(position_to_g(0, e));
            }
        }
        return ret;
    }
} // namespace shape
} // namespace tp