
///Imports
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define DELIMITER_COUNT 1
#define PARAM_COUNT 2

#define BUTTON_A_COST 3
#define BUTTON_B_COST 1

#define ENABLE_PART_TWO 1

///Functions
static char* jump_to_next_numeric_char(char *str)
{
    if (str == NULL)
        return NULL;

    char *curr_pos = str;
    while (!isdigit(*curr_pos) && *(curr_pos) != '\0')
        curr_pos++;

    return isdigit(*curr_pos) ? curr_pos : NULL;
}

static char* jump_to_next_delim(char *str, char delim)
{
    if (str == NULL)
        return NULL;

    char *curr_pos = str;
    while ((*curr_pos) != delim && *(curr_pos) != '\0')
        curr_pos++;

    return (*curr_pos) == delim ? curr_pos : NULL;
}

static size_t parse_params(char *str, int64_t *param_dest, char delim, size_t max_params)
{
    int i = 0;
    for (; i < max_params; i++)
    {
        str = jump_to_next_numeric_char(str);
        if (str == NULL)
            break;
        *(param_dest+i) = atol(str);
        str = jump_to_next_delim(str, delim);
    }
    return i;
}

int main (void)
{
    FILE *input_file_ptr = fopen("puzzle-13-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    const char delim = ' ';
    char *line_buff = NULL;
    size_t line_buff_len = 0;
    ssize_t read_result = 0;

    uint64_t total_best_token_cost = 0;

    uint64_t file_line_index = 0;
    int64_t button_a_params[PARAM_COUNT] = {0};
    int64_t button_b_params[PARAM_COUNT] = {0};
    int64_t prize_params[PARAM_COUNT] = {0};
    while ((read_result = getline(&line_buff, &line_buff_len, input_file_ptr)) != -1)
    {
        char *curr_line_pos = line_buff;
        switch (file_line_index%4)
        {
            case 0:
                memset(button_a_params, 0, sizeof(int64_t)*PARAM_COUNT);
                memset(button_b_params, 0, sizeof(int64_t)*PARAM_COUNT);
                memset(prize_params, 0, sizeof(int64_t)*PARAM_COUNT);
                parse_params(curr_line_pos, button_a_params, delim, PARAM_COUNT);
                break;
            case 1:
                parse_params(curr_line_pos, button_b_params, delim, PARAM_COUNT);
            case 2:
                parse_params(curr_line_pos, prize_params, delim, PARAM_COUNT);
#if ENABLE_PART_TWO
                prize_params[0] += 10000000000000;
                prize_params[1] += 10000000000000;
#endif
                break;
            case 3:
                int64_t a_num = ((button_b_params[0]*prize_params[1]) - (button_b_params[1]*prize_params[0]));
                int64_t a_denom = ((button_b_params[0]*button_a_params[1]) - (button_b_params[1]*button_a_params[0])); 
                int64_t a_presses_mod = a_num % a_denom;
                int64_t a_presses_div = a_num / a_denom;

                int64_t b_num = (prize_params[0] - (button_a_params[0]*a_presses_div));
                int64_t b_denom = button_b_params[0];
                int64_t b_presses_mod = b_num % b_denom;
                int64_t b_presses_div = b_num / b_denom;

                if (a_presses_mod == 0 && b_presses_mod == 0)
                    total_best_token_cost += (a_presses_div*BUTTON_A_COST) + (b_presses_div*BUTTON_B_COST);
                break;
            default:
                perror("Line logic OOB");
                exit(1);
        }

        file_line_index++;
    }

    printf("%lu\n", total_best_token_cost);

    fclose(input_file_ptr);
    free(line_buff);
    return 0;
}
