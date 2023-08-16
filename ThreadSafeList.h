#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

template <typename T>
class List {
public:
    // Constructor
    List();

    // Destructor
    ~List();

    class Node {
    public:
        T data;
        Node *next;
        pthread_mutex_t mutex;

        explicit Node(const T& data, Node* next = nullptr);
        ~Node();

        void lock();
        void unlock();
    };

    /**
    * Insert new node to list while keeping the list ordered in an ascending order
    * If there is already a node has the same data as @param data then return false (without adding it again)
    * @param data the new data to be added to the list
    * @return true if a new node was added and false otherwise
    */
    bool insert(const T& data);

    /**
    * Remove the node that its data equals to @param value
    * @param value the data to lookup a node that has the same data to be removed
    * @return true if a matched node was found and removed and false otherwise
    */
    bool remove(const T& value);

    /**
    * Returns the current size of the list
    * @return current size of the list
    */
    unsigned int getSize();

    // Don't remove
    void print() {
        Node* temp = dummy_head->next;
        if (temp == NULL)
        {
            cout << "";
        }
        else if (temp->next == NULL)
        {
            cout << temp->data;
        }
        else
        {
            while (temp != NULL)
            {
              cout << right << setw(3) << temp->data;
              temp = temp->next;
              cout << " ";
            }
        }
        cout << endl;
    }

    // Don't remove
    virtual void __insert_test_hook() {}
    // Don't remove
    virtual void __remove_test_hook() {}

private:
    Node* dummy_head;
    unsigned int size;
    pthread_mutex_t access_size_mutex;

    void incrementSize();
    void decrementSize();
};

template<typename T>
List<T>::Node::Node(const T& data, Node* next)
    : data(data), next(next) {
    pthread_mutex_init(&mutex, NULL);
}

template<typename T>
List<T>::Node::~Node() {
    pthread_mutex_destroy(&mutex);
}

template<typename T>
void List<T>::Node::lock() {
    pthread_mutex_lock(&mutex);
}

template<typename T>
void List<T>::Node::unlock() {
    pthread_mutex_unlock(&mutex);
}

template<typename T>
List<T>::List()
    : dummy_head(nullptr), size(0) {
    try {
        dummy_head = new Node(0, nullptr);
    } catch (const std::bad_alloc& e) {
        std::cerr << "new:failed" << std::endl;
        exit(-1);
    }
    if (pthread_mutex_init(&access_size_mutex, NULL) != 0) {
        std::cerr << "pthread_mutex_init:failed" << std::endl;
        exit(-1);
    }
}

template<typename T>
List<T>::~List() {
    pthread_mutex_destroy(&access_size_mutex);
    Node* current = dummy_head;
    while(current != nullptr) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }
}

template<typename T>
bool List<T>::insert(const T &data) {
    Node* pred = dummy_head;
    dummy_head->lock();
    Node* current = dummy_head->next;
    if (current != nullptr) {
        current->lock();
    }

    while(current != nullptr) {
        if (current->data == data) {
            // found a node with the same data => can't insert
            current->unlock();
            pred->unlock();
            return false;
        }
        if (current->data > data) {
            // insertion should be between pred and current
            break;
        }
        // otherwise, current data < data => continue traversing the list
        Node* old_pred = pred;
        pred = current;
        current = current->next;
        old_pred->unlock();
        if (current != nullptr) {
            current->lock();
        }
    }

    pred->next = new Node(data, current);
    incrementSize();
    __insert_test_hook();
    if (current != nullptr) {
        current->unlock();
    }
    pred->unlock();

    return true;
}

template<typename T>
bool List<T>::remove(const T &value) {
    Node* pred = dummy_head;
    dummy_head->lock();
    Node* current = pred->next;
    if (current != nullptr) {
        current->lock();
    }

    while (current != nullptr && current->data <= value) {
        if (current->data == value) {
            pred->next = current->next;
            __remove_test_hook();
            current->unlock();
            pred->unlock();
            decrementSize();
            delete current;
            return true;
        }
        // the current node's data < value
        Node* old_pred = pred;
        pred = current;
        current = current->next;
        old_pred->unlock();
        if (current != nullptr) {
            current->lock();
        }
    }

    if (current != nullptr) {
        current->unlock();
    }
    pred->unlock();

    return false;
}

template<typename T>
unsigned int List<T>::getSize() {
    unsigned int curr_size;
    pthread_mutex_lock(&access_size_mutex);
    curr_size = size;
    pthread_mutex_unlock(&access_size_mutex);
    return curr_size;
}

template<typename T>
void List<T>::incrementSize() {
    pthread_mutex_lock(&access_size_mutex);
    size++;
    pthread_mutex_unlock(&access_size_mutex);
}

template<typename T>
void List<T>::decrementSize() {
    pthread_mutex_lock(&access_size_mutex);
    size--;
    pthread_mutex_unlock(&access_size_mutex);
}

#endif //THREAD_SAFE_LIST_H_