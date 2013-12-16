#include "CheckedWallsList.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CheckedWallsList::CheckedWallsList()
{
	NumNodesAllocated = 0;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CheckedWallsList::~CheckedWallsList()
{
	ClearAll();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CheckedWallsList::InitRootNode()
{
	Root.Dir = -1;

	Root.Prev = NULL;
	Root.Next = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CheckedWallsList::GetNodesCount()
{
	UINT Count = 0;
	CheckedWallsNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		Count++;
	}

	// Return the number of nodes of this pile
	return Count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CheckedWallsNode* CheckedWallsList::GetNode(UINT Indx)
{
	UINT Count = 0;
	CheckedWallsNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the one we want
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		if(Count == Indx)
			return pNode;
		Count++;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CheckedWallsList::GetNodeDir(UINT Indx)
{
	UINT Count = 0;
	CheckedWallsNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the one we want
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		if(Count == Indx)
			return pNode->Dir;
		Count++;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CheckedWallsNode* CheckedWallsList::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CheckedWallsNode* CheckedWallsList::GetFirstNode(bool CanReturnRoot)
{
	CheckedWallsNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	if(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;
	
	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CheckedWallsNode* CheckedWallsList::GetLastNode(bool CanReturnRoot)
{
	CheckedWallsNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CheckedWallsList::PushNode(UINT Dir)
{
	CheckedWallsNode *pNode = GetLastNode(true);

	// Allocate the new node
	pNode->Next = new CheckedWallsNode;
	NumNodesAllocated++;
	CheckedWallsNode *pNewNode = pNode->Next;

	// Update pointers
	pNewNode->Next = NULL;
	pNewNode->Prev = pNode;

	// Copy the cell data 
	pNewNode->Dir = Dir;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckedWallsList::PopNode()
{
	CheckedWallsNode *pNode = GetLastNode(true);

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Update pointers
	pNode->Prev->Next = NULL;

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CheckedWallsList::DeleteNode(UINT Indx)
{
	CheckedWallsNode *pNode = GetNode(Indx);

	// If this the root node, we're done...
	if(!pNode || pNode == &Root){return false;} // return faillure

	// Update pointers
	CheckedWallsNode *pPrevNode = pNode->Prev;
	CheckedWallsNode *pNextNode = pNode->Next;

	pPrevNode->Next = pNextNode;
	if(pNextNode)
		pNextNode->Prev = pPrevNode;

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CheckedWallsList::ClearAll()
{
	// Delete everything...
	while(PopNode());
}
