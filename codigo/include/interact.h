#pragma once

#include <vector>
#include <list>
#include <string>
#include <functional>
#include <limits>
#include <iostream>

class end_of_file_exception : public std::exception {
public:
    virtual const char* what() const noexcept;
};

class validation_error : public std::exception {
    std::string reason;
public:
    validation_error(const char* what);
    validation_error(std::string what);
    virtual const char* what() const noexcept;
};


// Type alias
using MenuOption = std::pair<std::string, std::function<void()>>;

class MenuBlock {
    std::vector<MenuOption> options;

public:
    /**
     * @brief Adds a new option to the MenuBlock instance
     * 
     * @param text Text of the option
     * @param callback Callback function executed for the option
     */
    void add_option(const std::string &text, const std::function<void()> &callback);

    /**
     * @brief Gets the options object of the MenuBlock instance
     * 
     * @return std::vector<MenuOption> const& options of the MenuBlock
     */
    std::vector<MenuOption> const &get_options() const;
};

class Menu {
    std::string title;
    std::list<MenuBlock> blocks;
    MenuBlock special_block;

    /**
    * @brief Displays all the options on the console according to the menu
    */
    void print_options() const;

    /**
    * @brief Reads user's selected option by input
    * @return The selected option, if valid
    */
    MenuOption const &get_selected_option() const;

public:

    /**
     * @brief Creates a Menu instance
     * @param title The menu title
     */
    explicit Menu(const std::string &title);

    /**
    * @brief Adds the given block to the menu
    * @param block The block to add to the menu
    */
    void add_block(const MenuBlock &block);

    /**
    * @brief Adds the given special block to the menu
    * @param block The block to add to the menu
    */
    void set_special_block(const MenuBlock &block);

    /**
    * @brief Display's the menu's title and options, if they exist
    */
    void show() const;

    /**
    * @brief Display's the menu's title and subtitle and options, if they exist
    * @param subtitle The menu's subtitle
    */
    void show(const std::string& subtitle) const;
};

class GetLine : public std::string {
public:
    friend std::istream& operator>>(std::istream& in, GetLine &value);
};

/**
 * @brief Waits for any user input
 */
void wait_for_input();

/**
 * @brief Prompts the user to provide a value through console input.
 * On interactive terminals, it will be done in a "fancy" fashion and in a single line.
 * 
 * @param prompt The text that will be used to prompt the user for a value.
 * @param warning The error message that will be presented to the user if the value could not be parsed or if the validator function returns false.
 * @param validator An optional function that determines if the value is valid.
 * This function is only executed after the parsing of the value was successful.
 * If this function throws a string, it will be presented to the user as an error message.
 * If this function returns false, the warning message will be presented to the user.
 * 
 * @return The value which was read from the input. 
 */
template <typename T>
T read_value(const std::string prompt, const std::string warning, const std::function<bool(const T&)> validator = [](const T&) { return true; }) noexcept(false) {
    T result;

    std::cout << "\x1B[1;33m?\x1B[0m " << prompt << std::flush;
    while (true) {
        // Is true if, and only if, all the content present on the line is of the type T
        bool is_input_valid = false;
        std::string error = warning;

        try {
            is_input_valid = std::cin.peek() != '\n'
                             && std::cin >> result && std::cin.peek() == '\n'
                             && validator(result);
        } catch (validation_error exception) {
            error = exception.what();
        }

        // Clear the buffer
        // We do this to avoid having "123a[EOF]" trigger the loop more than once.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (is_input_valid) {
            std::cout << "\x1B[F\x1B[G\x1B[32m✓\x1B[0m\x1B[" << prompt.length() + 3 << "G\x1B[K" << result << "\n\x1B[K" << std::flush;
            return result;
        } else {
            if (!std::cin.eof())
                std::cout << "\x1B[31m>>\x1B[0m " << error << "\x1B[K\x1B[F";

            std::cout << "\x1B[" << prompt.length() + 3 << "G\x1B[K";

            if (std::cin.eof()) {
                std::cout << "EOF\n\x1B[F\x1B[G\x1B[31m×\x1B[0m\x1B[E\x1B[K" << std::flush;
                throw end_of_file_exception();
            }
        }
    }
}