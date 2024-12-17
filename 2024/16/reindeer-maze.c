
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

#define POINT_SET_ARR_SIZE 500

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

struct point_list_item_s
{
    struct point_list_item_s *next_item_ptr;
    struct grid_point_s point;
};

///Types
typedef struct point_list_item_s *point_item_p_t;
typedef point_item_p_t *point_item_p_ref_t;

///Global Variables
static uint8_t _main_grid[GRID_HEIGHT][GRID_WIDTH] = {0};
static struct grid_point_s _rein_loc = {0};
static struct grid_point_s _end_loc = {0};

///Functions
void print_grid(void)
{
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
            if (_rein_loc.height_index == i && _rein_loc.width_index == j)
                printf("@");
            else
                printf("%c", _main_grid[i][j]);
        printf("\n");
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

static point_item_p_t make_new_item(struct grid_point_s value)
{
    point_item_p_t new_item_ptr = calloc(1, sizeof(struct point_list_item_s));
    if (new_item_ptr == NULL)
    {
        perror("[ERROR]Item memory allocation failed!");
        exit(1);
    }
    new_item_ptr->point = value;
    return new_item_ptr;
}

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

static bool list_contains_point(struct grid_point_s point, point_item_p_t list_head_ptr)
{
    if (list_head_ptr == NULL)
        return false;

    point_item_p_t curr_item_ptr = list_head_ptr;
    while (curr_item_ptr != NULL)
    {
        if (are_point_eq(curr_item_ptr->point,point))
            return true;
        curr_item_ptr = curr_item_ptr->next_item_ptr;
    }
    return false;
}

static void list_add_point(struct grid_point_s point, point_item_p_ref_t list_head_ptr_ref)
{
    point_item_p_t new_point_item_ptr = make_new_item(point);
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

static void dispose_point_list(point_item_p_t item_ptr)
{
    if (item_ptr == NULL)
        return;
    if (item_ptr->next_item_ptr != NULL)
        dispose_point_list(item_ptr->next_item_ptr);
    free(item_ptr);
}

static point_item_p_t list_copy(point_item_p_ref_t list_head_ptr)
{
    if (list_head_ptr == NULL)
        return NULL;
    point_item_p_t curr_item_ptr = *list_head_ptr;
    point_item_p_t copy_list_head = make_new_item(curr_item_ptr->point);
    if (curr_item_ptr != NULL)
    {
        list_add_point(curr_item_ptr->point, &copy_list_head);
        curr_item_ptr = curr_item_ptr->next_item_ptr;
    }
    return copy_list_head;
}

int main(void)
{
    FILE *input_file_ptr = fopen("sample.input", "r");
    //FILE *input_file_ptr = fopen("puzzle-16-24.input", "r");
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
        switch (current_character)
        {
            case '\n':
                grid_width_index = 0;
                grid_height_index++;
                break;
            case 'S':
                _rein_loc.width_index = grid_width_index;
                _rein_loc.height_index = grid_height_index;
                _main_grid[grid_height_index][grid_width_index++] = '.';
                break;
            case 'E':
                _end_loc.width_index = grid_width_index;
                _end_loc.height_index = grid_height_index;
                _main_grid[grid_height_index][grid_width_index++] = '.';
            case '#':
            case '.':
                _main_grid[grid_height_index][grid_width_index++] = current_character;
                break;
        }
    }

    fclose(input_file_ptr);
    return 0;
}
