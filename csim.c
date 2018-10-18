// #define _GNU_SOURCE
// extern "C" {
	// #include "cachelab.h"
// }
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <ctime>
#include <fstream>

#include <stdint.h>
#include <math.h>
#include <string>
extern "C" void printSummary(int hits, int misses, int evictions);

using namespace std;

class Cache;
void readArgs(int argc, char* const argv[]);
bool v = false;
Cache* cache;
char* traceFileName;


class Cache {
	class Set {
		class Block {
		public:
			uint64_t tag;
			uint64_t address;
			Block() {}
			void set(uint64_t replaceWith, uint64_t addrForDebug) {
				address = addrForDebug;
				tag = replaceWith;
			}
		};
	public:
		int numBlocks;
		vector<Block> blockArray;
		Set(int n) {
			numBlocks = n;
		}
		uint64_t getTagBit(uint64_t address) {
			for (int i = 0; i < cache->cacheBlockBits+cache->setIndexBits; ++i) {
				address = address >> 1;
			}
			return address;
		}
		int find(uint64_t tag) {
			for (int i = 0; i < blockArray.size(); ++i) {
				if (blockArray[i].tag == tag) {
					Block b = blockArray[i];
					blockArray.erase(blockArray.begin()+i);
					blockArray.insert(blockArray.begin(), b);
					return 1;
				} 
			}
			return -1;
		}
		void fetch(uint64_t address) {
			uint64_t tag = getTagBit(address);

			if (blockArray.size() < numBlocks) {
				Block b;
				b.set(tag, address);
				blockArray.insert(blockArray.begin(), b);
				return;
			}

			blockArray.pop_back();
			Block b;
			b.set(tag, address);
			blockArray.insert(blockArray.begin(), b);
			cache->eviction();
		}

		void access(uint64_t address) {
			int i = find(getTagBit(address));
			if (i > 0) {
				cache->hit();
				if (v) cout << " "<< (void*)blockArray[i].address<< " ";
			} else {
				cache->miss();
				fetch(address);
			}
		}
	};
public:
	int setIndexBits, numSets, blocksPerSet, cacheBlockBits, cacheBlockSize;
	int hits, misses, evictions;

	vector<Set> setArray;

	Cache(int _setIndexBits, int _numSets, int _blocksPerSet, int _cacheBlockBits, int _cacheBlockSize) {
		setIndexBits = _setIndexBits;
		numSets = _numSets;
		blocksPerSet = _blocksPerSet;

		for (int i = 0; i < numSets; ++i) {
			setArray.push_back(Set(blocksPerSet));
		}

		cacheBlockBits = _cacheBlockBits;
		cacheBlockSize = _cacheBlockSize;

		hits = 0;
		misses = 0;
		evictions = 0;
	}
	void hit() {++hits; if (v) cout << "hit ";}
	void miss() {++misses; if (v) cout << "miss ";}
	void eviction() {++evictions; if (v) cout << "eviction ";}

	uint64_t getSetIndex(uint64_t address) {
		if (setIndexBits == 0) return 0;
		for (int i = 0; i < cacheBlockBits; ++i) {
			address = address >> 1;
		}

		uint64_t returnVal = 0x1;
		for (int i = 0; i < setIndexBits-1; ++i) {
			returnVal = returnVal << 1;
			returnVal = returnVal|0x1;
		}
		returnVal = returnVal&address;
		return returnVal;
	}
	void access(uint64_t address) {
		setArray[getSetIndex(address)].access(address);

	}
};


int main(int argc, char* const argv[])
{	
	readArgs(argc, argv);
	// Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>
	 // L 10,1
	 // M 20,1
	 // L 22,1
	char buff;
	ifstream file;
	file.open(traceFileName, ifstream::in);

	while (file.good()) {
		file >> buff;
		if (buff == ' ' || buff == '\n') {continue;}
		else if (buff == 'I') { string str = ""; getline(file, str); continue; }
		else if (buff != 'L' && buff != 'S' && buff != 'M') {continue;}
		char type = buff;

		string str = "";
		while (file >> buff) {
			if (buff == ' ') continue;
			if (buff == ',') break;
			str += buff;
		}

		// char* end = (char*)str.c_str()+str.length();
		uint64_t address = strtoul(str.c_str(), NULL, 16);
		int bytes;
		file >> bytes;
		if (v) {
			cout << type<<" ";
			cout << (void*)address << ",";
			// cout << str << " ";
			cout <<  bytes << " ";
		}

		if (type == 'M') {
			cache->access(address);
			cache->access(address);
		} else if (type == 'L') {
			cache->access(address);
		} else if (type == 'S') {
			cache->access(address);
		}
		if (v) {
			cout << endl;
		}

	}
    printSummary(cache->hits, cache->misses, cache->evictions);
    return 0;
}
/*
run -s 4 -E 1 -b 4 -t traces/yi.trace
./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace
./csim -s 4 -E 1 -b 4 -t traces/yi.trace
./csim-ref -v -s 2 -E 2 -b 3 -t traces/trans.trace
./csim -s 2 -E 2 -b 3 -t traces/trans.trace
*/
void readArgs(int argc, char* const argv[]) {
	int option;
	int setIndexBits, numSets, blocksPerSet, cacheBlockBits, cacheBlockSize;

	while ((option = getopt(argc, argv, "s:E:b:t:v::")) != -1) {
		switch (option) {
			case 's':  // -s <num>   Number of set index bits. numSets = 2^s
				setIndexBits = atoi(optarg);
				numSets = pow(2.0, setIndexBits);
				break;
			case 'E':  // -E <num>   Number of blocks per set.
				blocksPerSet = atoi(optarg);
				break;
			case 'b':  // -b <num>   Number of block offset bits. blocksize = 2^b
				cacheBlockBits = atoi(optarg);
				cacheBlockSize = pow(2.0, cacheBlockBits);
				break;
			case 't':  // -t <file>  Trace file.
				traceFileName = optarg;
				break;
			case 'v':  // -t <file>  Trace file.
				v = true;
				break;
		}
	}
	cache = new Cache(setIndexBits, numSets, blocksPerSet, cacheBlockBits, cacheBlockSize);
}

