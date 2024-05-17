# MemoryAllocation SimulatorS

This is a simple memory allocation simulation program that allows users to freely initialize memory space, experience three allocation strategies, continuously release memory partitions, and archive/load configurations.

Before entering the main menu, the memory space size must be initialized, and this value cannot be changed until the program is rerun.

## Feature 1: Initialize Memory Space
- **Recommended**: Initialize memory using a txt file that has partition data.
  - For this data file, ensure the start address is 1 and addresses are contiguous. Ensure the total partition size equals the set value, or the memory space cannot be initialized.
  - Partition format description: Partition ID, Start Address, Partition Size, Partition Type (`OS`, `free`, `allocated`). The OS partition is not mandatory, but if it exists, its ID is "O". Free partition IDs are uniformly 'F'.
- **Not recommended**: Manually enter partition information. There is no rollback functionality; mistakes require re-entry.

## Feature 2: Memory Management
- **Allocation Strategies**:
  - All inputs are manual, with RollBack functionality available, allowing reversion only to the last input.
- **Save**:
  - Save the current memory partition situation. Overwriting existing archives is possible, and comments are required when archiving.
- **Release Partition**:
  - Detailed functionality for partition release.

## Feature 3: Load Archive

## How to Use?
- In the terminal, compile and run the program as follows:
  ```bash
  g++ main.cpp
  ./a.out
