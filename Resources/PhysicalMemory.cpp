#include "PhysicalMemory.h"
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>


typedef std::vector<word_t> page_t;

int evict_counter = 0;

std::vector<page_t> RAM;
std::unordered_map<uint64_t, page_t> swapFile;

void initialize() {
    RAM.resize(NUM_FRAMES, page_t(PAGE_SIZE));
}

void PMread(uint64_t physicalAddress, word_t* value) {

    if (RAM.empty())
        initialize();

    assert(physicalAddress < RAM_SIZE);

    *value = RAM[physicalAddress / PAGE_SIZE][physicalAddress
             % PAGE_SIZE];
//    std::cout << "read " << *value << " from physical address " << physicalAddress << std::endl;
 }

void PMwrite(uint64_t physicalAddress, word_t value) {
//    std::cout << "write " << value << " into physical address " << physicalAddress<< std::endl;
    if (RAM.empty())
        initialize();

    assert(physicalAddress < RAM_SIZE);

    RAM[physicalAddress / PAGE_SIZE][physicalAddress
             % PAGE_SIZE] = value;
}

void PMevict(uint64_t frameIndex, uint64_t evictedPageIndex) {
//    std::cout << "evict " << evictedPageIndex << " from the frame " <<frameIndex<< std::endl;
    if (RAM.empty())
        initialize();

    assert(swapFile.find(evictedPageIndex) == swapFile.end());
    assert(frameIndex < NUM_FRAMES);
    assert(evictedPageIndex < NUM_PAGES);

    swapFile[evictedPageIndex] = RAM[frameIndex];
    evict_counter++;
}

void PMrestore(uint64_t frameIndex, uint64_t restoredPageIndex) {
//    std::cout << "restore " << restoredPageIndex << " from the hard drive to the frame " << frameIndex << std::endl;
    if (RAM.empty())
        initialize();

    assert(frameIndex < NUM_FRAMES);

    // page is not in swap file, so this is essentially
    // the first reference to this page. we can just return
    // as it doesn't matter if the page contains garbage
    if (swapFile.find(restoredPageIndex) == swapFile.end())
        return;

    RAM[frameIndex] = std::move(swapFile[restoredPageIndex]);
    swapFile.erase(restoredPageIndex);
}

void printRam()
{
    for (uint64_t  i = 0; i < RAM_SIZE; i++)
    {
        word_t tmp;
        PMread(i, &tmp);
        std::cout << i << ": " << tmp << std::endl;

    }
}


void printEvictionCounter()
{
    std::cout << evict_counter << std::endl;
}