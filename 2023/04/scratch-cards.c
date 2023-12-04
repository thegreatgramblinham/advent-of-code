
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

///Local Declarations
struct int_list_item_s
{
    struct int_list_item_s *next_item;
    uint16_t int_item;
};
typedef struct int_list_item_s *int_item_ptr;

// This could be size 100 (or an array), but I want to practice dealing with hash collisions.
#define INT_HASH_SET_INTERNAL_SIZE 25
struct int_hash_set_s
{
    uint32_t item_count;
    int_item_ptr contents[INT_HASH_SET_INTERNAL_SIZE];
};
typedef struct int_hash_set_s *int_hash_set_ptr;

struct scratch_card_content_s
{
    int_hash_set_ptr winning_number_set;
    int_hash_set_ptr card_number_set;
};

///Static Constants
#define MAX_CARD_QUANT 200

///Static Global Variables
static uint16_t _iterator_set_array_index = 0;
static uint16_t _iterator_list_index = 0;
static uint32_t _current_card_number = 1;

///Static Functions
static uint32_t int_item_get_hash(uint16_t item)
{
    // Unnecessary, but again, this is just for practice.
    return item ^ 391;
}

static uint32_t int_hash_set_calculate_internal_index(uint16_t item)
{
    return int_item_get_hash(item)%INT_HASH_SET_INTERNAL_SIZE;
}

static bool int_hash_set_contains(int_hash_set_ptr set_ptr, uint16_t item)
{
    uint32_t index = int_hash_set_calculate_internal_index(item);

    int_item_ptr curr = set_ptr->contents[index];
    while (curr != NULL)
    {
        if (curr->int_item == item)
            return true;
        curr = curr->next_item;
    }
    return false;
}

static void int_hash_set_add(int_hash_set_ptr set_ptr, uint16_t item)
{
    if (int_hash_set_contains(set_ptr, item))
        return;

    uint32_t index = int_hash_set_calculate_internal_index(item);

    int_item_ptr *curr = &(set_ptr->contents[index]);
    while (*curr != NULL)
        curr = &((*curr)->next_item);
    *curr = (int_item_ptr)calloc(1, sizeof(struct int_list_item_s));
    if (*curr == NULL)
    {
        perror("Item allocation failed.");
        exit(1);
    }
    (*curr)->int_item = item;
    set_ptr->item_count++;
}

static void int_hash_set_destroy(int_hash_set_ptr set_ptr)
{
    if (set_ptr == NULL)
        return;
    for (int i = 0; i < INT_HASH_SET_INTERNAL_SIZE; i++)
    {
        int_item_ptr curr = set_ptr->contents[i];
        while (curr != NULL)
        {
            int_item_ptr prev = curr;
            curr = prev->next_item;
            free(prev);
        }
    }
    free(set_ptr);
}

static void int_hash_set_iterator_init(void)
{
    _iterator_set_array_index = 0;
    _iterator_list_index = 0;
}

static int_item_ptr int_hash_set_iterator_get_next(int_hash_set_ptr set_ptr)
{
    if (set_ptr == NULL)
    {
        perror("Cannot get value from NULL.");
        exit(1);
    }

    for (; _iterator_set_array_index < INT_HASH_SET_INTERNAL_SIZE; _iterator_set_array_index++)
    {
        int_item_ptr item_ptr = set_ptr->contents[_iterator_set_array_index];

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

static struct scratch_card_content_s get_card_content(char *card_line_start_ptr)
{
    struct scratch_card_content_s card_content =
    {
        .winning_number_set = (int_hash_set_ptr)calloc(1,sizeof(struct int_hash_set_s)),
        .card_number_set = (int_hash_set_ptr)calloc(1,sizeof(struct int_hash_set_s)),
    };

    if (card_content.winning_number_set == NULL
        || card_content.card_number_set == NULL)
    {
        perror("Failed to allocate hash sets.");
        exit(1);
    }

    if (card_line_start_ptr == NULL)
        return card_content;

    char *curr_char_ptr = card_line_start_ptr;
    while (*curr_char_ptr != ':')
        curr_char_ptr++;

    // Advance to the next blank after the colon.
    curr_char_ptr++;

    // Parse the winning numbers.
    char *end_parse_char_ptr = curr_char_ptr;
    do
    {
        curr_char_ptr = end_parse_char_ptr;
        uint64_t parsed_number = strtoul(curr_char_ptr, &end_parse_char_ptr, 10);
        if (curr_char_ptr != end_parse_char_ptr)
            int_hash_set_add(card_content.winning_number_set, parsed_number);
    } while (curr_char_ptr != end_parse_char_ptr);

    while (*curr_char_ptr != '|')
        curr_char_ptr++;

    // Advance to the next blank after the vertical bar.
    curr_char_ptr++;
    end_parse_char_ptr = curr_char_ptr;

    // Now parse the card numbers.
    do
    {
        curr_char_ptr = end_parse_char_ptr;
        uint64_t parsed_number = strtoul(curr_char_ptr, &end_parse_char_ptr, 10);
        if (curr_char_ptr != end_parse_char_ptr)
            int_hash_set_add(card_content.card_number_set, parsed_number);
    } while (curr_char_ptr != end_parse_char_ptr);

    return card_content;
}

static uint64_t get_card_match_count(struct scratch_card_content_s card_content)
{
    int_hash_set_iterator_init();

    int_item_ptr item_ptr = int_hash_set_iterator_get_next(card_content.winning_number_set);
    uint8_t match_counter = 0;
    while (item_ptr != NULL)
    {
        uint16_t search_item = item_ptr->int_item;
        if (int_hash_set_contains(card_content.card_number_set, search_item))
            match_counter++;
        item_ptr = int_hash_set_iterator_get_next(card_content.winning_number_set);
    }
    return match_counter;
}

static uint64_t get_card_value(struct scratch_card_content_s card_content)
{
    uint64_t match_count = get_card_match_count(card_content);

    uint32_t card_value = 0;
    for (uint64_t i = 0; i < match_count; i++)
        card_value = card_value == 0 ? 1 : card_value*2;

    printf("CARD %d: Matches:%lu,Value:%u\n", _current_card_number, match_count, card_value);
    return card_value;
}

///Extern Functions
int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-04-23.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    uint64_t card_value_summation = 0;
    uint32_t processed_card_quant = 0;

    uint64_t card_repeats[MAX_CARD_QUANT];
    for (int i = 0; i < MAX_CARD_QUANT; i++)
        card_repeats[i] = 1;

    char *line_start_ptr = NULL;
    size_t line_len = 0;
    while (getline(&line_start_ptr, &line_len, input_file_ptr) && !feof(input_file_ptr))
    {
        if (line_start_ptr == NULL)
        {
            perror("Failed to get line.");
            exit(1);
        }

        if (_current_card_number >= MAX_CARD_QUANT)
        {
            perror("Max card quantity reached.");
            exit(1);
        }

        struct scratch_card_content_s card_content = get_card_content(line_start_ptr);
        uint64_t card_value = get_card_value(card_content);
        card_value_summation += card_value;

        uint64_t extra_card_quant = get_card_match_count(card_content);
        for (int i = 0; i < card_repeats[_current_card_number]; i++)
        {
            for (int j = 1; j <= extra_card_quant; j++)
                card_repeats[_current_card_number+j]++;
            processed_card_quant++;
        }

        int_hash_set_destroy(card_content.winning_number_set);
        int_hash_set_destroy(card_content.card_number_set);
        _current_card_number++;
    }

    printf("Total card value: %lu\n", card_value_summation);
    printf("Total cards processed: %u\n", processed_card_quant);
    fclose(input_file_ptr);
    return 0;
}
