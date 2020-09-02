# BI-ZUM
## Tasks results
- task01: **5/5** points
- task02: not assigned due to the `Covid-19` pandemic.
- task03: **10/10** points
- task04: **10/10** points

## Task 01 State space exploration
Implements algorithms for the state space exploration such as Random Search,
Depth-first search (DFS), Breadth-first search (BFS), Greedy search, Dijkstra's
algorithm and A*.

**Compile**:
```bash
cd 01/program
g++ -std=c++17 -Wall -pedantic -Wextra -g main.cpp
```

**Run**:
```bash
./a.out <random/dfs/bfs/greedy/dijkstra/astar> < dataset/<filename>.txt
```

## Task 02 Genetic algorithm
Not assigned due to the `COVID-19` pandemic.

## Task 03 Automatic planning
Automatic planning using `PDDL`.

**Run**: 
Download and install the [Fast Downward planner](http://www.fast-downward.org).
```bash
cd 03/main-task
./fast-downward.py domain.pddl task.pddl --search "astar(lmcut())"
```
## Task 04 Cooperative Pathfinding
Implements cooperative pathfinding using Cooperative A* algorithm. More about this topic can be found [here](https://www.aaai.org/Papers/AIIDE/2005/AIIDE05-020.pdf).

**Compile**:
```bash
cd 04/program
g++ -std=c++17 -Wall -pedantic -Wextra -g main.cpp
```

**Run**:
```bash
./a.out <number of agents>(1-9) < <filename>
e.g. ./a.out 2 <../dataset3/03.txt
```

**Sample input file**
```txt
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
X                         X                       X   X
X  XX XXXX   X  XXXXX XXXXX  X  XXXX   XXXX X X   X X X
X X                       X   X       X       X X X X X
X   XX X XXXXXXX XXXX X   XX  XXX  X    XX XXX  X XXX X
X       X                 X           X         X     X
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
start 43, 3
end 15, 5
start 2, 1
end 13, 5
start 43, 3
end 15, 5
```
This file is created for up to three agents.

**Please provide appropriate number of start and end coordinates according to the number of agents you want to use.** Otherwise, the behaviour is undefined (missing coordinates will be probably [0, 0], some dummy values or the same as coordinates for the last agent).

Also please **respect dimensions of the maze**. Entering inappropriate coordinates also results in undefined behaviour.

