
///Imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

///Constants
#define INSTRUCTION_BUFFER_SIZE 200
#define OUTPUT_BUFFER_SIZE 200

///Enums
enum opcode_e
{
    ADV,
    BXL,
    BST,
    JNZ,
    BXC,
    OUT,
    BDV,
    CDV,
};

///Global Variables
static int32_t _reg_a = 0;
static int32_t _reg_b = 0;
static int32_t _reg_c = 0;

static int32_t _program_operation_index = 0;

static int32_t _instruction_buffer[INSTRUCTION_BUFFER_SIZE] = {0};
static int32_t _instruction_buffer_insert_index = 0;

static int32_t _output_buffer[OUTPUT_BUFFER_SIZE] = {0};
static uint32_t _output_buffer_insert_index = 0;

///Functioons
static void print_output_buffer(void)
{
    for (int i = 0; i < _output_buffer_insert_index; i++)
        printf("%d,", _output_buffer[i]);
    printf("\n");
}

static int32_t get_combo_operand_value(int8_t operand)
{
    switch (operand)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            return operand;
        case 4:
            return _reg_a;
        case 5:
            return _reg_b;
        case 6:
            return _reg_c;
        default:
            perror("Operand OOB");
            exit(1);
    }
}

static void adv(int8_t operand)
{
    int32_t c_op = get_combo_operand_value(operand);
    _reg_a /= exp2(c_op);
}

static void bxl(int8_t operand)
{
    _reg_b ^= operand;
}

static void bst(int8_t operand)
{
    _reg_b = get_combo_operand_value(operand)%8;
}

static bool jnz(int8_t operand)
{
    if (_reg_a == 0)
        return false;
    _program_operation_index = operand;
    return true;
}

static void bxc(int8_t operand)
{
    (void)operand;
    _reg_b ^= _reg_c;
}

static void out(int8_t operand)
{
    _output_buffer[_output_buffer_insert_index++] = get_combo_operand_value(operand)%8;
}

static void bdv(int8_t operand)
{
    int32_t c_op = get_combo_operand_value(operand);
    _reg_b = _reg_a / exp2(c_op);
}

static void cdv(int8_t operand)
{
    int32_t c_op = get_combo_operand_value(operand);
    _reg_c = _reg_a / exp2(c_op);
}

static void run_program(void)
{
    while(_program_operation_index < (_instruction_buffer_insert_index - 1))
    {
        enum opcode_e opcode = _instruction_buffer[_program_operation_index];
        int8_t operand = _instruction_buffer[_program_operation_index+1];
        bool should_skip_index_advance = false;
        switch (opcode)
        {
            case ADV:
                adv(operand);
                break;
            case BXL:
                bxl(operand);
                break;
            case BST:
                bst(operand);
                break;
            case JNZ:
                should_skip_index_advance = jnz(operand);
                break;
            case BXC:
                bxc(operand);
                break;
            case OUT:
                out(operand);
                break;
            case BDV:
                bdv(operand);
                break;
            case CDV:
                cdv(operand);
                break;
            default:
                perror("Opcode OOB");
                exit(1);
        }
        if (!should_skip_index_advance)
            _program_operation_index += 2;
    }
}

static char* jump_to_next_numeric_char(char *str)
{
    if (str == NULL)
        return NULL;

    char *curr_pos = str;
    while (!isdigit(*curr_pos) && *(curr_pos) != '\0')
        curr_pos++;

    return isdigit(*curr_pos) ? curr_pos : NULL;
}

int main(void)
{
    FILE *input_file_ptr = fopen("puzzle-17-24.input", "r");
    //FILE *input_file_ptr = fopen("sample.input", "r");
    if (!input_file_ptr)
    {
        perror("Could not open input file.");
        exit(1);
    }

    char *line_buff = NULL;
    size_t line_buff_len = 0;
    ssize_t read_result = 0;
    uint8_t line_index = 0;
    while ((read_result = getline(&line_buff, &line_buff_len, input_file_ptr)) != -1)
    {
        char *curr_char = line_buff;
        curr_char = jump_to_next_numeric_char(curr_char);

        switch (line_index)
        {
            case 0:
                _reg_a = atoi(curr_char);
                break;
            case 1:
                _reg_b = atoi(curr_char);
                break;
            case 2:
                _reg_c = atoi(curr_char);
                break;
            case 3:
                break;
            case 4:
                while (curr_char != NULL)
                {
                    _instruction_buffer[_instruction_buffer_insert_index++] = atoi(curr_char);
                    curr_char++;
                    curr_char = jump_to_next_numeric_char(curr_char);
                }
                break;
            default:
                perror("Line index OOB");
                exit(1);
        }

        line_index++;
    }

    fclose(input_file_ptr);
    free(line_buff);

    run_program();
    print_output_buffer();
    return 0;
}
