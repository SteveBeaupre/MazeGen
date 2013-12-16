//----------------------------------------------------------------------//
#define EXP_FUNC __stdcall//use the default windows api calling convention
//----------------------------------------------------------------------//
#include "Maze.h"

CMaze Maze;

void EXP_FUNC _SetMainWndHandle(HWND hMainWnd);

bool EXP_FUNC _AllocateMazeCells(int NumCellWidth, int NumCellHeight);
void EXP_FUNC _BuildMaze();
void EXP_FUNC _CancelMaze();
void EXP_FUNC _FreeMazeCells();

void EXP_FUNC _SolveMaze();

bool EXP_FUNC _LoadMaze(char *pName, int *pWidth, int *pHeight);
bool EXP_FUNC _SaveMaze(char *pName);

void EXP_FUNC _GenMazeBitmapBuffer(BYTE *pBuf, int WidthX3, int HeightX3);
void EXP_FUNC _SaveMazeAsRawImage(char *pName);

// So the user can build it's own drawing routine...
bool EXP_FUNC _GetCellInfo(int x, int y, BOOL *pVisited, BOOL *pSolved, BOOL *pWallsStatus, BOOL *pSolvedWallsStatus);
