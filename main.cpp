#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <cstdio>


typedef struct Partition {
	char process_code;
    int start_address;
    int size;
    char label[20];
    struct Partition* next;
} Partition;


typedef struct PartitionInfo {
	char process_code;
    int start_address;
    int size;
    char label[20];
} PartitionInfo;


typedef struct Memory {
    Partition* free_partitions;
    Partition* allocated_partitions;
    int total_size;
} Memory;

typedef struct MemoryArchive {
	Partition* free_partitions;
    Partition* allocated_partitions;
    int total_size;
    char* comment;
} MemoryArchive;


void init_memory(Memory* memory, int total_size) {
    memory->total_size = total_size;
    memory->free_partitions = NULL; 
    memory->allocated_partitions = NULL; 
}


void free_partitions(Memory* memory) {
    Partition *current, *temp;

    current = memory->free_partitions;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    memory->free_partitions = NULL;

    current = memory->allocated_partitions;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    memory->allocated_partitions = NULL;
}


void modify_partitions(Memory* memory, PartitionInfo* modifications, int num_modifications) {
    free_partitions(memory); 
 
    for (int i = 0; i < num_modifications; i++) {
        Partition* new_part = (Partition*)malloc(sizeof(Partition));
        new_part->process_code = modifications[i].process_code;
        new_part->start_address = modifications[i].start_address;
        new_part->size = modifications[i].size;
        strcpy(new_part->label, modifications[i].label);
        new_part->next = NULL;

        if (strcmp(new_part->label, "free") == 0) {
            new_part->next = memory->free_partitions;
            memory->free_partitions = new_part;
        } 
        else {
            new_part->next = memory->allocated_partitions;
            memory->allocated_partitions = new_part;
        }
    }
}


int init_memory_from_file(Memory* memory, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return 0;
    }

    char buffer[256];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        count++;
    }

    rewind(file);

    PartitionInfo* modifications = (PartitionInfo*) malloc(count * sizeof(PartitionInfo));
    int i = 0;
    while (fscanf(file, "%c %d %d %s\n", &modifications[i].process_code, &modifications[i].start_address, &modifications[i].size, modifications[i].label) == 4) {
        i++;
    }
    fclose(file);

    modify_partitions(memory, modifications, i);
    free(modifications);
    
    return 1;
}


void init_memory_manual(Memory* memory) {
    int num_partitions;
    printf("Enter the number of partitions: ");
    scanf("%d", &num_partitions);

    PartitionInfo* partitions = (PartitionInfo*) malloc(num_partitions * sizeof(PartitionInfo));
    int i;

    for (i = 0; i < num_partitions; i++) {
        printf("Enter process code, start address, size, and label for partition %d: ", i + 1);
        scanf(" %c %d %d %s", &partitions[i].process_code, &partitions[i].start_address, &partitions[i].size, partitions[i].label);
    }

    modify_partitions(memory, partitions, num_partitions);
    free(partitions);
}


int allocate_process_general(Memory* memory, int process_size, char process_code, char _type) {
    Partition *selected_partition = NULL;
    Partition **selected_prev_ptr = NULL;
    Partition *current = memory->free_partitions;
    Partition *prev = NULL;

    int best_size_diff = memory->total_size;
    int max_size = 0;                       

    while (current != NULL) {
        if (current->size >= process_size) {
            int size_diff = current->size - process_size;

            switch (_type) {
                case 'b': 
                    if (size_diff < best_size_diff) {
                        best_size_diff = size_diff;
                        selected_partition = current;
                        selected_prev_ptr = prev ? &prev->next : &memory->free_partitions;
                    }
                    break;
                case 'f': 
                    if (selected_partition == NULL) { 
                        selected_partition = current;
                        selected_prev_ptr = prev ? &prev->next : &memory->free_partitions;
                    }
                    break;
                case 'w':
                    if (size_diff > max_size) {
                        max_size = size_diff;
                        selected_partition = current;
                        selected_prev_ptr = prev ? &prev->next : &memory->free_partitions;
                    }
                    break;
            }
        }
        
        if (_type == 'f')
        	if (selected_partition != NULL) break;
        prev = current;
        current = current->next;
    }

    if (selected_partition) {
        *selected_prev_ptr = selected_partition->next;
        selected_partition->next = NULL;

        if (selected_partition->size > process_size) {
            Partition* new_free_part = (Partition*)malloc(sizeof(Partition));
            new_free_part->process_code = 'F';
            new_free_part->start_address = selected_partition->start_address + process_size;
            new_free_part->size = selected_partition->size - process_size;
            strcpy(new_free_part->label, "free");
            new_free_part->next = *selected_prev_ptr;
            *selected_prev_ptr = new_free_part;
        }

        selected_partition->process_code = process_code;
        strcpy(selected_partition->label, "allocated");
        selected_partition->size = process_size;
        selected_partition->next = memory->allocated_partitions;
        memory->allocated_partitions = selected_partition;
        
        return 1; 
    } 
	else {
        printf("No suitable free partition found for the process.\n");
        return 0;
    }
}


void sort_partitions(Partition** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return; // 如果链表为空或只有一个元素，则不需要排序
    }

    Partition *sorted = NULL; // 创建一个新的链表用于存放排序后的节点
    Partition *current = *head; // 用于遍历原链表的指针
    Partition *next = NULL; // 用于保存当前节点的下一个节点，因为插入操作会改变链表结构

    while (current != NULL) {
        next = current->next; // 保存当前节点的下一个节点

        // 找到合适的插入位置
        if (sorted == NULL || sorted->start_address >= current->start_address) {
            // 如果新链表为空或当前节点应插在首位
            current->next = sorted;
            sorted = current;
        } else {
            // 寻找在新链表中的正确位置并插入
            Partition *cursor = sorted; // 用于遍历新链表找到插入点
            while (cursor->next != NULL && cursor->next->start_address < current->start_address) {
                cursor = cursor->next;
            }
            current->next = cursor->next;
            cursor->next = current;
        }

        current = next; // 移动到原链表的下一个节点
    }

    *head = sorted; // 更新头指针指向已排序的链表
}


int validate_memory(Memory* memory) {
    int total_used = 0;
    int last_address = 1; // 从地址1开始

    // 确保内存分区按地址排序
    sort_partitions(&memory->free_partitions);
    sort_partitions(&memory->allocated_partitions);

    Partition *free_current = memory->free_partitions;
    Partition *allocated_current = memory->allocated_partitions;
    Partition *next_partition = NULL;

    // 确定第一个分区的位置
    if (free_current != NULL && (allocated_current == NULL || free_current->start_address <= allocated_current->start_address)) {
        next_partition = free_current;
        free_current = free_current->next;
    } else if (allocated_current != NULL) {
        next_partition = allocated_current;
        allocated_current = allocated_current->next;
    }

    // 从第一个地址为1的分区开始
    if (next_partition != NULL && next_partition->start_address != 1) {
        printf("Error: Memory does not start from address 1.\n");
        return 0; // 分区没有从地址1开始，校验失败
    }

    // 迭代检查所有分区的连续性
    while (next_partition != NULL) {
        if (next_partition->start_address != last_address) {
            printf("Error: Non-continuous memory at address %d. Expected start at %d.\n", next_partition->start_address, last_address);
            return 0; // 分区不连续，校验失败
        }
        total_used += next_partition->size;
        last_address = next_partition->start_address + next_partition->size; // 更新下一个分区的预期起始地址

        // 确定下一个分区
        if (free_current != NULL && (allocated_current == NULL || free_current->start_address <= allocated_current->start_address)) {
            next_partition = free_current;
            free_current = free_current->next;
        } else if (allocated_current != NULL) {
            next_partition = allocated_current;
            allocated_current = allocated_current->next;
        } else {
            next_partition = NULL;
        }
    }

    // 检查分区总大小是否匹配内存的总大小
    if (total_used != memory->total_size) {
        printf("Error: Memory size mismatch. Total allocated: %d, Expected: %d\n", total_used, memory->total_size);
        return 0; // 总分区大小不匹配，校验失败
    }

    return 1; // 所有分区连续且总大小匹配，校验成功
}



void print_partitions(Memory* memory, char* print_type) {
    Partition *current;

	if (print_type == "free" || print_type == "all") {
	    // 打印空闲分区
	    printf("Free Partitions:\n");
	    printf("%-12s %-12s %-12s %-12s\n", "Proc Code", "Start Addr", "Size", "Label");
	    current = memory->free_partitions;
	    while (current != NULL) {
	        printf("%-12c %-12d %-12d %-12s\n",
	               current->process_code, current->start_address, current->size, current->label);
	        current = current->next;
    	}
	}

	if (print_type == "allocated" || print_type == "all"){
    // 打印已分配分区
    printf("\nAllocated Partitions:\n");
    printf("%-12s %-12s %-12s %-12s\n", "Proc Code", "Start Addr", "Size", "Label");
    current = memory->allocated_partitions;
    while (current != NULL) {
        printf("%-12c %-12d %-12d %-12s\n",
               current->process_code, current->start_address, current->size, current->label);
        current = current->next;
    	}
	}
}


void print_top_menu() {  
    system("cls");
    printf("========================================\n");
    printf("|   Memory Management Simulator Menu   |\n");
    printf("========================================\n");
    printf("| 1. Initialize memory                 |\n");
    printf("| 2. Memory management                 |\n");
    printf("| 3. Load archive                      |\n");
    printf("| 4. Help                              |\n");
    printf("| 0. Exit                              |\n");
    printf("========================================\n\n");
    printf("Enter your choice: ");
}


void print_secondary_menu() {
    printf("Memory Management Functions\n");
    printf(" 1. Best Fit\n");
    printf(" 2. First Fit\n");
    printf(" 3. Worst Fit\n");
    printf(" 4. Save\n");
    printf(" 5. Free Partion\n");
    printf(" 0. Return to the previous menu\n");
    printf("Enter your choice: ");
}


void merge_free_partitions(Memory* memory) {
    Partition *current = memory->free_partitions;
    while (current != NULL && current->next != NULL) {
        if (current->start_address + current->size == current->next->start_address) {
            // 合并当前分区和下一个分区
            current->size += current->next->size;
            Partition *temp = current->next;
            current->next = temp->next;
            free(temp);  // 释放合并后的节点
        } else {
            current = current->next;
        }
    }
}


void free_partition(Memory* memory) {
    char process_code;
    char choice;

    sort_partitions(&memory->allocated_partitions);

    do {
        printf("Enter process code of the partition to free: ");
        scanf(" %c", &process_code);  // 读取用户输入的进程代码

        // 查找并删除指定的已分配分区
        Partition **current_ptr = &memory->allocated_partitions;
        Partition *to_free = NULL;

        while (*current_ptr != NULL) {
            if ((*current_ptr)->process_code == process_code) {
                to_free = *current_ptr;
                *current_ptr = to_free->next;  // 从已分配链表中移除
                to_free->next = NULL; // 清除指向下一个分区的指针
                break;
            }
            current_ptr = &(*current_ptr)->next;
        }

        if (to_free) {
            // 更新分区标签和代码
            strcpy(to_free->label, "free");
            to_free->process_code = 'F';  // 统一标记为F表示空闲

            // 将释放的分区添加到空闲分区表中
            Partition **insert_ptr = &memory->free_partitions;
            while (*insert_ptr != NULL && (*insert_ptr)->start_address < to_free->start_address) {
                insert_ptr = &(*insert_ptr)->next;
            }
            to_free->next = *insert_ptr;
            *insert_ptr = to_free;
            printf("Partition '%c' freed and added to free partitions.\n", process_code);
        } else {
            printf("Partition with process code '%c' not found.\n", process_code);
        }

        // 询问用户是否继续删除分区
        printf("Do you want to free another partition? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    // 检查并合并相邻的空闲分区
    merge_free_partitions(memory);
}


void copy(Memory* dest, Memory* src) {
    free_partitions(dest); 
	printf("1");
    // Record free partitions
    Partition* current = src->free_partitions;
    Partition** last_ptr = &dest->free_partitions;
    printf("1");
    while (current != NULL) {
        Partition* new_part = (Partition*)malloc(sizeof(Partition));
        *new_part = *current;
        new_part->next = NULL;
        *last_ptr = new_part;
        last_ptr = &new_part->next;
        current = current->next;
    }
	printf("1");
    // Record allocated partitions
    current = src->allocated_partitions;
    last_ptr = &dest->allocated_partitions;
    while (current != NULL) {
        Partition* new_part = (Partition*)malloc(sizeof(Partition));
        *new_part = *current;
        new_part->next = NULL;
        *last_ptr = new_part;
        last_ptr = &new_part->next;
        current = current->next;
    }
    printf("1");
}


void init_archive(MemoryArchive archive[], int total_size) {
    for (int i = 0; i < 5; i++) {
        archive[i].comment = NULL;
        archive[i].total_size = total_size;
        archive[i].free_partitions = NULL;
        archive[i].allocated_partitions = NULL;
    }
}

void print_archive_info(MemoryArchive* archive) {
	int i;
    for (i = 0; i < 5; i++){
        if (archive[i].comment != NULL) {
            printf("Archive %d: %s\n", i + 1, archive[i].comment);
        } 
        else {
            printf("Archive %d: Empty\n", i + 1);
        }
    }
    
}


void free_partitions_archive(MemoryArchive* archive) {

    Partition* current = archive->free_partitions;
    while (current != NULL) {
        Partition* temp = current;
        current = current->next;
        free(temp);
    }
    archive->free_partitions = NULL;

    current = archive->allocated_partitions;
    while (current != NULL) {
        Partition* temp = current;
        current = current->next;
        free(temp);
    }
    archive->allocated_partitions = NULL;

    // Free the comment string
    if (archive->comment) {
        free(archive->comment);
        archive->comment = NULL;
    }
}


void copy_for_archive(Partition** dest, Partition* src) {
    Partition** current_dest = dest;
    while (src) {
        *current_dest = (Partition*)malloc(sizeof(Partition));
        if (*current_dest == NULL) {
            printf("Memory allocation failed while copying partitions.\n");
            return;
        }
        **current_dest = *src;
        (*current_dest)->next = NULL;
        current_dest = &((*current_dest)->next);
        src = src->next;
    }
}


int save_archive(Memory* memory, MemoryArchive archive[], int archive_num, char* comment) {
    // 先清理目标存档中旧的数据
    free_partitions_archive(&archive[archive_num]);

    // 深拷贝空闲和已分配的分区链表
    copy_for_archive(&archive[archive_num].free_partitions, memory->free_partitions);
    copy_for_archive(&archive[archive_num].allocated_partitions, memory->allocated_partitions);
    
    // 设置总内存大小
    archive[archive_num].total_size = memory->total_size;

    // 复制注释
    if (comment) {
        size_t length = strlen(comment) + 1;
        archive[archive_num].comment = (char*)malloc(length);
        if (archive[archive_num].comment) {
            strcpy(archive[archive_num].comment, comment);
        } 
		else {
            printf("Failed to allocate memory for comment.\n");
        }
    }
}


void load_archive(Memory* memory, MemoryArchive archive[], int archive_num) {
    if (archive_num < 0 || archive_num >= 5) {
        printf("Invalid archive number.\n");
        return;
    }

    free_partitions(memory);

    copy_for_archive(&memory->free_partitions, archive[archive_num].free_partitions);
    copy_for_archive(&memory->allocated_partitions, archive[archive_num].allocated_partitions);
    memory->total_size = archive[archive_num].total_size;
}


int main() {
    Memory memory;
    Memory pre_memory; 
    MemoryArchive archive[5];
    
    printf("Welcome to the memory allocation simulator\n\nPlease enter the size of the memory: ");
    int total_size;
    scanf("%d", &total_size);
    init_memory(&memory, total_size);
    init_memory(&pre_memory, total_size); // 初始化预备内存状态
    init_archive(archive, total_size); // 初始化存档

    char choice;
    int control;
    int choice2;
    int archive_num;
    char comment[256];

    while (1) {
        print_top_menu();
        scanf(" %c", &choice);

        switch (choice) {
            case '1': {
                int flag = 1;
                printf("Initialize memory from:\n1. File\n2. Manual Input\nEnter choice: ");
                int init_choice;
                scanf("%d", &init_choice);
                if (init_choice == 1) {
                    char filename[256];
                    printf("Enter filename: ");
                    scanf("%s", filename);
                    flag = init_memory_from_file(&memory, filename);
                } else if (init_choice == 2) {
                    init_memory_manual(&memory);
                } else {
                    printf("Invalid choice.\n");
                }
                if (flag && validate_memory(&memory)) {
                    printf("Memory initialized and partitions set. Current partitions:\n");
                    print_partitions(&memory, "all");
                }
                break;
            }
            case '2': {
            	control = 1;
                while (control) {
                    print_secondary_menu();
                    scanf("%d", &choice2);

                    int process_size;
                    char process_code;
                    char allocation_type;
                    char continue_do;
                    char roll;
                    switch (choice2) {
                        case 1: // Best Fit
                        case 2: // First Fit
                        case 3: // Worst Fit
                            allocation_type = (choice2 == 1) ? 'b' : (choice2 == 2 ? 'f' : 'w');
                            do {
                                copy(&pre_memory, &memory);
                                printf("Enter process code and size for allocation: ");
                                scanf(" %c %d", &process_code, &process_size);
                                if (!allocate_process_general(&memory, process_size, process_code, allocation_type)) {
                                    printf("Allocation failed. No suitable partition found.\n");
                                } else {
                                    print_partitions(&memory, "all");
                                }

                                printf("Do you need to roll back (y/n)? ");
                                scanf(" %c", &roll);
                                if (roll == 'y' || roll == 'Y') {
                                    copy(&memory, &pre_memory);
                                }

                                printf("Do you want to continue (y/n)? ");
                                scanf(" %c", &continue_do);
                            } while (continue_do == 'y' || continue_do == 'Y');
                            break;
                        case 4: // Save
                            print_archive_info(archive);
                            printf("Enter the archive number to save to (1-5): ");
                            scanf("%d", &archive_num);
                            if (archive_num < 1 || archive_num > 5) {
                                printf("Invalid archive number.\n");
                                break;
                            }
                            printf("Enter a comment for the archive: ");
                            scanf(" %[^\n]", comment);
                            archive_num--;
                            save_archive(&memory, archive, archive_num, comment);
                            printf("Archive saved successfully!\n");
                            break;
                        case 5: // Free Partition
                            free_partition(&memory);
                            print_partitions(&memory, "all");
                            break;
                        case 0: // Return to the previous menu
                            control = 0;
                            break;
                        default:
                            printf("Invalid choice. Please try again.\n");
                    }
                    printf("\nPress enter to continue...\n");
                    getchar(); getchar(); // Pause for user to read the output
                }
                break;
            }
            case '3': 
                print_archive_info(archive);
                printf("Enter the archive number to load from (1-5): ");
                scanf("%d", &archive_num);
                if (archive_num < 1 || archive_num > 5) {
                    printf("Invalid archive number.\n");
                } 
				else {
                    load_archive(&memory, archive, archive_num - 1);
                    printf("Archive %d loaded successfully!\n", archive_num);
                    print_partitions(&memory, "all");
                }
                break;
            case '4':
                printf("Help information: \n");
                break;
            case '0': 
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        printf("\nPress enter to continue...\n");
        getchar(); getchar();
    }

    return 0;
}


