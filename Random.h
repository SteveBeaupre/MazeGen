#ifndef _CRANDOM_H_
#define _CRANDOM_H_
#ifdef __cplusplus

#include "Windows.h"

class CRandom {
public:
	CRandom();
private:
	void GenerateNumbers();
public:
	void InitializeGenerator(int seed);
	int  ExtractNumber();
};

#endif
#endif //_CRANDOM_H_






 
