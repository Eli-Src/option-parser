#include <iostream>

#include "parser.h"

int main(int argc, char* argv[]) {
    Parser parser = {"Parser"};
    parser.add_option('h', "help", "Display this help message");
    parser.add_option('i', "input", "Input file name", Parser::Argument_Types::String_View);
    parser.add_option('o', "output", "Give output", Parser::Argument_Types::Bool);

    parser.parse(argc, argv);

    if (parser.get_option_value("help")) {
        parser.print_options({"input", "output", "", "help"});
        return 0;
    }

    const std::optional<bool> b = parser.get_option_value("output");
    if (!b) {
        std::cerr << parser.get_program_name() << ": option --output is missing\n";
        return 1;
    }

    const std::optional<std::string_view> i = parser.get_option_value<std::string_view>("input");
    if (!i) {
        std::cerr << parser.get_program_name() << ": option --input is missing\n";
        return 1;
    }

    std::cout << std::boolalpha << parser.get_program_name() << ": --output: " << *b << '\n';
    std::cout << parser.get_program_name() << ": --input: " << *i << '\n';

    return 0;
}
