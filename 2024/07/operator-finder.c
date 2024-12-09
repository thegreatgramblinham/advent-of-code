
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define OPERAND_ARR_MAX 50
#define CAT_BUFF_LEN 100

#define ENABLE_PART_TWO 1

///Enums
enum operator_e
{
    ADD,
    MUL,
    CAT, // =^_^=
};

///Global Variables
uint64_t _total_summation = 0;
uint64_t _current_result_target = 0;

///Functions
static bool try_operator(uint64_t *operand_ptr, uint64_t curr_total, enum operator_e op_to_apply)
{
    switch (op_to_apply)
    {
        case ADD:
            curr_total += *(operand_ptr);
            break;
        case MUL:
            curr_total *= *(operand_ptr);
            break;
        case CAT:
            char cat_buff[CAT_BUFF_LEN] = {0};
            int32_t populated_buff_len = sprintf(cat_buff, "%lu", curr_total);
            sprintf((cat_buff+populated_buff_len), "%lu", *(operand_ptr));
            curr_total = atoll(cat_buff);
            break;
        default:
            perror("Operator OOB");
            break;
    }

    if (*(operand_ptr+1) == 0)
        return _current_result_target == curr_total;
#if ENABLE_PART_TWO
    for (int i = ADD; i <= CAT; i++)
#else
    for (int i = ADD; i <= MUL; i++)
#endif
        if (try_operator(operand_ptr+1, curr_total, i))
            return true;

    return false;
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-07-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }
    char *line_buff = NULL;
    size_t line_buff_len = 0;
    ssize_t read_result = 0;
    while ((read_result = getline(&line_buff, &line_buff_len, input_file_ptr)) != -1)
    {
        char *curr_char = line_buff;
        _current_result_target = atoll(curr_char);
        uint64_t operand_arr[OPERAND_ARR_MAX] = {0};
        int16_t operand_count = 0;

        while (*curr_char != ':')
            curr_char++;
        curr_char++;

        do
        {
            curr_char++;
            operand_arr[operand_count++] = atoll(curr_char);
            while (*curr_char != 0x20 && *curr_char != '\n')
                curr_char++;
        } while (*curr_char != '\n');

#if ENABLE_PART_TWO
        for (int i = ADD; i <= CAT; i++)
#else
        for (int i = ADD; i <= MUL; i++)
#endif
            if (try_operator(operand_arr+1, *operand_arr, i))
            {
                _total_summation += _current_result_target;
                break;
            }
    }

    printf("%lu\n",_total_summation);
    fclose(input_file_ptr);
    free(line_buff);
    return 0;
}
