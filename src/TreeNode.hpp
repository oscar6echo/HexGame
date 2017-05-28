
#ifndef DEF_TREENODE
#define DEF_TREENODE

#include <vector>
#include <map>
#include <memory>
#include <random>

#include "Stone.hpp"
#include "Random.hpp"

using namespace std;

class TreeNode
{
  public:
    // constructor from parent with given board
    TreeNode(TreeNode *ptr_root,                        // pointer to root
             TreeNode *ptr_parent,                      // pointer to parent
             vector<Stone> stone,                       // vector of stones on the hexboard representing the state of the game
             int lastStonePos,                          // position in 1d representation of last stone put
             Stone lastStone,                           // type of last stone put (EW or NS)
             Stone nextStone,                           // type of next stone to be played
             vector<vector<int>> &adjListInclSourceEnd, // ref to adjacency list representing the hexboard - used to determine winner
             Random &random,                            //random source
             int depth);                                // depth of node in tree - debug

    // destructor
    ~TreeNode();

    // run Monte Carlo Tree Search algo
    void runMonteCarloTreeSearch();

    // get best move from the current state of the tree
    int getBestMove();

    // getters
    int getNbVisit();
    int getNbWin();
    int getLastStonePos();
    Stone getLastStone();
    Stone getNextStone();
    float getAvgNbWin();

  private:
    // root - top node
    TreeNode *ptr_root;

    // parent
    TreeNode *ptr_parent;

    // board status and last/next stone and last stone pos
    vector<Stone> stone;
    int lastStonePos;
    Stone lastStone;
    Stone nextStone;

    // adjacency list - to determine winner after rollout
    vector<vector<int>> *ptr_adjListInclSourceEnd;

    // random source
    Random *ptr_random;
    mt19937 gen;

    // state variables
    int nbVisit;
    int nbWin;
    int depth;

    // empty hex ie possible next moves
    vector<int> subset;

    // children
    map<int, TreeNode> children;

    // selection next node by UCT() when going donw the MCST
    int selectNode();

    // expansion - create all children under one leaf node ie one per empty hex
    void addChildren();

    // simulation - play randomly (and legally) until full board - returns true if machine wins else false
    bool randomRolloutFromHere();

    // back propagation - propagate boolean win up the tree - recursively to root
    void updateTree(bool win);

    // whether node is terminal ie board is full
    bool isTerminal();

    // compute UCT function for a node - higher score favors selection
    float computeUCT();

    // check if machine has won with final board stone2
    bool hasMachineWon(vector<Stone> &stone2);

    // run one random game ie go down the MCTS until leaf node
    // the possibly expand it
    // then roll out to full board
    // then determine winner
    // then propagate result up the tree
    void runOneRandomGame();

    // debug
    void showGamePlay();
};

#endif
