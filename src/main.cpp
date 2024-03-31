#include "utils/args_parser.h"
#include "utils/statuses.h"
#include "bmp_lib/bmp_lib.h"
#include "alpha_blend.h"

/**
 * @brief This array contains console options, their functions and descriptions
 */
const Argument ARGS_DICT[] = {
    {"-h", print_help,               "#   -h - prints help information\n"},

    {"-b", read_background_filename, "#   -b - specify backgorund file name after this\n"},

    {"-f", read_foreground_filename, "#   -f - specify foreground file name after this\n"},

    {"-o", read_output_filename,     "#   -o - specify output file name after this\n"},
};

const int ARGS_DICT_LEN = sizeof(ARGS_DICT) / sizeof(ARGS_DICT[0]); //< args_dict array len

int main(int argc, char* argv[]) {

    ArgsVars args_vars = {};
    args_vars.background_filename = "img/table.bmp";     //< default value
    args_vars.foreground_filename = "img/cat.bmp";       //< default value
    args_vars.output_filename     = "img/table_cat.bmp"; //< default value

    STATUS_CHECK_RAISE(args_parse(argc, argv, &args_vars, ARGS_DICT, ARGS_DICT_LEN));

    BmpImg background = {};
    STATUS_CHECK_RAISE(background.read_from_file(args_vars.background_filename));

    BmpImg foreground = {};
    STATUS_CHECK_RAISE(foreground.read_from_file(args_vars.foreground_filename));

    STATUS_CHECK_RAISE(alpha_blend(&background, &foreground));

    STATUS_CHECK_RAISE(background.write_to_file(args_vars.output_filename));

    return Status::OK_EXIT;
}
