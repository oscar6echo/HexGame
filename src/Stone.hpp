
#ifndef DEF_STONE
#define DEF_STONE

// #include <string>

using namespace std;

enum class Stone
{
    EW,  // Player East-West
    NS,  // Player North_South
    None // Empty
};

#include <iostream>
std::ostream &operator<<(std::ostream &out, const Stone &s);
// ostream &operator<<(ostream &out, const Stone &s);

#endif

