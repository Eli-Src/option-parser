#ifndef PARSER_H
#define PARSER_H

#include <charconv>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

class Parser {
public:
    enum class Argument_Types {
        Int,
        Double,
        Bool,
        String_View
    };

private:
    using parsed_option_types = std::variant<int, double, bool, std::string_view>;

    struct Option {
        bool requires_argument;
        Argument_Types argument_type;
        std::string_view description;
        std::string_view long_identifier;
        std::optional<char> short_identifier;
    };

    std::string_view m_program_name;
    std::unordered_map<std::string_view, Option> m_options;
    std::unordered_map<std::string_view, parsed_option_types> m_parsed_options;

    std::optional<int> to_int(std::string_view input) const {
        int out;
        const char *end = input.begin() + input.size();
        const auto result = std::from_chars(input.begin(), end, out);
        if(result.ec == std::errc::invalid_argument ||
           result.ec == std::errc::result_out_of_range ||
           result.ptr != end) {
            return std::nullopt;
        }
        return out;
    }

    std::optional<double> to_double(std::string_view input) {
        const std::string str{input}; 
        try {
            size_t pos;
            const double result = std::stod(str, &pos);

            // Ensure that the entire string was used in conversion
            if (pos == str.size()) {
                return result;
            } else {
                return std::nullopt;
            }
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }

    std::optional<bool> to_bool(std::string_view input) {
        if (input == "true") {
            return true;
        } else if (input == "false") {
            return false;
        } else {
            return std::nullopt;
        }
}


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
            
            switch (m_options[option_name].argument_type) {
                case Argument_Types::Int: {
                    const std::optional<int> result = to_int(option_argument);
                    if (result) {
                        m_parsed_options[option_name] = *result;
                        break;
                    } else {
                        std::cerr << m_program_name <<
                            ": option --" << option_name << " requires a integer argument\n";
                        std::exit(1);
                    }
                } 
                case Argument_Types::Double: {
                    const std::optional<double> result = to_double(option_argument);
                    if (result) {
                        m_parsed_options[option_name] = *result;
                        break;
                    } else {
                        std::cerr << m_program_name <<
                            ": option --" << option_name << " requires a double argument\n";
                        std::exit(1);
                    }
                }
                case Argument_Types::Bool: {
                    const std::optional<bool> result = to_bool(option_argument);
                    if (result) {
                        m_parsed_options[option_name] = *result;
                        break;
                    } else {
                        std::cerr << m_program_name <<
                            ": option --" << option_name << " requires a bool argument\n";
                        std::exit(1);
                    }
                }
                case Argument_Types::String_View:
                    m_parsed_options[option_name] = option_argument;
            }

        } else {
            m_parsed_options[option_name] = true;
        }
    }

    void process_short_option(char short_option_name, std::string_view option_argument) {
        // checks if short option name exists
        for (const auto& pair : m_options) {
            if (pair.second.short_identifier.has_value()) {
                if (pair.second.short_identifier == short_option_name) {
                    // change short identifer to long identifier
                    const std::string_view long_option_name = pair.second.long_identifier;
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
    std::string_view get_program_name() const {return m_program_name;}

    Parser(std::string_view program_name) :
        m_program_name{program_name}
    {}

    void add_option(std::string_view long_identifier,
                    std::string_view description) {
        // check for duplicates
        if (m_options.find(long_identifier) != m_options.end()) {
            std::cerr << "Different options cannot have the same long identifier" << '\n';
            std::exit(1);
        }

        m_options[long_identifier] = Option{false, Argument_Types::Bool, description, long_identifier};
    }

    void add_option(std::string_view long_identifier,
                    std::string_view description,
                    Argument_Types argument_type) {
        // check for duplicates
        if (m_options.find(long_identifier) != m_options.end()) {
            std::cerr << "Different options cannot have the same long identifier" << '\n';
            std::exit(1);
        }

        m_options[long_identifier] = Option{true, argument_type, description, long_identifier};
    }

    void add_option(char short_identifier,
                    std::string_view long_identifier,
                    std::string_view description) {
        // check for duplicates
        if (m_options.find(long_identifier) != m_options.end()) {
            std::cerr << "Different options cannot have the same long identifier" << '\n';
            std::exit(1);
        } 
        for (const auto& pair : m_options) {
            if (pair.second.short_identifier == short_identifier) {
                std::cerr << "Different options cannot have the same short identifier" << '\n';
                std::exit(1);
            }
        }

        m_options[long_identifier] = Option{false, Argument_Types::Bool, description, long_identifier, short_identifier};
    }

    void add_option(char short_identifier,
                    std::string_view long_identifier,
                    std::string_view description,
                    Argument_Types argument_type) {
        // check for duplicates
        if (m_options.find(long_identifier) != m_options.end()) {
            std::cerr << "Different options cannot have the same long identifier" << '\n';
            std::exit(1);
        } 
        for (const auto& pair : m_options) {
            if (pair.second.short_identifier == short_identifier) {
                std::cerr << "Different options cannot have the same short identifier" << '\n';
                std::exit(1);
            }
        }

        m_options[long_identifier] = Option{true, argument_type, description, long_identifier, short_identifier};
    }

    void parse(int argc, char* argv[]) {
        if (argc < 2) return;

        for (int i = 1; i < argc; ++i) {
            const std::string_view arg = argv[i];
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
            const std::size_t equal_sign_pos = option_name.find('='); 

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
            } else process_long_option(option_name, option_argument);
        }
    }

    template <typename T = bool>
    std::optional<T> get_option_value(std::string_view long_identifier) const {
        const auto it = m_parsed_options.find(long_identifier);
        if (it != m_parsed_options.end()) {
            if (std::holds_alternative<T>(it->second)) {
                return std::get<T>(it->second);
            }
        }
        return std::nullopt;
    }

    void print_options(std::vector<std::string_view> print_order = {}) const {
        std::cout << "Options: \n";
        if (!print_order.empty()) {
            for (std::string_view option_name: print_order) {
                if (option_name.empty()) {
                    std::cout << '\n';
                    continue;
                }

                const auto it = m_options.find(option_name);
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
