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

#include "shape.hpp"

#include <algorithm>
#include <lodepng/lodepng.h>
namespace tp {
namespace shape {

    image_t new_image(int width, int height, unsigned char c)
    {
        return std::make_tuple(width, height, std::vector<short int>(width * height, c));
    }
    void save_image(const image_t& img, const std::string& fname)
    {
        auto [width, height, data] = img;
        std::vector<unsigned int> tosave;
        std::vector<unsigned char> image(data.size() * 4);
        for (unsigned i = 0; i < data.size(); i++) {
            for (unsigned n = 0; n < 3; n++)
                image[i * 4 + n] = data[i];
            image[i * 4 + 3] = 255;
        }
        unsigned error = lodepng::encode(fname, image, width, height);
    }
    image_t load_image(const std::string& fname)
    {
        image_t img;
        auto& [width, height, data] = img;
        std::vector<unsigned char> out;
        unsigned error = lodepng::decode(out, width, height, fname);

        //if there's an error, display it
        if (error)
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

        data.resize(width*height);
        for (size_t i = 0; i < width*height; i++) data[i] = out[i*4];
        return img;
    }
    void draw_circle(image_t& img,
        const int x0, const int y0, const double r0,
        const unsigned char color)
    {
        double r2 = r0 * r0;
        for (int y = std::max(0.0, y0 - r0 - 1); y < std::min((int)std::get<1>(img), (int)(y0 + r0 + 1)); y++) {
            for (int x = std::max(0.0, x0 - r0 - 1); x < std::min((int)std::get<0>(img), (int)(x0 + r0 + 1)); x++) {
                double l = (x - x0) * (x - x0) + (y - y0) * (y - y0);
                if (l <= r2)
                    std::get<2>(img)[(int)(y * std::get<0>(img)) + (int)(x)] = color;
            }
        }
    }

    image_t draw_shape(const shape_t shape, const double radius, double depth, const int margin)
    {
        int width = 0, height = 0;
        generic_position_t<double, 4> transform;
        //generic_position_t<double, 4> multiply = { 1.0, 1.0, 1.0, 1.0 };

        for (const auto& p : shape) {
            width = std::max(width, (int)(p[0]));
            height = std::max(height, (int)(p[0]));
        }

        image_t ret = new_image(width + margin * 2, height + margin * 2, 255);
        for (const auto& p : shape) {
            if (p[2] < 0)
                draw_circle(ret, (int)(p[0]) + margin, (int)(p[0]) + margin, radius, 255 * ((depth - p[2]) / depth));
        }

        return ret;
    }

} // namespace shape
} // namespace tp
