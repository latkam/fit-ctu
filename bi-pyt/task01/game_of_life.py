def generate_all_cells(size):
    all_cells = set()
    for x in range(size[0]):
        for y in range(size[1]):
            all_cells.add((x, y))
    return all_cells

def check(cell, size, alive):
    if (cell[0] >= 0 and cell[0] <= size[0] and
        cell[1] >= 0 and cell[1] <= size[1] and
        cell in alive ):
        return True
    return False

def generate_valid_alive_cells(cell, size, alive):
    valid_alive_cells = []
    tmp = (cell[0]-1, cell[1]  ) #1
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)
    tmp = (cell[0]-1, cell[1]-1) #2
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)    
    tmp = (cell[0]  , cell[1]-1) #3
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)    
    tmp = (cell[0]+1, cell[1]-1) #4
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)    
    tmp = (cell[0]+1, cell[1]  ) #5
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)    
    tmp = (cell[0]+1, cell[1]+1) #6
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)    
    tmp = (cell[0]  , cell[1]+1) #7
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)    
    tmp = (cell[0]-1, cell[1]+1) #8
    if (check(tmp, size, alive)):
        valid_alive_cells.append(tmp)    
    return valid_alive_cells
    
    
def update(alive, size, iter_n):
    all_cells = generate_all_cells(size)
    for iteration in range(iter_n):
        alive_old = alive.copy()
        dead_old  = all_cells - alive_old # set operation
        alive_new = alive.copy()
        dead_new  = all_cells - alive_new # set operation
        for cell in alive_old:
            valid_alive_cells = generate_valid_alive_cells(cell, size, alive_old)
            if len(valid_alive_cells) < 2 or len(valid_alive_cells) > 3: # 1.rule, 3.rule
                alive_new.discard(cell)
                dead_new.add(cell)
            else: # 2.rule
                pass
        for cell in dead_old:
            valid_alive_cells = generate_valid_alive_cells(cell, size, alive_old)
            if len(valid_alive_cells) == 3: # 4.rule
                dead_new.discard(cell)
                alive_new.add(cell)
        alive = alive_new.copy()
    return alive

def draw(alive, size):
    """
    alive - set of cell coordinates marked as alive, can be empty
    size - size of simulation grid as  tuple - ( )

    output - string showing the board state with alive cells marked with X
    """
    result = """"""
    result += "+"
    for _ in range(size[1]):
        result += "-"
    result += "+\n"
    for x in range(size[0]):
        result += "|"
        for y in range(size[1]):
            if (x,y) in alive:
                result += "X"
            else:
                result += " "
        result += "|\n"
    result += "+"
    for _ in range(size[1]):
        result += "-"
    result += "+"
    return result
