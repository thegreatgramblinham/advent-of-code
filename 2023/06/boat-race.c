
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

///Local Declarations

///Static Constants
#define RACE_QUANTITY 4
#define RACE_METERIC_QUANTITY 2
#define MAX_DIGIT_LENGTH 20

///Static Global Variables

///Static Functions
static uint64_t compute_winning_race_paths_linear(uint64_t time, uint64_t dist)
{
    uint64_t beats_record_options = 0;
    for (int hold_time = 0; hold_time < time; hold_time++)
    {
        if (hold_time*(time-hold_time) > dist)
            beats_record_options++;
    }
    return beats_record_options;
}

///Extern Functions
int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-06-23.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    uint64_t race_arr[RACE_METERIC_QUANTITY][RACE_QUANTITY] = {0};

    char *curr_char_ptr = NULL;
    size_t line_len = 0;
    uint16_t line_index = 0;
    while (getline(&curr_char_ptr, &line_len, input_file_ptr) && !feof(input_file_ptr))
    {
        while (*curr_char_ptr != ':')
            curr_char_ptr++;
        curr_char_ptr++;

        bool could_parse_number = false;
        uint16_t numbers_parsed = 0;
        do
        {
            char *end_parse_char_ptr = NULL;
            uint64_t parsed_number = strtoul(curr_char_ptr, &end_parse_char_ptr, 10);
            could_parse_number = (curr_char_ptr != end_parse_char_ptr);
            if (could_parse_number)
            {
                race_arr[line_index][numbers_parsed] = parsed_number;
                numbers_parsed++;
            }
            curr_char_ptr = end_parse_char_ptr;
        } while (could_parse_number);

        line_index++;
    }

    uint64_t total_path_multiplier = 1;
    for (int i = 0; i < RACE_QUANTITY; i++)
    {
        uint64_t winning_race_paths =
            compute_winning_race_paths_linear(race_arr[0][i], race_arr[1][i]);
        total_path_multiplier *= winning_race_paths;
    }

    printf("Total path multiplier: %lu\n", total_path_multiplier);

    uint64_t final_numbers[RACE_METERIC_QUANTITY] = {0};
    char number_buff[MAX_DIGIT_LENGTH] = {0};
    uint8_t number_buff_pos = 0;
    for (int i = 0; i < RACE_METERIC_QUANTITY; i++)
    {
        for (int j = 0; j < RACE_QUANTITY; j++)
        {
            sprintf((number_buff+number_buff_pos), "%lu", race_arr[i][j]);
            number_buff_pos += ceil(log10(race_arr[i][j]));
        }
        char *end_parse_char_ptr;
        uint64_t parsed_number = strtoul(number_buff, &end_parse_char_ptr, 10);
        *(final_numbers + i)  = parsed_number;
        memset(&number_buff, 0, MAX_DIGIT_LENGTH*sizeof(char));
        number_buff_pos = 0;
    }

    uint64_t paths = compute_winning_race_paths_linear(final_numbers[0], final_numbers[1]);
    printf("Total paths: %lu\n", paths);

    fclose(input_file_ptr);
    return 0;
}
