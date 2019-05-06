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

#include "catch.hpp"

#include <shape.hpp>

#include <iostream>
#include <vector>

using namespace tp::shape;
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

TEST_CASE( "create empty image", "[shape][new_image]" ) {
    SECTION("black image") {
        image_t img = new_image(320,200);
        REQUIRE( std::get<0>(img) == 320 );
        REQUIRE( std::get<1>(img) == 200 );
        REQUIRE( std::get<2>(img).size() == 320*200 );
    }
    SECTION("white image") {
        image_t img = new_image(10,10, 1);
        REQUIRE( std::get<0>(img) == 10 );
        REQUIRE( std::get<1>(img) == 10 );
        REQUIRE( std::get<2>(img).size() == 100 );
        auto s = std::accumulate(std::get<2>(img).begin(), std::get<2>(img).end(), 0);
        REQUIRE( s == 100 );
    }
}

TEST_CASE("void draw_circle(image_t &img);","[shape][draw_circle]") {
    image_t img = new_image(64,44);
    image_t expected_result = new_image(64,44);
    SECTION("circle inside") {
        double x0 = 10;
        double y0 = 10;
        double r0 = 6;
        for (double x = 0; x < 64; x++) {
            for (double y = 0; y < 44; y++) {
                double l = (x-x0)*(x-x0)+(y-y0)*(y-y0);
                if (l <= (r0*r0)) std::get<2>(expected_result).at((int)(y*64)+(int)(x)) = 123;
                else std::get<2>(expected_result).at(y*64+x) = 0;
            }
        }
        draw_circle(img, x0, y0, r0, 123);
        REQUIRE(std::get<2>(img) == std::get<2>(expected_result));
    }

    SECTION("circle outside") {
        double x0 = 10;
        double y0 = -100;
        double r0 = 6;
        draw_circle(img, x0, y0, r0, 123);
        REQUIRE(std::get<2>(img) == std::get<2>(expected_result));
    }

    SECTION("circle half way") {
        double x0 = 10;
        double y0 = 1;
        double r0 = 6;
        for (double x = 0; x < 64; x++) {
            for (double y = 0; y < 44; y++) {
                double l = (x-x0)*(x-x0)+(y-y0)*(y-y0);
                if (l <= (r0*r0)) std::get<2>(expected_result).at((int)(y*64)+(int)(x)) = 100;
                else std::get<2>(expected_result).at(y*64+x) = 0;
            }
        }
        draw_circle(img, x0, y0, r0, 100);
        REQUIRE(std::get<2>(img) == std::get<2>(expected_result));
    }
    SECTION("circle half way") {
        double x0 = 1;
        double y0 = 10;
        double r0 = 6;
        for (double x = 0; x < 64; x++) {
            for (double y = 0; y < 44; y++) {
                double l = (x-x0)*(x-x0)+(y-y0)*(y-y0);
                if (l <= (r0*r0)) std::get<2>(expected_result).at((int)(y*64)+(int)(x)) = 123;
                else std::get<2>(expected_result).at(y*64+x) = 0;
            }
        }
        draw_circle(img, x0, y0, r0, 123);
        REQUIRE(std::get<2>(img) == std::get<2>(expected_result));
    }
}

TEST_CASE( "image_t draw_shape(const shape_t shape, const double radius, double depth, const int margin)", "[shape][draw_shape]" ) {
    SECTION("empty shape") {
        shape_t shape = {};
        auto result = draw_shape(shape, 6, -3, 5);
        REQUIRE(std::get<0>(result) == 10);
        REQUIRE(std::get<1>(result) == 10);
        REQUIRE(std::get<2>(result).size() == 100);
    }
    SECTION("one step shape 1") {
        shape_t shape = {{0.0,0.0,-1.0,1.0}};
        auto result = draw_shape(shape, 60, -1, 5);
        REQUIRE(std::get<0>(result) == 10);
        REQUIRE(std::get<1>(result) == 10);
        auto s = std::accumulate(std::get<2>(result).begin(), std::get<2>(result).end(), 0);
        REQUIRE( s == 0 );
    }
    SECTION("one step shape 2") {
        shape_t shape = {{0.0,0.0,0.0,1.0}};
        auto result = draw_shape(shape, 60, -1, 5);
        REQUIRE(std::get<0>(result) == 10);
        REQUIRE(std::get<1>(result) == 10);
        auto s = std::accumulate(std::get<2>(result).begin(), std::get<2>(result).end(), 0);
        REQUIRE( s == 255*100 );
    }
    SECTION("one step shape 3") {
        shape_t shape = {{0.0,0.0,-1.0,1.0}};
        auto result = draw_shape(shape, 60, -2, 5);
        REQUIRE(std::get<0>(result) == 10);
        REQUIRE(std::get<1>(result) == 10);
        auto s = std::accumulate(std::get<2>(result).begin(), std::get<2>(result).end(), 0);
        REQUIRE( s == 127*100 );
    }
    SECTION("one step shape 4") {
        shape_t shape = {{0.0,0.0,-1.0,1.0}};
        auto result = draw_shape(shape, 0.5, -2, 5);
        REQUIRE(std::get<0>(result) == 10);
        REQUIRE(std::get<1>(result) == 10);
        auto s = std::accumulate(std::get<2>(result).begin(), std::get<2>(result).end(), 0);
        //int i = 0;
        //for (auto e : (std::get<2>(result))) {
        //    std::cout << ((int)(e)) <<  " " ;
        //    i++;
        //    if ((i % 10) == 0) std::cout << std::endl;
        //}
        //std::cout  << std::endl;
        save_image(result, std::string("test_log_") + __FILENAME__  + "_001.png");
        REQUIRE( s == (255*100-128) );
  
    }
}
