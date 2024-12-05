
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//Constants
#define MAX_PAGE_NUM 100
#define MAX_PAGE_COUNT 100
#define INPUT_BUFFER_ARR_SIZE 10

//Structs
struct int_list_item_s
{
    struct int_list_item_s *next_item;
    uint32_t int_value;
};

//Types
typedef struct int_list_item_s *int_item_ptr;

//Global Variables
static int_item_ptr _page_order_lookup[MAX_PAGE_NUM] = {0};

//Functions
static bool can_page_exist_before(uint16_t exist_before, uint16_t page)
{
    int_item_ptr curr_ptr = _page_order_lookup[exist_before];
    while(curr_ptr != NULL)
    {
        if (curr_ptr->int_value == page)
            return false;
        curr_ptr = curr_ptr->next_item;
    }
    return true;
}

static bool is_page_order_valid(uint16_t* page_arr, size_t arr_len)
{
    for (int i = 1; i < arr_len; i++)
    {
        uint16_t current_page = *(page_arr+i);
        for (int j = 0; j < i; j++)
        {
            if (!can_page_exist_before(current_page, *(page_arr+j)))
                return false;
        }
    }
    return true;
}

static void dispose_list_item(int_item_ptr item_ptr)
{
    if (item_ptr == NULL)
        return;
    if (item_ptr->next_item != NULL)
        dispose_list_item(item_ptr->next_item);
    free(item_ptr);
}

static void dispose_list_map(void)
{
    for (int i = 0; i < MAX_PAGE_NUM; i++)
        dispose_list_item(_page_order_lookup[i]);
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-05-24.input", "r");
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
        if (isspace(*line_buff))
            break;
        uint16_t page_key = atoi(line_buff);
        uint16_t page_val = atoi(line_buff+3);

        int_item_ptr new_item = calloc(1, sizeof(struct int_list_item_s));
        if (new_item == NULL)
            exit(1);

        new_item->int_value = page_val;

        int_item_ptr curr_item = _page_order_lookup[page_key];
        if (curr_item == NULL)
            _page_order_lookup[page_key] = new_item;
        else
        {
            while (curr_item->next_item != NULL)
                curr_item = curr_item->next_item;
            curr_item->next_item = new_item;
        }
    }

    uint64_t middle_digit_summation = 0;
    uint64_t corrected_middle_digit_summation = 0;

    while ((read_result = getline(&line_buff, &line_buff_len, input_file_ptr)) != -1)
    {
        uint16_t page_nums[MAX_PAGE_COUNT] = {0};
        int16_t page_nums_index = -1;

        char num_buff[INPUT_BUFFER_ARR_SIZE] = {0};
        int16_t num_buff_index = -1;
        char *curr_char = line_buff;
        while (!isspace(*curr_char))
        {
            while (isdigit(*curr_char))
            {
                num_buff[++num_buff_index] = *(curr_char);
                curr_char++;
            }

            page_nums[++page_nums_index] = atoi(num_buff);

            if (*curr_char == '\n')
                break;
            else
                curr_char++;

            memset(num_buff, 0, sizeof(char)*INPUT_BUFFER_ARR_SIZE);
            num_buff_index = -1;
        }

        uint8_t page_nums_count = page_nums_index+1;
        if (is_page_order_valid(page_nums, page_nums_count))
            middle_digit_summation += page_nums[page_nums_index/2];
        else
        {
            uint16_t page_nums_sorted[MAX_PAGE_COUNT] = {0};
            size_t sorted_nums_count = 0;

            for (int i = 0; i < page_nums_count; i++)
            {
                uint16_t num_sort_test[MAX_PAGE_COUNT] = {0};
                size_t num_sort_test_count = sorted_nums_count;
                memcpy(num_sort_test, page_nums_sorted, sorted_nums_count*sizeof(uint16_t));

                num_sort_test[i] = page_nums[i];
                num_sort_test_count++;

                uint8_t failed_count = 0;
                while (!is_page_order_valid(num_sort_test, num_sort_test_count))
                {
                    failed_count++;
                    // Insert our sort target one to the left and try again.
                    memcpy(num_sort_test+(i-(failed_count-1)), page_nums_sorted+(i-failed_count), failed_count*sizeof(uint16_t));
                    num_sort_test[i-failed_count] = page_nums[i];
                }

                memcpy(page_nums_sorted, num_sort_test, num_sort_test_count*sizeof(uint16_t));
                sorted_nums_count++;
            }

            corrected_middle_digit_summation += page_nums_sorted[sorted_nums_count/2];
        }
    }

    dispose_list_map();
    free(line_buff);
    fclose(input_file_ptr);

    printf("%lu\n", middle_digit_summation);
    printf("%lu\n", corrected_middle_digit_summation);
    return 0;
}
