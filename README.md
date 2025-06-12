# Operating Systems Exercise 4

Welcome to the repository for the fourth assignment in the Operating Systems course. This project provides starter code, assignment instructions, and test materials to help you implement and test memory management modules in C++.

## Repository Structure

- `Resources/`   – C++ stubs for memory management implementation
- `ex4.pdf`      – Assignment instructions and detailed requirements
- `Test.zip`     – Sample tests and build scripts

## Getting Started

1. **Extract Resources:**
   - Unpack `Resources.tar` and `Test.zip` in the project directory.
2. **Read the Instructions:**
   - Open `ex4.pdf` for a detailed description of the tasks and requirements.
3. **Implement the Assignment:**
   - Complete the required modules as described in the PDF using the provided stubs.
4. **Build and Test:**
   - Use your preferred C++ toolchain to build the project and run the tests from `Test.zip`.

## Usage

After implementing the required modules:

```sh
# Example build and test commands (adjust as needed)
make
./run_tests.sh
```

## Virtual Memory Implementation

`Resources/VirtualMemory.cpp` provides the implementation of a simple
hierarchical page table. The following functions are key to understanding how
virtual addresses are mapped to physical frames:

- **`decomposeAddress`** – breaks a virtual address into indices for each table
  level and the page offset.
- **`clearFrame`** – fills a frame with zeros before it is reused.
- **`weightedCyclicDistance`** – computes a weighted distance between pages. Even
  pages use `WEIGHT_EVEN` while odd pages use `WEIGHT_ODD` from
  `MemoryConstants.h`.
- **`traverse`** – performs a depth‑first search of the page tables, recording
  empty frames and the best page to evict if none are free.
- **`allocateFrame`** – chooses a frame for a new table or page by searching for
  empty tables, unused frames or by evicting the page with the largest weighted
  cyclic distance.
- **`getFrame`** – walks the page‑table hierarchy, allocating frames and
  restoring pages when necessary, and returns the frame index associated with a
  virtual page.
- **Public API** – `VMinitialize` zeros the root frame, and `VMread`/`VMwrite`
  translate addresses using `getFrame` before reading or writing with the
  physical memory interface.

These helpers encapsulate the logic needed to locate frames, perform page
eviction and maintain the table structure.

## Contribution

This repository is intended for coursework. If you find issues or have suggestions, feel free to open an issue or submit a pull request (if permitted by course policy).

## Notes

- The `.gitignore` is configured to exclude sensitive files such as PDFs and archives from version control.
- Ensure you do not commit any provided test materials or assignment instructions.

---

For any questions, please refer to your course instructor or teaching assistant.
