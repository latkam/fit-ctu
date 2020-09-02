## Sudoku for 100 % (CSP)
For detailed information, see the [project report](https://github.com/latkam/fit-ctu/blob/master/bi-zum-sudoku-csp/report/report-cs.pdf) (in Czech).

**Compile**:
```bash
make all
```

>Please note that default INPUT_FILE is examples/sudoku0.in.

**Run**:
```bash
# Manual mode
make INPUT_FILE=<your_input_file> manual

# Half-automatic mode
make INPUT_FILE=<your_input_file> half-automatic

# Automatic mode
make INPUT_FILE=<your_input_file> automatic
```

**Clean object files**:
```bash
make clean
```

**Sample input file:**
```txt
9 3
1 0 0 0 0 3 9 0 0
0 7 0 5 0 0 8 6 0
0 0 8 0 0 4 0 2 7
0 0 5 2 0 1 4 0 6
9 1 4 8 0 0 0 0 0
0 2 0 0 4 9 0 1 0
0 3 9 0 1 8 6 0 0
8 0 1 6 2 0 0 3 0
0 5 0 0 3 0 0 0 4
```

## Results
Got **20/20** points.
