README for Homework 2 CMSC858D

The github url for this repo is: https://github.com/mharding15/bbhash

My implementation of the bbhash algorithm is used by providing a point to a vector of string keys and a gamma value to the constructor of the bbhash class. After creating a
bbhash object the index for any key can be retrieved by using the getBbhashIndex(string key) function. It will return a uint64_t which is a unique index from 0 to n - 1
as long as the key came from the original key set.

I have provided a Makefile that can be used to compile the source code, which includes a "main.cpp" file. This file can be used to create a bbhash object and create a
bbhash hash table. 

The code uses a xxhash impementation by Stephan Brumme, based on Yann Collet's xxHash32 and xxHash64 algorithms
https://github.com/stbrumme/xxhash
https://github.com/Cyan4973/xxHash

It also uses sdsl-lite, written by Simon Gog
https://github.com/simongog/sdsl-lite
