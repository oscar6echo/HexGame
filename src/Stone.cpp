

#include <iostream>

#include "Stone.hpp"

using namespace std;

ostream &operator<<(ostream &out, const Stone &s)
{
    string so;
    switch (s)
    {
    case Stone::EW:
        so = "EW";
        break;
    case Stone::NS:
        so = "NS";
        break;
    case Stone::None:
        so = "_";
        break;
    }
    out << so;
    return out;
}
