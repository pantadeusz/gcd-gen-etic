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

#include <random>
#include <tuple>
#include <vector>

namespace tp {

using std::pair;
using std::vector;

std::random_device m_seed;
std::mt19937 randgen(m_seed());

// indeksy w liscie punktow do odwiedzenia
using chromosome_t = std::pair<double, std::vector<int>>;

auto ga = [](
              auto init_population,
              auto fitness,
              auto selection,
              auto crossover,
              auto mutation,
              auto term_condition) -> vector<chromosome_t> {
    vector<chromosome_t> population = init_population();
    int iteration = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < population.size(); i++)
        population[i] = fitness(population[i]);
    while (term_condition(iteration, population)) {
        auto parents = selection(population);
        auto offspring = crossover(parents);
        offspring = mutation(offspring);
        population = offspring;
        #pragma omp parallel for
        for (size_t i = 0; i < population.size(); i++)
            population[i] = fitness(population[i]);

        iteration++;
    }
    return population;
};

vector<chromosome_t> selection(vector<chromosome_t>& pop)
{
    vector<chromosome_t> new_pop;
    std::uniform_int_distribution rand_dist(0, (int)pop.size() - 1);
    // #pragma omp parallel for
    for (size_t i = 0; i < pop.size(); i++) {
        chromosome_t specimen[2] = {
            pop.at(rand_dist(randgen)),
            pop.at(rand_dist(randgen))
        };
        new_pop.push_back((specimen[0].first > specimen[1].first) ? specimen[0] : specimen[1]);
    }
    return new_pop;
}
chromosome_t get_random_chromosome(const std::vector<int>& base_shapes)
{
    chromosome_t ret = { 0.0, {} };
    ret.second = base_shapes;
    std::random_shuffle(ret.second.begin(), ret.second.end()); //,randgen);
    return ret;
}

vector<chromosome_t> init_population(int pop_size, const std::vector<int>& base_shapes)
{
    vector<chromosome_t> population;
    population.reserve(pop_size);
    for (int i = 0; i < pop_size; i++) {
        population.push_back(get_random_chromosome(base_shapes));
    }
    return population;
}

auto print_list = [](auto c, auto l, auto f) {
    std::cerr << c;
    for (auto e : l)
        std::cerr << "\t" << e;
    std::cerr << f;
};

vector<chromosome_t> crossover_one_pt(vector<chromosome_t>& parents)
{
    std::uniform_int_distribution rand_dist(0, 100);
    vector<chromosome_t> ret;
    for (unsigned int i = 0; i < parents.size(); i += 2) {
        std::uniform_int_distribution rand_crosspoint((size_t)1, parents[i].second.size());
        chromosome_t a0 = parents[i], b0 = parents[i + 1];
        auto del_val = [&](auto c, int i) {
            chromosome_t ret;
            ret.first = c.first;
            for (auto e : c.second)
                if (i != e)
                    ret.second.push_back(e);
            return ret;
        };
        if (rand_dist(randgen) < 50) {
            int crosspoint = rand_crosspoint(randgen);
            chromosome_t a, b;
            chromosome_t a0l = a0, b0l = b0;
            for (int j = 0; j < a0.second.size(); j++) {
                if (j < crosspoint) {
                    a.second.push_back(a0.second[j]);
                    b.second.push_back(b0.second[j]);
                } else {
                    std::vector<int>::iterator it = std::find(a.second.begin(), a.second.end(), b0.second[j]);
                    if (it == a.second.end())
                        a.second.push_back(b0.second[j]);
                    it = std::find(b.second.begin(), b.second.end(), a0.second[j]);
                    if (it == b.second.end())
                        b.second.push_back(a0.second[j]);
                }
            }
            for (auto idx : a.second)
                a0l = del_val(a0l, idx);
            for (auto idx : b.second)
                b0l = del_val(b0l, idx);
            for (auto idx : a0l.second)
                a.second.push_back(idx);
            for (auto idx : b0l.second)
                b.second.push_back(idx);

            ret.push_back(a);
            ret.push_back(b);
            if (a0.second.size() != a.second.size()) {
                throw std::invalid_argument("WRONG SIZES");
            }
            if (b0.second.size() != b.second.size()) {
                throw std::invalid_argument("WRONG SIZES");
            }
        } else {
            ret.push_back(a0);
            ret.push_back(b0);
        }
    }
    return ret;
}
vector<chromosome_t> mutation_swap(vector<chromosome_t>& parents)
{
    std::uniform_int_distribution rand_dist(0, 100);
    vector<chromosome_t> ret;
    for (unsigned int i = 0; i < parents.size(); i++) {
        if (rand_dist(randgen) < 20) {
            std::uniform_int_distribution rand_dist_p((size_t)0, parents[i].second.size() - 1);
            auto e = parents[i];
            auto a = rand_dist_p(randgen);
            auto b = rand_dist_p(randgen);
            if (a != b) {
                auto tmp = e.second.at(a);
                e.second.at(a) = e.second.at(b);
                e.second.at(b) = tmp;
            }
            ret.push_back(e);
        } else {
            ret.push_back(parents[i]);
        }
    }
    return ret;
}

bool is_inside(shape::shape_t current_shape, shape::shape_t shape_to_test, double min_dist = 2) {
    if ((shape_to_test.front()-shape_to_test.back()).length() > min_dist) return false;
    int x0=-1,y0=-1,x1=-1,y1=-1;
    for (auto e : shape_to_test) {
        if (x0 == -1) x0 = e[0];
        if (y0 == -1) y0 = e[1];
        if (x1 == -1) x1 = e[0];
        if (y1 == -1) y1 = e[1];
        x0 = std::min(x0,(int)e[0]);
        y0 = std::min(y0,(int)e[1]);
        x1 = std::max(x1,(int)e[0]);
        y1 = std::max(y1,(int)e[1]);
    }
    return (x0 < current_shape.front()[0]) && 
    (x1 > current_shape.front()[0]) &&
    (y0 < current_shape.front()[1])  &&
    (y1 > current_shape.front()[1]); 
}

std::list<shape::shape_t> genetc_algorithm_optimize(
    std::list<shape::shape_t> init_list)
{

    int max_iterations = 100;

    std::vector<shape::shape_t> shapes(init_list.begin(), init_list.end());
    std::vector<int> init_order_shapes;
    for (int i = shapes.size() - 1; i >= 0; i--)
        init_order_shapes.push_back(i);

    auto fitness = [&](chromosome_t& chromosome) -> chromosome_t {
        chromosome_t ret = chromosome;
        double sum_distance = 0.1;
        double penalty = 0;
        raspigcd::distance_t pos = {};
        for (int idx = 0; idx < chromosome.second.size(); idx++) {
            auto& e = chromosome.second[idx];
            if (shapes[e].size() > 0) {
                sum_distance += (shapes[e].front() - pos).length();
                pos = shapes[e].back();
                for (int j = 0; j < idx; j++) {
                    if(is_inside(shapes[e],shapes[chromosome.second[j]])) {
                        penalty += 1;
                        break;
                    };
                }
            }
        }
        ret.first = (1000.0 / sum_distance)/(1.0 + penalty);
        return ret;
    };

    double best_fit = 0;
    int best_fit_iterations = 0;
    auto term_condition = [&](int iteration, auto& population) {
        std::cerr << "population[" << iteration << "]:";
        for (size_t i = 0; i < population.size(); i++)
            std::cerr << " " << population[i].first;
        std::cerr << std::endl;
        for (auto &[fit,v]:population) {
            if (fit > best_fit) {
                best_fit = fit;
                best_fit_iterations = 0;
            }
        }
        best_fit_iterations++;
        if (best_fit_iterations > 30) {
            std::cerr << "best_fit_iterations hit at " << iteration << std::endl;
            return false;
        }
        return (iteration < max_iterations);
    };
    auto result = ga(
        [&]() { return init_population(std::min((int)init_list.size() * 6,100), init_order_shapes); },
        fitness,
        selection,
        crossover_one_pt,
        mutation_swap,
        term_condition);
    auto best = result.front();
    for (auto& e : result) {
        if (e.first > best.first)
            best = e;
    }
    std::list<shape::shape_t> result_shapes;
    for (auto i : best.second)
        result_shapes.push_back(shapes[i]);
    std::cerr << "Best fitness is " << best.first << std::endl;
    return result_shapes;
}
} // namespace tp