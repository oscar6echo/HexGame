
## HexGame - Rudimentary AI Player

### Description

Command line implementation of [Hex Game](https://en.wikipedia.org/wiki/Hex_(board_game)) in C++.  
Play against 3 opponents:
1. Random player
1. AI running simple [Monte Carlo](https://en.wikipedia.org/wiki/Monte_Carlo_method) to assess the next best move
1. AI running [Monte Carlo Tree Search](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) to assess the next best move


### Instructions

Clone this repo then build.  
You need a [C++11 compliant](http://en.cppreference.com/w/cpp/compiler_support) compiler.
````
bash  build.sh
````

Run:
````
./bin/main.out
````

### Game examples

1. [Transcript of a full game board 4x4](./games/example_game_4x4.txt)

2. [Transcript of the first steps of a game on board 9x9](./games/example_game_4x4.txt)
