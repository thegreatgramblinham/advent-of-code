
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

#define POINT_SET_ARR_SIZE 500

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

struct grid_point_list_item_s
{
    struct grid_point_list_item_s *next_item_ptr;
    struct grid_point_s point;
};

struct garden_plot_s
{
    uint64_t area;
    uint64_t perimiter;
};

///Types
typedef struct grid_point_list_item_s *point_item_p_t;
typedef point_item_p_t *point_item_p_ref_t;

///Global Variables
static uint8_t _main_grid[GRID_HEIGHT][GRID_WIDTH] = {0};
static point_item_p_t _visited_point_hashset[POINT_SET_ARR_SIZE] = {0};

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

static bool list_contains_point(struct grid_point_s point, point_item_p_t list_head_ptr)
{
    point_item_p_t curr_item_ptr = list_head_ptr;
    while (curr_item_ptr != NULL)
    {
        if (are_point_eq(curr_item_ptr->point,point))
            return true;
        curr_item_ptr = curr_item_ptr->next_item_ptr;
    }
    return false;
}

static bool hashset_contains_point(struct grid_point_s point, point_item_p_ref_t hashset_head_ptr_ref)
{
    uint64_t point_hash = get_point_hash(point);
    return list_contains_point(point, *(hashset_head_ptr_ref + (point_hash%POINT_SET_ARR_SIZE)));
}

static void list_insert_point(struct grid_point_s point, point_item_p_ref_t list_head_ptr_ref)
{
    if (list_contains_point(point, *list_head_ptr_ref))
        return;

    point_item_p_t new_point_item_ptr = calloc(1, sizeof(struct grid_point_list_item_s));
    new_point_item_ptr->point = point;
    point_item_p_t curr_item_ptr = *list_head_ptr_ref;
    if (curr_item_ptr == NULL)
        *list_head_ptr_ref = new_point_item_ptr;
    else
    {
        while(curr_item_ptr->next_item_ptr != NULL)
            curr_item_ptr = curr_item_ptr->next_item_ptr;
        curr_item_ptr->next_item_ptr = new_point_item_ptr;
    }
}

static void hashset_insert_point(struct grid_point_s point, point_item_p_ref_t hashset_head_ptr_ref)
{
    if (hashset_contains_point(point, hashset_head_ptr_ref))
        return;

    uint64_t point_hash = get_point_hash(point);
    list_insert_point(point, (hashset_head_ptr_ref + (point_hash%POINT_SET_ARR_SIZE)));
}

static void dispose_point_list(point_item_p_t item_ptr)
{
    if (item_ptr == NULL)
        return;
    if (item_ptr->next_item_ptr != NULL)
        dispose_point_list(item_ptr->next_item_ptr);
    free(item_ptr);
}

static void dispose_point_hashset_arr(point_item_p_ref_t point_arr_ptr)
{
    for (int i = 0; i < POINT_SET_ARR_SIZE; i++)
        dispose_point_list(*(point_arr_ptr+i));
    memset(point_arr_ptr, 0, sizeof(point_item_p_t)*POINT_SET_ARR_SIZE);
}

static struct garden_plot_s build_plot(struct grid_point_s loc)
{
    struct garden_plot_s return_plot = {0};
    if (!is_within_grid_bounds(loc))
        return return_plot;

    if (hashset_contains_point(loc, _visited_point_hashset))
        return return_plot;
    else
        hashset_insert_point(loc, _visited_point_hashset);

    return_plot.area = 1;
    return_plot.perimiter = 4;

    uint8_t target_char = _main_grid[loc.height_index][loc.width_index];

    for (int i = NORTH; i <= WEST; i++)
    {
        struct grid_point_s advance_loc = loc;
        switch (i)
        {
            case NORTH:
                advance_loc.height_index--;
                break;
            case EAST:
                advance_loc.width_index++;
                break;
            case SOUTH:
                advance_loc.height_index++;
                break;
            case WEST:
                advance_loc.width_index--;
                break;
            default:
                perror("Direction OOB");
                exit(1);
        }
        if (!is_within_grid_bounds(advance_loc))
            continue;
        uint8_t advance_char = _main_grid[advance_loc.height_index][advance_loc.width_index];
        if (target_char != advance_char)
            continue;
        struct garden_plot_s sub_plot = build_plot(advance_loc);
        return_plot.perimiter -= 1;
        return_plot.area += sub_plot.area;
        return_plot.perimiter += sub_plot.perimiter;
    }
    return return_plot;
}

static uint64_t plot_garden(void)
{
    uint64_t garden_total = 0;
    for (int i = 0; i < GRID_HEIGHT; i++)
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            struct grid_point_s curr_point =
            {
                .height_index = i,
                .width_index = j,
            };
            struct garden_plot_s result_plot = build_plot(curr_point);
            garden_total += result_plot.area * result_plot.perimiter;
        }
    return garden_total;
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-12-24.input", "r");
    //FILE *input_file_ptr = fopen("sample.input", "r");
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

    printf("%lu\n",plot_garden());

    dispose_point_hashset_arr(_visited_point_hashset);
    fclose(input_file_ptr);
    return 0;
}
