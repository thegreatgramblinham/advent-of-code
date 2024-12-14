
///Imports
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define ROBO_ARR_SIZE 500
#define SECOND_COUNT 100 //8000

#define GRID_WIDTH  101
#define GRID_HEIGHT 103
#define QUAD_COUNT 4

#define PRINT_GRID 0

///Structs
struct grid_point_s
{
    int32_t height_index;
    int32_t width_index;
};

struct robo_state_s
{
    struct grid_point_s loc;
    struct grid_point_s slope;
};

///Global Variables
static struct robo_state_s _robo_arr[ROBO_ARR_SIZE] = {0};
static uint64_t _robo_count = 0;

static uint32_t _quandrant_counts[QUAD_COUNT] = {0};

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

static void update_quad_counts(void)
{
    for (int i = 0; i < QUAD_COUNT; i++)
    {
        uint32_t quad_start_width_index = 0;
        uint32_t quad_end_width_index = 0;
        uint32_t quad_start_height_index = 0;
        uint32_t quad_end_height_index = 0;
        switch (i)
        {
            case 0:
                //Quad 1
                quad_start_width_index = GRID_WIDTH-(GRID_WIDTH/2);
                quad_end_width_index = GRID_WIDTH-1;
                quad_start_height_index = 0;
                quad_end_height_index = (GRID_HEIGHT/2)-1;
                break;
            case 1:
                //Quad 2
                quad_start_width_index = 0;
                quad_end_width_index = (GRID_WIDTH/2)-1;
                quad_start_height_index = 0;
                quad_end_height_index = (GRID_HEIGHT/2)-1;
                break;
            case 2:
                //Quad 3
                quad_start_width_index = 0;
                quad_end_width_index = (GRID_WIDTH/2)-1;
                quad_start_height_index = GRID_HEIGHT-(GRID_HEIGHT/2);
                quad_end_height_index = GRID_HEIGHT-1;
                break;
            case 3:
                //Quad 4
                quad_start_width_index = GRID_WIDTH-(GRID_WIDTH/2);
                quad_end_width_index = GRID_WIDTH-1;
                quad_start_height_index = GRID_HEIGHT-(GRID_HEIGHT/2);
                quad_end_height_index = GRID_HEIGHT-1;
                break;
            default:
                perror("Quad OOB");
                exit(1);
        }

        _quandrant_counts[i] = 0;
        for (int j = 0; j < _robo_count; j++)
        {
            struct grid_point_s curr_robo_loc = _robo_arr[j].loc;
            if (curr_robo_loc.width_index >= quad_start_width_index
                && curr_robo_loc.width_index <= quad_end_width_index
                && curr_robo_loc.height_index >= quad_start_height_index
                && curr_robo_loc.height_index <= quad_end_height_index)
                _quandrant_counts[i]++;
        }
    }
}

static void step_robos(void)
{
    for (int i = 0; i < _robo_count; i++)
    {
        struct robo_state_s *curr_robo = &_robo_arr[i];
        int32_t new_width_loc = curr_robo->loc.width_index + curr_robo->slope.width_index;
        int32_t new_height_loc = curr_robo->loc.height_index + curr_robo->slope.height_index;

        while (new_width_loc < 0)
            new_width_loc = GRID_WIDTH + new_width_loc;
        while (new_width_loc >= GRID_WIDTH)
            new_width_loc = new_width_loc - (GRID_WIDTH);

        while (new_height_loc < 0)
            new_height_loc = GRID_HEIGHT + new_height_loc;
        while (new_height_loc >= GRID_HEIGHT)
            new_height_loc = new_height_loc - GRID_HEIGHT;

        curr_robo->loc.width_index = new_width_loc;
        curr_robo->loc.height_index = new_height_loc;
    }
}

uint64_t calc_safety_factor(void)
{
    uint64_t total = 1;
    for (int i = 0; i < QUAD_COUNT; i++)
        total *= _quandrant_counts[i];
    return total;
}

void print_grid(void)
{
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            uint8_t cnt = 0;
            for (int k = 0; k < _robo_count; k++)
            {
                if (_robo_arr[k].loc.height_index == i
                        && _robo_arr[k].loc.width_index == j)
                    cnt++;
            }
            printf("%c",cnt > 0 ? '#' : '-');
        }
        printf("\n");
    }
}

int main (void)
{
    FILE *input_file_ptr = fopen("puzzle-14-24.input", "r");
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
        char *curr_line_pos = line_buff;
        struct robo_state_s *curr_robo = &_robo_arr[_robo_count];

        curr_line_pos = jump_to_next_numeric_char(curr_line_pos);
        curr_robo->loc.width_index = atoi(curr_line_pos);
        curr_line_pos = jump_to_next_delim(curr_line_pos, ',');
        curr_line_pos++;
        curr_robo->loc.height_index = atoi(curr_line_pos);
        curr_line_pos = jump_to_next_delim(curr_line_pos, '=');
        curr_line_pos++;

        curr_robo->slope.width_index = atoi(curr_line_pos);
        curr_line_pos = jump_to_next_delim(curr_line_pos, ',');
        curr_line_pos++;
        curr_robo->slope.height_index = atoi(curr_line_pos);

        _robo_count++;
    }

    for (int i = 0; i < SECOND_COUNT; i++)
    {
        step_robos();
#if PRINT_GRID
        printf(">%d\n", i);
        print_grid();
#endif
    }

    update_quad_counts();
    printf("%lu\n", calc_safety_factor());

    fclose(input_file_ptr);
    free(line_buff);
    return 0;
}
