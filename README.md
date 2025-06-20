# Snakes and Ladders Simulator

This program simulates the board game **Snakes and Ladders**.

## Build

The program can be compiled via the provided Makefile in the root directory of the project

```
make
```

or by calling the clang compiler directly with the following command.

```
clang -Wall -Wextra -Werror --std=c17 -D_XOPEN_SOURCE=500 -Iinclude -DVERSION_MAJOR=1 -DVERSION_MINOR=0 -DVERSION_PATCH=0 -UDEBUG src/*.c
```

## Command Line Interface

The C library `getopt.h` is used for processing command line arguments.

When running the program with the `-h` or `--help` option a help text is printed and the program terminates.
It includes the program version, usage and description of the available options and values.

```
sals v1.0.0
Usage: sals [options] <snake-or-ladder>...

  <snake-or-ladder>         A string containing two positive integers separated by a '-' character. Format: a-b.
                             a is the index of the starting cell and b is the index of the ending cell.
                             Ladders start in a cell before the ending cell (a < b) and propel the player forward from a to b.
                             Snakes start in a cell after the ending cell (a > b) and propel the player backward from a to b.
                             All specified snakes and ladders must not end in the last cell of the playing field and
                             they must not start/end in the same cell as themselves or any other snake or ladder. (i.e. overlap is disallowed)
                             e.g. 0-16 13-45 23-27 15-3 48-20 54-11

options:
  -h, --help                Prints this help text and terminates the program.
  -c, --config-file val     The filepath of the configuration file which is processed the same way as the command line arguments
                             are processed after splitting the file content similarly as e.g. the linux bash splits it's input.
                             The option -c, --config-file is not allowed inside a configuration file.
                             Non-option argument values that appear in the cli arguments before the -c, --config-file option are lost
                             because the necessary argument reordering fails after processing the configuration file.
  -x, --width val           The width of the playing field which must be an integer value >= 2. The default value is 10.
  -y, --height val          The height of the playing field which must be an integer value >= 2. The default value is 10.
  -s, --die-sides val       The number of sides the used die has which must be an integer value >= 1. The default value is 6.
  -e, --exact-ending        Enables the exact ending requirement. To end a game the player must land exactly on the last cell
                             of the playing field. If a rolled die would overshoot the last cell the player is not moved.
  -d, --distribution val    The distribution when generating a random number in a die. The value can be a sequence of weights
                             separated by commas in the format 'W1,W2,W3,W4,...,Wn' where Wn is the weight of the n-th die side.
                             Thus this specified weight sequence must have a weight count n less than or equal the die sides s (n <= s).
                             If less weights are specified than the number of die sides (n < s) the remaining die sides are automatically weighted with 0.
                             Each weight Wi is an unsigned integer value. The distribution describes that the side i's probability to be diced
                             in a dice roll Pi is Wi divided by the sum of all Ws S.
                             i.e. s  // number of die sides (unsigned integer)
                                  n  // number of die specified weights in the weight sequence (unsigned integer)
                                  i  // an arbitrary integer from 1 to s inclusive
                                  Wi  // the weight of the side i (unsigned integer)
                                  S = W1 + W2 + W3 + W4 + ... + Ws  // sum of the weights of all sides 1 through s inclusive
                                  Pi = Wi / S  // probability of side i to be diced in a dice roll
                             Alternatively to providing a weight sequence as described above, it is possible to use one of the following presets
                             whose weights are calculated in a certain way based on the die sides s.
                             - uniform       A uniform distribution (default)
                                              e.g. for s = 6 the weights are 1,1,1,1,1,1
                             - twodice       The distribution that arises when using two uniform and equal dice with s/2 sides each
                                              and using the sum of the two diced values as overall dice result.
                                              e.g. for s = 12 (two uniform dice with s/2 = 6 sides each)
                                                   SUM │ 1  2  3  4  5  6 
                                                   ────┼───────────────────
                                                     1 │ 2  3  4  5  6  7 
                                                     2 │ 3  4  5  6  7  8 
                                                     3 │ 4  5  6  7  8  9 
                                                     4 │ 5  6  7  8  9 10 
                                                     5 │ 6  7  8  9 10 11 
                                                     6 │ 7  8  9 10 11 12 
                                                   // count how often side (sum) is hit to determine their weights.
                                                   // these are all possible ways to dice each value (sum) when using two uniform 6-sided dice
                                                   -> distribution weights: 0,1,2,3,4,5,6,5,4,3,2,1
                             - upstairs      A distribution with it's first weight being 1 and each following weight being incremented by 1.
                                              e.g. for s = 6 the weights are 1,2,3,4,5,6
                             - downstairs    A distribution with it's last weight being 1 and each previous weight being incremented by 1.
                                              e.g. for s = 6 the weights are 6,5,4,3,2,1
  -i, --iterations val      The number of times the game should be simulated which must be an integer value >= 1. The default is 1000.
  -l, --dice-limit val      The number of times a simulation is allowed to dice before resigning if the game was not won yet
                             which must be an integer value >= 1. The default is 10000.
  -b, --bar-length val      The length of the bars that visualize the probability of each side of the used die
                             which must be an integer value >= 1. The default is 50.
```

## Game

The player starts a game of **Snakes and Ladders** outside the playing field. They repeatedly roll a dice which is the number of steps they take forward on the board. The first dice places the player into the cell with the same number as the diced value. Every following dice advances the player on the field by the diced value. The game ends when the player reaches the last cell. If an exact ending is required the player must land exactly on the last cell, otherwise the player is allowed to overshoot the ending cell.

The field can contain **snakes**, which propel the player to a specific cell before the current cell, and **ladders**, which propel the player to a specific cell after the current cell. If the player lands on a cell where a snake or ladder starts they must use the snake or ladder. Snakes and ladders are not allowed to overlap or start or end in the last cell of the playing field.

An arbitrary die can be used while playing. The number of sides the die has can be specified via the `-s, --die-sides` option and the distribution (i.e. the probability of dicing specific sides) can be set via the `-d, --distribution` option.

## Simulation

The game that was defined via cli arguments and/or a configuration file is simulated the number of times defined by the `-i, --iterations` option. The simulator is multi-threaded running all simulations simultaneously in separate threads. Each simulation plays the game and tracks it's diced values and usages of snakes and ladders. If the simulation diced the number of times specified via the `-l, --dice-limit` option and still has not won the game the simulation resigns and notes that the game was not won. This dice limit ensures that simulations that potentially ran into infinite loops (e.g. due to the game being unwinnable because of the snakes and ladders and used die) don't run endlessly.

In case the simulations take a long time to finish a loading animation is displayed to both make the waiting a bit more interesting and indicate that the program is still actively running the simulations.

## Statistical Analysis

The ran simulations are statistically analyzed determining a variety of informative values. They include the total number of dices, wins, losses (resigned simulations), the shortest dice sequence that lead to a win, the usages of snakes and ladders and more. The statistics are printed in an easily digestible format.

## Example Configuration Files

The `examples` folder in the project's root directory contains a multitude of different potentially interesting configuration files. A configuration file can be used by setting the `-c, --config-file` option to the file's path.

```
./sals -c funkyargs.sals
```

The `examples` directory contains general examples as well as more specific examples in a respective subfolder.

- The `examples/distributions` folder showcases the use of various ways to specify the die distribution, using custom weight sequence to specify the weight of each individual side of the die or using a predefined preset which automatically generate a weight sequence of a specific type.
- The `examples/invalid` folder showcases invalid arguments, including unknown options, missing option values, invalid numbers, distributions and snakes and ladders.

## Assignment: 6th Assignment

Originating in India as **Moksha Patam**, Snakes and Ladders is a classic board game that relies on pure luck when rolling a die. The board features numbered squares, with players moving their pieces based on die rolls. Some squares contain snakes that send players backward, while others have ladders that propel them forward.

The classic version of **Chutes and Ladders** as published by **Milton Bradley** in 1943 consists of 100 squares arranged in a 10x10 grid. It adds 9 ladders and 10 chutes, and is played with a 6-sided die (or spinner). Play starts outside the board, and players roll the die to move their piece. Landing on the bottom of a ladder allows the player to move up to the top of the ladder, while landing on the head of a snake sends the player down to its tail.

### Snakes and Ladders Simulator

Implement a simulator for the game **Snakes and Ladders** using **Markov Chain Monte Carlo** (MCMC) methods.

The simulator should allow for the following features:

- **Game Setup**
    - The board should consist of an _N_ × _M_ grid of squares, with the default size being 10 × 10.
    - The board should accomodate an arbitrary number of snakes and ladders.
        - No snake or ladder should start or end on the same square as itself or any other snake or ladder.
        - No snake or ladder should start at the last square.
- **Game Rules**
    - The game is played with an _n_-sided die, with the default being a 6-sided die.
        - (optional) Allow for non-uniform die distributions.
    - The game is won when either the last square is reached exactly, or when a player rolls a die that would exceed the last square.
        - If the game must be won by reaching the last square exactly, any roll that would overshoot the last square does not move the player.
- **Game Simulation**
    - The program simulates games of Snakes and Ladders by repeatedly rolling the die and advancing the position according to the game rules.
    - The simulation is run for a specified number of iterations, and the results are collected.
        - (optional) Run simulations in parallel.
- **Game Statistics**
    - The program should calculate and display statistics about the game:
        - The average number of rolls to win the game.
        - The shortest number and sequence of rolls that lead to a win.
        - The number and relative frequency of each snake and ladder being traversed.

### Specific Requirements

- Implement the game board as a **graph**, where each square is represented by a node.
- Allow for non-interactive use of the program, e.g. using command line arguments or configuration files.
- Individual simulations should abort after a certain number of steps (e.g. in an infinite loop) even if no win condition is encountered.
- Prepare multiple board and game configurations (ideally with interesting properties) and compare their results.

### Common Requirements

- The program should be implemented in C.
    - The code should be well structured into header and source files.
    - Any public functions should be well documented, private functions should be commented where necessary.
    - The code should be compiled using clang with the "`-Wall -Wextra -Werror`" flags.
    - The application MUST not crash or leak memory.
- The assignment MUST be handed in through the provided GitHub Classroom assignment.
    - Assignments pushed after the deadline will be marked as late and will incur a point penalty.
    - Only push plain-text files, but no binaries, archives, or build artifacts.
