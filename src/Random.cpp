

#include <random>
#include <chrono>

#include "Random.hpp"

using namespace std;

Random::Random()
{
    int seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    gen = gen;
};

Random::~Random(){};

int Random::getSeed()
{
    return seed;
};

mt19937 Random::getGen()
{
    return gen;
};

int Random::getRndNumber(int low, int high)
{
    uniform_int_distribution<> distr(low, high); // define the range low/high included
    return distr(gen);
};
