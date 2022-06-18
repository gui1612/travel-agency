#include <queue>
#include <set>
#include <limits>
#include "entities/graph.h"
#include "max_heap.h"

using namespace std;

Edge::Edge(unsigned long origin, unsigned long destination, unsigned long capacity, unsigned long duration): flow(0) {
    this->origin = origin;
    this->destination = destination;
    this->capacity = capacity;
    this->duration = duration;
}

Edge::Edge(const Edge &edge) : Edge(edge.get_origin(), edge.get_destination(), edge.get_capacity(), edge.get_duration()) {};


Edge Edge::from_entry(const FileEntry &entry) {
    unsigned long origin = entry.at(0);
    unsigned long destination = entry.at(1);
    unsigned long capacity = entry.at(2);
    unsigned long duration = entry.at(3);

    return Edge{origin, destination, capacity, duration};
}

vector<Edge> Edge::from_file(const File &file) {
    vector<Edge> edges;

    for (const FileEntry &entry : file.get_entries()) {
        edges.push_back(from_entry(entry));
    }

    return edges;
}

unsigned long Edge::get_origin() const {
    return origin;
}

unsigned long Edge::get_destination() const {
    return destination;
}

unsigned long Edge::get_capacity() const {
    return capacity;
}

unsigned long Edge::get_duration() const {
    return duration;
}



unsigned long Edge::get_flow() const {
    return flow;
}

void Edge::set_flow(unsigned long flow) {
    this->flow = flow;
}

Graph::Graph(int n) : nodes(n + 1), n(n) {}

Graph::Graph(const Graph &g) : nodes(g.nodes), edges(g.edges), n(g.n) {}

void Graph::add_edge(const Edge &edge) {
    if (edge.get_origin() == 0 || edge.get_destination() == 0) {
        throw "Edge has origin or destination at 0";
    }

    unsigned long index = edges.size();
    edges.push_back(edge);

    nodes.at(edge.get_origin()).outgoing.push_back(index);
    nodes.at(edge.get_destination()).incoming.push_back(index);
}

void Graph::max_capacity_dijkstra(unsigned long start) {
    for (unsigned long i = 1; i <= n; i++) {
        nodes.at(i).visited = false;
        nodes.at(i).parent = 0;
        nodes.at(i).capacity = 0;
    }

    nodes.at(start).capacity = 999999;

    MaxHeap<int, unsigned long> maxh(n, -1);
    maxh.insert(start, nodes[start].capacity);

    while (maxh.get_size() > 0) {
        unsigned int node = maxh.remove_max();
        nodes.at(node).visited = true;

        for (const unsigned long &e : nodes[node].outgoing) {
            const Edge &edge = edges[e];

            unsigned long dest = edge.get_destination();
            if (nodes[dest].visited) {
                continue;
            }

            unsigned long minCap = min(nodes[node].capacity, edge.get_capacity());
            if (minCap > nodes[dest].capacity) {
                nodes[dest].capacity = minCap;
                nodes[dest].parent = node;
                
                maxh.insert(dest, nodes[dest].capacity);
                maxh.increase_key(dest, nodes[dest].capacity);
            }
        }
    }
}

void Graph::pareto_optimal_dijkstra(unsigned long start, bool max_capacity) {

    for (unsigned long i = 1; i <= n; i++) {
        nodes.at(i).visited = false;
        nodes.at(i).parent = 0;
        nodes.at(i).capacity = 0;
        nodes.at(i).distance = 999999;
    }

    nodes.at(start).capacity = 999999;
    nodes.at(start).distance = 0;

    MaxHeap<int, pair<long, long>> maxh(n, -1);
    maxh.insert(start, 
        max_capacity ? make_pair(nodes[start].capacity, -nodes[start].distance) : make_pair(-nodes[start].distance, nodes[start].capacity));

    while (maxh.get_size() > 0) {
        unsigned int node = maxh.remove_max();
        nodes.at(node).visited = true;

        for (const unsigned long &e : nodes[node].outgoing) {
            const Edge &edge =  edges[e];

            unsigned long dest = edge.get_destination();
            if (nodes[dest].visited) {
                continue;
            }

            unsigned long minCap = min(nodes[node].capacity, edge.get_capacity());

            pair<long, long> possibleWeight = max_capacity ? make_pair(minCap, -nodes[node].distance - 1) : make_pair(-nodes[node].distance - 1, minCap);
            pair<long, long> currWeight = max_capacity ? make_pair(nodes[dest].capacity, -nodes[dest].distance) : make_pair(-nodes[dest].distance, nodes[dest].capacity);
            
            if (possibleWeight > currWeight) {
                nodes[dest].capacity = minCap;
                nodes[dest].distance = nodes[node].distance + 1;
                nodes[dest].parent = node;
                
                maxh.insert(dest, possibleWeight);
                maxh.increase_key(dest, possibleWeight);
            }
        }
    }
}


unsigned long Graph::size() {
    return n;
}

void Graph::max_flow_increase_bfs(unsigned long start) {
    for (unsigned long i = 1; i <= n; i++) {
        nodes.at(i).visited = false;
        nodes.at(i).parent = 0;
        nodes.at(i).flow_increase = 0;
    }

    queue<unsigned long> next;
    next.push(start);
    
    nodes.at(start).flow_increase = 999999;

    while (!next.empty()) {
        unsigned long curr = next.front(); next.pop();
        Node &curr_node = nodes.at(curr);

        curr_node.visited = true;

        for (unsigned long e : curr_node.outgoing) {
            Edge &edge = edges.at(e);
            Node &destination = nodes.at(edge.get_destination());

            if (destination.visited) {
                continue;
            }

            unsigned long flow_increase = min(curr_node.flow_increase, edge.get_capacity() - edge.get_flow());
            if (flow_increase > 0) {
                destination.flow_increase = flow_increase;
                destination.parent = e;

                next.push(edge.get_destination());
            }
            
        }

        for (unsigned long e : curr_node.incoming) {
            Edge &edge = edges.at(e);
            Node &origin = nodes.at(edge.get_origin());

            if (origin.visited) {
                continue;
            }

            unsigned long flow_increase = min(curr_node.flow_increase, edge.get_flow());
            if (flow_increase > 0) {
                origin.flow_increase = flow_increase;
                origin.parent = e;

                next.push(edge.get_origin());
            }
        }
    }
}


tuple<list<unsigned long>, unsigned long, unsigned long> Graph::get_path(unsigned long start, unsigned long end) {
    list<unsigned long> path;
    if (!nodes[end].visited) {
        return { path, 0, 0 };
    }

    unsigned long currNode = end;
    while (nodes[currNode].parent != 0) {
        path.push_front(currNode);
        currNode = nodes[currNode].parent;
    }

    path.push_front(currNode);

    return { path, nodes[end].capacity, nodes[end].distance };
}

tuple<list<unsigned long>, unsigned long, unsigned long> Graph::get_max_capacity_path(unsigned long start, unsigned long end) {
    max_capacity_dijkstra(start);
    return get_path(start, end);
}

tuple<list<unsigned long>, unsigned long, unsigned long> Graph::get_pareto_optimal_path(unsigned long start, unsigned long end, bool max_capacity) {
    pareto_optimal_dijkstra(start, max_capacity);
    return get_path(start, end);
}

void Graph::biggest_duration(unsigned long start) {
    for (unsigned long i = 1; i <= n; i++) {
        Node &node = nodes[i];

        node.parent = 0;
        node.visited = false;

        node.earliest_start = 0;
        node.in_degree = 0;
    }

    for (Edge &edge : edges) {
        if (edge.get_flow() > 0) {
            Node &node = nodes[edge.get_destination()];
            node.in_degree += 1;
        }
    }

    queue<unsigned long> next;
    next.push(start);

    while (!next.empty()) {
        unsigned long index = next.front(); next.pop();
        Node &node = nodes[index];

        node.visited = true;

        for (unsigned long e : node.outgoing) {
            const Edge &edge = edges[e];
            if (edge.get_flow() == 0) {
                continue;
            }

            Node &neighbor = nodes[edge.get_destination()];
            if (neighbor.visited) {
                continue;
            }
            
            unsigned long end = node.earliest_start + edge.get_duration();

            if (neighbor.earliest_start < end) {
                neighbor.earliest_start = end;
                neighbor.parent = index;
            }

            neighbor.in_degree -= 1;
            if (neighbor.in_degree == 0) {
                next.push(edge.get_destination());
            }
        }
    }
}

void Graph::edmonds_karp(unsigned long start, unsigned long end, unsigned long flow_increase) {    
    while (flow_increase > 0) {
        max_flow_increase_bfs(start);

        if (!nodes.at(end).visited) {
            break;
        }

        unsigned long increment = nodes.at(end).flow_increase;

        unsigned long curr = end;
        while (curr != start) {
            
            Node &curr_node = nodes.at(curr);
            Edge &edge = edges.at(curr_node.parent);

            if (curr == edge.get_destination()) {
                edge.set_flow(edge.get_flow() + increment);
                curr = edge.get_origin();
            } else {
                edge.set_flow(edge.get_flow() - increment);
                curr = edge.get_destination();
            }
        }


        flow_increase -= min(flow_increase, increment); // prevent overflow
    }
}


list<tuple<unsigned long, unsigned long, unsigned long>> Graph::get_flow_path() {
    list<tuple<unsigned long, unsigned long, unsigned long>> path;
    for (Edge &edge : edges) {
        if (edge.get_flow() > 0) {
            path.push_back({ edge.get_origin(), edge.get_destination(), edge.get_flow() });
        }
    }

    return path;
}

list<tuple<unsigned long, unsigned long, unsigned long>> Graph::get_path_for_group_of_size(unsigned long start, unsigned long end, unsigned long size) {
    for (Edge &edge : edges) {
        edge.set_flow(0);
    }

    return get_path_with_increment(start, end, size);
}

list<tuple<unsigned long, unsigned long, unsigned long>> Graph::get_path_with_increment(unsigned long start, unsigned long end, unsigned long increment) {
    if (start != end) {
        edmonds_karp(start, end, increment);
    }
    
    return get_flow_path();
}

list<tuple<unsigned long, unsigned long, unsigned long>> Graph::get_path_for_group_of_max_size(unsigned long start, unsigned long end) {
        for (Edge &edge : edges) {
        edge.set_flow(0);
    }

    return get_path_with_increment(start, end, numeric_limits<unsigned long>::max());
}


unsigned long Graph::get_earliest_meetup(unsigned long start, unsigned long end) {
    biggest_duration(start);

    Node &destination = nodes[end];
    if (!destination.visited || destination.in_degree > 0) {
        return 0;
    } else {
        return destination.earliest_start;
    }
}

list<pair<unsigned long, unsigned int>> Graph::get_waiting_periods(unsigned long start, unsigned long end) {
    biggest_duration(start);

    list<pair<unsigned long, unsigned int>> waiting_periods;

    for (unsigned long i = 1; i <= n; i++) {
        Node &node = nodes[i];

        unsigned long earliest_arrival = node.earliest_start;

        for (unsigned long e : node.incoming) {
            const Edge &edge = edges[e];

            if (edge.get_flow() == 0) {
                continue;
            }

            const Node &previous = nodes[edge.get_origin()];
            unsigned long arrival = previous.earliest_start + edge.get_duration();
            if (arrival < earliest_arrival) {
                earliest_arrival = arrival;
            }
        }

        if (earliest_arrival < node.earliest_start) {
            unsigned long wait = node.earliest_start - earliest_arrival;
            waiting_periods.push_back({ i, wait });
        }
    }

    return waiting_periods;
}

