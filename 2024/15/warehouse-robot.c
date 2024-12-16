
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define GRID_WIDTH 50
#define GRID_HEIGHT 50

#define PRINT_GRID 0

///Enums
enum dir_e
{
    NORTH,
    EAST,
    SOUTH,
    WEST,
};

//Structs
struct grid_point_s
{
    int32_t height_index;
    int32_t width_index;
};

///Global Variables
static uint8_t _main_grid[GRID_HEIGHT][GRID_WIDTH] = {0};
static struct grid_point_s _robo_loc = {0};

///Functions
void print_grid(void)
{
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
            if (_robo_loc.height_index == i && _robo_loc.width_index == j)
                printf("@");
            else
                printf("%c", _main_grid[i][j]);
        printf("\n");
    }
}

uint64_t calc_gps_score(void)
{
    uint64_t total_gps = 0;
    for (int i = 0; i < GRID_HEIGHT; i++)
        for (int j = 0; j < GRID_WIDTH; j++)
            if (_main_grid[i][j] == 'O')
                total_gps += ((100*i)+j);
    return total_gps;
}

static bool is_dir_instruct(uint8_t c)
{
    switch (c)
    {
        case '>':
        case '<':
        case '^':
        case 'v':
            return true;
        default:
            return false;
    }
}

static enum dir_e parse_dir_from_char(char c)
{
    switch (c)
    {
        case '>':
            return EAST;
        case '<':
            return WEST;
        case '^':
            return NORTH;
        case 'v':
            return SOUTH;
        default:
            perror("Dir sym OOB");
            exit(1);
    }
}

static struct grid_point_s advance_point_in_dir(struct grid_point_s point, enum dir_e move_dir)
{
    struct grid_point_s new_point = point;
    switch (move_dir)
    {
        case NORTH:
            new_point.height_index--;
            break;
        case EAST:
            new_point.width_index++;
            break;
        case SOUTH:
            new_point.height_index++;
            break;
        case WEST:
            new_point.width_index--;
            break;
        default:
            perror("dir_e OoB");
            exit(1);
    }
    return new_point;
}

static bool is_within_grid_bounds(struct grid_point_s point)
{
    return (point.height_index >= 0 && point.height_index < GRID_HEIGHT
        && point.width_index >= 0 && point.width_index < GRID_WIDTH)
        && _main_grid[point.height_index][point.width_index] != 0;
}

static bool attempt_box_scoot(struct grid_point_s box_loc, enum dir_e move_dir)
{
    struct grid_point_s curr_loc = box_loc;
    uint8_t curr_loc_content = _main_grid[curr_loc.height_index][curr_loc.width_index];
    if (curr_loc_content != 'O')
        return false;

    while (is_within_grid_bounds(curr_loc) 
            && curr_loc_content != '.'
            && curr_loc_content != '#')
    {
        curr_loc = advance_point_in_dir(curr_loc, move_dir);
        curr_loc_content = is_within_grid_bounds(curr_loc)
            ? _main_grid[curr_loc.height_index][curr_loc.width_index]
            : 0;
    }

    if (curr_loc_content == 0 || curr_loc_content == '#')
        return false;

    _main_grid[box_loc.height_index][box_loc.width_index] = '.';
    _main_grid[curr_loc.height_index][curr_loc.width_index] = 'O';
    return true;
}

static void attempt_move_dir(enum dir_e move_dir)
{
    struct grid_point_s new_loc = _robo_loc;
    new_loc = advance_point_in_dir(new_loc, move_dir);

    uint8_t new_loc_content = _main_grid[new_loc.height_index][new_loc.width_index];
    switch (new_loc_content)
    {
        case '#':
            // Do nothing
            break;
        case 'O':
            bool result = attempt_box_scoot(new_loc, move_dir);
            if (result)
                _robo_loc = new_loc;
            break;
        case '.':
            _robo_loc = new_loc;
            break;
        default:
            perror("new_loc_content OoB");
            exit(1);
    }
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-15-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    uint32_t current_character;
    int32_t grid_width_index = 0;
    int32_t grid_height_index = 0;
    bool parsing_grid = true;
    while ((current_character = fgetc(input_file_ptr)) != EOF)
    {
        if (parsing_grid)
            switch (current_character)
            {
                case '\n':
                    grid_width_index = 0;
                    grid_height_index++;
                    break;
                case '@':
                    _robo_loc.width_index = grid_width_index;
                    _robo_loc.height_index = grid_height_index;
                    _main_grid[grid_height_index][grid_width_index++] = '.';
                    break;
                case '#':
                case 'O':
                case '.':
                    _main_grid[grid_height_index][grid_width_index++] = current_character;
                    break;
                case '>':
                case '<':
                case '^':
                case 'v':
                    parsing_grid = false;
                    break;
            }

        if (parsing_grid)
            continue;
        if (!is_dir_instruct(current_character))
            continue;

        enum dir_e instruction_dir = parse_dir_from_char(current_character);
        attempt_move_dir(instruction_dir);

#if PRINT_GRID
        print_grid();
        printf("\n");
#endif
    }

    printf("%lu\n",calc_gps_score());

    fclose(input_file_ptr);
    return 0;
}
