#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <algorithm>

// Helper to clear a frame by writing zero to all words
static void clearFrame(uint64_t frame)
{
    for (uint64_t i = 0; i < PAGE_SIZE; ++i)
    {
        PMwrite(frame * PAGE_SIZE + i, 0);
    }
}

// Extract the indices of each level and the offset from a virtual address
static void decomposeAddress(uint64_t virtualAddress,
                             uint64_t indices[TABLES_DEPTH],
                             uint64_t *offset)
{
    *offset = virtualAddress & ((1ULL << OFFSET_WIDTH) - 1);
    virtualAddress >>= OFFSET_WIDTH;
    for (int i = TABLES_DEPTH - 1; i >= 0; --i)
    {
        indices[i] = virtualAddress & (PAGE_SIZE - 1);
        virtualAddress >>= OFFSET_WIDTH;
    }
}

// Calculate cyclic distance between two page numbers
// Calculate a weighted cyclic distance between two page numbers.
// The weight is determined by the parity of the compared page (b).
static uint64_t weightedCyclicDistance(uint64_t a, uint64_t b)
{
    uint64_t diff = a > b ? a - b : b - a;
    uint64_t alt = NUM_PAGES - diff;
    uint64_t baseDist = std::min(diff, alt);

    uint64_t weight = (b % 2 == 0) ? WEIGHT_EVEN : WEIGHT_ODD;
    return baseDist * weight;
}

struct TraversalResult
{
    bool emptyFound = false;
    uint64_t emptyFrame = 0;
    uint64_t emptyParent = 0;
    uint64_t emptyIndex = 0;

    uint64_t maxFrame = 0;

    uint64_t evictFrame = 0;
    uint64_t evictParent = 0;
    uint64_t evictIndex = 0;
    uint64_t evictPage = 0;
    uint64_t bestScore = 0;
};

// Depth-first traversal used for locating frames for allocation
static void traverse(uint64_t frame, int depth, uint64_t pagePrefix,
                     const bool forbidden[NUM_FRAMES], uint64_t targetPage,
                     uint64_t parent, uint64_t parentIdx, TraversalResult &res)
{
    res.maxFrame = std::max(res.maxFrame, frame);

    if (depth == TABLES_DEPTH)
    {
        if (!forbidden[frame])
        {
            uint64_t dist = weightedCyclicDistance(targetPage, pagePrefix);
            if (dist > res.bestScore)
            {
                res.bestScore = dist;
                res.evictFrame = frame;
                res.evictParent = parent;
                res.evictIndex = parentIdx;
                res.evictPage = pagePrefix;
            }
        }
        return;
    }

    bool isEmpty = true;
    for (uint64_t i = 0; i < PAGE_SIZE; ++i)
    {
        word_t val = 0;
        PMread(frame * PAGE_SIZE + i, &val);
        if (val != 0)
        {
            isEmpty = false;
            traverse(val, depth + 1, (pagePrefix << OFFSET_WIDTH) | i,
                     forbidden, targetPage, frame, i, res);
        }
    }

    if (isEmpty && frame != 0 && !forbidden[frame] && !res.emptyFound)
    {
        res.emptyFound = true;
        res.emptyFrame = frame;
        res.emptyParent = parent;
        res.emptyIndex = parentIdx;
    }
}

// Choose a frame for a new page or table according to the rules
static uint64_t allocateFrame(uint64_t targetPage,
                              const uint64_t path[TABLES_DEPTH + 1], int pathLen)
{
    bool forbidden[NUM_FRAMES] = {false};
    for (int i = 0; i < pathLen; ++i)
    {
        if (path[i] < NUM_FRAMES)
        {
            forbidden[path[i]] = true;
        }
    }

    TraversalResult res{};
    traverse(0, 0, 0, forbidden, targetPage, 0, 0, res);

    if (res.emptyFound)
    {
        PMwrite(res.emptyParent * PAGE_SIZE + res.emptyIndex, 0);
        return res.emptyFrame;
    }

    if (res.maxFrame + 1 < NUM_FRAMES)
    {
        return res.maxFrame + 1;
    }

    PMwrite(res.evictParent * PAGE_SIZE + res.evictIndex, 0);
    PMevict(res.evictFrame, res.evictPage);
    return res.evictFrame;
}

static uint64_t getFrame(uint64_t pageNumber,
                         const uint64_t indices[TABLES_DEPTH],
                         uint64_t path[TABLES_DEPTH + 1])
{
    uint64_t frame = 0; // start from root
    path[0] = 0;
    for (int depth = 0; depth < TABLES_DEPTH; ++depth)
    {
        word_t val = 0;
        PMread(frame * PAGE_SIZE + indices[depth], &val);
        if (val == 0)
        {
            uint64_t newFrame = allocateFrame(pageNumber, path, depth + 1);
            clearFrame(newFrame);
            if (depth == TABLES_DEPTH - 1)
            {
                PMrestore(newFrame, pageNumber);
            }
            PMwrite(frame * PAGE_SIZE + indices[depth], newFrame);
            val = newFrame;
        }
        frame = val;
        path[depth + 1] = frame;
    }
    return frame;
}

void VMinitialize()
{
    clearFrame(0);
}

int VMread(uint64_t virtualAddress, word_t* value)
{
    // Validate the output pointer before accessing it
    if (value == nullptr)
    {
        return 0;
    }
    if (virtualAddress >= VIRTUAL_MEMORY_SIZE)
    {
        return 0;
    }

    uint64_t indices[TABLES_DEPTH];
    uint64_t offset = 0;
    decomposeAddress(virtualAddress, indices, &offset);

    uint64_t path[TABLES_DEPTH + 1];
    uint64_t frame = getFrame(virtualAddress >> OFFSET_WIDTH, indices, path);

    PMread(frame * PAGE_SIZE + offset, value);
    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value)
{
    if (virtualAddress >= VIRTUAL_MEMORY_SIZE)
    {
        return 0;
    }
    uint64_t indices[TABLES_DEPTH];
    uint64_t offset = 0;
    decomposeAddress(virtualAddress, indices, &offset);

    uint64_t path[TABLES_DEPTH + 1];
    uint64_t page = virtualAddress >> OFFSET_WIDTH;
    uint64_t frame = getFrame(page, indices, path);

    PMwrite(frame * PAGE_SIZE + offset, value);
    return 1;
}

