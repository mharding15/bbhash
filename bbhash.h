////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This algorithm makes use of xxhash64.h written by Stephan Brumme based on Yann Collet's xxHash32 and xxHash64 algorithms (2016)//
// https://github.com/stbrumme/xxhash/blob/master/xxhash64.h																																			//
//																																																																//
// AND																																																														//
//																																																																//
// sdsl-lite, written by Simon Gog (2014)																																													//
// https://github.com/simongog/sdsl-lite																																													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <unordered_map>
#include <sdsl/bit_vectors.hpp>
#include "xxhash64.h"

using namespace std;
using namespace sdsl;

class bbhash
{
public:
	// the different bitvectors that store whether a key for the corresponsing hash exists
	vector<bit_vector> levels;
	// the vector of rank support bit vectors for each level
	vector<rank_support_v<>> rankSupports;
	// the array of strings that will act as the regular hash table created at the end
	unordered_map<string, uint64_t> endMap;
	// the gamma parameter
 	double gamma;
	// various fields about sizes
	uint64_t keySetSize, maxLevels, numLevels;
	// true if the use of a regular hash table at the end was necessary and false if not
	bool regularHashTable;

	bbhash(vector<string> *keysPtr, uint64_t n, double gamma);
	void buildBbhash(vector<string> keys);
	uint64_t getBbhashIndex(string key);

	uint64_t getTotalBits();
	uint64_t getNumKeysInEndTable();

	static vector<string> getKeysFromFile(string& infile);
	static void createDataFile(uint64_t n, string& outfile);
	void printStructure();

private:
	uint64_t getHash(string val, uint64_t seed);
	void printRankSupport(rank_support_v<> rs);
};
