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

#ifndef __TP_GENETIC_OPT_PATH__
#define __TP_GENETIC_OPT_PATH__

#include "imgops.hpp"

namespace tp {



    std::list<shape::shape_t> genetc_algorithm_optimize(
        std::list<shape::shape_t> init_list);


}



#endif
