
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

///Static Constants
#define CHAR_DIGIT_ARR_SIZE 2

#define ENABLE_PART_TWO 1
#define ALPHA_BUFFER_ARR_SIZE 200

#define ALPHA_NUMBER_TEXT_ARR_SIZE 10

///Local Declarations
struct alpha_parse_result_s
{
    bool could_parse;
    char parse_result;
};

struct alpha_number_text_metadata_s
{
    const char* number_text;
    size_t text_length;
    char ascii_character;
};

static const struct alpha_number_text_metadata_s ALPHA_NUMBER_TEXT_METADATA[ALPHA_NUMBER_TEXT_ARR_SIZE] =
{
    { "zero", 4, '0' },
    { "one", 3, '1' },
    { "two", 3, '2' },
    { "three", 5, '3' },
    { "four", 4, '4' },
    { "five", 4, '5'},
    { "six", 3, '6' },
    { "seven", 5, '7' },
    { "eight", 5, '8' },
    { "nine", 4, '9' }
};

///Static Functions
static uint64_t combine_digit_chars(const char* arr_ptr, size_t len)
{
    char *end_ptr;
    return strtol(arr_ptr, &end_ptr, 10);
}

static struct alpha_parse_result_s try_parse_alpha_buffer(char* alpha_buffer_ptr, size_t len)
{
    for (size_t i = 0; i < ALPHA_NUMBER_TEXT_ARR_SIZE; i++)
    {
        struct alpha_number_text_metadata_s current_alpha_metadata = ALPHA_NUMBER_TEXT_METADATA[i];
        int16_t len_diff = len - current_alpha_metadata.text_length;
        if (len_diff < 0)
            continue;
        uint8_t equivalent_char_count = 0;
        for (uint16_t j = 0; j < current_alpha_metadata.text_length; j++)
        {
            const char *buf_char = (alpha_buffer_ptr + (len_diff + j));
            const char *meta_comp_char = (current_alpha_metadata.number_text + j);

            if (*buf_char != *meta_comp_char)
                break;
            else
                equivalent_char_count++;
        }

        if (equivalent_char_count == current_alpha_metadata.text_length)
        {
            struct alpha_parse_result_s found_result = { true, current_alpha_metadata.ascii_character };
            return found_result;
        }
    }

    struct alpha_parse_result_s not_found_result = { false, 0 };
    return not_found_result;
}

///Extern Functions
int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-01-23.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    uint64_t calibration_total = 0;

#if ENABLE_PART_TWO
    char alpha_buffer[ALPHA_BUFFER_ARR_SIZE] = {0};
    int16_t abuf_index = -1;
#endif

    int32_t current_value;
    char digit_chars[CHAR_DIGIT_ARR_SIZE];
    int8_t arr_index = -1;
    while ((current_value = fgetc(input_file_ptr)) != EOF)
    {
        if (isdigit(current_value))
        {
            if (arr_index < CHAR_DIGIT_ARR_SIZE-1)
                arr_index++;
            *(digit_chars + arr_index) = current_value;
#if ENABLE_PART_TWO
            abuf_index = -1;
            memset(alpha_buffer, 0, ALPHA_BUFFER_ARR_SIZE*sizeof(char));
#endif
            continue;
        }

#if ENABLE_PART_TWO
        if (isalpha(current_value))
        {
            abuf_index++;
            *(alpha_buffer + abuf_index) = current_value;
            struct alpha_parse_result_s result = try_parse_alpha_buffer(alpha_buffer, abuf_index+1);
            if (result.could_parse)
            {
                if (arr_index < CHAR_DIGIT_ARR_SIZE-1)
                    arr_index++;
                *(digit_chars + arr_index) = result.parse_result;
            }
        }
#endif

        if (current_value != '\n')
            continue;

        if (arr_index <= CHAR_DIGIT_ARR_SIZE-1)
            // If we didn't find enough, copy the last over.
            digit_chars[CHAR_DIGIT_ARR_SIZE-1] = digit_chars[arr_index];

        for (int i = 0; i < CHAR_DIGIT_ARR_SIZE; i++)
        {
            printf("%d=%c;", i, digit_chars[i]);
        }
        uint64_t combination_int = combine_digit_chars(digit_chars, CHAR_DIGIT_ARR_SIZE);
        printf("Combined int:%lu\n", combination_int);
        calibration_total += combination_int;
        arr_index = -1;
        memset(digit_chars, 0, CHAR_DIGIT_ARR_SIZE*sizeof(char));

#if ENABLE_PART_TWO
        abuf_index = -1;
        memset(alpha_buffer, 0, ALPHA_BUFFER_ARR_SIZE*sizeof(char));
#endif
    }

    fclose(input_file_ptr);
    calibration_total += combine_digit_chars(digit_chars, CHAR_DIGIT_ARR_SIZE);
    printf("Calibration total is: %lu", calibration_total);

    return 0;
}

