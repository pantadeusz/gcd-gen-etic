#include "imgops.hpp"
#include "genetic.hpp"

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
    for (auto &shape : shapes) {
        for (auto &p: shape) {
             p = p * 25.4 *(1.0/ dpi); 
        }
    }
    for (auto &shape: shapes) {
        shape = optimize_path_dp(shape, 0.1);
    }
    shapes = tp::genetc_algorithm_optimize(shapes);

    auto gcdlist = shapes_to_gcd(shapes,{10,10,1,0},{100,100,10,1});
    for (auto e : gcdlist) {
        cout << e << endl;
    }
    return 0;
}
