#ifndef _CHECKED_WALLS_LIST_H_
#define _CHECKED_WALLS_LIST_H_
#ifdef __cplusplus

#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
#include "Windows.h"
#include "stdio.h"
#include "SafeKill.h"

//----------------------------------------------------------------------//
// Cell data node
//----------------------------------------------------------------------//
struct CheckedWallsNode {
	UINT Dir;

	struct CheckedWallsNode *Prev;
	struct CheckedWallsNode *Next;
};

//----------------------------------------------------------------------//
// The unvisited cells list class
//----------------------------------------------------------------------//
class CheckedWallsList {
public:
	CheckedWallsList();
	~CheckedWallsList();
private:
	UINT NumNodesAllocated;

	CheckedWallsNode  Root;
	CheckedWallsNode* GetRootNode();
	void              InitRootNode();
public:
	UINT GetNodesCount();
	UINT GetNumAllocatedNodes(){return NumNodesAllocated;}

	CheckedWallsNode* GetNode(UINT Indx);
	UINT              GetNodeDir(UINT Indx);

	CheckedWallsNode* GetFirstNode(bool CanReturnRoot = false);
	CheckedWallsNode* GetLastNode(bool CanReturnRoot = false);
public:
	void PushNode(UINT Dir);
	bool PopNode();
	bool DeleteNode(UINT Indx);
	void ClearAll();
};

#endif
#endif //_CHECKED_WALLS_LIST_H_
