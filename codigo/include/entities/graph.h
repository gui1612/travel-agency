#pragma once

#include <vector>
#include <list>
#include <set>
#include <tuple>

class Graph;

#include "dataset.h"

struct Node {
    std::vector<unsigned long> outgoing;
    std::vector<unsigned long> incoming;

    bool visited;
    unsigned int parent;

    /** @brief Search parameters */
    unsigned long distance;
    unsigned long capacity;
    unsigned long flow_increase;
    unsigned long earliest_start;
    unsigned long in_degree;
};

/** @brief Manages a Edge */
class Edge {

    /** @brief The edge's origin */
    unsigned long origin;

    /** @brief The Edge' destination */
    unsigned long destination;

    /** @brief The Edge' capacity */
    unsigned long capacity;

    /** @brief The Edge's flow */
    unsigned long flow;

    /** @brief The trip's duration */
    unsigned long duration;

    /** @brief Whether this edge is active or not */
    bool active = true;

    public:

        /**
         * @brief Construct a new Edge object
         * 
         * @param origin The edge's origin
         * @param destination The Edge' destination
         * @param capacity The Edge' capacity
         * @param duration The trip's duration
         */
        Edge(unsigned long origin, unsigned long destination, unsigned long capacity, unsigned long duration);

        /**
         * @brief Edge copy constructor
         */
        Edge(const Edge &edge);

        /**
         * @brief Creates a Edge object from a vector of entries
         * 
         * @param entry 
         * @return Edge object
         */
        static Edge from_entry(const FileEntry &entry);

        /**
         * @brief Creates all Edge from a file
         * 
         * @param file 
         * @return vector<Edge> 
         */
        static std::vector<Edge> from_file(const File &file);

        /** Getters */
        unsigned long get_origin() const;
        unsigned long get_destination() const;
        unsigned long get_capacity() const;
        unsigned long get_flow() const;
        unsigned long get_duration() const;

        /** Setters */
        void set_flow(unsigned long flow);

};

class Graph {
    unsigned long n;
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    public:

        Graph() {};
  
        /**
         * @brief Construct a new Graph object
         * 
         * @param n The number of nodes in this graph
         */
        Graph(int n);


        /**
         * @brief Copy constructor for a graph
         * 
         * @param g 
         */
        Graph(const Graph &g);

        /**
         * @brief Add a edge to this graph
         * 
         * @param edge The edge to add to this graph
         */
        void add_edge(const Edge &edge);

        /**
         * @brief Alters the graph to contain information about the maximum capacity path
         * 
         * @param start The starting point
         */
        void max_capacity_dijkstra(unsigned long start);

        unsigned long size();

        /**
         * @brief Alters the graph to contain information about the pareto optimal paths
         * 
         * @param start The starting point
         */
        void pareto_optimal_dijkstra(unsigned long start, bool max_capacity);

        /**
         * @brief Alters the graph to contain information about the shortest paths and their maximum flow increase
         * 
         * @param start The starting point
         */
        void max_flow_increase_bfs(unsigned long start);

        /**
         * @brief Alters the graph to contain information about the biggest duration path
         * 
         * @param start The starting point
         */
        void biggest_duration(unsigned long start);

        /**
         * @brief Calculates the optimal path between two points
         * 
         * @param start The starting point
         * @param end The ending point
         * @return A list containing all the nodes in the path
         */
        std::tuple<std::list<unsigned long>, unsigned long, unsigned long> get_path(unsigned long start, unsigned long end);

        /**
         * @brief Calculates the path that holds the most people in a group
         * 
         * @param start The starting point
         * @param end The ending point
         * @return A list containing all the nodes in the optimal path
         */
        std::tuple<std::list<unsigned long>, unsigned long, unsigned long> get_max_capacity_path(unsigned long start, unsigned long end);

        /**
         * @brief Calculates the path that requires least bus transfers
         * 
         * @param start The starting point
         * @param end The ending point
         * @return A list containing all the nodes in the optimal path
         */
        std::tuple<std::list<unsigned long>, unsigned long, unsigned long> get_pareto_optimal_path(unsigned long start, unsigned long end, bool max_capacity);

        
        /**
         * @brief Increases the flow in the graph by a specified amount
         * 
         * @param start The starting point 
         * @param end The ending point
         * @param flow_increase The amount to increase the flow
         */
        void edmonds_karp(unsigned long start, unsigned long end, unsigned long flow_increase);

        /**
         * @brief Reads the flow path from the graph
         * 
         * @return A list containing all the start and end nodes and their respective flows in the current state
         */
        std::list<std::tuple<unsigned long, unsigned long, unsigned long>> get_flow_path();

        /**
         * @brief Gets the path from the graph for a group of certain size
         * 
         * @param start The starting point
         * @param end The ending point
         * @param size The size of the group
         * @return A list containing all the start and end nodes and their respective flows in the current state
         */
        std::list<std::tuple<unsigned long, unsigned long, unsigned long>> get_path_for_group_of_size(unsigned long start, unsigned long end, unsigned long size);

        /**
         * @brief Gets the path with a certain increment
         * 
         * @param start The starting point
         * @param end The ending point
         * @param increment The increment value
         * @return A list containing all the start and end nodes and their respective flows in the current state
         */
        std::list<std::tuple<unsigned long, unsigned long, unsigned long>> get_path_with_increment(unsigned long start, unsigned long end, unsigned long increment);

        /**
         * @brief Gets the path for a group with the max possible size
         * 
         * @param start The starting point
         * @param end The ending point
         * @return A list containing all the start and end nodes and their respective flows in the current state
         */
        std::list<std::tuple<unsigned long, unsigned long, unsigned long>> get_path_for_group_of_max_size(unsigned long start, unsigned long end);


        /**
         * @brief Get the earliest time at which a group of people would meetup again at a given destination
         * 
         * @param start The starting point
         * @param end The ending point
         * @return The time at which the group would meetup again
         */
        unsigned long get_earliest_meetup(unsigned long start, unsigned long end);

        /**
         * @brief Get the places and durations of where people will have to wait for others in order to proceed with their trip
         * 
         * @param start The starting point
         * @param end The ending point
         * @return A list of indexes of where the person will have to wait and the respective durations
         */
        std::list<std::pair<unsigned long, unsigned int>> get_waiting_periods(unsigned long start, unsigned long end);

};
