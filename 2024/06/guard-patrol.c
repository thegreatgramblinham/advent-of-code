
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define GRID_WIDTH 130
#define GRID_HEIGHT 130

#define POINT_SET_ARR_SIZE 500

#define DIRECTION_COUNT 4

//Enums
enum walk_dir_e
{
    NORTH,
    EAST,
    SOUTH,
    WEST,
};

enum guard_advance_result_e
{
    OK,
    COLLIDE,
    LOOP,
    END,
};

//Structs
struct grid_point_s
{
    int32_t height_index;
    int32_t width_index;
    enum walk_dir_e faced_dir;
};

struct list_item_grid_point_s
{
    struct list_item_grid_point_s *next_item_ptr;
    struct grid_point_s point;
};

//Types
typedef struct list_item_grid_point_s *list_point_item_ptr;
typedef list_point_item_ptr *point_lookup_arr_ptr_t;
typedef bool (*point_equality_func_ptr_t)(struct grid_point_s, struct grid_point_s);

///Global Variables
static uint8_t _main_grid[GRID_HEIGHT][GRID_WIDTH] = {0};
static struct grid_point_s _guard_start_position = {0};
static struct grid_point_s _curr_guard_position = {0};

static list_point_item_ptr _visited_point_lookup[POINT_SET_ARR_SIZE] = {0};

static list_point_item_ptr _curr_obstacle_path_lookup[POINT_SET_ARR_SIZE] = {0};
static uint64_t _loop_count = 0;

//Functions
bool are_base_point_eq(struct grid_point_s x, struct grid_point_s y)
{
    return x.height_index == y.height_index && x.width_index == y.width_index;
}

bool are_full_point_eq(struct grid_point_s x, struct grid_point_s y)
{
    return are_base_point_eq(x,y) && x.faced_dir == y.faced_dir;
}

uint64_t get_base_point_hash(struct grid_point_s point)
{
    return ((point.height_index)^391) * ((point.width_index)^391);
}

uint64_t get_full_point_hash(struct grid_point_s point)
{
    return get_base_point_hash(point) * ((point.faced_dir)^391);
}

bool contains_visited_point(struct grid_point_s point, point_lookup_arr_ptr_t lookup_arr,
    point_equality_func_ptr_t equality_func_ptr)
{
    uint64_t point_hash = get_base_point_hash(point);
    list_point_item_ptr list_head_ptr = *(lookup_arr + (point_hash%POINT_SET_ARR_SIZE));
    list_point_item_ptr curr_item_ptr = list_head_ptr;
    while (curr_item_ptr != NULL)
    {
        if ((*equality_func_ptr)(curr_item_ptr->point, point))
            return true;
        curr_item_ptr = curr_item_ptr->next_item_ptr;
    }
    return false;
}

void insert_visited_point(struct grid_point_s point, point_lookup_arr_ptr_t lookup_arr,
    point_equality_func_ptr_t equality_func_ptr)
{
    if (contains_visited_point(point, lookup_arr, equality_func_ptr))
        return;

    uint64_t point_hash = get_base_point_hash(point);
    list_point_item_ptr new_point_item_ptr = calloc(1, sizeof(struct list_item_grid_point_s));
    new_point_item_ptr->point = point;
    list_point_item_ptr *insert_loc_ptr = (lookup_arr + (point_hash%POINT_SET_ARR_SIZE));
    if (*insert_loc_ptr == NULL)
        *insert_loc_ptr = new_point_item_ptr;
    else
    {
        list_point_item_ptr curr_item_ptr = *insert_loc_ptr;
        while(curr_item_ptr->next_item_ptr != NULL)
            curr_item_ptr = curr_item_ptr->next_item_ptr;
        curr_item_ptr->next_item_ptr = new_point_item_ptr;
    }
}

bool is_within_grid_bounds(struct grid_point_s point)
{
    return (point.height_index >= 0 && point.height_index < GRID_HEIGHT
        && point.width_index >= 0 && point.width_index < GRID_WIDTH)
        && _main_grid[point.height_index][point.width_index] != 0;
}

bool is_obstacle(struct grid_point_s point)
{
    if (!is_within_grid_bounds(point))
        return false;
    return _main_grid[point.height_index][point.width_index] == '#';
}

enum guard_advance_result_e try_advance_guard_position(enum walk_dir_e walk_dir)
{
    struct grid_point_s next_guard_loc = _curr_guard_position;
    switch (walk_dir)
    {
        case NORTH:
            next_guard_loc.height_index--;
            break;
        case EAST:
            next_guard_loc.width_index++;
            break;
        case SOUTH:
            next_guard_loc.height_index++;
            break;
        case WEST:
            next_guard_loc.width_index--;
            break;
        default:
            perror("walk_dir_e OoB");
            exit(1);
    }
    if (!is_within_grid_bounds(next_guard_loc))
        return END;
    if (is_obstacle(next_guard_loc))
        return COLLIDE;
    if (contains_visited_point(next_guard_loc, _curr_obstacle_path_lookup, are_full_point_eq))
        return LOOP;

    _curr_guard_position = next_guard_loc;
    return OK;
}

void dispose_lookup_item(list_point_item_ptr item_ptr)
{
    if (item_ptr == NULL)
        return;
    if (item_ptr->next_item_ptr != NULL)
        dispose_lookup_item(item_ptr->next_item_ptr);
    free(item_ptr);
}

void dispose_lookup_arr(point_lookup_arr_ptr_t point_arr_ptr)
{
    for (int i = 0; i < POINT_SET_ARR_SIZE; i++)
        dispose_lookup_item(*(point_arr_ptr+i));

    memset(point_arr_ptr, 0, sizeof(list_point_item_ptr)*POINT_SET_ARR_SIZE);
}

uint64_t plot_guard_path(point_lookup_arr_ptr_t lookup_arr, point_equality_func_ptr_t equality_func_ptr)
{
    uint64_t total_steps = 0;
    uint32_t turn_count = 0;
    enum guard_advance_result_e advance_result;
    while ((advance_result = try_advance_guard_position(_curr_guard_position.faced_dir)) != END)
    {
        if (advance_result == COLLIDE)
        {
            turn_count++;
            _curr_guard_position.faced_dir = turn_count % DIRECTION_COUNT;
        }
        else if (advance_result == LOOP)
        {
            _loop_count++;
            return 0;
        }
        else
        {
            if (!contains_visited_point(_curr_guard_position, lookup_arr, equality_func_ptr))
                total_steps++;

            insert_visited_point(_curr_guard_position, lookup_arr, equality_func_ptr);
        }
    }

    return total_steps;
}

void count_obstacle_loops(void)
{
    for (int i = 0; i < POINT_SET_ARR_SIZE; i++)
    {
        list_point_item_ptr curr_lookup_container_start = _visited_point_lookup[i];
        list_point_item_ptr curr_obstacle_trial_loc = curr_lookup_container_start;
        while (curr_obstacle_trial_loc != NULL)
        {
            struct grid_point_s trial_point = curr_obstacle_trial_loc->point;
            if (are_base_point_eq(trial_point, _guard_start_position))
            {
                curr_obstacle_trial_loc = curr_obstacle_trial_loc->next_item_ptr;
                continue;
            }
            _main_grid[trial_point.height_index][trial_point.width_index] = '#';

            _curr_guard_position = _guard_start_position;
            plot_guard_path(_curr_obstacle_path_lookup, are_full_point_eq);

            _main_grid[trial_point.height_index][trial_point.width_index] = '.';
            dispose_lookup_arr(_curr_obstacle_path_lookup);
            curr_obstacle_trial_loc = curr_obstacle_trial_loc->next_item_ptr;
        }
    }
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-06-24.input", "r");
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

        _main_grid[grid_height_index][++grid_width_index] = current_character;

        if (current_character == '^')
        {
            _curr_guard_position.width_index = grid_width_index;
            _curr_guard_position.height_index = grid_height_index;
            _curr_guard_position.faced_dir = NORTH;
            _guard_start_position = _curr_guard_position;
        }
    }

    printf("%lu\n",plot_guard_path(_visited_point_lookup, are_base_point_eq));
    count_obstacle_loops();
    printf("%lu\n",_loop_count);

    dispose_lookup_arr(_visited_point_lookup);
    fclose(input_file_ptr);
    return 0;
}
