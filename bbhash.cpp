#include "bbhash.h"

// constructor which takes a pointer to a vector of strings and a double gamma value
bbhash::bbhash(vector<string> *keysPtr, uint64_t n, double gamma) {
	this->keySetSize = n;
	this->gamma = gamma;
	this->maxLevels = 3;
	this->regularHashTable = false;
	this->numLevels = 0;

	buildBbhash(*keysPtr);
}

// this function creates the hash levels and the regular hash table if necessary
void bbhash::buildBbhash(vector<string> keys) {
	keySetSize = keys.size();
	// need the current keys to look at and the keys that collided which will need to be looked at the next level
	vector<string> currKeys = keys;
	vector<string> nextKeys;
	for (uint64_t i = 0; i < maxLevels; i++){
		numLevels++;
		// the level size indicates how large the bitvector at this level will be (based on gamma)
		int levelSize = floor(currKeys.size() * gamma);
		// this is the actual bitvector that will be stored
		bit_vector a(levelSize, 0);
		// a 1 index in this bitvector means there was a collision at this index
		bit_vector c(levelSize, 0);

		// this string array holds the first key that hashed to this index, in case there is a collision
		unordered_map<uint64_t, string> firstCollisionMap;
		unordered_map<uint64_t, string>::iterator itr;

		// for each key left, hash it to an index and see if there are any collisions
		for (int j = 0; j < currKeys.size(); j++){
			uint64_t hash = getHash(currKeys[j], i) % levelSize;
			// a value of 0 in c means that there have not yet been any collisions at this index
			if (c[hash] == 0){
				// if 'a' is 0 at this index, then this is the first key to hash to this index
				if (a[hash] == 0){
					a[hash] = 1;
					firstCollisionMap[hash] = currKeys[j];
				// if 'a' is 1, then there has been 1 key that has hashed to this index, so we now have a collision
				} else {
					a[hash] = 0;
					c[hash] = 1;
					nextKeys.push_back(currKeys[j]);
					itr = firstCollisionMap.find(hash);
					nextKeys.push_back(itr->second);
				}
			// there has already been a collision here, so just send this key to the next round
			} else {
				nextKeys.push_back(currKeys[j]);
			}
		}

		// store the bitvector for this level
		levels.push_back(a);

		currKeys = nextKeys;
		nextKeys.clear();

		// if none of the keys collided, then we are done
		if (currKeys.size() == 0){
			break;
		}
	}

	// now need to check to see if there are still more keys, in which case they will be stored in a regular hash table
	if (currKeys.size() != 0){
		regularHashTable = true;
		uint64_t count = keySetSize - currKeys.size();
		unordered_map<string, uint64_t> umap;
		for (int i = 0; i < currKeys.size(); i++){
			umap[currKeys[i]] =  count;
			count++;
		}
		endMap = umap;
	}

	// maybe this will work better if I do all the rank supports at once.
	for (int i = 0; i < levels.size(); i++){
		rank_support_v<> rs(&levels[i]);
		rankSupports.push_back(rs);
	}
}

// *** Hash Use Functions *** //

// based on the key, the index in the values array is returned (or -1 if the key is not found)
uint64_t bbhash::getBbhashIndex(string key){
	//cout << "In getBbhashIndex, getting index for key: " << key << endl;
	uint64_t totalRank = keySetSize;
	for (uint64_t i = 0; i < levels.size(); i++){
		uint64_t hash = getHash(key, i) % levels[i].size();
		// if the bitvector is 1, we're done
		if (levels[i][hash] == 1){
			totalRank = rankSupports[i](hash);
			// now have to go through all previous levels to get the number of 1's which came before
			for (int j = i - 1; j >= 0; j--){
				totalRank += rankSupports[j](levels[j].size());
			}
			break;
		}
	}

	// the key has not been found, so need to look in the regular hash table if it exists
	if (totalRank == keySetSize && regularHashTable){
		unordered_map<string, uint64_t>::iterator it;
		it = endMap.find(key);
		if (it != endMap.end()){
			totalRank = it->second;
		}
	}

	return totalRank;
}

// *** Hashing Functions *** //

uint64_t bbhash::getHash(string val, uint64_t seed) {
	const void * buffer = val.c_str();
	// remember to cite the use of this
	return XXHash64::hash(buffer, val.size(), seed);
}

// *** Benchmarking Functions *** //

uint64_t bbhash::getTotalBits(){
	uint64_t numBits = 0;
	for (int i = 0; i < levels.size(); i++){
		numBits += levels[i].size();
	}
	return numBits;
}

uint64_t bbhash::getNumKeysInEndTable(){
	if (regularHashTable){
		return endMap.size();
	}
	return 0;
}

// *** Utility Functions *** //

vector<string> bbhash::getKeysFromFile(string& fname) {
	ifstream infile;
	infile.open(fname, ios::in);

	vector<string> keyVec;
	string line;
	// get each line (which will have key ;; value
	while (getline(infile, line)) {
		keyVec.push_back(line);
	}
	infile.close();

	return keyVec;
}

void bbhash::createDataFile(uint64_t n, string& fname){
	ofstream outfile;
	outfile.open(fname);
	for (uint64_t i = 0; i < n; i++){
		outfile << i << endl;
	}
	outfile.close();
}

void bbhash::printStructure(){
	for (uint64_t i = 0; i < numLevels; i++){
		cout << "Level: " << i << endl;
		cout << "Bitvector:" << endl;
		bit_vector bv = levels[i];
		for (int j = 0; j < bv.size(); j++){
			cout << bv[j] << endl;
		}
		cout << "Rank Support:" << endl;
		rank_support_v<> rs = rankSupports[i];
		for (int j = 0; j < bv.size(); j++){
			cout << rs(j) << endl;
		}
		cout << endl;
	}
	if (regularHashTable){
		cout << "Printing the ending map:" << endl;
		unordered_map<string, uint64_t>::iterator itr;
		for (itr = endMap.begin(); itr != endMap.end(); itr++){
        cout << itr->first << "  " << itr->second << endl;
     }
	}
}

void bbhash::printRankSupport(rank_support_v<> rs){
	cout << "Printing Rank Support:" << endl;
	cout << "Size is: " << rs.size() << endl;
	for (int i = 0; i < rs.size(); i++){
		cout << rs(i) << endl;
	}
}
