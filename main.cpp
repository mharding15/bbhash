#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <vector>
#include <string>
#include <math.h>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include "bbhash.h"
#include <sdsl/bit_vectors.hpp>

using namespace std;
using namespace sdsl;

int main() {

	int n = 1000000;
	int quotient = 1000000/n;
	vector<string> keys;

	unordered_map<string, uint64_t> testMap;

	for (uint64_t i = 0; i < n; i++){
		keys.push_back(to_string(i));
		testMap[to_string(i)] = i;
	}

	string filename = "OUTPUT_n_" + to_string(n) + ".txt";
	ofstream outfile;
	outfile.open(filename);

	cout << "About to start the bbhash tests...";
	for (double gamma = 1.0; gamma < 10.1; gamma += 1){
		cout << "gamma = " << gamma << endl;

		bbhash h(&keys, keys.size(), gamma);

		auto start = chrono::steady_clock::now();
		for (int j = 0; j < quotient; j++){
			for (int k = 0; k < keys.size(); k++){
				h.getBbhashIndex(keys[k]);
			}
		}
		auto end = chrono::steady_clock::now();

		cout << "ellapsed time for bbhash (ms): " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;

		outfile << "gamma = " << to_string(gamma) << endl;
		outfile << "ellapsed time for bbhash (ms): " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;
		outfile << "number of bits: " << h.getTotalBits() << endl;
		outfile << "end hash size: " << h.getNumKeysInEndTable() << endl;

		// alien keys
		int alienKeysFound = 0;
		for (int i = n; i < (n + 1000); i++){
			uint64_t idx = h.getBbhashIndex(to_string(i));
			if (idx == n){
				alienKeysFound++;
			}
		}
		outfile << "number of alien keys found: " << to_string(alienKeysFound) << endl;
		outfile << "\n-------------------\n" << endl;
	}

	cout << "About to do the timing for the regular hash table..." << endl;
	unordered_map<string, uint64_t>::iterator it;
	auto start = chrono::steady_clock::now();
	for (int j = 0; j < quotient; j++){
		for (int k = 0; k < keys.size(); k++){
			it = testMap.find(keys[k]);
		}
	}
	auto end = chrono::steady_clock::now();

	cout << "ellapsed Time for regular hash: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;
	outfile << "ellapsed Time for regular hash: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;

	outfile.close();

}
