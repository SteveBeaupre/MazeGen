#include "Random.h"

// Create a length 624 array to store the state of the generator
int MT[624]; 
int index = 0;

//
CRandom::CRandom()
{
	InitializeGenerator(GetTickCount());
}

// Initialize the generator from a seed
void CRandom::InitializeGenerator(int seed)
{
	MT[0] = seed;
	for(int i = 1; i <= 623; i++){ // loop over each other element
		MT[i] = (1812433253 * (MT[i-1] ^ (MT[i-1] >> 30)) + i);
	}
}


// Extract a tempered pseudorandom number based on the index-th value,
// calling generateNumbers() every 624 numbers
int CRandom::ExtractNumber()
{
	if(index == 0)
		GenerateNumbers();
	
	int y = MT[index];
	y = y ^ (y >> 11);
	y = y ^ ((y << 7) & (2636928640));
	y = y ^ ((y << 15) & (4022730752));
	y = y ^ (y >> 18);
	
	index = (index + 1) % 624;
	return y;
}

// Generate an array of 624 untempered numbers
void CRandom::GenerateNumbers()
{
	for(int i = 1; i <= 623; i++){ 
		
		int y = ((MT[i] & 0x80000000) + (MT[i+1] & 0x7FFFFFFF)) % 624;
		
		MT[i] = MT[(i + 397) % 624] ^ (y >> 1);
		
		if((y % 2) == 1) // y is odd
			MT[i] = MT[i] ^ (2567483615);
	}
}