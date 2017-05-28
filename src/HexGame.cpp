
#include <iostream>
#include <iomanip>
#include <deque>
#include <string>
#include <sstream>
#include <chrono>

#include "Color.hpp"
#include "HexGame.hpp"
#include "Random.hpp"
#include "TreeNode.hpp"

using namespace std;

// constructor
HexGame::HexGame(int _dim) : dim(_dim)
{
    if (dim == 0)
    {
        cout << endl
             << "BUILDING HEXBOARD" << endl
             << "Choose dimension (recommended between 2 and 20)" << endl
             << "dimension = ?" << endl;
        dim = askNumber(1, 2, 25)[0];
    }
    nbHex = dim * dim;
    stone.resize(nbHex, Stone::None);
    adjList.resize(nbHex);
    adjListInclSourceEnd.resize(nbHex + 4);
    adjMatrix.resize(nbHex);
    latestStoneEW = -1;
    latestStoneNS = -1;

    for (int r = 0; r < nbHex; r++)
    {
        adjMatrix[r].resize(nbHex, false);
    }

    for (int r = 0; r < dim; r++)
    {
        for (int c = 0; c < dim; c++)
        {
            if (r > 0)
            {
                connectHex(Hex(r, c), Hex(r - 1, c)); // up
            }
            if (r > 0 && c < dim - 1)
            {
                connectHex(Hex(r, c), Hex(r - 1, c + 1)); // up right
            }
            if (c > 0)
            {
                connectHex(Hex(r, c), Hex(r, c - 1)); // left
            }
            if (c < dim - 1)
            {
                connectHex(Hex(r, c), Hex(r, c + 1)); // right
            }
            if (r < dim - 1 && c > 0)
            {
                connectHex(Hex(r, c), Hex(r + 1, c - 1)); // down left
            }
            if (r < dim - 1)
            {
                connectHex(Hex(r, c), Hex(r + 1, c)); // down
            }
        }
    }

    connectSourceWest();
    connectEndEast();
    connectSourceNorth();
    connectEndSouth();

    showSummary();
    cout << "UNDER THE HOOD:" << endl
         << "Do you want to see the adjacency Matrix" << endl;
    if (askYesNo())
        showAdjMatrixCompact();
    cout << "Do you want to see the adjacency List" << endl;
    if (askYesNo())
        showAdjList(false);
    cout << "Do you want to see the adjacency List " << endl
         << "incl. the extra sources/ends" << endl
         << "\t" << nbHex << " = West source" << endl
         << "\t" << nbHex + 1 << " = East end" << nbHex + 1 << endl
         << "\t" << nbHex + 2 << " = North source" << nbHex + 2 << endl
         << "\t" << nbHex + 3 << " = South end" << endl;
    if (askYesNo())
        showAdjList(true);
};

HexGame::~HexGame()
{
}

// ask player to input n number between limitLow and limitHigh
vector<int> HexGame::askNumber(int n, int limitLow, int limitHigh)
{
    while (true)
    {
        string line;
        getline(cin, line);
        istringstream is(line);
        vector<int> data;
        int x;
        while (is >> x)
            data.push_back(x);

        bool withinLimits = true;
        for (auto e : data)
        {
            if (e < limitLow || e > limitHigh)
                withinLimits = false;
        }
        if (data.size() == n && withinLimits)
        {
            return data;
        }
        else
        {
            cout << "INVALID: You must input " << n << " integers each between " << limitLow << " and " << limitHigh << endl;
        }
    }
}

// ask player to answer yes or no
bool HexGame::askYesNo()
{
    char answer;
    do
    {
        cout << "You answer [y/n] ? ";
        cin >> answer;
    } while (!cin.fail() && answer != 'y' && answer != 'n');
    cin.ignore();
    if (answer == 'y')
        return true;
    return false;
}

// convert hex from 2d (obj) to 1d int
int HexGame::hex_1d(Hex h)
{
    return h.r * dim + h.c;
}

// convert from 1d int to 2d (obj)
Hex HexGame::hex_2d(int n)
{
    int r = n / dim;
    int c = n % dim;
    return Hex(r, c);
}

// in context of creating graph associated with hexboard connect to hex
void HexGame::connectHex(Hex h1, Hex h2)
{
    int n1 = hex_1d(h1);
    int n2 = hex_1d(h2);
    adjMatrix[n1][n2] = true;
    adjMatrix[n2][n1] = true;
    adjList[n1].push_back(n2);
    adjListInclSourceEnd[n1].push_back(n2);
}

// in context of creating graph associated with hexboard connect West source to all West hex
// used to determine winner
void HexGame::connectSourceWest()
{
    // source
    int s = nbHex;
    for (int r = 0; r < dim; r++)
    {
        int n = hex_1d(Hex(r, 0));
        adjListInclSourceEnd[s].push_back(n);
    }
}

// in context of creating graph associated with hexboard connect North source to all North hex
// used to determine winner
void HexGame::connectSourceNorth()
{
    // source
    int s = nbHex + 2;
    for (int c = 0; c < dim; c++)
    {
        int n = hex_1d(Hex(0, c));
        adjListInclSourceEnd[s].push_back(n);
    }
}

// in context of creating graph associated with hexboard connect East end to all East hex
// used to determine winner
void HexGame::connectEndEast()
{
    // end
    int e = nbHex + 1;
    for (int r = 0; r < dim; r++)
    {
        int n = hex_1d(Hex(r, dim - 1));
        adjListInclSourceEnd[n].push_back(e);
    }
}

// in context of creating graph associated with hexboard connect South end to all South hex
// used to determine winner
void HexGame::connectEndSouth()
{
    // end
    int e = nbHex + 3;
    for (int c = 0; c < dim; c++)
    {
        int n = hex_1d(Hex(dim - 1, c));
        adjListInclSourceEnd[n].push_back(e);
    }
}

// getter
int HexGame::getNbHex()
{
    return nbHex;
}

// getter with extra calculations
int HexGame::getNbEdge()
{
    int nbEdge = 0;

    for (int u = 0; u < nbHex; u++)
    {
        // only top right triange is considered
        for (int v = u + 1; v < nbHex; v++)
        {
            if (adjMatrix[u][v])
                nbEdge++;
        }
    }
    return nbEdge;
}

// getter
vector<int> HexGame::getAdjList(int u)
{
    return adjList[u];
}

// getter
bool HexGame::getAdjMatrix(int u, int v)
{
    return adjMatrix[u][v];
}

// display - debug: summary
void HexGame::showSummary()
{
    cout << "HexGame: "
         << "dimension " << dim << "x" << dim << ", i.e. " << getNbHex() << " hex, " << getNbEdge() << " edges" << endl
         << endl;
}

// display - debug: adjacency matrix
void HexGame::showAdjMatrix()
{
    // important to reset default precision at the end of function
    streamsize ss = cout.precision();

    cout << "Adjacency Matrix" << endl;

    cout << setw(3) << "-"
         << ": ";
    for (int u = 0; u < adjMatrix.size(); u++)
    {
        cout << setw(3) << u << " ";
    }
    cout << endl
         << endl;

    for (int u = 0; u < adjMatrix.size(); u++)
    {
        cout << setw(3) << u << ": ";
        for (int v = 0; v < adjMatrix[u].size(); v++)
        {
            // setw to display a number over a fixed number of digits
            cout << setw(3) << (adjMatrix.at(u).at(v) ? "X" : " ") << " ";
        }
        cout << endl;
    }
    cout << "Done" << endl
         << endl;
    cout.precision(ss);
};

// display - debug: adjacency matrix - COMPACT
void HexGame::showAdjMatrixCompact()
{
    cout << "Adjacency Matrix Compact" << endl
         << endl;

    for (int u = 0; u < adjMatrix.size(); u++)
    {
        cout << setw(3) << u << ": ";
        for (int v = 0; v < adjMatrix[u].size(); v++)
        {
            cout << (adjMatrix.at(u).at(v) ? "X" : " ") << " ";
        }
        cout << endl;
    }
    cout << "Done" << endl
         << endl;
};

// display - debug: adjacency lists
void HexGame::showAdjList(bool inclSourceEnd)
{
    // auto adjL = adjList;
    vector<vector<int>> adjL;
    if (inclSourceEnd)
    {
        adjL = adjListInclSourceEnd;
    }
    else
    {
        adjL = adjList;
    }

    // important to reset default precision at the end of function
    streamsize ss = cout.precision();
    cout << "Adjacency List";
    if (inclSourceEnd)
    {
        cout << " including source & end";
    }
    cout << endl;

    for (int u = 0; u < adjL.size(); u++)
    {
        cout << setw(3) << u << ": ";
        for (int v = 0; v < adjL[u].size(); v++)
        {
            cout << setprecision(4) << adjL.at(u).at(v) << " ";
        }
        cout << endl;
    }
    cout << "Done" << endl
         << endl;
    cout.precision(ss);
};

// get nb of stones played by stone type
int HexGame::getNbStone(Stone s)
{
    int n = 0;
    for (auto i : stone)
    {
        if (i == s)
            n++;
    }
    return n;
}

// display board with stones, each stone type has a symbol
void HexGame::showBoard()
{
    char symbol;
    string color;
    int nbStoneEW = getNbStone(Stone::EW);
    int nbStoneNS = getNbStone(Stone::NS);
    int nbStoneTot = nbStoneEW + nbStoneNS;

    cout << "-------------------------------------------------------------" << endl
         << "Board" << endl
         << "Player East-West has played " << nbStoneEW << " stones (" << BLUE << "X" << RESET << ")" << endl
         << "Player North-South has played " << nbStoneNS << " stones (" << RED << "O" << RESET << ")" << endl
         << "Total played = " << nbStoneTot << "/" << dim * dim << " stones" << endl;
    if (latestStoneEW >= 0)
    {
        Hex hEW = hex_2d(latestStoneEW);
        cout << "Player East-West last stone: (" << hEW.r << "," << hEW.c << ")=" << latestStoneEW << endl;
    }
    if (latestStoneNS >= 0)
    {
        Hex hNS = hex_2d(latestStoneNS);
        cout << "Player North-South last stone: (" << hNS.r << "," << hNS.c << ")=" << latestStoneNS << endl;
    }
    cout << endl;

    cout << setw(2) << 0;
    for (int c = 1; c < dim; c++)
    {
        cout << setw(4) << c;
    }
    cout << endl;

    for (int r = 0; r < dim; r++)
    {
        for (int i = 0; i < 2 * r; i++)
        {
            cout << " ";
        }

        for (int c = 0; c < dim; c++)
        {
            Hex h = Hex(r, c);
            int u = hex_1d(h);
            Stone s = stone[u];
            if (s == Stone::EW)
            {
                symbol = 'X';
                if (u == latestStoneEW)
                {
                    color = BOLDBLUEVSWHITE;
                }
                else
                {
                    color = BOLDBLUE;
                }
                cout << " " << color << symbol << RESET << " ";
            }

            if (s == Stone::NS)
            {
                symbol = 'O';
                if (u == latestStoneNS)
                {
                    color = BOLDREDVSWHITE;
                }
                else
                {
                    color = BOLDRED;
                }
                cout << " " << color << symbol << RESET << " ";
            }

            if (s == Stone::None)
            {
                symbol = '.';
                cout << " " << WHITE << symbol << RESET << " ";
            }

            if (c < dim - 1)
            {
                cout << "-";
            }
            else
            {
                cout << setw(3) << r;
            }
        }
        cout << endl;

        if (r < dim - 1)
        {
            for (int i = 0; i < 2 * r + 1; i++)
            {
                cout << " ";
            }
            for (int i = 0; i < dim - 1; i++)
            {
                cout << " \\ /";
            }
            cout << " \\";
            cout << endl;
        }
    }
    cout << endl;
};

// check if human has won
bool HexGame::humanHasWon()
{
    return hasWonStone(humanStone, stone);
}

// check if machine has won
bool HexGame::machineHasWon()
{
    return hasWonStone(machineStone, stone);
}

// check if player playing stone has won
// start from source and check if end can be reached
bool HexGame::hasWonStone(Stone &s, vector<Stone> &stone2)
{
    int source, end;
    vector<bool> subset;
    subset.resize(nbHex + 4, false);
    for (int u = 0; u < nbHex; u++)
    {
        if (stone2[u] == s)
            subset[u] = true;
    }
    if (s == Stone::EW)
    {
        source = nbHex;
        end = nbHex + 1;
        subset[nbHex] = true;
        subset[nbHex + 1] = true;
    }
    else
    {
        source = nbHex + 2;
        end = nbHex + 3;
        subset[nbHex + 2] = true;
        subset[nbHex + 3] = true;
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

        for (auto i : adjListInclSourceEnd[u])
        {
            if (subset[i] && !visited[i])
            {
                visited[i] = true;
                queue.push_back(i);
            }
        }
    }

    return visited[end];
}

// game presentation
void gamePresentation()
{
    cout << endl
         << "HEX GAME" << endl
         << "You are to play against the machine" << endl
         << "You will each alternatively put one stone on the board" << endl
         << "The winner is the first to make a path from East to West (first player) or from North to South (second player)" << endl;
}

// askIfHumanPlaysFirst
bool HexGame::askIfHumanPlaysFirst()
{
    cout << endl
         << "Do you want to play first (East-West) ? Or second (North-South) ?" << endl
         << "[y] for first, [n] for second" << endl;
    bool answer = askYesNo();
    cout << "You have chosen to play " << (answer ? "first" : "second") << endl;
    return answer;
}

// askWhichMonteCarlo
int HexGame::askWhichOpponent()
{
    cout << endl
         << "Do you want to play against " << endl
         << "[1] The Quiche  - Random" << endl
         << "[2] The amateur - Monte Carlo Basic" << endl
         << "[3] The expert  - Monte Carlo Tree Search" << endl
         << "Your choice is ?" << endl;
    int answer = askNumber(1, 1, 3)[0];
    cout << "You have chosen opponent " << answer << endl;
    return answer;
}

// put stone on hex board
bool HexGame::putStone(Stone s, int u)
{
    if (stone[u] == Stone::None)
    {
        stone[u] = s;
        return true;
    }
    else
    {
        cout << "************* ERROR: Cannot put stone - hex occupied" << endl;
        return false;
    }
}

// human move
void HexGame::humanPlays()
{
    string direction, symbol;
    int r, c, u;
    if (humanStone == Stone::EW)
    {
        direction = "an East-West";
    }
    else
    {
        direction = "a North-South";
    }

    bool valid = false;
    while (!valid)
    {
        cout << "Where do you put " << direction << " stone ? (enter 'row col' e.g. '5 2') " << endl;
        vector<int> data = askNumber(2, 0, dim);
        r = data[0];
        c = data[1];
        u = hex_1d(Hex(r, c));
        if (stone[u] != Stone::None)
        {
            if (stone[u] == Stone::EW)
                symbol = "EW";
            if (stone[u] == Stone::NS)
                symbol = "NS";
            cout << "Illegal Move: position (" << r << "," << c << ")=" << u << " already occupied by stone " << symbol << "." << endl;
        }
        else
        {
            putStone(humanStone, u);
            if (humanStone == Stone::EW)
                latestStoneEW = u;
            if (humanStone == Stone::NS)
                latestStoneNS = u;
            cout << "MOVE --> You have played r=" << r << " c=" << c << endl;
            valid = true;
        }
    }
}

// machine move
void HexGame::machinePlays()
{
    cout << "Machine is thinking: " << endl;

    chrono::time_point<std::chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    int u;
    switch (machineAlgo)
    {
    case 1:
        u = posFromAlgoRandom();
        break;
    case 2:
        u = posFromAlgoMonteCarloNaive();
        break;
    case 3:
        u = posFromAlgoMonteCarloTreeSearch();
        break;
    }
    putStone(machineStone, u);
    Hex h = hex_2d(u);
    if (machineStone == Stone::EW)
        latestStoneEW = u;
    if (machineStone == Stone::NS)
        latestStoneNS = u;
    cout << "MOVE --> Machine has played r=" << h.r << " c=" << h.c << endl;

    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    cout << "Machine played in " << elapsed_seconds.count() << " s" << endl;
}

// machine algo - very simple so inline
int HexGame::posFromAlgoRandom()
{
    vector<int> subset;
    for (int u = 0; u < nbHex; u++)
    {
        if (stone[u] == Stone::None)
            subset.push_back(u);
    }

    int k = random.getRndNumber(0, subset.size() - 1);
    return subset[k];
}

// machine algo - quite simple so inline
int HexGame::posFromAlgoMonteCarloNaive()
{

    cout << "\tUntil the sooner of (i) 2m games and (ii) about 40 seconds" << endl
         << "\tRandom (Monte Carlo) games are equally spread over the possible next moves " << endl
         << "\tOne dot represent a Monte Carlo for a next move" << endl;

    vector<int> subset;
    for (int u = 0; u < nbHex; u++)
    {
        if (stone[u] == Stone::None)
            subset.push_back(u);
    }
    cout << "\tThere are " << subset.size() << " possible next move(s)" << endl;

    chrono::time_point<std::chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

    // time calibration: how long to explore one next move with 10k random games
    auto gen = random.getGen();
    vector<int> nbWin(subset.size());

    start = chrono::system_clock::now();

    int s = 0; // first next move
    vector<Stone> stone2 = stone;
    stone2[subset[s]] = machineStone;
    vector<int> subset2 = subset;
    subset2.erase(subset2.begin() + s);
    vector<int> subset3 = subset2;
    for (int k = 0; k < 1e3; k++) // 10k random games
    {
        shuffle(subset3.begin(), subset3.end(), gen);
        vector<Stone> stone3 = stone2;
        for (int i = 0; i < subset3.size(); i = i + 2)
            stone3[subset3[i]] = humanStone;
        for (int i = 1; i < subset3.size(); i = i + 2)
            stone3[subset3[i]] = machineStone;
        hasWonStone(machineStone, stone3);
    }

    end = chrono::system_clock::now();
    elapsed_seconds = end - start;

    int NbRndGamePerNextMoveTimeBased = static_cast<int>(40 * 1e3 / (elapsed_seconds.count() * subset.size()));
    int NbRndGamePerNextMoveNumberBased = static_cast<int>(2e6 / subset.size());
    int N = min(NbRndGamePerNextMoveTimeBased, NbRndGamePerNextMoveNumberBased);

    cout << "\tOne dot represent a " << N << " games" << endl
         << "\t";

    for (int s = 0; s < subset.size(); s++)
    {
        cout << "." << flush;
        vector<Stone> stone2 = stone;
        stone2[subset[s]] = machineStone;
        vector<int> subset2 = subset;
        subset2.erase(subset2.begin() + s);
        vector<int> subset3 = subset2;

        for (int k = 0; k < N; k++)
        {
            shuffle(subset3.begin(), subset3.end(), gen);
            vector<Stone> stone3 = stone2;
            for (int i = 0; i < subset3.size(); i = i + 2)
                stone3[subset3[i]] = humanStone;
            for (int i = 1; i < subset3.size(); i = i + 2)
                stone3[subset3[i]] = machineStone;
            if (hasWonStone(machineStone, stone3))
                nbWin[s]++;
        }
    }
    int argmax = distance(nbWin.begin(),
                          max_element(nbWin.begin(), nbWin.end()));

    cout << endl
         << "Monte Carlo ratio wins per next move: (row,col)=ratio" << endl;
    for (int j = 0; j < nbWin.size(); j++)
    {
        int r = static_cast<int>(subset[j] / dim);
        int c = subset[j] % dim;
        float ratioWin = static_cast<float>(nbWin[j]) / static_cast<float>(N);
        cout << "(" << r << "," << c << ")=" << ratioWin << " ";
    }
    cout << endl;

    return subset[argmax];
}

// machine algo - more complex to outsourced to TreeNode class
int HexGame::posFromAlgoMonteCarloTreeSearch()
{
    TreeNode root = TreeNode(nullptr,              // root in tree - nullptr as of creation cos will be itself
                             nullptr,              // nullptr as no parent since root
                             stone,                // vector of stone representing board state
                             -1,                   // last position played in 1d representation: -1 as empty board
                             Stone::None,          // last stone played - none as empty board
                             machineStone,         // next stone to be played - machineStone necessarily
                             adjListInclSourceEnd, // graph representation of board - used to determine winner
                             random,               // random source
                             0);                   // depth in tree - 0 as root
    root.runMonteCarloTreeSearch();
    return root.getBestMove();
}

// full game
void HexGame::playGame()
{

    gamePresentation();

    bool humanPlaysFirst = askIfHumanPlaysFirst();
    machineAlgo = askWhichOpponent();

    if (humanPlaysFirst)
    {
        cout << endl
             << "HEX GAME START" << endl
             << "You play East-West and first" << endl;
        humanStone = Stone::EW;
        machineStone = Stone::NS;
    }
    else
    {
        cout << endl
             << "HEX GAME START" << endl
             << "You play North-South and second" << endl;
        machineStone = Stone::EW;
        humanStone = Stone::NS;
    }

    bool finished = false;
    string winner;

    // Human plays
    if (humanPlaysFirst)
    {
        // Show status
        showBoard();

        humanPlays();
        humanPlaysFirst = false;
    }

    while (!finished)
    {
        // Show status
        showBoard();

        // Machine plays
        machinePlays();
        if (machineHasWon())
        {
            finished = true;
            winner = "MACHINE WINS !!! :-(";
        }

        if (!finished)
        {
            // Show status
            showBoard();

            humanPlays();
            if (humanHasWon())
            {
                finished = true;
                winner = "YOU  WIN !!! :-)";
                break;
            }
        }
    }

    // Show status
    showBoard();

    cout << endl
         << winner << endl
         << endl;
}
