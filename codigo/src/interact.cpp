#include <iostream>
#include <stdexcept>
#include <limits>
#include <sstream>
#include "interact.h"

using namespace std;

const char *end_of_file_exception::what() const noexcept {
    return "EOF";
}

validation_error::validation_error(const char* what) : reason(what) {}

validation_error::validation_error(string what) : reason(what) {}

const char *validation_error::what() const noexcept {
    return this->reason.c_str();
}

void MenuBlock::add_option(const std::string &text, const std::function<void()> &callback) {
    MenuOption option = make_pair(text, callback);
    this->options.push_back(option);
}

const std::vector<MenuOption> &MenuBlock::get_options() const {
    return this->options;
}

Menu::Menu(const std::string &title) : title(title) {}

void Menu::add_block(const MenuBlock &block) {
    if (block.get_options().empty())
        throw invalid_argument("Block cannot be empty");

    this->blocks.push_back(block);
}

void Menu::set_special_block(const MenuBlock &block) {
    if (block.get_options().empty())
        throw invalid_argument("Block cannot be empty");

    this->special_block = block;
}

void Menu::print_options() const {
    size_t option_number = 1;
    for (const MenuBlock &block : this->blocks) {
        for (const MenuOption &option: block.get_options()) {
            cout << '[' << option_number << "] "
                 << option.first << '\n';

            option_number++;
        }

        cout << endl;
    }

    if (!special_block.get_options().empty()) {
        for (const MenuOption &option: this->special_block.get_options()) {
            cout << '[' << option_number << "] "
                 << option.first << '\n';

            option_number++;
        }

        cout << endl;
    }
}

MenuOption const &Menu::get_selected_option() const {
    size_t num_options = this->special_block.get_options().size();
    for (const MenuBlock &block : this->blocks)
        num_options += block.get_options().size();

    ostringstream prompt_stream;
    prompt_stream << "Your option [1";
    if (num_options > 1)
        prompt_stream << " - " << num_options;

    prompt_stream << "]: ";

    string prompt = prompt_stream.str();
    size_t selected_option = read_value<size_t>(prompt, "Please input a number from the list", [num_options](const size_t &value) {
        return value >= 1 && value <= num_options;
    });

    for (const MenuBlock &block : this->blocks) {
        if (selected_option > block.get_options().size()) {
            selected_option -= block.get_options().size();
            continue;
        }

        return block.get_options().at(selected_option - 1);
    }

    return this->special_block.get_options().at(selected_option - 1);
}

void wait_for_input() {
    if (!cin) {
        if (cin.eof())
            throw end_of_file_exception();

        return;
    }

    cout << "Press ENTER to continue..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void Menu::show() const {
    cout << "\x1B[2J\x1B[;H"
         << title << '\n' << endl;

    if (this->blocks.empty() && this->special_block.get_options().empty()) {
        cout << "Spooky... There is nothing to see here..." << endl;
        return;
    }

    this->print_options();
    MenuOption const &option = this->get_selected_option();
    cout << endl;

    option.second();
}

void Menu::show(const string &subtitle) const {
    cout << "\x1B[2J\x1B[;H"
         << title << '\n' << endl;

    cout << subtitle << endl;

    if (this->blocks.empty() && this->special_block.get_options().empty()) {
        cout << "Spooky... There is nothing to see here..." << endl;
        return;
    }

    this->print_options();
    MenuOption const &option = this->get_selected_option();
    cout << endl;

    option.second();
}

istream &operator>>(istream &in, GetLine &value) {
    value.clear();

    while (in && in.peek() != '\n')
        value.push_back(in.get());

    return in;
}