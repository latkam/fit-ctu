#pragma once

#include "NConfig.hpp"
#include "CContext.hpp"

#include <cstring>

/**
 * @class CMazeLoader
 * @brief Loads the maze from the file.
 */
class CMazeLoader : public CContext
{
public:
               CMazeLoader   ( WINDOW * inWin, int inWinH, int inWinW );
  virtual void Run           ( void ) override;
};
