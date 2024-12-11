
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

///Constants
#define STR_BUFF_LEN 100
#define HASHSET_ARR_SIZE 500

#define ENABLE_PART_TWO 1

///Structs
struct list_item_int_s
{
    struct list_item_int_s *next_item_ptr;
    uint64_t value;
};

struct result_cache_list_item_s
{
    struct result_cache_list_item_s *next_item_ptr;
    uint16_t blink_count;
    uint64_t value;
    uint64_t result;
};

///Types
typedef struct list_item_int_s *int_item_ptr_t;
typedef int_item_ptr_t *int_item_ptr_ref_t;

typedef struct result_cache_list_item_s *result_item_ptr_t;
typedef result_item_ptr_t *result_item_ptr_ref_t;

///Global Variables
static int_item_ptr_t _stone_list_head_ptr = NULL;
static result_item_ptr_t _result_list_head_ptr = NULL;

static int_item_ptr_t make_new_item(uint64_t value)
{
    int_item_ptr_t new_item_ptr = calloc(1, sizeof(struct list_item_int_s));
    if (new_item_ptr == NULL)
    {
        perror("[ERROR]Item memory allocation failed!");
        exit(1);
    }
    new_item_ptr->value = value;
    return new_item_ptr;
}

static void list_add_item(uint64_t value, int_item_ptr_ref_t list_head_ptr_ref)
{
    int_item_ptr_t new_item_ptr = make_new_item(value);
    int_item_ptr_t curr_item_ptr = *list_head_ptr_ref;
    if (curr_item_ptr == NULL)
        *list_head_ptr_ref = new_item_ptr;
    else
    {
        while(curr_item_ptr->next_item_ptr != NULL)
            curr_item_ptr = curr_item_ptr->next_item_ptr;
        curr_item_ptr->next_item_ptr = new_item_ptr;
    }
}

static int_item_ptr_t list_insert_item(uint64_t value, int_item_ptr_t insert_after_item_ptr)
{
    if (insert_after_item_ptr == NULL)
    {
        perror("[ERROR]Cannot insert after NULL");
        exit(1);
    }
    int_item_ptr_t new_item_ptr = make_new_item(value);

    int_item_ptr_t temp_next = insert_after_item_ptr->next_item_ptr;
    insert_after_item_ptr->next_item_ptr = new_item_ptr;
    new_item_ptr->next_item_ptr = temp_next;

    return new_item_ptr;
}

static void dispose_list(int_item_ptr_t item_ptr)
{
    if (item_ptr != NULL)
    {
        dispose_list(item_ptr->next_item_ptr);
        free(item_ptr);
    }
}

// TODO this should absolutely be a hash set but I am not feeling well
// and needed something easier.
static int64_t result_list_has_item(uint16_t blink_count, uint64_t value)
{
    result_item_ptr_t curr_result_ptr = _result_list_head_ptr;
    while (curr_result_ptr != NULL)
    {
        if (curr_result_ptr->blink_count == blink_count
            && curr_result_ptr->value == value)
        {
            return curr_result_ptr->result;
        }
        curr_result_ptr = curr_result_ptr->next_item_ptr;
    }
    return -1;
}

static void result_list_add_item(uint16_t blink_count, uint64_t value, uint64_t result)
{
    result_item_ptr_t new_item_ptr = calloc(1, sizeof(struct result_cache_list_item_s));
    if (new_item_ptr == NULL)
    {
        perror("[ERROR]Item memory allocation failed!");
        exit(1);
    }
    new_item_ptr->blink_count = blink_count;
    new_item_ptr->value = value;
    new_item_ptr->result = result;

    result_item_ptr_t curr_item_ptr = _result_list_head_ptr;
    if (curr_item_ptr == NULL)
        _result_list_head_ptr = new_item_ptr;
    else
    {
        while(curr_item_ptr->next_item_ptr != NULL)
            curr_item_ptr = curr_item_ptr->next_item_ptr;
        curr_item_ptr->next_item_ptr = new_item_ptr;
    }
}

static void dispose_results(result_item_ptr_t item_ptr)
{
    if (item_ptr != NULL)
    {
        dispose_results(item_ptr->next_item_ptr);
        free(item_ptr);
    }
}

static int_item_ptr_t observe_stones_linear(uint64_t blink_count, int_item_ptr_t list_head_ptr)
{
    for (int i = 0; i < blink_count; i++)
    {
        int_item_ptr_t curr_item_ptr = list_head_ptr;
        char str_buff[STR_BUFF_LEN] = {0};
        int32_t printed_len = 0;

        while (curr_item_ptr != NULL)
        {
            if (curr_item_ptr->value == 0)
                curr_item_ptr->value = 1;
            else if ((printed_len = sprintf(str_buff, "%lu", curr_item_ptr->value))%2 == 0)
            {
                char str_temp_buff[STR_BUFF_LEN] = {0};
                uint32_t sub_len = printed_len/2;
                memcpy(str_temp_buff, (str_buff+sub_len), sub_len);
                memset((str_buff+sub_len), 0, sub_len);

                uint64_t sub_val1 = atoll(str_buff);
                uint64_t sub_val2 = atoll(str_temp_buff);

                curr_item_ptr->value = sub_val1;
                curr_item_ptr = list_insert_item(sub_val2, curr_item_ptr);
            }
            else
                curr_item_ptr->value *= 2024;

            curr_item_ptr = curr_item_ptr->next_item_ptr;
            memset(str_buff, 0, STR_BUFF_LEN);
            printed_len = 0;
        }
    }
    return list_head_ptr;
}

static uint64_t observe_stones_recursive(uint64_t blink_count, int_item_ptr_t list_head_ptr)
{
    if (blink_count <= 0)
        return 1;

    int_item_ptr_t curr_item_ptr = list_head_ptr;
    uint64_t stone_count = 0;
    do
    {
        int_item_ptr_t curr_item_cpy = make_new_item(curr_item_ptr->value);
        int_item_ptr_t observed_list_head_ptr = observe_stones_linear(1, curr_item_cpy);
        int_item_ptr_t curr_obs_item_ptr = observed_list_head_ptr;
        while (curr_obs_item_ptr != NULL)
        {
           int64_t cache_result = result_list_has_item(blink_count-1, curr_obs_item_ptr->value);
           if (cache_result > 0)
               stone_count += cache_result;
           else
           {
               int_item_ptr_t obs_val_cpy = make_new_item(curr_obs_item_ptr->value);
               uint64_t result = observe_stones_recursive(blink_count-1, obs_val_cpy);
               result_list_add_item(blink_count-1, curr_obs_item_ptr->value, result);
               stone_count += result;
           }
           curr_obs_item_ptr = curr_obs_item_ptr->next_item_ptr;
        }
        dispose_list(observed_list_head_ptr);
        curr_item_ptr = curr_item_ptr->next_item_ptr;
    } while (curr_item_ptr != NULL);

    return stone_count;
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-11-24.input", "r");
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
        char *curr_char = line_buff;
        do
        {
            while (*curr_char == 0x20)
                curr_char++;
            uint64_t curr_value = atoll(curr_char);
            list_add_item(curr_value, &_stone_list_head_ptr);
            while (*curr_char != 0x20 && *curr_char != '\n')
                curr_char++;
        } while (*curr_char != '\n');
    }

    fclose(input_file_ptr);
    free(line_buff);

#if ENABLE_PART_TWO
    const int32_t BLINK_COUNT = 75;
    printf("%lu\n", observe_stones_recursive(BLINK_COUNT, _stone_list_head_ptr));
#else
    const int32_t BLINK_COUNT = 25;
    printf("%lu\n", list_count_items(observe_stones_linear(BLINK_COUNT, _stone_list_head_ptr)));
#endif

    dispose_list(_stone_list_head_ptr);
    dispose_results(_result_list_head_ptr);
    return 0;
}
