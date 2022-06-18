#include "ui.h"
#include "entities/graph.h"
#include "dataset.h"

#include <iostream>
#include <string>
#include <utility>
#include <list>
#include <set>
#include <fstream>
#include <iomanip>
#include <regex>
#include <string>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

UI::UI(bool sort) : sort(sort) {
    bool is_running = true;

    string filename = choose_file();
    File file(filename);
    int n = file.get_header()[0];

    Graph graph{n}; 

    for (Edge &edge : Edge::from_file(file)) {
        graph.add_edge(edge);
    }

    this->graph = graph;
}

void UI::start() {
    Menu scenario_choice = get_scenario_menu();

    while (is_running) {

        MenuBlock special_block;    
        special_block.add_option("Exit", [this]() { is_running = false; });

        scenario_choice.set_special_block(special_block);
        scenario_choice.show();

        if (!is_running)
            break;

        wait_for_input();
    }
}

string UI::choose_file() {
    string user_choice = read_value<GetLine>("Please choose the dataset file: ", "File not found", [](const string filename) {
        ifstream infile(filename);
        return infile.is_open() && infile.ignore(1).good();
    });

    return user_choice;
}

std::pair<unsigned long, unsigned long> UI::choose_starting_and_ending_nodes() {
    std::pair<unsigned long, unsigned long> pair;

    pair.first = read_value<unsigned long>("Choose a starting node: ", "Invalid node", [this](const unsigned long &start) {
        return start >= 1 && start <= this->graph.size();
    });

    pair.second = read_value<unsigned long>("Choose an ending node: ", "Invalid node", [this](const unsigned long &end) {
        return end >= 1 && end <= this->graph.size();
    });

    return pair;
}

Menu UI::get_separation_menu() {
    Menu scenario("Choose one of the following scenarios:");


    MenuBlock scenario_options;
    scenario_options.add_option("[Scenario 2.1] Determine a route to a group, given its dimension", [this]() {
        this->scenario_two_executed = true;
        std::pair<unsigned long, unsigned long> start_end_nodes = choose_starting_and_ending_nodes();

        unsigned long size = read_value<unsigned long>("Choose the group size: ", "Invalid size", [](const unsigned long &start) {
            return start > 0;
        });

        list<tuple<unsigned long, unsigned long, unsigned long>> lst = graph.get_path_for_group_of_size(start_end_nodes.first, start_end_nodes.second, size);

        if (sort) {
            lst.sort([](tuple<unsigned long, unsigned long, unsigned long> t1, tuple<unsigned long, unsigned long, unsigned long> t2) {
                auto [origin1, destination1, flow1] = t1;
                auto [origin2, destination2, flow2] = t2;

                if (origin1 == origin2) {
                    if (destination1 == destination2) {
                        return flow1 < flow2;
                    }
                    
                    return destination1 < destination2;
                }
                
                return origin1 < origin2;
            });
        }

        unsigned long max_capacity = 0;

        cout << "\nFormat: [ origin -> destination / flow ]\n\n";
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            auto [origin, destination, flow] = *it;
            if (origin == start_end_nodes.first)
                max_capacity += flow;

            cout << "[ " << origin << " -> " << destination << " / " << flow << " ]" << endl;
        }

        cout << "\nDimension of the group: " << max_capacity << endl << endl;
    });

    if (scenario_two_executed) {
        scenario_options.add_option("[Scenario 2.2] Correct a route, if needed, so that the size of the group can increase from a number of units given", [this]() {
            std::pair<unsigned long, unsigned long> start_end_nodes = choose_starting_and_ending_nodes();

            unsigned long increment = read_value<unsigned long>("Choose the increment value: ", "Invalid increment", [](const unsigned long &start) {
                return start > 0;
            });

            list<tuple<unsigned long, unsigned long, unsigned long>> lst = graph.get_path_with_increment(start_end_nodes.first, start_end_nodes.second, increment);

            if (sort) {
                lst.sort([](tuple<unsigned long, unsigned long, unsigned long> t1, tuple<unsigned long, unsigned long, unsigned long> t2) {
                    auto [origin1, destination1, flow1] = t1;
                    auto [origin2, destination2, flow2] = t2;

                    if (origin1 == origin2) {
                        if (destination1 == destination2) {
                            return flow1 < flow2;
                        }
                        
                        return destination1 < destination2;
                    }
                    
                    return origin1 < origin2;
                });
            }

            unsigned long max_capacity = 0;

            cout << "\nFormat: [ origin -> destination / flow ]\n\n";
            for (auto it = lst.begin(); it != lst.end(); ++it) {
                auto [origin, destination, flow] = *it;
                if (origin == start_end_nodes.first)
                    max_capacity += flow;

                cout << "[ " << origin << " -> " << destination << " / " << flow << " ]" << endl;
            }

            cout << "\nDimension of the group: " << max_capacity << endl << endl;
    });}

    scenario_options.add_option("[Scenario 2.3] Determine the maximum dimension of a group and a path", [this]() {
        this->scenario_two_executed = true;

        std::pair<unsigned long, unsigned long> start_end_nodes = choose_starting_and_ending_nodes();
        list<tuple<unsigned long, unsigned long, unsigned long>> lst = graph.get_path_for_group_of_max_size(start_end_nodes.first, start_end_nodes.second);

        if (sort) {
            lst.sort([](tuple<unsigned long, unsigned long, unsigned long> t1, tuple<unsigned long, unsigned long, unsigned long> t2) {
                auto [origin1, destination1, flow1] = t1;
                auto [origin2, destination2, flow2] = t2;

                if (origin1 == origin2) {
                    if (destination1 == destination2) {
                        return flow1 < flow2;
                    }
                        
                    return destination1 < destination2;
                }
                    
                return origin1 < origin2;
            });
        }

        unsigned long max_capacity = 0;

        cout << "\nFormat: [ origin -> destination / flow ]\n\n";
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            auto [origin, destination, flow] = *it;
            if (origin == start_end_nodes.first)
                max_capacity += flow;

            cout << "[ " << origin << " -> " << destination << " / " << flow << " ]" << endl;
        }

        cout << "\nMax dimension of the group: " << max_capacity << endl << endl;
    });

    if (scenario_two_executed) {
            scenario_options.add_option("[Scenario 2.4] Determine when the group would meet again at the destination", [this]() {
            std::pair<unsigned long, unsigned long> start_end_nodes = choose_starting_and_ending_nodes();
            unsigned long earliest_meetup = graph.get_earliest_meetup(start_end_nodes.first, start_end_nodes.second);

            cout << "\nEarliest time at which a group of people would meetup again: " << earliest_meetup << endl << endl;
        });

        scenario_options.add_option("[Scenario 2.5] Determine the maximum waiting time and the places where there would be elements that wait for that time, assuming that the elements that leave the same location depart from that location at the same time (and as soon as possible), ", [this]() {
            std::pair<unsigned long, unsigned long> start_end_nodes = choose_starting_and_ending_nodes();
            std::list<std::pair<unsigned long, unsigned int>> lst = graph.get_waiting_periods(start_end_nodes.first, start_end_nodes.second);

            cout << endl;
            for (auto it = lst.begin(); it != lst.end(); ++it) {
                auto [place, duration] = *it;
                cout << "[ node: " << place << " ] (duration: " << duration << ")" << endl;
            }
            cout << endl;

    });}

    scenario.add_block(scenario_options);
    return scenario;
}

Menu UI::get_non_separation_menu() {
    Menu scenario("Choose one of the following scenarios:");

    MenuBlock scenario_options;

    scenario_options.add_option("[Scenario 1.1] Maximize the size of the group and indicate any routing", [this]() {
        std::pair<unsigned long, unsigned long> start_end_nodes = choose_starting_and_ending_nodes();

        auto [lst, capacity, distance] = graph.get_max_capacity_path(start_end_nodes.first, start_end_nodes.second);

        cout << endl << "Path: ";
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            if (it == --lst.end())
                cout << *it;
            else
                cout << *it << " -> ";
        }
        cout << "\nCapacity: " << capacity << endl << endl;
    });

    scenario_options.add_option("[Scenario 1.2] Find pareto-optimal paths", [this]() {
        std::pair<unsigned long, unsigned long> start_end_nodes = choose_starting_and_ending_nodes();
       
        auto [lst, cap, dist] = this->graph.get_pareto_optimal_path(start_end_nodes.first, start_end_nodes.second, true);

        cout << endl << "Path: ";
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            if (it == --lst.end())
                cout << *it;
            else
                cout << *it << " -> ";
        }
        cout << "\nCapacity: " << cap << "\nDistance: " << dist << endl;

        auto [lst2, cap2, dist2] = graph.get_pareto_optimal_path(start_end_nodes.first, start_end_nodes.second, false);
        
        if (cap != cap2 && dist != dist2) {    
            cout << "------" << endl << "Path: ";
            for (auto it = lst2.begin(); it != lst2.end(); ++it) {
                
                if (it == --lst2.end())
                    cout << *it;
                else
                    cout << *it << " -> ";
            }
            cout << "\nCapacity: " << cap2 << "\nDistance: " << dist2 << endl;
        }

        cout << endl;
    });

    scenario.add_block(scenario_options);
    return scenario;

}

Menu UI::get_scenario_menu() {

    Menu scenario("Choose one of the following scenarios:");

    MenuBlock scenario_options;
    scenario_options.add_option("[Scenario 1] Groups that don't separate", [this]() {
        Menu scenario_choice = get_non_separation_menu();

        MenuBlock special_block;    
        special_block.add_option("Exit", [this]() { this->is_running = false; });

        scenario_choice.set_special_block(special_block);
        scenario_choice.show();
    });

    scenario_options.add_option("[Scenario 2] Groups that separate", [this]() {
            bool is_on_menu = true;
        
            MenuBlock special_block;    
            special_block.add_option("Back", [&is_on_menu]() { is_on_menu = false; });
            special_block.add_option("Exit", [this]() { this->is_running = false; });

            while (this->is_running) {
                Menu scenario_choice = get_separation_menu();
                scenario_choice.set_special_block(special_block);
                scenario_choice.show();

                if (!is_on_menu) {
                    break;
                }

                wait_for_input();
            }
            
            this->scenario_two_executed = false;
    });

    scenario_options.add_option("[Options] Change the dataset file", [this](){
        string filename = choose_file();
        File file(filename);
        int n = file.get_header()[0];

        Graph graph{n}; 

        for (Edge &edge : Edge::from_file(file)) {
            graph.add_edge(edge);
        }

        this->graph = graph;
    });

    scenario.add_block(scenario_options);
    return scenario;
}