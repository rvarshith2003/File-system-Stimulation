#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_FILE_SIZE 100
#define MAX_FILE_NAME 20
#define DISK_SIZE 1000
typedef struct {
    char name[MAX_FILE_NAME];
    int start_block;
    int size;
} file;
int allocate_blocks_contiguous(int size, int *disk) {
    int start_block = -1;
    int consecutive_blocks = 0;
    for (int i = 0; i < DISK_SIZE; i++) {
        if (disk[i] == 0) {
            if (consecutive_blocks == 0) {
                start_block = i;
            }
            consecutive_blocks++;
            if (consecutive_blocks == size) {
                for (int j = start_block; j < start_block + size; j++) {
                    disk[j] = 1;
                }
                return start_block;
            }
        } else {
            consecutive_blocks = 0;
        }
    }
    return -1;
}
void delete_file(file *files, int *file_count, char *name, int *disk) {
    for (int i = 0; i < *file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            for (int j = files[i].start_block; j < files[i].start_block + files[i].size; j++) {
                disk[j] = 0;
            }
            for (int j = i; j < *file_count - 1; j++) {
                files[j] = files[j+1];
            }
            (*file_count)--;
            printf("File %s deleted\n", name);
            return;
        }
    }
    printf("File %s not found\n", name);
}
void rename_file(file *files, int file_count, char *old_name, char *new_name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, old_name) == 0) {
            strcpy(files[i].name, new_name);
            printf("File %s renamed to %s\n", old_name, new_name);
            return;
        }
    }
    printf("File %s not found\n", old_name);
}
void move_file(file *files, int file_count, char *name, int new_start_block, int *disk) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            int old_start_block = files[i].start_block;
            int size = files[i].size;
            if (new_start_block + size > DISK_SIZE) {
                printf("Error: Not enough space to move file\n");
                return;
            }
            for (int j = old_start_block; j < old_start_block + size; j++) {
                disk[j] = 0;
            }
            files[i].start_block = new_start_block;
            for (int j = new_start_block; j < new_start_block + size; j++) {
                disk[j] = 1;
            }
            printf("File %s moved from block %d to block %d\n", name, old_start_block, new_start_block);
            return;
        }
    }
    printf("File %s not found\n", name);
}

int main() {
    int disk[DISK_SIZE] = {0};
    int file_count = 0;
    file files[MAX_FILE_SIZE];
    for (int time = 0; time < 10; time++) {
        char new_file_name[MAX_FILE_NAME];
        int new_file_size;
        printf("Enter name and size of new file: ");
        scanf("%s %d", new_file_name, &new_file_size);
        if (new_file_size > DISK_SIZE) {
            printf("Error: File size too large\n");
            continue;
        }
        int start_block = allocate_blocks_contiguous(new_file_size, disk);
        if (start_block == -1) {
            printf("Error: Not enough contiguous space to create file\n");
            continue;
        }
        file new_file;
        strcpy(new_file.name, new_file_name);
        new_file.start_block = start_block;
        new_file.size = new_file_size;
        files[file_count] = new_file;
        file_count++;
        printf("File %s created with size %d starting at block %d\n", new_file_name, new_file_size, start_block);
        char delete_file_name[MAX_FILE_NAME];
        printf("Enter name of file to delete: ");
        scanf("%s", delete_file_name);
        delete_file(files, &file_count, delete_file_name, disk);
        char old_name[MAX_FILE_NAME];
        char new_name[MAX_FILE_NAME];
        printf("Enter name of file to rename: ");
        scanf("%s", old_name);
        printf("Enter new name for file: ");
        scanf("%s", new_name);
        rename_file(files, file_count, old_name, new_name);
        char move_file_name[MAX_FILE_NAME];
        int new_start_block;
        printf("Enter name of file to move: ");
        scanf("%s", move_file_name);
        printf("Enter new start block for file: ");
        scanf("%d", &new_start_block);
        move_file(files, file_count, move_file_name, new_start_block, disk);
        printf("Disk state at end of time unit %d:\n", time);
        for (int i = 0; i < DISK_SIZE; i++) {
            printf("%d ", disk[i]);
        }
        printf("\n");
        int consecutive_free_blocks = 0;
        int fragment_count = 0;
        int wasted_blocks = 0;
        for (int i = 0; i < DISK_SIZE; i++) {
            if (disk[i] == 0) {
                consecutive_free_blocks++;
            } else {
                if (consecutive_free_blocks > 0) {
                    fragment_count++;
                    wasted_blocks += consecutive_free_blocks;
                    consecutive_free_blocks = 0;
                }
            }
        }
        if (consecutive_free_blocks > 0) {
            fragment_count++;
            wasted_blocks += consecutive_free_blocks;
        }
        float avg_fragment_size = (float) wasted_blocks / fragment_count;
        printf("Average fragment size: %.2f\n", avg_fragment_size);
        printf("Number of wasted blocks: %d\n", wasted_blocks);
    }

    return 0;
}