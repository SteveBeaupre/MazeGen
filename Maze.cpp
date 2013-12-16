#include "Maze.h"

//-----------------------------------------------------------------------------
// Constructor...
//-----------------------------------------------------------------------------
CMaze::CMaze()
{
	hWnd = NULL;
	IsCellsAllocated = FALSE;
	Canceled = FALSE;
	Width  = 0;
	Height = 0;
	ZeroMemory(&EntryPoint, sizeof(ENTRYPOINT));
	ZeroMemory(&ExitPoint,  sizeof(ENTRYPOINT));
	pCells = NULL;
}

//-----------------------------------------------------------------------------
// Destructor...
//-----------------------------------------------------------------------------
CMaze::~CMaze()
{
	FreeCells();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Return the pointer of a cell, if not out of bound...
//-----------------------------------------------------------------------------
MazeCell* CMaze::GetCellPtr(int x, int y)
{
	// Error handling...
	if((x < 0 || x >= Width) || (y < 0 || y >= Height))
		return NULL;

	return &pCells[(y*Width)+x];
}

//-----------------------------------------------------------------------------
// Save some debug lines of text showing the progress
//-----------------------------------------------------------------------------
void CMaze::PrintPercentDone()
{
	int NumCells = Width * Height;
	int NumVisitedCells = 0;

	for(int y = 0; y < Height; y++){
		for(int x = 0; x < Width; x++){
			MazeCell *pCell = GetCellPtr(x, y);
			if(pCell->Visited)
				NumVisitedCells++;
		}
	}

	float fPercentDone = ((float)NumVisitedCells / (float)NumCells) * 100.0f;

	printf("\rPercent Done: %2.2f%%", fPercentDone);
}

//-----------------------------------------------------------------------------
// Save some debug lines of text showing the progress
//-----------------------------------------------------------------------------
int CMaze::CaclNumVisitedCells()
{
	int NumVisitedCells = 0;

	for(int y = 0; y < Height; y++){
		for(int x = 0; x < Width; x++){
			MazeCell *pCell = GetCellPtr(x, y);
			if(pCell->Visited)
				NumVisitedCells++;
		}
	}

	return NumVisitedCells;
}

//-----------------------------------------------------------------------------
// Save some debug lines of text showing the progress
//-----------------------------------------------------------------------------
int CMaze::CaclNumSolvedCells()
{
	int NumVisitedCells = 0;

	for(int y = 0; y < Height; y++){
		for(int x = 0; x < Width; x++){
			MazeCell *pCell = GetCellPtr(x, y);
			if(pCell->IsSolved)
				NumVisitedCells++;
		}
	}

	return NumVisitedCells;
}

//-----------------------------------------------------------------------------
// Reset the solving variables to start a new one
//-----------------------------------------------------------------------------
void CMaze::ResetSolvingVariables()
{
	for(int y = 0; y < Height; y++){
		for(int x = 0; x < Width; x++){
			MazeCell *pCell = GetCellPtr(x, y);
			pCell->IsSolved = FALSE;
			for(int Cpt = 0; Cpt < 4; Cpt++)
				pCell->SolvedWallsStatus[Cpt] = WALL_UNSOLVED;
		}
	}
}

//-----------------------------------------------------------------------------
// Return the pointer of a cell, if not out of bound...
//-----------------------------------------------------------------------------
int CMaze::MirrorDir(int Dir)
{
	// Return the requested wall status
	switch(Dir)
	{
	case EAST_WALL:  return WEST_WALL;
	case WEST_WALL:  return EAST_WALL;
	case NORTH_WALL: return SOUTH_WALL;
	case SOUTH_WALL: return NORTH_WALL;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Generate the 4 possible direction randomized in a list
//-----------------------------------------------------------------------------
void CMaze::GenRandomDirs(int *pDirs)
{
	// Generate a list of the 4 possible direction
	CheckedWallsList CheckedWalls;
	CheckedWalls.PushNode(EAST_WALL);
	CheckedWalls.PushNode(WEST_WALL);
	CheckedWalls.PushNode(NORTH_WALL);
	CheckedWalls.PushNode(SOUTH_WALL);

	// Randomize it
	for(int CptLoop = 0; CptLoop < 20; CptLoop++){

		int Dirs[4];

		// Save each card(randomly selected) in the temp array
		int IndxInArray = 0;
		int NodeCount   = 4;
		
		while(NodeCount > 0){

			// Find the node to copy/delete
			int NodeToSwapIndx = RandGen.ExtractNumber() % NodeCount;
			
			// Save it in our array
			Dirs[IndxInArray] = CheckedWalls.GetNodeDir(NodeToSwapIndx);

			// Delete the node
			CheckedWalls.DeleteNode(NodeToSwapIndx);
			
			//Update our counters
			IndxInArray++;
			NodeCount--;
		}

		// Re-Fill the Pile
		for(NodeCount = 0; NodeCount < 4; NodeCount++)
			CheckedWalls.PushNode(Dirs[NodeCount]);
	}

	for(CptLoop = 0; CptLoop < 4; CptLoop++)
		pDirs[CptLoop] = CheckedWalls.GetNodeDir(CptLoop);

	// Clear the list
	CheckedWalls.ClearAll();
}

//-----------------------------------------------------------------------------
// Create the maze entry and exit points
//-----------------------------------------------------------------------------
void CMaze::GenEntryPoint(ENTRYPOINT *pPoint)
{
	// Pick 2 random direction to start with
	int RandomDir  = -1;
	int RandomIndx = -1;

	RandomDir = RandGen.ExtractNumber() % 4;

	// Pick an index for this direction
	switch(RandomDir)
	{
	case EAST_WALL: 
	case WEST_WALL:  RandomIndx = RandGen.ExtractNumber() % Height; break;
	case NORTH_WALL: 
	case SOUTH_WALL: RandomIndx = RandGen.ExtractNumber() % Width;  break;
	}
	
	switch(RandomDir)
	{
	case EAST_WALL:  pPoint->x = Width-1;    pPoint->y = RandomIndx; break;
	case WEST_WALL:  pPoint->x = 0;          pPoint->y = RandomIndx; break;
	case NORTH_WALL: pPoint->x = RandomIndx; pPoint->y = 0;          break;
	case SOUTH_WALL: pPoint->x = RandomIndx; pPoint->y = Height-1;   break;
	}

	pPoint->Dir = RandomDir;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Return the pointer of a cell, if not out of bound...
//-----------------------------------------------------------------------------
void CMaze::SetWndHandle(HWND hMainWnd)
{
	hWnd = hMainWnd;
}

//-----------------------------------------------------------------------------
// Allocate the maze cells
//-----------------------------------------------------------------------------
bool CMaze::AllocateCells(int NumWidth, int NumHeight)
{
	// Make sure the cells aren't already allocated
	if(!IsCellsAllocated){
		//if(NumWidth > MAX_SIZE){NumWidth   = MAX_SIZE;}
		//if(NumHeight > MAX_SIZE){NumHeight = MAX_SIZE;}

		Width  = NumWidth;
		Height = NumHeight;

		// Error Handling...
		if(Width <= 0 || Height <= 0)
			return false;

		#ifdef SEED_RANDOM_GEN
		// Seed the random number generator
		RandGen.InitializeGenerator((int)GetTickCount());
		#else
		// Seed the random number generator
		RandGen.InitializeGenerator(1000);
		#endif

		// Allocate the maze cells
		pCells = new MazeCell[Width * Height];	

		// Initialize the cell 
		for(int y = 0; y < Height; y++){
			for(int x = 0; x < Width; x++){
				// Create a pointer to the current cell
				MazeCell *pCell = GetCellPtr(x, y);
				// Initialize it...
				BuildCell(pCell, x, y);
			}
		}

		IsCellsAllocated = TRUE;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Desallocate the maze cells
//-----------------------------------------------------------------------------
void CMaze::FreeCells()
{
	if(IsCellsAllocated){
		// Delete the maze cells
		SAFE_DELETE_ARRAY(pCells);	
		// Reset those variables
		IsCellsAllocated = FALSE;
		Width  = 0;
		Height = 0;
		ZeroMemory(&EntryPoint, sizeof(ENTRYPOINT));
		ZeroMemory(&ExitPoint,  sizeof(ENTRYPOINT));
	}
}

//-----------------------------------------------------------------------------
// Create the entire maze 
//-----------------------------------------------------------------------------
void CMaze::CreateMaze()
{
	// Make sure our cells are allocated
	if(IsCellsAllocated){
		Canceled = FALSE;
		// Reset the progress bar 
		SendMessage(hWnd, WM_MAZE_CREATION_PROGRESS, (WPARAM)0, (LPARAM)(Width * Height));
		// Generate the maze using recursion
		CreateMaze(NULL, -1, 0);
		// Make sure the progress bar is at 100%
		SendMessage(hWnd, WM_MAZE_CREATION_PROGRESS, (WPARAM)(Width * Height), (LPARAM)(Width * Height));

		SaveMaze("C:\\Temp\\MazeOutput.raw");
	}
}

//-----------------------------------------------------------------------------
// Create the entire maze 
//-----------------------------------------------------------------------------
void CMaze::SolveMaze()
{
	// Make sure our cells are allocated
	if(IsCellsAllocated){
		Canceled = FALSE;
		// Erase previous results
		ResetSolvingVariables();
		// Reset the progress bar 
		SendMessage(hWnd, WM_MAZE_SOLVING_PROGRESS, (WPARAM)0, (LPARAM)(Width * Height));
		// Generate the maze using recursion
		SolveMaze(NULL, -1, 0);
		// Make sure the progress bar is at 100%
		SendMessage(hWnd, WM_MAZE_SOLVING_PROGRESS, (WPARAM)(Width * Height), (LPARAM)(Width * Height));
	}
}

//-----------------------------------------------------------------------------
// Cancel the maze generation or solve
//-----------------------------------------------------------------------------
void CMaze::CancelMaze()
{
	Canceled = TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Initialise a cell
//-----------------------------------------------------------------------------
void CMaze::BuildCell(MazeCell *pCell, int x, int y)
{
	if(pCell){
		// Store the cell index
		pCell->Indx = (y*Width)+x;

		// Mark the cell as 'unvisited'
		pCell->Visited = FALSE;
		pCell->IsSolved = FALSE;

		// Set each wall to true
		pCell->WallsStatus[EAST_WALL]  = INTACT;
		pCell->WallsStatus[WEST_WALL]  = INTACT;
		pCell->WallsStatus[SOUTH_WALL] = INTACT;
		pCell->WallsStatus[NORTH_WALL] = INTACT;

		// Set each wall to true
		pCell->SolvedWallsStatus[EAST_WALL]  = WALL_UNSOLVED;
		pCell->SolvedWallsStatus[WEST_WALL]  = WALL_UNSOLVED;
		pCell->SolvedWallsStatus[SOUTH_WALL] = WALL_UNSOLVED;
		pCell->SolvedWallsStatus[NORTH_WALL] = WALL_UNSOLVED;

		// Set the neighborg pointers
		pCell->pNeighborgCells[EAST_WALL]  = GetCellPtr(x-1, y);
		pCell->pNeighborgCells[WEST_WALL]  = GetCellPtr(x+1, y);
		pCell->pNeighborgCells[NORTH_WALL] = GetCellPtr(x, y+1);
		pCell->pNeighborgCells[SOUTH_WALL] = GetCellPtr(x, y-1);
	}
}

//-----------------------------------------------------------------------------
// 'Break' one of the cell's wall
//-----------------------------------------------------------------------------
void CMaze::BreakWall(MazeCell *pCell, int Dir)
{
	if(pCell)
		pCell->WallsStatus[Dir] = BROKEN;
}

//-----------------------------------------------------------------------------
// Tell if a wall of a given cell's wall is broken or not
//-----------------------------------------------------------------------------
bool CMaze::IsWallBroken(MazeCell *pCell, int Dir)
{
	if(pCell)
		return pCell->WallsStatus[Dir]  == BROKEN;
	// Shouldn't happen...
	return false;
}

//-----------------------------------------------------------------------------
// Tell if a given cell been 'visited'
//-----------------------------------------------------------------------------
bool CMaze::IsCellVisited(MazeCell *pCell)
{
	if(pCell)
		return pCell->Visited == TRUE;
	// Shouldn't happen...
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Create the maze recursively
//-----------------------------------------------------------------------------
void CMaze::CreateMaze(MazeCell *pCell, int IncomingDir, int Level)
{
	// Abort the current operations if the user press esc
	if(Canceled || GetAsyncKeyState(VK_ESCAPE) & 0x8000){
		if(!Canceled)
			Canceled = TRUE;
		return;
	}

	// Pick a random unvisited cell to start with
	if(!pCell){
		do
		{
			int RandCell = RandGen.ExtractNumber() % (Width * Height);
			// Create a pointer to a random cell
			pCell = GetCellPtr(RandCell % Width, RandCell / Width);
		} while(pCell->Visited == TRUE);
	}

	// Generate the 4 directions at random
	int RandomDirs[4] = {0,0,0,0};
	GenRandomDirs(&RandomDirs[0]);

	// Loop through them...
	for(int Cpt = 0; Cpt < 4; Cpt++){

		// Make sure we dont start going from where we came...
		if(RandomDirs[Cpt] == IncomingDir)
			continue;

		// Create a pointer to the neighborg cell, make sure it exist and wasen't visited before...
		MazeCell *NeighborgCell = pCell->pNeighborgCells[RandomDirs[Cpt]];
		if(!NeighborgCell || NeighborgCell->Visited)
			continue;

		// Break the walls
		BreakWall(NeighborgCell, RandomDirs[Cpt]);
		BreakWall(pCell, MirrorDir(RandomDirs[Cpt]));

		// Mark the nodes as visited
		pCell->Visited = TRUE;
		NeighborgCell->Visited = TRUE;

		// Send the progress update message
		if(hWnd)
			SendMessage(hWnd, WM_MAZE_CREATION_PROGRESS, (WPARAM)CaclNumVisitedCells(), (LPARAM)(Width * Height));

		// Continue the process recursively...
		CreateMaze(NeighborgCell, MirrorDir(RandomDirs[Cpt]), Level+1);
	}

	// Are we done? If so, create the entry and exit point
	if(Level == 0){
		if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			return;

		GenEntryPoint(&EntryPoint);
		MazeCell *pCell = GetCellPtr(EntryPoint.x, EntryPoint.y);
		BreakWall(pCell, EntryPoint.Dir);

		do {
			GenEntryPoint(&ExitPoint);
		} while((EntryPoint.x == ExitPoint.x && EntryPoint.y == ExitPoint.y) || EntryPoint.Dir == ExitPoint.Dir);

		pCell = GetCellPtr(ExitPoint.x, ExitPoint.y);
		BreakWall(pCell, ExitPoint.Dir);

	}
}

//-----------------------------------------------------------------------------
// Try to solve the maze using the given entry and exit points
//-----------------------------------------------------------------------------
bool CMaze::SolveMaze(MazeCell *pCell, int IncomingDir, int Level)
{
	// Abort the current operations if the user press esc
	if(Canceled || GetAsyncKeyState(VK_ESCAPE) & 0x8000){
		if(!Canceled)
			Canceled = TRUE;
		if(Level == 0)
			ResetSolvingVariables();
		return true;
	}

	// Pick the entry point cell to start with
	if(!pCell){
		pCell = GetCellPtr(EntryPoint.x, EntryPoint.y);
		pCell->SolvedWallsStatus[EntryPoint.Dir] = WALL_SOLVING;
	}

	// Generate the 4 directions using a bit of logic...
	int RandomDirs[4] = {0,0,0,0};
	GenRandomDirs(&RandomDirs[0]);

	// Loop through them...
	for(int Cpt = 0; Cpt < 4; Cpt++){

		// Make sure we dont start going from where we came...
		if(RandomDirs[Cpt] == IncomingDir)
			continue;

		// Create a pointer to the neighborg cell, make sure it exist and wasen't visited before...
		MazeCell *NeighborgCell = pCell->pNeighborgCells[RandomDirs[Cpt]];
		if(!NeighborgCell || NeighborgCell->IsSolved)
			continue;

		if(!IsWallBroken(NeighborgCell, RandomDirs[Cpt]))
			continue;

		// Mark the nodes as 'in solving'
		pCell->IsSolved = TRUE;
		NeighborgCell->IsSolved = TRUE;

		// Mark the nodes as 'in solving'
		pCell->SolvedWallsStatus[MirrorDir(RandomDirs[Cpt])] = WALL_SOLVING;
		pCell->SolvedWallsStatus[MirrorDir(IncomingDir)]     = WALL_SOLVING;
		NeighborgCell->SolvedWallsStatus[RandomDirs[Cpt]]    = WALL_SOLVING;

		// Send the progress update message
		if(hWnd){
			SendMessage(hWnd, WM_MAZE_SOLVING_PROGRESS, (WPARAM)CaclNumSolvedCells(), (LPARAM)(Width * Height));
			Sleep(25);
		}

		// Check if the neightborg cell is the exit point
		int x = NeighborgCell->Indx % Width;
		int y = NeighborgCell->Indx / Width;
		
		if(x == ExitPoint.x && y == ExitPoint.y){
			pCell->SolvedWallsStatus[MirrorDir(RandomDirs[Cpt])] = WALL_SOLVING;
			pCell->SolvedWallsStatus[MirrorDir(IncomingDir)]     = WALL_SOLVING;
			NeighborgCell->SolvedWallsStatus[RandomDirs[Cpt]]    = WALL_SOLVING;
			NeighborgCell->SolvedWallsStatus[ExitPoint.Dir]      = WALL_SOLVING;
			return true;
		} else {
			// Continue the solving process recursively...
			BOOL IsSolved = SolveMaze(NeighborgCell, MirrorDir(RandomDirs[Cpt]), Level+1);
			if(IsSolved){
				pCell->SolvedWallsStatus[MirrorDir(RandomDirs[Cpt])] = WALL_SOLVING;
				pCell->SolvedWallsStatus[MirrorDir(IncomingDir)]     = WALL_SOLVING;
				NeighborgCell->SolvedWallsStatus[RandomDirs[Cpt]]    = WALL_SOLVING;
				return true;
			} else {
				// Mark the nodes as 'in solving'
				pCell->IsSolved = FALSE;
				NeighborgCell->IsSolved = FALSE;

				// Mark the nodes as 'in solving'
				pCell->SolvedWallsStatus[MirrorDir(RandomDirs[Cpt])] = WALL_UNSOLVED;
				pCell->SolvedWallsStatus[MirrorDir(IncomingDir)]     = WALL_UNSOLVED;
				NeighborgCell->SolvedWallsStatus[RandomDirs[Cpt]]    = WALL_UNSOLVED;


				// Send the progress update message
				if(hWnd){
					SendMessage(hWnd, WM_MAZE_SOLVING_PROGRESS, (WPARAM)CaclNumSolvedCells(), (LPARAM)(Width * Height));
					Sleep(25);
				}

			}
		}
	}

	return false;
}


//-----------------------------------------------------------------------------
// Generate the maze image
//-----------------------------------------------------------------------------
void CMaze::GenMazeBitmapBuffer(BYTE *pBuf, int WidthX3, int HeightX3)
{
	const int BufSize  = WidthX3 * HeightX3;

	// Erase the buffer
	ZeroMemory(pBuf, BufSize);

	// Fill up the buffer...
	int RealX = 0, RealY = 0;
	for(int y = 1; y < HeightX3; y+=3){
		for(int x = 1; x < WidthX3; x+=3){
		
			MazeCell *pCell = GetCellPtr(RealX, RealY);

			// Make sure the cell exist...
			if(pCell){
				// Set the center white
				if(pCell->Visited)
					pBuf[(y*WidthX3)+x] = 0xFF;

				// Set the broken walls white
				if(pCell->WallsStatus[EAST_WALL]  == BROKEN)
					pBuf[(y*WidthX3)+(x+1)] = 0xFF;
				if(pCell->WallsStatus[WEST_WALL]  == BROKEN)
					pBuf[(y*WidthX3)+(x-1)] = 0xFF;
				if(pCell->WallsStatus[NORTH_WALL] == BROKEN)
					pBuf[((y-1)*WidthX3)+x] = 0xFF;
				if(pCell->WallsStatus[SOUTH_WALL] == BROKEN)
					pBuf[((y+1)*WidthX3)+x] = 0xFF;

				// Set the broken walls white
				if(pCell->SolvedWallsStatus[EAST_WALL]  == WALL_SOLVING){
					pBuf[(y*WidthX3)+(x+1)] = 0x40;
					pBuf[(y*WidthX3)+x] = 0x40;
				}
				if(pCell->SolvedWallsStatus[WEST_WALL]  == WALL_SOLVING){
					pBuf[(y*WidthX3)+(x-1)] = 0x40;
					pBuf[(y*WidthX3)+x] = 0x40;
				}
				if(pCell->SolvedWallsStatus[NORTH_WALL] == WALL_SOLVING){
					pBuf[((y-1)*WidthX3)+x] = 0x40;
					pBuf[(y*WidthX3)+x] = 0x40;
				}
				if(pCell->SolvedWallsStatus[SOUTH_WALL] == WALL_SOLVING){
					pBuf[((y+1)*WidthX3)+x] = 0x40;
					pBuf[(y*WidthX3)+x] = 0x40;
				}
			}
			RealX++;
		}	
		RealY++;
		RealX = 0;
	}
}

//-----------------------------------------------------------------------------
// Save the maze as a .raw image
//-----------------------------------------------------------------------------
void CMaze::SaveMazeAsRawImage(char *pName)
{
	const int WidthX3  = Width  * 3;
	const int HeightX3 = Height * 3;
	const int BufSize  = WidthX3 * HeightX3;
	
	FILE *f = fopen(pName, "wb+");
	if(!f){return;}

	BYTE *pBuf = new BYTE[BufSize];
	GenMazeBitmapBuffer(pBuf, WidthX3, HeightX3);

	// Write the buffer data into the file
	fwrite(pBuf, 1, BufSize, f);

	// Free the buffer memory
	SAFE_DELETE_ARRAY(pBuf);

	// Close the file
	fclose(f);
}

//-----------------------------------------------------------------------------
// Load the maze from binary data
//-----------------------------------------------------------------------------
bool CMaze::LoadMaze(char *pName, int *pWidth, int *pHeight)
{
	// Try to open the file
	FILE *f = fopen(pName, "rb");
	if(!f){return false;}

	int w, h;
	int NumBytesRead = 0;

	// Read the maze dimension
	NumBytesRead = fread(&w,  1, sizeof(int), f);
	if(NumBytesRead != sizeof(int)){return false;}
	NumBytesRead = fread(&h, 1, sizeof(int), f);
	if(NumBytesRead != sizeof(int)){return false;}

	// Make sure values are ok
	if(w <= 0 || h <= 0)
		return false;

	// If the maze is allocated, free it and create a new one
	FreeCells();
	AllocateCells(w, h);

	*pWidth  = w;
	*pHeight = h;
	const int Size = w * h;

	// Read the cells, one by one
	for(int Cpt = 0; Cpt < Size; Cpt++){
		NumBytesRead = fread(&pCells[Cpt].Visited, 1, sizeof(int), f);
		if(NumBytesRead != sizeof(int)){return false;}
		NumBytesRead = fread(&pCells[Cpt].WallsStatus[0], 1, sizeof(BOOL)*4, f);
		if(NumBytesRead != sizeof(BOOL)*4){return false;}
	}

	// Read the entry and exit points
	fread(&EntryPoint, 1, sizeof(EntryPoint), f);
	fread(&ExitPoint,  1, sizeof(ExitPoint),  f);

	// Close the file
	fclose(f);

	return true;
}

//-----------------------------------------------------------------------------
// Save the maze as binary data
//-----------------------------------------------------------------------------
bool CMaze::SaveMaze(char *pName)
{
	if(!IsCellsAllocated)
		return false;

	// Try to create the file
	FILE *f = fopen(pName, "wb+");
	if(!f){return false;}

	// Write the maze dimension
	fwrite(&Width,  1, sizeof(int), f);
	fwrite(&Height, 1, sizeof(int), f);

	const int Size = Width * Height;

	// Write the cells, one by one
	for(int Cpt = 0; Cpt < Size; Cpt++){
		fwrite(&pCells[Cpt].Visited, 1, sizeof(int), f);
		fwrite(&pCells[Cpt].WallsStatus[0], 1, sizeof(BOOL)*4, f);
	}

	// Write the entry and exit points
	fwrite(&EntryPoint, 1, sizeof(EntryPoint), f);
	fwrite(&ExitPoint,  1, sizeof(ExitPoint),  f);

	// Close the file
	fclose(f);

	return true;
}

