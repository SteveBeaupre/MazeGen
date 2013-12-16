#ifndef _MAZE_H_
#define _MAZE_H_
#ifdef __cplusplus

#include "CheckedWallsList.h"
#include "Random.h"

#define SEED_RANDOM_GEN  

#define WM_MAZE_CREATION_PROGRESS   WM_USER + 100  
#define WM_MAZE_SOLVING_PROGRESS    WM_USER + 101  

//#define MAX_SIZE  200

//----------------------------------------------------------------------//
// Wall Status...
//----------------------------------------------------------------------//
#define BROKEN    0
#define INTACT    1

#define WALL_UNSOLVED  0
#define WALL_SOLVING   1

//----------------------------------------------------------------------//
// Direction flags...
//----------------------------------------------------------------------//
#define EAST_WALL    0
#define WEST_WALL    1
#define NORTH_WALL   2
#define SOUTH_WALL   3

//----------------------------------------------------------------------//
// Hold a cell data
//----------------------------------------------------------------------//
struct MazeCell {
	int  Indx;
	BOOL Visited;
	BOOL WallsStatus[4];
	BOOL IsSolved;
	BOOL SolvedWallsStatus[4];
	MazeCell *pNeighborgCells[4];
};

//----------------------------------------------------------------------//
// Hold a entry point cell position
//----------------------------------------------------------------------//
struct ENTRYPOINT {
	int  Dir, x, y;
};

//----------------------------------------------------------------------//
// The maze class...
//----------------------------------------------------------------------//
class CMaze {
public:
	CMaze();
	~CMaze();
private:
	HWND hWnd;
private:
	BOOL IsCellsAllocated;
	BOOL Canceled;
	int Width, Height;
	ENTRYPOINT EntryPoint;		
	ENTRYPOINT ExitPoint;
private:
	CRandom RandGen;
private:
	MazeCell* pCells;
public:
	MazeCell* GetCellPtr(int x, int y);
private:
	void PrintPercentDone();
private:
	int  CaclNumVisitedCells();
	int  CaclNumSolvedCells();
private:
	void ResetSolvingVariables();
private:
	int  MirrorDir(int Dir);
	void GenRandomDirs(int *pDirs);
	void GenEntryPoint(ENTRYPOINT *pPoint);
public:
	void SetWndHandle(HWND hMainWnd);
	bool AllocateCells(int NumWidth, int NumHeight);
	void FreeCells();
	void CreateMaze();
	void SolveMaze();
	void CancelMaze();
private:
	void BuildCell(MazeCell *pCell, int x, int y);
	void BreakWall(MazeCell *pCell, int Dir);
	bool IsWallBroken(MazeCell *pCell, int Dir);
	bool IsCellVisited(MazeCell *pCell);
private:
	void CreateMaze(MazeCell *pCell, int IncomingDir, int Level);
	bool SolveMaze(MazeCell *pCell, int IncomingDir, int Level);
public:
	void GenMazeBitmapBuffer(BYTE *pBuf, int WidthX3, int HeightX3);
	void SaveMazeAsRawImage(char *pName);
public:
	bool LoadMaze(char *pName, int *pWidth, int *pHeight);
	bool SaveMaze(char *pName);
};

#endif
#endif //_MAZE_H_
