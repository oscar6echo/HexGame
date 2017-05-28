
#include <iostream>
#include <limits>
#include <deque>

#include "TreeNode.hpp"
#include "Random.hpp"

using namespace std;


////////////////////////// MAIN PARAMS TO SET IN THIS FILE ////////////////////

// MAX_NB_GAME
// maximum time allowed to machine to compute one move
// default value: 500k

// MAX_TIME
// maximum time allowed to machine to compute one move
// default value: 40 seconds

// EXPLORATION_PARAM in computeUCT()
// manages balance between exploitation term and exploration term
// default value: 2

// MAX_TREE_DEPTH
// limits the depth of the monte carlo tree
// to try and fine tune
// default value: 4

//////////////////////////////////////////////////////////////////////////////


// constructor
TreeNode::TreeNode(TreeNode *ptr_root,
                   TreeNode *ptr_parent,
                   vector<Stone> _stone,
                   int _lastStonePos,
                   Stone _lastStone,
                   Stone _nextStone,
                   vector<vector<int>> &adjListInclSourceEnd,
                   Random &random,
                   int _depth) : ptr_root(ptr_root),
                                 ptr_parent(ptr_parent),
                                 stone(_stone),
                                 lastStonePos(_lastStonePos),
                                 lastStone(_lastStone),
                                 nextStone(_nextStone),
                                 ptr_adjListInclSourceEnd(&adjListInclSourceEnd),
                                 ptr_random(&random),
                                 depth(_depth)
{
    // init
    nbVisit = 0;
    nbWin = 0;
    gen = ptr_random->getGen();

    for (int u = 0; u < stone.size(); u++)
    {
        if (stone[u] == Stone::None)
            subset.push_back(u);
    }
};

// destructor
TreeNode::~TreeNode(){};

// getter
int TreeNode::getNbVisit()
{
    return nbVisit;
};

// getter
int TreeNode::getNbWin()
{
    return nbWin;
};

// getter
Stone TreeNode::getNextStone()
{
    return nextStone;
};

// getter
Stone TreeNode::getLastStone()
{
    return lastStone;
};

// getter
int TreeNode::getLastStonePos()
{
    return lastStonePos;
};

// getter - with calculation
float TreeNode::getAvgNbWin()
{
    return static_cast<float>(nbWin) / static_cast<float>(nbVisit);
}

// top function: run simulation from root node
void TreeNode::runMonteCarloTreeSearch()
{
    cout << "\tUntil the sooner of (i) 2m games and (ii) 40 seconds" << endl
         << "\tOne dot represent a 10000 random (Monte Carlo Tree Search) games" << endl
         << "\t";

    chrono::time_point<std::chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

    int MAX_NB_GAME = 2e6; // max number of simulated games
    int MAX_TIME = 40;     // 40 seconds

    int nbRndGame = 0;
    while (true)
    {
        for (int i = 0; i < 10000; i++)
        {
            runOneRandomGame(); // run a random game
            nbRndGame++;
        }
        cout << "." << flush;
        end = chrono::system_clock::now();
        elapsed_seconds = end - start;
        if ((nbRndGame >= MAX_NB_GAME) || (elapsed_seconds.count() > MAX_TIME))
            break;
    }

    cout << endl
         << "\tMachine simulated " << nbRndGame << " games in " << elapsed_seconds.count() << " s" << endl;
};

// main function - run a random game going down (and building along the way) the monte carlo tree
void TreeNode::runOneRandomGame()
{
    int MAX_TREE_DEPTH = 3;
    if (children.size() == 0)
    // leaf node
    {
        if (nbVisit == 0)
        // never visited --> rollout
        {
            bool win = randomRolloutFromHere();
            updateTree(win);
        }
        else
        // visited before
        {
            if (!isTerminal() && depth < MAX_TREE_DEPTH)
            // --> ARTIFICIAL LIMIT TO MONTE CARLO SEARCH TREE DEPTH - TO FINE TUNE
            // expand if not terminal node
            // then continue go down tree through any node - random choice
            {
                addChildren();
                int k = ptr_random->getRndNumber(0, children.size() - 1);
                (children.find(subset[k])->second).runOneRandomGame();
            }
            else
            // other wise ie terminal or max depth -> rollout
            {
                bool win = randomRolloutFromHere();
                updateTree(win);
            }
        }
    }
    else
    // not leaf node -> select best node (UCT criterion)
    // then continue game through it
    {
        int selected_node = selectNode();
        (children.find(selected_node)->second).runOneRandomGame();
    }
};

// read best next move from tree - from depth=1 nodes UCT score to be more specific
int TreeNode::getBestMove()
{
    cout << endl
         << "Monte Carlo ratio wins per next move: (row,col)=ratioWin(#nbVisit)" << endl;
    int bestMove = -1;
    float bestRatioWin = -1;

    for (int j = 0; j < subset.size(); j++)
    {
        int dim = static_cast<int>(sqrt(stone.size()));
        TreeNode *ptr_child = &(children.find(subset[j])->second);
        int r = static_cast<int>(subset[j] / dim);
        int c = subset[j] % dim;
        int nbWin2 = (*ptr_child).getNbWin();
        int nbVisit2 = (*ptr_child).getNbVisit();
        float ratioWin = static_cast<float>(nbWin2) / static_cast<float>(nbVisit2);
        cout << "(" << r << "," << c << ")=" << ratioWin << "(" << nbVisit2 << ") ";
        if (ratioWin > bestRatioWin)
        {
            bestMove = subset[j];
            bestRatioWin = ratioWin;
        }
    }
    cout << endl;
    return bestMove;
};

// back propagate random game result up the tree
// increment nbVisit and nbWin (if applicable) in all nodes from leaf to root
void TreeNode::updateTree(bool win)
{
    nbVisit++;
    if (win)
    {
        nbWin++;
    }
    if (ptr_parent)
    {
        ptr_parent->updateTree(win);
    }
};

// select node to go go down the tree
// based on highest UCT score
int TreeNode::selectNode()
{
    float score;
    float best_score = numeric_limits<float>::lowest();
    int best_node = -1;
    for (int u = 0; u < subset.size(); u++)
    {
        score = (children.find(subset[u])->second).computeUCT();
        if (score > best_score)
        {
            best_node = subset[u];
            best_score = score;
        }
    }
    return best_node;
};

// expand leaf node by adding all children below it - one per next possible move
void TreeNode::addChildren()
{
    Stone otherStone = (nextStone == Stone::EW) ? Stone::NS : Stone::EW;
    auto pRoot = (depth == 0) ? this : ptr_root;

    for (int u = 0; u < subset.size(); u++)
    {
        auto stone2 = stone;
        stone2[subset[u]] = nextStone;
        // auto child = TreeNode(pRoot, this, stone2, subset[u], nextStone, otherStone, *ptr_adjListInclSourceEnd, *ptr_random, depth + 1);
        // children.insert(pair<int, TreeNode>(subset[u], child));
        children.insert(pair<int, TreeNode>(subset[u], TreeNode(pRoot, this, stone2, subset[u], nextStone, otherStone, *ptr_adjListInclSourceEnd, *ptr_random, depth + 1)));
    }
};

// rollout random play until full game (even if winner is certain before - as not reversible)
bool TreeNode::randomRolloutFromHere()
{
    if (isTerminal())
        return hasMachineWon(stone);

    Stone otherStone = (nextStone == Stone::EW) ? Stone::NS : Stone::EW;
    vector<Stone> stone2 = stone;
    vector<int> subset2 = subset;
    shuffle(subset2.begin(), subset2.end(), gen);
    for (int i = 0; i < subset2.size(); i = i + 2)
        stone2[subset2[i]] = nextStone;
    for (int i = 1; i < subset2.size(); i = i + 2)
        stone2[subset2[i]] = otherStone;
    bool win = hasMachineWon(stone2);
    return win;
};

// check if terminal node ie board is full ie no more move possible
bool TreeNode::isTerminal()
{
    int nbEmpty = 0;
    for (int u = 0; u < stone.size(); u++)
    {
        if (stone[u] == Stone::None)
            nbEmpty++;
    }
    bool boardFull = (nbEmpty == 0);
    return boardFull;
};

// compute UCT function -> crucial to determine how the explore the monte carlo tree
float TreeNode::computeUCT()
{
    // See wikipedia for more info: https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
    float EXPLORATION_PARAM = 2; // PARAM TO FINE TUNE

    int rootNbVisit = (ptr_root) ? ptr_root->getNbVisit() : nbVisit;
    Stone machineStone = (ptr_root) ? ptr_root->getNextStone() : nextStone;

    float score;
    if (nbVisit == 0)
    {
        // exploration term is infinite
        score = numeric_limits<float>::max();
        return score;
    }

    // exploitation term from machine perspective
    float exploitation = static_cast<float>(nbWin) / static_cast<float>(nbVisit);
    // human turn to play - convert to human perspective - symmetric
    if (nextStone != machineStone)
        exploitation = 1.0 - exploitation;

    // exploration term
    float exploration = sqrt(log(static_cast<float>(rootNbVisit) / (static_cast<float>(nbVisit))));

    // score is combination of exploitation and exploration terms
    // relative importance is controled by EXPLORATION_PARAM
    score = exploitation + EXPLORATION_PARAM * exploration;

    return score;
};

// determine if machine has won - from full board positions stone2 and adjacency list representing hexboard
bool TreeNode::hasMachineWon(vector<Stone> &stone2)
{
    int nbHex = stone.size();
    int source, end;
    vector<bool> subset2;
    subset2.resize(nbHex + 4, false);
    Stone machineStone = (ptr_root) ? ptr_root->getNextStone() : nextStone;

    for (int u = 0; u < nbHex; u++)
    {
        if (stone2[u] == machineStone)
            subset2[u] = true;
    }
    if (machineStone == Stone::EW)
    {
        source = nbHex;
        end = nbHex + 1;
        subset2[nbHex] = true;
        subset2[nbHex + 1] = true;
    }
    else
    {
        source = nbHex + 2;
        end = nbHex + 3;
        subset2[nbHex + 2] = true;
        subset2[nbHex + 3] = true;
    }

    vector<bool> visited;
    visited.resize(nbHex + 4, false);
    deque<int> queue;

    int u = source;
    visited[u] = true;
    queue.push_back(u);

    while (!queue.empty())
    {
        u = queue.front();
        queue.pop_front();
        for (auto i : (*ptr_adjListInclSourceEnd)[u])
        {
            if (subset2[i] && !visited[i])
            {
                visited[i] = true;
                queue.push_back(i);
            }
        }
    }

    bool win = visited[end];
    return win;
}

// debug
void TreeNode::showGamePlay()
{
    cout << "Game Played" << endl
         << " pos:stone on board (from last to first): ";
    int pos;
    Stone s;
    pos = getLastStonePos();
    s = getLastStone();
    if (pos >= 0)
        cout << pos << ":" << s << " ";

    auto ptr = ptr_parent;
    while (ptr)
    {
        pos = ptr->getLastStonePos();
        s = ptr->getLastStone();
        if (pos >= 0)
            cout << pos << ":" << s << " ";
        ptr = ptr->ptr_parent;
    }
    cout << endl;
};