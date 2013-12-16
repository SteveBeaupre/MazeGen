#include "MazeGen.h"

/**************************************************
main procedure.....................................
**************************************************/
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwreason, LPVOID lpReserved)
{
    switch(fdwreason)
	{
    case DLL_PROCESS_ATTACH: DisableThreadLibraryCalls(hInst); break;
    case DLL_PROCESS_DETACH: break;
    case DLL_THREAD_ATTACH:  break;
    case DLL_THREAD_DETACH:  break;
    }
    return TRUE;
}

void EXP_FUNC _SetMainWndHandle(HWND hMainWnd)
{
	Maze.SetWndHandle(hMainWnd);
}

bool EXP_FUNC _AllocateMazeCells(int NumCellWidth, int NumCellHeight)
{
	return Maze.AllocateCells(NumCellWidth, NumCellHeight);
}

void EXP_FUNC _BuildMaze()
{
	Maze.CreateMaze();
}

void EXP_FUNC _CancelMaze()
{
	Maze.CancelMaze();
}

void EXP_FUNC _FreeMazeCells()
{
	Maze.FreeCells();
}

void EXP_FUNC _SolveMaze()
{
	Maze.SolveMaze();
}

bool EXP_FUNC _LoadMaze(char *pName, int *pWidth, int *pHeight)
{
	return Maze.LoadMaze(pName, pWidth, pHeight);
}

bool EXP_FUNC _SaveMaze(char *pName)
{
	return Maze.SaveMaze(pName);
}

void EXP_FUNC _GenMazeBitmapBuffer(BYTE *pBuf, int WidthX3, int HeightX3)
{
	Maze.GenMazeBitmapBuffer(pBuf, WidthX3, HeightX3);
}

void EXP_FUNC _SaveMazeAsRawImage(char *pName)
{
	Maze.SaveMazeAsRawImage(pName);
}

bool EXP_FUNC _GetCellInfo(int x, int y, BOOL *pVisited, BOOL *pSolved, BOOL *pWallsStatus, BOOL *pSolvedWallsStatus)
{
	MazeCell *pCell = Maze.GetCellPtr(x, y);

	if(pCell){
		*pVisited = pCell->Visited;	
		*pSolved  = pCell->IsSolved;
		
		pWallsStatus[EAST_WALL]  = pCell->WallsStatus[EAST_WALL];
		pWallsStatus[WEST_WALL]  = pCell->WallsStatus[WEST_WALL];
		pWallsStatus[NORTH_WALL] = pCell->WallsStatus[NORTH_WALL];
		pWallsStatus[SOUTH_WALL] = pCell->WallsStatus[SOUTH_WALL];
	
		pSolvedWallsStatus[EAST_WALL]  = pCell->SolvedWallsStatus[EAST_WALL];
		pSolvedWallsStatus[WEST_WALL]  = pCell->SolvedWallsStatus[WEST_WALL];
		pSolvedWallsStatus[NORTH_WALL] = pCell->SolvedWallsStatus[NORTH_WALL];
		pSolvedWallsStatus[SOUTH_WALL] = pCell->SolvedWallsStatus[SOUTH_WALL];

		return true;
	}

	return false;
}

