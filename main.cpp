#include <iostream>

#include "parser.h"

int main(int argc, char* argv[]) {
    Parser parser = {"Parser"};
    parser.add_option("Display this help message", false, "help", 'h');
    parser.add_option("Input file name", true, "input", 'i');
    parser.add_option("Output file name", true, "output", 'o');
    parser.add_option("Goofy ahhh", true, "goofy");

    parser.parse(argc, argv);

    if (!parser.get_option_value("help").empty()) {
        parser.print_options({"goofy", "", "input", "output", "help"});
        return 0;
    }

    const std::string_view input = parser.get_option_value("input");
    if (input.empty()) {
        std::cerr << parser.get_program_name() << ": option --input is missing\n";
        return 1;
    }
    if (!input.empty()) {
        std::cout << parser.get_program_name() << ": --input: " << input << '\n';
    }

    return 0;
}
