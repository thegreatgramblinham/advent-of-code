
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

///Local Declarations
struct gear_ratio_parse_result_s
{
    uint16_t gear_a;
    uint16_t gear_b;
    bool gear_ratio_found;
};

struct point_s
{
    uint16_t row;
    uint16_t col;
};

struct number_parse_result_s
{
    uint64_t number;
    bool could_parse;
};

///Static Constants
#define INPUT_GRID_ROWS 140
#define INPUT_GRID_COLS 240

#define ENABLE_PART_TWO 1
#define GEAR_RATIO_REQ_QUANT 2

///Static Global Variables
static uint8_t input_grid[INPUT_GRID_ROWS][INPUT_GRID_COLS] = {0};

///Static Functions
static struct number_parse_result_s parse_number_at_location(uint16_t row, uint16_t col)
{
    struct number_parse_result_s result =
    {
        .number = 0,
        .could_parse = false,
    };

    char *start_character_ptr = &input_grid[row][col];
    if (!isdigit(*start_character_ptr))
        return result;

    char *current_character_ptr = start_character_ptr;
    // Determine number start.
    while (isdigit(*current_character_ptr)
            && (col - (start_character_ptr-current_character_ptr)) >= 0)
        current_character_ptr--;
    current_character_ptr++;

    //char *number_start_ptr = current_character_ptr + 1;

    //current_character_ptr = start_character_ptr;
    //// Determine number end.
    //while (isdigit(*current_character_ptr)
    //        && (row + (start_character_ptr+current_character_ptr++)) < INPUT_GRID_ROWS)
    //    ;

    char *end_parse_ptr;
    uint64_t fullNumber = strtoul(current_character_ptr, &end_parse_ptr, 10);

    result.number = fullNumber;
    result.could_parse = true;

    return result;
}

static struct gear_ratio_parse_result_s try_parse_gear_ratio(uint16_t row, uint16_t col)
{
    struct gear_ratio_parse_result_s result =
    {
        .gear_a = 0,
        .gear_b = 0,
        .gear_ratio_found = false,
    };

    // I know all this code could be genericised with function
    // pointers, but it's Sunday and my head hurts.
    const uint8_t grid_search_dist = 1;
    const uint8_t grid_search_area = 3;

    const int16_t grid_search_start = -grid_search_dist;
    const int16_t grid_search_stop = grid_search_area-(2*grid_search_dist);

    struct point_s num_points[GEAR_RATIO_REQ_QUANT] = {0};
    uint8_t num_points_index = 0;
    for (int i = grid_search_start; i <= grid_search_stop; i++)
    {
        bool was_previous_char_digit = false;
        for (int j = grid_search_start; j <= grid_search_stop; j++)
        {
            int16_t search_row = row + i;
            int16_t search_col = col + j;

            if (search_row < 0 || search_col < 0)
                continue;
            if (search_row >= INPUT_GRID_ROWS || search_col >= INPUT_GRID_COLS)
                continue;

            char location_character = input_grid[search_row][search_col];

            if (!isdigit(location_character))
            {
                was_previous_char_digit = false;
                continue;
            }

            //TODO this check is wrong.
            if (num_points_index >= GEAR_RATIO_REQ_QUANT)
                return result;

            num_points[num_points_index].row = search_row;
            num_points[num_points_index].col = search_col;
            if (!was_previous_char_digit)
                num_points_index++;

            was_previous_char_digit = true;
        }
    }

    if (num_points_index != GEAR_RATIO_REQ_QUANT)
        return result;

    // Now it's time to figure out what those two numbers are.
    struct number_parse_result_s result_a =
        parse_number_at_location(num_points[0].row, num_points[0].col);
    if (!result_a.could_parse)
        return result;
    result.gear_a = result_a.number;

    struct number_parse_result_s result_b =
        parse_number_at_location(num_points[1].row, num_points[1].col);
    if (!result_b.could_parse)
        return result;
    result.gear_b = result_b.number;

    result.gear_ratio_found = true;
    return result;
}

static bool location_has_adjacent_symbol(uint16_t row, uint16_t col)
{
    const uint8_t grid_search_dist = 1;
    const uint8_t grid_search_area = 3;

    const int16_t grid_search_start = -grid_search_dist;
    const int16_t grid_search_stop = grid_search_area-(2*grid_search_dist);

    for (int i = grid_search_start; i <= grid_search_stop; i++)
    {
        for (int j = grid_search_start; j <= grid_search_stop; j++)
        {
            int16_t search_row = row + i;
            int16_t search_col = col + j;

            if (search_row < 0 || search_col < 0)
                continue;
            if (search_row >= INPUT_GRID_ROWS || search_col >= INPUT_GRID_COLS)
                continue;

            char location_character = input_grid[search_row][search_col];
            if (location_character == '.' || isblank(location_character) || isspace(location_character))
                continue;
            if (!isalnum(location_character))
                return true;
        }
    }
    return false;
}

static bool is_adjacent_to_symbol(uint16_t row, uint16_t col, size_t str_len)
{
    for (size_t i = 0; i < str_len; i++)
    {
        if (location_has_adjacent_symbol(row, col+i))
            return true;
    }
    return false;
}

///Extern Functions
int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-03-23.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    char *line_ptr = NULL;
    size_t line_len;
    size_t line_index = 0;

    while (getline(&line_ptr, &line_len, input_file_ptr) && !feof(input_file_ptr))
    {
        if (line_len > INPUT_GRID_COLS)
            perror("Line width out of bounds!");
        if (line_index > INPUT_GRID_ROWS)
            perror("Line count out of bounds!");
        if (line_ptr == NULL)
            perror("Line could not be allocated.");

        memcpy(input_grid[line_index++], line_ptr, line_len);
    }
    free(line_ptr);

    uint64_t part_number_summation = 0;
    uint64_t gear_ratio_summation = 0;

    for (int i = 0; i < INPUT_GRID_ROWS; i++)
    {
        for (int j = 0; j < INPUT_GRID_COLS; j++)
        {
            char *current_character_ptr = &input_grid[i][j];

            if (*current_character_ptr == '*')
            {
               struct gear_ratio_parse_result_s gear_parse_result = try_parse_gear_ratio(i,j);
               if (gear_parse_result.gear_ratio_found)
               {
                   gear_ratio_summation += (gear_parse_result.gear_a * gear_parse_result.gear_b);
                   printf("Multiplying %d * %d\n", gear_parse_result.gear_a, gear_parse_result.gear_b);
               }
            }

            if (!isdigit(*current_character_ptr))
                continue;

            char *after_parse_ptr = NULL;
            uint64_t parsed_number = strtoul(current_character_ptr, &after_parse_ptr, 10);
            size_t str_len = (after_parse_ptr - current_character_ptr);

            bool is_adjacent = is_adjacent_to_symbol(i, j, str_len);

            printf("Found %lu\n", parsed_number);
            printf("IsValid:%d\n", is_adjacent);

            if (is_adjacent)
                part_number_summation += parsed_number;

            j += str_len;
        }
    }

    printf("Adjacent part number total:%lu\n", part_number_summation);
    printf("Gear ratio total:%lu", gear_ratio_summation);
    fclose(input_file_ptr);
    return 0;
}
