
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define DISK_BLOCK_ARR_SIZE 100000
#define ENABLE_PART_TWO 1

///Enums
enum block_state_e
{
    FREE,
    ALLOC,
};

///Structs
struct disk_block_s
{
    enum block_state_e state;
    uint32_t file_id;
};

///Functions
bool are_disk_block_eq(struct disk_block_s b1, struct disk_block_s b2)
{
    return b1.state == b2.state && b1.file_id == b2.file_id;
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-09-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    struct disk_block_s disk_arr[DISK_BLOCK_ARR_SIZE] = {0};
    uint64_t disk_insert_index = 0;

    uint64_t read_index = 0;
    uint32_t current_file_id = 0;

    uint32_t character_read_result;
    while ((character_read_result = fgetc(input_file_ptr)) != EOF)
    {
        if (!isdigit(character_read_result))
            continue;

        bool is_file = !(read_index % 2);
        char current_character = character_read_result;
        uint8_t curr_block_size = atoi(&current_character);
        struct disk_block_s new_block_value =
        {
            .state = is_file ? ALLOC : FREE,
            .file_id = is_file ? current_file_id : 0,
        };

        for (int i = 0; i < curr_block_size; i++)
        {
            if (disk_insert_index == DISK_BLOCK_ARR_SIZE)
            {
                perror("Out of Memory!");
                exit(1);
            }
            disk_arr[disk_insert_index] = new_block_value;
            disk_insert_index++;
        }

        if (is_file)
            current_file_id++;
        read_index++;
    }

#if ENABLE_PART_TWO
    uint32_t current_alloc_block_len = 0;
    for (int i = disk_insert_index; i > 0; i--)
    {
        struct disk_block_s curr_block_value = disk_arr[i];
        struct disk_block_s next_block_value = disk_arr[i-1];
        if (curr_block_value.state == FREE)
        {
            current_alloc_block_len = 0;
            continue;
        }

        current_alloc_block_len++;
        if (are_disk_block_eq(curr_block_value, next_block_value))
            continue;

        uint32_t current_free_block_len = 0;
        for (int j = 0; j < i; j++)
        {
            struct disk_block_s curr_insert_candidate = disk_arr[j];
            if (curr_insert_candidate.state == ALLOC)
            {
                current_free_block_len = 0;
                continue;
            }
            current_free_block_len++;

            if (current_free_block_len < current_alloc_block_len)
                continue;

            uint32_t insert_start_index = j-(current_free_block_len-1);
            memcpy((disk_arr+insert_start_index),(disk_arr+i),sizeof(struct disk_block_s)*current_alloc_block_len);
            memset((disk_arr+i), 0, sizeof(struct disk_block_s)*current_alloc_block_len);
            break;
        }
        current_alloc_block_len = 0;
    }
#else
    uint64_t fragment_insert_index = 0;
    for (int i = disk_insert_index; fragment_insert_index < i; i--)
    {
        struct disk_block_s curr_block_value = disk_arr[i];
        if (curr_block_value.state == FREE)
            continue;

        while ((disk_arr[fragment_insert_index].state != FREE))
            fragment_insert_index++;

        if (fragment_insert_index < i)
        {
            disk_arr[fragment_insert_index] = curr_block_value;
            memset(disk_arr+i, 0, sizeof(struct disk_block_s));
        }
    }
#endif

    uint64_t checksum = 0;
    for (int i = 0; i <= disk_insert_index; i++)
        checksum += disk_arr[i].file_id*i;

    printf("%lu\n",checksum);

    fclose(input_file_ptr);
    return 0;
}

