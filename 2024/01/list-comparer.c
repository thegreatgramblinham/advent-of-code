///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

///Constants
#define LIST_INPUT_BUFFER_ARR_SIZE 30
#define LIST_SIZE 1000

///Global Variables
static int32_t _l_list[LIST_SIZE] = {0};
static size_t _l_list_count = 0;
static int32_t _r_list[LIST_SIZE] = {0};
static size_t _r_list_count = 0;

///Functions
static int comp_int(const void* x, const void* y)
{
    int64_t arg1 = *(const int64_t*)x;
    int64_t arg2 = *(const int64_t*)y;
    return arg1 - arg2;
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-01-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    uint32_t current_character;
    char digit_buff[LIST_INPUT_BUFFER_ARR_SIZE] = {0};
    int8_t digit_buff_index = -1;

    // Read our lists
    while ((current_character = fgetc(input_file_ptr)) != EOF)
    {
        if (isdigit(current_character))
            digit_buff[++digit_buff_index] = current_character;
        else if (digit_buff_index > 0 &&
            (current_character == 0x20 || current_character == '\n'))
        {
            int32_t result_num = atoi(digit_buff);
            switch (current_character)
            {
                case 0x20:
                    _l_list[_l_list_count++] = result_num;
                    break;
                case '\n':
                    _r_list[_r_list_count++] = result_num;
                    break;
                default:
                    exit(1);
            }
            memset(digit_buff, 0, LIST_INPUT_BUFFER_ARR_SIZE*sizeof(char));
            digit_buff_index = -1;
        }
    }

    fclose(input_file_ptr);

    // Order our lists
    qsort(_l_list, LIST_SIZE, sizeof(int32_t), comp_int);
    qsort(_r_list, LIST_SIZE, sizeof(int32_t), comp_int);

    uint64_t deviance_summation = 0;
    for (int i = 0; i < LIST_SIZE; i++)
    {
        int64_t result = labs(_l_list[i]-_r_list[i]);
        deviance_summation += result;
    }
    printf("%lu\n", deviance_summation);

    // Part 2: Too lazy to implement a hash set right now...
    uint64_t similarity_score = 0;
    for (int i = 0; i < LIST_SIZE; i++)
    {
        int16_t occurrences = 0;
        for (int j = 0; j < LIST_SIZE; j++)
            if (_l_list[i] == _r_list[j])
                occurrences++;
        similarity_score += _l_list[i]*occurrences;
    }
    printf("%lu\n", similarity_score);
}

