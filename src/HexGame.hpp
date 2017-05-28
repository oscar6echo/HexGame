
#ifndef DEF_HEXBOARD
#define DEF_HEXBOARD

#include <vector>

#include "Stone.hpp"
#include "Hex.hpp"
#include "Random.hpp"

using namespace std;

class HexGame
{
  public:
    // constructor for hex graph of dimensions dim x dim
    // if dim = 0 ask player to choose dim
    HexGame(int dim = 0);

    // desctructor
    ~HexGame();

    // getters
    int getDim();
    int getNbHex();
    int getNbEdge();

    // getters
    vector<int> getAdjList(int u);
    bool getAdjMatrix(int u, int v);
    int getNbStone(Stone s);

    // display
    void showAdjMatrix();
    void showAdjMatrixCompact();
    void showAdjList(bool plustwo = false);
    void showSummary();
    void showBoard();

    // play game: full game
    void playGame();

  private:
    // hex grid dimension
    int dim;
    // number of Hex on the board = dimension^2
    int nbHex;

    // 2 graph representations
    vector<vector<int>> adjList;
    vector<vector<bool>> adjMatrix;

    // extra graph representations = adjList + extra source node and end node
    // source node is west of the west border and connected to all west most hex
    // source node index is nbHex
    // end node is east of the east border and all east most hex are connected to it
    // end node index is nbHex+1
    vector<vector<int>> adjListInclSourceEnd;

    // value of hex ie stone on hex
    vector<Stone> stone;

    // latest stone added by each player
    int latestStoneEW;
    int latestStoneNS;

    // convert 2d coords to 1d
    int hex_1d(Hex h);

    // convert 1d coord to 2d
    Hex hex_2d(int n);

    // connect 2 hex - update adjList and adjMatrix correspondingly
    void connectHex(Hex h1, Hex h2);

    // connect source and ends to relevant border
    void connectSourceWest();
    void connectSourceNorth();
    void connectEndEast();
    void connectEndSouth();

    // game play
    // machine algo selected
    int machineAlgo;

    // human & machine stones
    Stone humanStone;
    Stone machineStone;

    // command line input - under control
    vector<int> askNumber(int n, int limitLow, int limitHigh);
    bool askYesNo();

    // init questions
    int askWhichOpponent();
    bool askIfHumanPlaysFirst();

    // game play steps
    void humanPlays();
    void machinePlays();

    // check if stone s has won
    bool hasWonStone(Stone &s, vector<Stone> &stone2);
    // check if player has won
    bool humanHasWon();
    bool machineHasWon();

    // game play - action
    bool putStone(Stone s, int u);

    // machine AI
    int posFromAlgoRandom();
    int posFromAlgoMonteCarloNaive();
    int posFromAlgoMonteCarloTreeSearch();

    // random object- note: no need to instantiate it: default generator does it
    Random random;
};

#endif
