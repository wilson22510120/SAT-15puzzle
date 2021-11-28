# 15-PUZZLE solver by MINISAT
## Introduce of 15-PUZZLE
The 15 puzzle is a sliding puzzle having 15 square tiles numbered 1–15 in a frame that is 4 tiles high and 4 tiles wide, leaving one unoccupied tile position. [(wiki)](https://en.wikipedia.org/wiki/15_puzzle)

## Usage

### Input format

 \<square size> 
 
 \<steps to run>
 
 \<matrix of initial puzzle, "0" denotes the empty tile, other number denotes the tile of that number>


eg.

<img src="https://imgur.com/PjOJO2u.png" width="250">

format:
```
4
81
14 15 4 12
5 1 10 8
13 6 9 3
11 2 7 0
```
## Run the code

### Execute
```
.\satPuzzle <input file, eg. case\01.in>
```

### modifying files
if you want modify the files, then do
```
make
```
after you finish modify.

