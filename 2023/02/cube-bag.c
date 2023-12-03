
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

///Local Declarations
struct cube_game_state_s
{
    uint8_t r_cube_quant;
    uint8_t g_cube_quant;
    uint8_t b_cube_quant;
};

///Static Constants
#define MAX_RED_CUBES 12
#define MAX_GREEN_CUBES 13
#define MAX_BLUE_CUBES 14

#define CUBE_GAME_STATE_BUFFER_ARR_SIZE 200

///Static Global Variables
static uint64_t possible_game_number_summation = 0;
static uint64_t game_power_summation = 0;
static uint16_t game_number = 1;
static bool is_current_game_valid = true;
static struct cube_game_state_s highest_cube_quantity_in_current_game = {0};

///Static Functions
static int64_t imax(int64_t x, int64_t y)
{
    return x > y ? x : y;
}

static struct cube_game_state_s parse_game_state_from_buffer(char *cube_text_buf, size_t len)
{
    struct cube_game_state_s parsed_result = {0};
    int64_t last_parsed_num = -1;

    for (int i = 0; i < len; i++)
    {
        char *current_character_ptr = (cube_text_buf + i);

        if (isblank(*current_character_ptr))
            continue;

        if (isdigit(*current_character_ptr))
        {
            char *after_parse_ptr;
            last_parsed_num = strtol(current_character_ptr, &after_parse_ptr, 10);
            i += (after_parse_ptr - current_character_ptr);
            continue;
        }

        switch (*current_character_ptr)
        {
            case 'r':
                if (last_parsed_num > 0)
                    parsed_result.r_cube_quant = last_parsed_num;
                break;
            case 'g':
                if (last_parsed_num > 0)
                    parsed_result.g_cube_quant = last_parsed_num;
                break;
            case 'b':
                if (last_parsed_num > 0)
                    parsed_result.b_cube_quant = last_parsed_num;
                break;
        }
        last_parsed_num = -1;
    }

    printf("Parsed Game %d: R:%d,G:%d,B:%d ",
        game_number,
        parsed_result.r_cube_quant,
        parsed_result.g_cube_quant,
        parsed_result.b_cube_quant);

    return parsed_result;
}

static void process_possible_game_state(struct cube_game_state_s game_state)
{
    bool is_invalid_state = (game_state.r_cube_quant > MAX_RED_CUBES)
        || (game_state.g_cube_quant > MAX_GREEN_CUBES)
        || (game_state.b_cube_quant > MAX_BLUE_CUBES);

    if (is_invalid_state)
        is_current_game_valid = false;

    printf("IsValid:%d\n",!is_invalid_state);

    highest_cube_quantity_in_current_game.r_cube_quant =
        imax(highest_cube_quantity_in_current_game.r_cube_quant, game_state.r_cube_quant);
    highest_cube_quantity_in_current_game.g_cube_quant =
        imax(highest_cube_quantity_in_current_game.g_cube_quant, game_state.g_cube_quant);
    highest_cube_quantity_in_current_game.b_cube_quant =
        imax(highest_cube_quantity_in_current_game.b_cube_quant, game_state.b_cube_quant);
}

///Extern Functions
int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-02-23.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    char game_state_buffer[CUBE_GAME_STATE_BUFFER_ARR_SIZE] = {0};
    uint16_t game_buffer_index = 0;

    int16_t current_value = 0;
    while ((current_value = fgetc(input_file_ptr)) != EOF)
    {
        if (current_value == '\n')
        {
            struct cube_game_state_s parsed_game_state =
                parse_game_state_from_buffer(game_state_buffer, game_buffer_index);
            process_possible_game_state(parsed_game_state);

            if (is_current_game_valid)
            {
                possible_game_number_summation += game_number;
                printf(">>Game %d is valid!\n", game_number);
            }
            else
                printf(">>Game %d is NOT valid!\n", game_number);

            // It seems to be that the input can never produce zero power?
            uint32_t game_power =
                highest_cube_quantity_in_current_game.r_cube_quant *
                highest_cube_quantity_in_current_game.g_cube_quant *
                highest_cube_quantity_in_current_game.b_cube_quant;
            game_power_summation += game_power;

            // Reset for a new game.
            memset(&highest_cube_quantity_in_current_game, 0, sizeof(struct cube_game_state_s));
            memset(game_state_buffer, 0, CUBE_GAME_STATE_BUFFER_ARR_SIZE*sizeof(char));
            game_buffer_index = 0;
            game_number++;
            is_current_game_valid = true;
            continue;
        }

        if (current_value != ';')
        {
            *(game_state_buffer + game_buffer_index) = current_value;
            game_buffer_index++;
            continue;
        }

        // We've reach a semi-colon "view" divider.
        struct cube_game_state_s parsed_game_state =
            parse_game_state_from_buffer(game_state_buffer, game_buffer_index);
        process_possible_game_state(parsed_game_state);
        memset(game_state_buffer, 0, CUBE_GAME_STATE_BUFFER_ARR_SIZE*sizeof(char));
        game_buffer_index = 0;
    }

    fclose(input_file_ptr);
    printf("Total games: %d, Valid game number summation: %lu\n", game_number-1, possible_game_number_summation);
    printf("Total game power: %lu", game_power_summation);
    return 0;
}
