
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

///Local Declarations
struct int_map_item_s
{
    struct int_map_item_s *next_item;
    uint64_t int_key;
    uint64_t int_value;
};
typedef struct int_map_item_s *int_map_item_ptr;

#define INT_MAP_INTERNAL_SIZE 25
struct int_map_s
{
    uint32_t item_count;
    int_map_item_ptr contents[INT_MAP_INTERNAL_SIZE];
};
typedef struct int_map_s *int_map_ptr;

struct int_map_query_container_s
{
    uint64_t value;
    bool could_get_value;
};

///Static Constants
#define SEED_ARR_SIZE 25
#define MAP_COUNT 6

///Static Global Variables
static uint16_t _iterator_set_array_index = 0;
static uint16_t _iterator_list_index = 0;

///Static Functions
static uint32_t int_key_get_hash(uint64_t key)
{
    return key ^ 391;
}

static uint32_t int_map_calculate_internal_index(uint64_t key)
{
    return int_key_get_hash(key)%INT_MAP_INTERNAL_SIZE;
}

static bool int_map_contains(int_map_ptr map_ptr, uint64_t key)
{
    uint32_t index = int_map_calculate_internal_index(key);

    int_map_item_ptr curr = map_ptr->contents[index];
    while (curr != NULL)
    {
        if (curr->int_key == key)
            return true;
        curr = curr->next_item;
    }
    return false;
}

static void int_map_add(int_map_ptr map_ptr, uint64_t key, uint64_t value)
{
    if (int_map_contains(map_ptr, key))
        return;

    uint32_t index = int_map_calculate_internal_index(key);

    int_map_item_ptr *curr = &(map_ptr->contents[index]);
    while (*curr != NULL)
        curr = &((*curr)->next_item);
    *curr = (int_map_item_ptr)calloc(1, sizeof(struct int_map_item_s));
    if (*curr == NULL)
    {
        perror("KVP allocation failed.");
        exit(1);
    }
    (*curr)->int_key = key;
    (*curr)->int_value = value;
    map_ptr->item_count++;
}

static struct int_map_query_container_s int_map_try_get_value(int_map_ptr map_ptr, uint64_t key)
{
    struct int_map_query_container_s result = {0};

    uint32_t index = int_map_calculate_internal_index(key);
    int_map_item_ptr curr = map_ptr->contents[index];
    while (curr != NULL)
    {
        if (curr->int_key == key)
        {
            result.value = curr->int_value;
            result.could_get_value = true;
            return result;
        }
        curr = curr->next_item;
    }
    return result;
}

static void int_map_destroy(int_map_ptr map_ptr)
{
    if (map_ptr == NULL)
        return;
    for (int i = 0; i < INT_MAP_INTERNAL_SIZE; i++)
    {
        int_map_item_ptr curr = map_ptr->contents[i];
        while (curr != NULL)
        {
            int_map_item_ptr prev = curr;
            curr = prev->next_item;
            free(prev);
        }
    }
    free(map_ptr);
}

static void int_map_iterator_init(void)
{
    _iterator_set_array_index = 0;
    _iterator_list_index = 0;
}

static int_map_item_ptr int_map_iterator_get_next(int_map_ptr map_ptr)
{
    if (map_ptr == NULL)
    {
        perror("Cannot get value from NULL.");
        exit(1);
    }

    for (; _iterator_set_array_index < INT_MAP_INTERNAL_SIZE; _iterator_set_array_index++)
    {
        int_map_item_ptr item_ptr = map_ptr->contents[_iterator_set_array_index];

        if (item_ptr == NULL)
        {
            // Move to the next backing array index.
            _iterator_list_index = 0;
            continue;
        }

        uint16_t curr_list_index = 0;
        while (item_ptr != NULL)
        {
            if (_iterator_list_index <= curr_list_index)
            {
                _iterator_list_index++;
                return item_ptr;
            }

            item_ptr = item_ptr->next_item;
            curr_list_index++;
        }
        _iterator_list_index = 0;
    }

    return NULL;
}

///Extern Functions
int main(void)
{
    FILE *input_file_ptr = fopen("test.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    char *line_start_ptr = NULL;
    size_t line_len = 0;

    // Parse seeds.
    char *curr_char_ptr = NULL
    getline(&curr_char_ptr, &line_len, input_file_ptr);
    while (*curr_char_ptr != ':')
        curr_char_ptr++;
    curr_char_ptr++;

    uint64_t seed_arr[SEED_ARR_SIZE] = {0};
    uint16_t seed_count = 0;
    bool could_parse_seed = false;
    do
    {
        char *end_parse_char_ptr = NULL;
        uint64_t parsed_number = strtoul(curr_char_ptr, &end_parse_char_ptr, 10);
        could_parse_seed = (curr_char_ptr != end_parse_char_ptr);
        if (could_parse_seed)
        {
            seed_arr[seed_count] = parsed_number;
            seed_count++;
        }
    } while (could_parse_seed);

    int_map_ptr map_arr[MAP_COUNT] = {0};
    uint8_t curr_map_index = 0;
    while (getline(&line_start_ptr, &line_len, input_file_ptr) && !feof(input_file_ptr))
    {
        if (line_start_ptr == NULL)
        {
            perror("Failed to get line.");
            exit(1);
        }

        if (isblank(*line_start_ptr))
        {
            //TODO UNFINISHED: start new map
        }
    }

    fclose(input_file_ptr);
    return 0;
}
