
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


ostream &operator<<(ostream &out, const Stone &s);

#endif

