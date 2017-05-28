

#ifndef DEF_RANDOM
#define DEF_RANDOM

#include <random>

using namespace std;

class Random
{
  public:
    // constructor
    Random();

    // destructor
    ~Random();

    // getter
    int getSeed();

    // getter
    mt19937 getGen();

    // convenient access
    int getRndNumber(int low, int high);

  private:
    int seed;    // seed number
    mt19937 gen; // random generator
};

#endif
