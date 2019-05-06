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
#include "genetic.hpp"
#include "imgops.hpp"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <regex>
#include <streambuf>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

int main(int argc, char** argv)
{
    using namespace std;
    using namespace tp::shape;
    double dpi = 600;
    if (argc < 3) {
        std::cout << "calling\n";
        std::cout << argv[0] << " [filename] [resolution]\n";
        return 0;
    }
    auto loaded_image = load_image(argv[1]);
    if (std::get<2>(loaded_image).size() == 0) {
        std::cout << "image empty !" << std::get<0>(loaded_image) << " " << std::get<1>(loaded_image) << std::endl;
        return -1;
    }
    std::list<shape_t> shapes = image_to_shapes(loaded_image);
    shapes = smoothen_shape_simple(shapes);
    shapes = smoothen_shape_simple(shapes);
    // scale X and Y to the given resolution. 
    // scale z to 1
    // keep feedrate untouched
    for (auto& shape : shapes) {
        for (auto& p : shape) {
            p[2] = p[2]/255.0;
            p[0] = p[0] * 25.3999991872 * (1.0 / dpi);
            p[1] = p[1] * 25.3999991872 * (1.0 / dpi);
        }
    }
    for (auto& shape : shapes) {
        shape = optimize_path_dp(shape, 0.1);
    }
    shapes = tp::genetc_algorithm_optimize(shapes);

    auto gcdlist = shapes_to_gcd(shapes, { 10, 10, 1, 0 }, { 100, 100, 10, 1 });
    for (auto e : gcdlist) {
        cout << e << endl;
    }
    return 0;
}
