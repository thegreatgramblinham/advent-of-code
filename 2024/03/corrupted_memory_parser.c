
///Imports
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define MAX_MATCH_COUNT 1
#define NUM_BUFFER_LEN 10

#define ENABLE_PART_TWO 0

///Global Variables

///Functions
static uint64_t process_mul_params(char *param_start_ptr)
{
    char *curr_char = param_start_ptr;
    uint32_t arg1 = 0;
    uint32_t arg2 = 0;

    char num_buff[NUM_BUFFER_LEN] = {0};
    int16_t num_buff_index = -1;
    while (*curr_char != ',')
    {
        num_buff_index++;
        num_buff[num_buff_index] = *(curr_char);
        curr_char++;
    }
    curr_char++; //Move past ','

    arg1 = atoi(num_buff);
    memset(num_buff, 0, sizeof(char)*NUM_BUFFER_LEN);
    num_buff_index = -1;

    while (*curr_char != ')')
    {
        num_buff_index++;
        num_buff[num_buff_index] = *(curr_char);
        curr_char++;
    }
    arg2 = atoi(num_buff);

    return arg1*arg2;
}


int main(void)
{
    regex_t mul_finder_exp;
    int32_t regex_comp_ret;

    regex_comp_ret = regcomp(&mul_finder_exp, "mul[(][0-9]+,[0-9]+[)]", REG_EXTENDED);
    if (regex_comp_ret)
    {
        perror("Finder regex failed to compile.");
        exit(1);
    }

    regex_t mul_disable_extractor;
    // NOTE: THIS DOESN'T WORK - POSIX regex has no non-greedy operators so the '?' does nothing...
    regex_comp_ret = regcomp(&mul_disable_extractor, "don't[(].*?do[(]", REG_EXTENDED);
    if (regex_comp_ret)
    {
        perror("Extractor regex failed to compile.");
        exit(1);
    }

    FILE *input_file_ptr = fopen("puzzle-03-24-manual-regex-application.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    int64_t mul_summation = 0;

    char *line_buff = NULL;
    size_t line_buff_len = 0;
    ssize_t read_result = 0;

    const uint32_t param_offset = 4;

    size_t max_match_count = MAX_MATCH_COUNT;
    regmatch_t match_arr[MAX_MATCH_COUNT] = {0};

    while ((read_result = getline(&line_buff, &line_buff_len, input_file_ptr)) != -1)
    {
        char *curr_line_pos = line_buff;
        uint64_t curr_start_index = 0;

#if ENABLE_PART_TWO
        while (!regexec(&mul_disable_extractor, curr_line_pos, max_match_count, match_arr, 0))
        {
            //TODO chop out the returns from this.
            size_t sub_str_len = match_arr[0].rm_so-curr_start_index;
            char *sub_str = (char*)malloc(sizeof(char)*sub_str_len);
            char *curr_sub_str_ptr = sub_str;
            memcpy(sub_str, curr_line_pos, sub_str_len);
            size_t sub_max_match_count = MAX_MATCH_COUNT;
            regmatch_t sub_match_arr[MAX_MATCH_COUNT] = {0};

            while (!regexec(&mul_finder_exp, curr_sub_str_ptr, sub_max_match_count, sub_match_arr, 0))
            {
                mul_summation += process_mul_params((sub_str + sub_match_arr[0].rm_so + param_offset));
                curr_sub_str_ptr += sub_match_arr[0].rm_eo;
            }

            curr_line_pos += match_arr[0].rm_eo;

            free(sub_str);
        }
#else
        while (!regexec(&mul_finder_exp, curr_line_pos, max_match_count, match_arr, 0))
        {
            mul_summation += process_mul_params((curr_line_pos + match_arr[0].rm_so + param_offset));
            curr_line_pos += match_arr[0].rm_eo;
        }
#endif


    }

    free(line_buff);
    fclose(input_file_ptr);

    printf("%lu\n", mul_summation);
    return 0;
}
