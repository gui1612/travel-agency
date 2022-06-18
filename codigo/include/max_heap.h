#pragma once

#include <vector>
#include <unordered_map>

#define LEFT(i) (2*(i))
#define RIGHT(i) (2*(i)+1)
#define PARENT(i) ((i)/2)

using namespace std;

// Binary min-heap to represent integer keys of type K with values (priorities) of type V
template <class K, class V>
class MaxHeap {
    struct Node { // An element of the heap: a pair (key, value)
        K key;
        V value;
    };

    int size;                  // Number of elements in heap
    int max_size;               // Maximum number of elements in heap
    vector<Node> a;            // The heap array
    unordered_map<K, int> pos; // maps a key into its position on the array a
    const K KEY_NOT_FOUND;

    void up_heap(int i);
    void down_heap(int i);
    void swap(int i1, int i2);

public:
    MaxHeap(int n, const K& not_found); // Create a min-heap for a max of n pairs (K,V) with notFound returned when empty
    int get_size();              // Return number of elements in the heap
    bool has_key(const K& key);  // Heap has key?
    void insert(const K& key, const V& value);      // Insert (key, value) on the heap
    void increase_key(const K& key, const V& value); // Decrease value of key
    K remove_max(); // remove and return key with smaller value
};

// ----------------------------------------------

// Make a value go "up the tree" until it reaches its position
template <class K, class V>
void MaxHeap<K,V>::up_heap(int i) {
    while (i>1 && a[i].value > a[PARENT(i)].value) { // while pos bigger than parent, keep swapping to upper position
        swap(i, PARENT(i));
        i = PARENT(i);
    }
}

// Make a value go "down the tree" until it reaches its position
template <class K, class V>
void MaxHeap<K,V>::down_heap(int i) {
    while (LEFT(i) <= size) { // while within heap limits
        int j = LEFT(i);
        if (RIGHT(i)<=size && a[RIGHT(i)].value > a[j].value) j = RIGHT(i); // choose smaller child
        if (a[i].value > a[j].value) break;   // node already smaller than children, stop
        swap(i, j);                    // otherwise, swap with smaller child
        i = j;
    }
}

// Swap two positions of the heap (update their positions)
template <class K, class V>
void MaxHeap<K,V>::swap(int i1, int i2) {
    Node tmp = a[i1]; a[i1] = a[i2]; a[i2] = tmp;
    pos[a[i1].key] = i1;
    pos[a[i2].key] = i2;
}

// ----------------------------------------------

// Create a min-heap for a max of n pairs (K,V) with notFound returned when empty
template <class K, class V>
MaxHeap<K,V>::MaxHeap(int n, const K& notFound) : KEY_NOT_FOUND(notFound), size(0), max_size(n), a(n+1) {
}

// Return number of elements in the heap
template <class K, class V>
int MaxHeap<K,V>::get_size() {
    return size;
}

// Heap has key?
template <class K, class V>
bool MaxHeap<K, V>::has_key(const K& key) {
    return pos.find(key) != pos.end();
}

// Insert (key, value) on the heap
template <class K, class V>
void MaxHeap<K,V>::insert(const K& key, const V& value) {
    if (size == max_size) return; // heap is full, do nothing
    if (has_key(key)) return;     // key already exists, do nothing
    a[++size] = {key, value};
    pos[key] = size;
    up_heap(size);
}

// Decrease value of key to the indicated value
template <class K, class V>
void MaxHeap<K,V>::increase_key(const K& key, const V& value) {
    if (!has_key(key)) return; // key does not exist, do nothing
    int i = pos[key];
    if (value < a[i].value) return; // value would decrease, do nothing
    a[i].value = value;
    up_heap(i);
}

// remove and return key with smaller value
template <class K, class V>
K MaxHeap<K,V>::remove_max() {
    if (size == 0) return KEY_NOT_FOUND;
    K max = a[1].key;
    pos.erase(max);
    a[1] = a[size--];
    down_heap(1);
    return max;
}
