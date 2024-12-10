
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

#define HASHSET_ARR_SIZE 200
#define TRAIL_PATH_ARR_SIZE 10

#define ENABLE_PART_TWO 1

static uint8_t TRAIL_PATH[TRAIL_PATH_ARR_SIZE] =
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

///Enums
enum dir_e
{
    NORTH,
    EAST,
    SOUTH,
    WEST,
};

///Structs
struct grid_point_s
{
    int32_t height_index;
    int32_t width_index;
};

struct list_item_grid_point_s
{
    struct list_item_grid_point_s *next_item_ptr;
    struct grid_point_s point;
};

///Types
typedef struct list_item_grid_point_s *point_item_ptr_t;
typedef point_item_ptr_t *point_arr_ptr_t;

///Global Variables
static uint8_t _main_grid[GRID_HEIGHT][GRID_WIDTH] = {0};

static point_item_ptr_t _trail_endpoint_set_arr[HASHSET_ARR_SIZE] = {0};
static uint64_t _trail_total = 0;

///Functions
static bool is_within_grid_bounds(struct grid_point_s point)
{
    return (point.height_index >= 0 && point.height_index < GRID_HEIGHT
        && point.width_index >= 0 && point.width_index < GRID_WIDTH)
        && _main_grid[point.height_index][point.width_index] != 0;
}

static bool are_point_eq(struct grid_point_s x, struct grid_point_s y)
{
    return x.height_index == y.height_index && x.width_index == y.width_index;
}

static uint64_t get_point_hash(struct grid_point_s point)
{
    return ((point.height_index)^391) * ((point.width_index)^391);
}

static bool list_contains_point(struct grid_point_s point, point_item_ptr_t list_head_ptr)
{
    point_item_ptr_t curr_item_ptr = list_head_ptr;
    while (curr_item_ptr != NULL)
    {
        if (are_point_eq(curr_item_ptr->point,point))
            return true;
        curr_item_ptr = curr_item_ptr->next_item_ptr;
    }
    return false;
}

static bool hashset_contains_point(struct grid_point_s point, point_arr_ptr_t hashset_head_ptr_ref)
{
    uint64_t point_hash = get_point_hash(point);
    return list_contains_point(point, *(hashset_head_ptr_ref + (point_hash%HASHSET_ARR_SIZE)));
}

static void list_insert_point(struct grid_point_s point, point_arr_ptr_t list_head_ptr_ref)
{
    point_item_ptr_t new_point_item_ptr = calloc(1, sizeof(struct list_item_grid_point_s));
    new_point_item_ptr->point = point;
    point_item_ptr_t curr_item_ptr = *list_head_ptr_ref;
    if (curr_item_ptr == NULL)
        *list_head_ptr_ref = new_point_item_ptr;
    else
    {
        while(curr_item_ptr->next_item_ptr != NULL)
            curr_item_ptr = curr_item_ptr->next_item_ptr;
        curr_item_ptr->next_item_ptr = new_point_item_ptr;
    }
}

static void hashset_insert_point(struct grid_point_s point, point_arr_ptr_t hashset_head_ptr_ref)
{
    if (hashset_contains_point(point, hashset_head_ptr_ref))
        return;

    uint64_t point_hash = get_point_hash(point);
    list_insert_point(point, (hashset_head_ptr_ref + (point_hash%HASHSET_ARR_SIZE)));
}

static void dispose_point_list(point_item_ptr_t item_ptr)
{
    if (item_ptr == NULL)
        return;
    if (item_ptr->next_item_ptr != NULL)
        dispose_point_list(item_ptr->next_item_ptr);
    free(item_ptr);
}

static void dispose_point_list_arr(point_arr_ptr_t point_arr_ptr)
{
    for (int i = 0; i < HASHSET_ARR_SIZE; i++)
        dispose_point_list(*(point_arr_ptr+i));

    memset(point_arr_ptr, 0, sizeof(point_item_ptr_t)*HASHSET_ARR_SIZE);
}

static void find_next_step(struct grid_point_s loc, uint8_t *target_step)
{
    if (_main_grid[loc.height_index][loc.width_index] != *(target_step))
        return;

    if (*(target_step) == '9')
    {
        if (!hashset_contains_point(loc, _trail_endpoint_set_arr))
        {
            hashset_insert_point(loc, _trail_endpoint_set_arr);
#if !ENABLE_PART_TWO
            _trail_total++;
#endif
        }
#if ENABLE_PART_TWO
        _trail_total++;
#endif
        return;
    }

    for (int i = NORTH; i <= WEST; i++)
    {
        struct grid_point_s new_loc = loc;
        switch (i)
        {
            case NORTH:
                new_loc.height_index--;
                break;
            case EAST:
                new_loc.width_index++;
                break;
            case SOUTH:
                new_loc.height_index++;
                break;
            case WEST:
                new_loc.width_index--;
                break;
        }
        if (is_within_grid_bounds(new_loc))
            find_next_step(new_loc, target_step+1);
    }
}

static void count_trails()
{
    for (int i = 0; i < GRID_HEIGHT; i++)
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            struct grid_point_s curr_loc = { i, j };
            find_next_step(curr_loc, TRAIL_PATH);
            dispose_point_list_arr(_trail_endpoint_set_arr);
        }
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-10-24.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    uint32_t current_character;
    int32_t grid_width_index = 0;
    int32_t grid_height_index = 0;
    while ((current_character = fgetc(input_file_ptr)) != EOF)
    {
        if (current_character == '\n')
        {
            grid_width_index = 0;
            grid_height_index++;
            continue;
        }

        _main_grid[grid_height_index][grid_width_index] = current_character;
        grid_width_index++;
    }
    fclose(input_file_ptr);

    count_trails();

    printf("%lu\n",_trail_total);
    return 0;
}
