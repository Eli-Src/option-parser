#include <iostream>

#include "parser.h"

int main(int argc, char* argv[]) {
    Parser parser = {"Parser"};
    parser.add_option('h', "help", "Display this help message");
    parser.add_option('i', "input", "Input file name", true, Parser::argument_types::String_view);
    parser.add_option('o', "output", "Output file", true, Parser::argument_types::Bool);
    // TODO: if require_argument=false then return bool automaticly
    parser.add_option('a', "idk", "This is a test", false, Parser::argument_types::Double);

    parser.parse(argc, argv);

    if (parser.get_option_value("help")) {
        parser.print_options({"idk", "", "input", "output", "help"});
        return 0;
    }

    // const std::optional<bool> b = parser.get_option_value<bool>("output");
    // if (!b) {
    //     std::cerr << parser.get_program_name() << ": option --output is missing\n";
    //     return 1;
    // }

    const std::optional<double> idk = parser.get_option_value<double>("idk");
    if (!idk) {
        std::cerr << parser.get_program_name() << ": option --idk is missing\n";
        return 1;
    }

    // std::cout << parser.get_program_name() << ": --output: " << *b << '\n';
    std::cout << parser.get_program_name() << ": --idk: " << *idk << '\n';

    return 0;
}
