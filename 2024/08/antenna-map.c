
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

#define POINT_SET_ARR_SIZE 500
#define ANTENNA_MAP_ARR_SIZE 256

#define ENABLE_PART_TWO 1

///Enums
enum broadcast_proc_result_e
{
    OK,
    NOT_APPLICABLE,
    OOB,
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
static point_item_ptr_t _antenna_list_map[ANTENNA_MAP_ARR_SIZE] = {0};
static point_item_ptr_t _broadcast_point_set[POINT_SET_ARR_SIZE] = {0};
static uint64_t _broadcast_point_set_count = {0};

//Functions
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
    return list_contains_point(point, *(hashset_head_ptr_ref + (point_hash%POINT_SET_ARR_SIZE)));
}

static void list_insert_point(struct grid_point_s point, point_arr_ptr_t list_head_ptr_ref)
{
    if (list_contains_point(point, *list_head_ptr_ref))
        return;

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
    list_insert_point(point, (hashset_head_ptr_ref + (point_hash%POINT_SET_ARR_SIZE)));
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
    for (int i = 0; i < POINT_SET_ARR_SIZE; i++)
        dispose_point_list(*(point_arr_ptr+i));

    memset(point_arr_ptr, 0, sizeof(point_item_ptr_t)*POINT_SET_ARR_SIZE);
}

static enum broadcast_proc_result_e process_broadcast_location(struct grid_point_s start_loc, int32_t slope_height, int32_t slope_width)
{
    struct grid_point_s broadcast_loc =
    {
        .height_index = start_loc.height_index+slope_height,
        .width_index = start_loc.width_index+slope_width,
    };

    if (slope_height == 0 && slope_width == 0)
    {
        return NOT_APPLICABLE;
    }

    if (!is_within_grid_bounds(broadcast_loc))
        return OOB;

    if (!hashset_contains_point(broadcast_loc, _broadcast_point_set))
    {
        hashset_insert_point(broadcast_loc, _broadcast_point_set);
        _broadcast_point_set_count++;
        printf("[%u,%u]\n", broadcast_loc.height_index, broadcast_loc.width_index);
        return OK;
    }
    return NOT_APPLICABLE;
}

static void process_full_slope_line(struct grid_point_s starting_point, int32_t slope_height, int32_t slope_width)
{
    const int8_t MULTIPLIER_START_COUNT = 2;
    int8_t multiplier_starts[] = {1, -1};
    for (int i = 0; i < MULTIPLIER_START_COUNT; i++)
    {
        enum broadcast_proc_result_e curr_poc_result;
        int32_t slope_muliplier = multiplier_starts[i];
        while ((curr_poc_result =
            process_broadcast_location(
                starting_point, slope_height*slope_muliplier, slope_width*slope_muliplier)) != OOB)
        {
            slope_muliplier = (slope_muliplier > 0)
                ? slope_muliplier+1
                : slope_muliplier-1;
        }
    }
}

static void count_broadcast_locations(void)
{
    for (int i = 0; i < ANTENNA_MAP_ARR_SIZE; i++)
    {
        point_arr_ptr_t current_point_list_head_ptr = (_antenna_list_map+i);
        point_item_ptr_t curr_eval_antenna_loc_ptr = *(current_point_list_head_ptr);
        while (curr_eval_antenna_loc_ptr != NULL)
        {
            point_item_ptr_t curr_compare_antenna_loc_ptr = *(current_point_list_head_ptr);
            while (curr_compare_antenna_loc_ptr != NULL)
            {
                int32_t slope_height = curr_eval_antenna_loc_ptr->point.height_index - curr_compare_antenna_loc_ptr->point.height_index;
                int32_t slope_width = curr_eval_antenna_loc_ptr->point.width_index - curr_compare_antenna_loc_ptr->point.width_index;

                if (slope_height == 0 && slope_width == 0)
                {
                    curr_compare_antenna_loc_ptr = curr_compare_antenna_loc_ptr->next_item_ptr;
                    continue;
                }

#if ENABLE_PART_TWO
                process_full_slope_line(curr_eval_antenna_loc_ptr->point, slope_height, slope_width);
#else
                process_broadcast_location(curr_eval_antenna_loc_ptr->point, slope_height, slope_width);
#endif

                curr_compare_antenna_loc_ptr = curr_compare_antenna_loc_ptr->next_item_ptr;
            }
            curr_eval_antenna_loc_ptr = curr_eval_antenna_loc_ptr->next_item_ptr;
        }
    }
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-08-24.input", "r");
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

        if (current_character != '.')
        {
            struct grid_point_s antenna_loc =
            {
                .height_index = grid_height_index,
                .width_index = grid_width_index,
            };
            list_insert_point(antenna_loc, &_antenna_list_map[current_character]);
        }

        grid_width_index++;
    }

    count_broadcast_locations();
    printf("%lu\n",_broadcast_point_set_count);

    dispose_point_list_arr(_antenna_list_map);
    dispose_point_list_arr(_broadcast_point_set);
    fclose(input_file_ptr);
    return 0;
}
