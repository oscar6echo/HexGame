
#ifndef DEF_STATE
#define DEF_STATE

#include <vector>
#include "HexGame.hpp"
#include "MCTS.hpp"
#include "Random.hpp"

using namespace std;

class State
{
  public:
    // default constructor
    State();

    // constructor from positions of Stone on board
    State(vector<Stone> &stone, int round, Stone nextStone, const MCTS &mcts, Random &random);

    // constructor from positions of Stone on board
    State(const State &state);

    // getters
    vector<Stone> getStone();
    int getNbStonePlayed();
    int getNbStoneToPlay();
    Stone getNextStone();

    // make random move
    void makeRandomMove();

    // roll out random monte carlo simul until all stones on board
    // then determine winner EW or NS
    Stone getWinnerAfterMonteCarloSimul();

  private:
    // position of stones on board
    vector<Stone> stone;

    // nb of stones played / to play
    int nbStonePlayed;
    int nbStoneToPlay;

    // next player
    Stone nextStone;

    // generate valid new stone
    int generateValidRandomNewStone();

    // random generator
    Random random;
};

#endif