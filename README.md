# 内存分配模拟器

这是一个简单的内存分配模拟程序，用户可以自由初始化内存空间，体验三种分配策略，连续释放内存分区以及存档和加载配置。

在进入主菜单之前，必须初始化内存空间大小，且在程序重新运行前该值无法更改。

## 功能1：初始化内存空间
- **推荐**：使用包含分区数据的txt文件初始化内存。
  - 对于该数据文件，确保起始地址为1且地址连续。控制总分区大小等于设定的值，否则无法初始化内存空间。
  - 分区格式说明：分区编号，起始地址，分区大小，分区类别（`OS`，`free`，`allocated`）。OS分区不是必需的，但如果存在，其编号为“O”。空闲分区的编号统一为'F'。
- **不推荐**：手动输入分区信息。不存在回滚功能，一旦输入错误只能重新输入。

## 功能2：内存管理
- **分配策略**：
  - 所有输入均为手动，存在回滚功能，只能回退到上一次输入。
- **保存**：
  - 保存当前的内存分区情况，可以覆盖已有的存档。存档时必须添加备注。
- **释放分区**：
  - 释放分区的详细功能。

## 功能3：加载存档

## 如何使用？
- 在终端中，编译并运行程序，如下所示：
  ```bash
  g++ main.cpp
  ./a.out




# MemoryAllocation Simulator

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
