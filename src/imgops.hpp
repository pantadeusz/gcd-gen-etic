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
    std::list<shape_t> image_to_shapes(image_t& image);

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