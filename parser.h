#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

class Parser {
public:
    struct Option {
        std::string_view description;
        bool requires_argument;
        std::string_view long_identifier;
        std::optional<char> short_identifier;
    };

private:
    std::string_view m_program_name;
    std::unordered_map<std::string_view, Option> m_options;
    std::unordered_map<std::string_view, std::string_view> m_parsed_options;

    void process_long_option(std::string_view option_name, std::string_view option_argument) {
        if (m_options.find(option_name) == m_options.end()) {
            std::cerr << m_program_name <<
                ": unknown option: --" << option_name << "\n";
            std::exit(1);
        }
        if (m_options[option_name].requires_argument) {
            // if argument needed but isn't given
            if (option_argument.empty()) {
                std::cerr << m_program_name <<
                    ": option --" << option_name << " requires an argument\n";
                std::exit(1);
            }

            m_parsed_options[option_name] = option_argument;
        } else {
            m_parsed_options[option_name] = " ";
        }
    }

    void process_short_option(char short_option_name, std::string_view option_argument) {
        // checks if short option name exists
        for (const auto& pair : m_options) {
            if (pair.second.short_identifier.has_value()) {
                if (pair.second.short_identifier == short_option_name) {
                    // change short identifer to long identifier
                    std::string_view long_option_name = pair.second.long_identifier;
                    if (m_options[long_option_name].requires_argument) {
                        // if argument needed but isn't given
                        if (option_argument.empty()) {
                            std::cerr << m_program_name <<
                                ": option -" << short_option_name << " requires an argument\n";
                            std::exit(1);
                        }

                        m_parsed_options[long_option_name] = option_argument;
                    } else {
                        m_parsed_options[long_option_name] = " ";
                    }
                }
            }
        }
    }

public:
    Parser(std::string_view program_name) :
        m_program_name{program_name}
    {}

    void add_option(std::string_view description,
                    bool requires_argument,
                    std::string_view long_identifier,
                    const std::optional<char>& short_identifier = std::nullopt) {
        // check for duplicates
        if (m_options.find(long_identifier) != m_options.end()) {
            std::cerr << "Different options cannot have the same long identifier" << '\n';
            std::exit(1);
        } else if (short_identifier.has_value()) {
            for (const auto& pair : m_options) {
                if (pair.second.short_identifier == short_identifier) {
                    std::cerr << "Different options cannot have the same short identifier" << '\n';
                    std::exit(1);
                }
            }
        }

        m_options[long_identifier] = Option{description, requires_argument, long_identifier, short_identifier};
    }

    std::string_view get_program_name() const {return m_program_name;}

    std::string_view get_option_value(std::string_view name) const {
        auto it = m_parsed_options.find(name);
        if (it != m_parsed_options.end()) {
            return it->second;
        }
        return "";
    }

    void parse(int argc, char* argv[]) {
        if (argc < 2) return;

        for (int i = 1; i < argc; ++i) {
            std::string_view arg = argv[i];
            std::string_view option_name;
            std::string_view option_argument;
            bool is_short_argument = false;

            if (arg.rfind("--", 0) == 0)
                option_name = arg.substr(2);
            else if (arg.rfind("-", 0) == 0) {
                option_name = arg.substr(1);
                is_short_argument = true; 
            } else {
                std::cerr << m_program_name << ": unrecognized option format: " << arg << "\n";
                std::exit(1);
            }

            // split string to check if flag value got assigned with equal sign
            std::size_t equal_sign_pos = option_name.find('='); 

            // check if equal sign was found
            if (equal_sign_pos != option_name.npos) {
                option_argument = option_name.substr(equal_sign_pos + 1);
                option_name = option_name.substr(0, equal_sign_pos);
            } else if (i + 1 < argc && m_options[option_name].requires_argument)
                option_argument = argv[++i];

            if (is_short_argument) {
                if (option_name.size() == 1)
                    process_short_option(option_name[0], option_argument);
                else {
                    for (size_t j = 0; j < option_name.size(); ++j) {
                        if (option_name[j] == '=') {
                            std::cerr << m_program_name <<
                                ": Char option stacking does not allow arguments\n";
                            std::exit(1);
                        }
                        process_short_option(option_name[j], option_argument);
                    }
                }
            } else
            process_long_option(option_name, option_argument);
        }
    }

    void print_options(std::vector<std::string_view> print_order = {}) const {
        std::cout << "Options: \n";
        if (!print_order.empty()) {
            for (std::string_view option_name: print_order) {
                if (option_name.empty()) {
                    std::cout << '\n';
                    continue;
                }

                auto it = m_options.find(option_name);
                if (it == m_options.end()) {
                    std::cerr << option_name << " is not a existing option\n";
                    std::exit(1);
                } 
                const Option option = it->second; 

                std::cout << "   ";
                std::cout << "--" << option.long_identifier << ", ";
                if (option.short_identifier.has_value()) std::cout << '-' << *option.short_identifier;
                std::cout << "\t\t" << option.description << '\n';
            }
        } else {
            for (const auto& pair : m_options) {
                std::cout << "   ";
                std::cout << "--" << pair.second.long_identifier << ", ";
                if (pair.second.short_identifier.has_value()) std::cout << '-' << *pair.second.short_identifier;
                std::cout << "\t\t" << pair.second.description << '\n';
            }
        }
    }
};

#endif // PARSER_H
