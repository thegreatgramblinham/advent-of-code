
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define GRID_WIDTH 140
#define GRID_HEIGHT 140

#define ENABLE_PART_TWO 1

///Global Variables
static uint8_t _search_grid[GRID_HEIGHT][GRID_WIDTH] = {0};

//Enums
enum search_dir_e
{
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_WEST,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST,
};

///Functions
bool search_direction_for_targe(char* targe_char_ptr, enum search_dir_e search_dir,
    uint32_t curr_height_pos, uint32_t curr_width_pos)
{
    if (*targe_char_ptr == '\0')
        return true;

    if (curr_height_pos < 0 || curr_height_pos >= GRID_HEIGHT)
        return false;
    if (curr_width_pos < 0 || curr_width_pos >= GRID_WIDTH)
        return false;

    if (*targe_char_ptr != _search_grid[curr_height_pos][curr_width_pos])
        return false;

    switch (search_dir)
    {
        case NORTH:
            return search_direction_for_targe(++targe_char_ptr, NORTH, --curr_height_pos, curr_width_pos);
        case SOUTH:
            return search_direction_for_targe(++targe_char_ptr, SOUTH, ++curr_height_pos, curr_width_pos);
        case EAST:
            return search_direction_for_targe(++targe_char_ptr, EAST, curr_height_pos, ++curr_width_pos);
        case WEST:
            return search_direction_for_targe(++targe_char_ptr, WEST, curr_height_pos, --curr_width_pos);
        case NORTH_WEST:
            return search_direction_for_targe(++targe_char_ptr, NORTH_WEST, --curr_height_pos, --curr_width_pos);
        case NORTH_EAST:
            return search_direction_for_targe(++targe_char_ptr, NORTH_EAST, --curr_height_pos, ++curr_width_pos);
        case SOUTH_WEST:
            return search_direction_for_targe(++targe_char_ptr, SOUTH_WEST, ++curr_height_pos, --curr_width_pos);
        case SOUTH_EAST:
            return search_direction_for_targe(++targe_char_ptr, SOUTH_EAST, ++curr_height_pos, ++curr_width_pos);
        default:
            perror("Direction out of bounds!");
            exit(1);

        return false;
    }
}

uint16_t targe_word_search_count(char* targe_word_ptr, uint32_t curr_height_pos, uint32_t curr_width_pos)
{
    uint64_t targe_count = 0;
    for (int i = NORTH; i <= SOUTH_EAST; i++)
        if (search_direction_for_targe(targe_word_ptr, i, curr_height_pos, curr_width_pos))
            targe_count++;

    return targe_count;
}

uint64_t find_word_target_count(char* targe_word)
{
    uint64_t targe_total = 0;
    for (int curr_height_pos = 0; curr_height_pos < GRID_HEIGHT; curr_height_pos++)
        for (int curr_width_pos = 0; curr_width_pos < GRID_WIDTH; curr_width_pos++)
            targe_total += targe_word_search_count(targe_word, curr_height_pos, curr_width_pos);

    return targe_total;
}

bool test_for_mas_cross(uint32_t curr_height_pos, uint32_t curr_width_pos)
{
    char* m_char = "AM";
    char* s_char = "AS";

    // Search NW and SE for a M and S
    bool nw_m = search_direction_for_targe(m_char, NORTH_WEST, curr_height_pos, curr_width_pos);
    bool nw_s = search_direction_for_targe(s_char, NORTH_WEST, curr_height_pos, curr_width_pos);

    bool se_m = search_direction_for_targe(m_char, SOUTH_EAST, curr_height_pos, curr_width_pos);
    bool se_s = search_direction_for_targe(s_char, SOUTH_EAST, curr_height_pos, curr_width_pos);

    bool diag1 = (nw_m && se_s) || (nw_s && se_m);

    // Search NE and SW for a M and S
    bool ne_m = search_direction_for_targe(m_char, NORTH_EAST, curr_height_pos, curr_width_pos);
    bool ne_s = search_direction_for_targe(s_char, NORTH_EAST, curr_height_pos, curr_width_pos);

    bool sw_m = search_direction_for_targe(m_char, SOUTH_WEST, curr_height_pos, curr_width_pos);
    bool sw_s = search_direction_for_targe(s_char, SOUTH_WEST, curr_height_pos, curr_width_pos);

    bool diag2 = (ne_m && sw_s) || (ne_s && sw_m);

    return diag1 && diag2;
}

uint64_t find_xmas_count(void)
{
    uint64_t xmas_count = 0;
    for (int curr_height_pos = 0; curr_height_pos < GRID_HEIGHT; curr_height_pos++)
        for (int curr_width_pos = 0; curr_width_pos < GRID_WIDTH; curr_width_pos++)
            if (_search_grid[curr_height_pos][curr_width_pos] == 'A')
                if (test_for_mas_cross(curr_height_pos, curr_width_pos))
                    xmas_count++;

    return xmas_count;
}

int main()
{
    FILE *input_file_ptr = fopen("puzzle-04-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    uint32_t current_character;
    int32_t grid_width_index = -1;
    int32_t grid_height_index = 0;
    while ((current_character = fgetc(input_file_ptr)) != EOF)
    {
        if (current_character == '\n')
        {
            grid_width_index = -1;
            grid_height_index++;
            continue;
        }
        _search_grid[grid_height_index][++grid_width_index] = current_character;
    }
    fclose(input_file_ptr);

#if ENABLE_PART_TWO
    printf("%lu\n", find_xmas_count());
#else
    char* targe_word = "XMAS";
    printf("%lu\n", find_word_target_count(targe_word));
#endif
    return 0;
}
