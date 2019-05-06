#include "catch.hpp"

#include <imgops.hpp>

#include <iostream>
#include <vector>

using namespace tp::shape;
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

TEST_CASE("std::string shapes_to_gcd(...);","[imgops][shapes_to_gcd]") {
    SECTION("empty shape gives empty gcode") {
        auto ret = shapes_to_gcd({},{2.0,2.0,2.0,1.0},{2.0,2.0,2.0,1.0});
        REQUIRE(ret.size() == 0);
    }
    SECTION("empty shape gives empty gcode") {
        auto ret = shapes_to_gcd({{}},{2.0,2.0,2.0,1.0},{2.0,2.0,2.0,1.0});
        REQUIRE(ret.size() == 0);
    }
    SECTION("empty shape gives empty gcode") {
        auto ret = shapes_to_gcd({{{10,0,0,0}}},{2.0,2.0,2.0,1.0},{2.0,2.0,2.0,1.0});
        REQUIRE(ret.size() == 6);
    }
    SECTION("empty shape gives empty gcode") {
        auto ret = shapes_to_gcd({{{10,0,0,0},{10,20,0,0}}},{2.0,2.0,2.0,1.0},{2.0,2.0,2.0,1.0});
        REQUIRE(ret.size() == 7);
        std::string cmpto;
        for (auto e : ret) {
            cmpto = cmpto + e + "\n";
        }
        REQUIRE(cmpto == R"(G0X0.000000Y0.000000Z5.000000
G0X10.000000Y0.000000Z5.000000
G0X10.000000Y0.000000Z0.000000
G1X10.000000Y20.000000Z0.000000
G0X10.000000Y20.000000Z5.000000
G0X0.000000Y0.000000Z5.000000
G0X0.000000Y0.000000Z0.000000
)");
        
    }

}


TEST_CASE("std::list<shape_t> image_to_shapes(image_t& image);","[imgops][image_to_shapes]") {
    SECTION("empty image gives empty result") {
        image_t image = {};
        std::list<shape_t> ret = image_to_shapes(image);
        REQUIRE(ret.size() == 0);
    }
    SECTION("black image gives empty result") {
        image_t image = new_image(100,100,0);
        std::list<shape_t> ret = image_to_shapes(image);
        REQUIRE(ret.size() == 0);
    }
    SECTION("one pixel on image should give one shape with 8 points") {
        image_t image = new_image(100,100,0);
        std::get<2>(image)[50*100+50] = 255;
        save_image(image, "onepixel.png");
        std::list<shape_t> ret = image_to_shapes(image);
        for (auto e : ret) {
            for (auto p : e) {
                std::cout << p << " ";
            }
            std::cout  << std::endl;
        }
        REQUIRE(ret.size() == 1);
        REQUIRE(ret.front().size() == 8);
        
    }
}