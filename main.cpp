#include <atomic>
#include <iostream>
#include <memory>
using namespace std;
template <typename T>
struct node {
    T data;
    node* next;
    node(const T& data) : data(data), next(nullptr) {}
};

template <typename T>
class queue {
    atomic<node<T>*> head;
    atomic<node<T>*> tail;

public:
    void push(const T& data) {
        node<T>* const new_node = new node<T>(data);

        node<T>* old_tail = tail.load(std::memory_order_acquire);
        if (!old_tail) {
            head.store(new_node);
            tail.store(new_node);
        } else {
            old_tail->next = new_node; 
            while (!tail.compare_exchange_weak(
                old_tail, new_node, memory_order_release, memory_order_relaxed)) {
                old_tail = tail.load();
            }
        }
        cout << "Pushed Node with Value: " << new_node->data << endl;
    }

    void pop() {
        node<T>* old_head = head.load();
        if (!old_head) {
            cout << "No Nodes in Queue" << std::endl;
            return;
        }

        node<T>* new_head = old_head->next;
        while (!head.compare_exchange_weak(
            old_head, new_head, memory_order_release, memory_order_relaxed)) {
            old_head = head.load();
        }

        cout << "Popped Node with Value: " << old_head->data << endl;
        delete old_head;
    }
};

int main() {
    queue<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    s.pop();
    s.pop();
    s.pop();
    s.pop();
}
