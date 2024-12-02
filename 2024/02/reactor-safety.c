///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define REPORT_SIZE_MAX 10
#define MAX_REPORT_DEVIANCE 3
#define MIN_REPORT_DEVIANCE 1
#define INPUT_BUFFER_ARR_SIZE 10

#define ENABLE_PART_TWO 0

///Functions
static bool is_report_safe(int32_t* report_ptr, size_t report_length)
{
    bool have_assigned_prev_diff = false;
    int32_t prev_diff = 0;
    for (int i = 1; i < report_length; i++)
    {
        int32_t diff_from_prev_element = *(report_ptr+i) - *(report_ptr+(i-1));

        if (abs(diff_from_prev_element) > MAX_REPORT_DEVIANCE
            || abs(diff_from_prev_element) < MIN_REPORT_DEVIANCE)
            return false;

        if (have_assigned_prev_diff)
        {
            uint32_t sign_bit_index = sizeof(int32_t)-1;
            uint32_t sign_mask = 1 << sign_bit_index;
            uint8_t curr_diff_sign = (diff_from_prev_element & sign_mask) >> sign_bit_index;
            uint8_t prev_diff_sign = (prev_diff & sign_mask) >> sign_bit_index;

            if (curr_diff_sign != prev_diff_sign)
                return false;
        }

        have_assigned_prev_diff = true;
        prev_diff = diff_from_prev_element;
    }

    return true;
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-02-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    int32_t current_report[REPORT_SIZE_MAX] = {0};
    size_t current_report_count = 0;
    uint16_t safe_report_count = 0;

    uint32_t current_character;
    char digit_buff[INPUT_BUFFER_ARR_SIZE] = {0};
    int8_t digit_buff_insert_index = -1;

    while ((current_character = fgetc(input_file_ptr)) != EOF)
    {
        if (isdigit(current_character))
            digit_buff[++digit_buff_insert_index] = current_character;
        else if (current_character == 0x20 || current_character == '\n')
        {
            int32_t result_num = atoi(digit_buff);
            current_report[current_report_count++] = result_num;

            memset(digit_buff, 0, INPUT_BUFFER_ARR_SIZE*sizeof(char));
            digit_buff_insert_index = -1;

            if (current_character == '\n')
            {
                if (is_report_safe(current_report, current_report_count))
                    safe_report_count++;
#if ENABLE_PART_TWO
                else
                {
                    for (int i = 0; i < current_report_count; i++)
                    {
                        int32_t sub_report[REPORT_SIZE_MAX] = {0};
                        for (int j = 0; j < current_report_count; j++)
                        {
                            if (i == j)
                                continue;
                            sub_report[i <= j ? j-1 : j] = current_report[j];
                        }
                        if (is_report_safe(sub_report, current_report_count-1))
                        {
                            safe_report_count++;
                            break;
                        }
                    }
                }
#endif
                memset(current_report, 0, REPORT_SIZE_MAX*sizeof(int32_t));
                current_report_count = 0;
            }
        }
    }

    fclose(input_file_ptr);
    printf("%u\n", safe_report_count);
}
