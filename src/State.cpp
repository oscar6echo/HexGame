
#include <iostream>
#include <iomanip>
#include <deque>
#include <string>
#include <sstream>

#include "TreeNode.hpp"
#include "State.hpp"
#include "Random.hpp"

using namespace std;

State::State(vector<Stone> &stone, int nbStonePlayed, Stone nextStone, const MCTS &mcts, Random &random) : stone(stone),
                                                                                                           nbStonePlayed(nbStonePlayed),
                                                                                                           nextStone(nextStone),
                                                                                                           random(random){};

int State::getNbStonePlayed()
{
    return nbStonePlayed;
}

int State::getNbStoneToPlay()
{
    return nbStoneToPlay;
}

int State::generateValidRandomNewStone()
{
    vector<int> candidate;
    for (int i = 0; stone.size(); i++)
    {
        if (stone[i] == nextStone)
        {
            candidate.push_back(i);
        }
    }
    int new_hex = candidate[random.getRndNumber(candidate.size())];
    return new_hex;
}

void State::makeRandomMove()
{
    int new_hex = generateValidRandomNewStone();

    stone[new_hex] = nextStone;
    nbStonePlayed++;
    nbStoneToPlay--;

    if (nextStone == Stone::EW)
    {
        nextStone = Stone::NS;
    }
    else
    {
        nextStone = Stone::EW;
    };
}
