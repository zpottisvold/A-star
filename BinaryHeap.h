#pragma once
#include <iostream>
#include <vector>

template <typename T>
class BinaryHeap {
private:
  // Binary heap with elements of pair<T, ID>.
  // T must be a pointer to a struct or class with an integer member weight.
  // Weight will be used to sort the heap.
  std::vector<std::pair<T, unsigned>> heap;
  // Array that maps ID -> heapIndex. Must also be kept in sync.
  // This makes checking if an ID is in the heap O(1).
  std::vector<int> mappingArray;
  // Use avalableIdsList to reuse IDs of removed nodes.
  // Will not be kept properly updated if the given T nodes have their own IDs.
  std::vector<int> availableIdsList;


  int getNewId();
  // - complexity: O(log(n))
  void SiftUp(int index);
  // - complexity: O(log(n))
  void SiftDown(int index);
  // - complexity: O(1)
  void Swap(int index1, int index2);

public:
  BinaryHeap();
  // - complexity: O(N)
  BinaryHeap(std::vector<T>);
  ~BinaryHeap();

  // - complexity: O(1)
  T PeekAtTop();
  // - complexity: O(1)
  T GetNode(unsigned nodeId);
  // - complexity: O(1), thanks to the mappingArray.
  bool Search(unsigned nodeId);

  // This Insert function function uses the ID of the given struct/class node if possible,
  // creates new ID's otherwise.
  // Mixing nodes with their own ID memers and nodes without is not recommended.
  // Using the given struct/class node's ID might cause the mappingArray to grow very big
  // while only containing a few elements.
  // - complexity: O(log(n)), bounded by SiftUp.
  void Insert(T node);
  // - complexity: O(log(n)), bounded by SiftDown.
  void Remove(int nodeId);

  // Prints the heap as a list and with every element representing a node's ID.
  void Display();

  // - complexity: O(log(n)), bounded by SiftUp.
  // If the key, or weight, is already smaller than or equal to val; nothing will be done.
  void DecreaseKey(unsigned nodeId, int val);
  // - complexity: O(log(n)), bounded by SiftDown.
  // If the key, or weight, is already bigger than or equal to val; nothing will be done.
  void IncreaseKey(unsigned nodeId, int val);

};


// #####################################################################
// The implementation
// #####################################################################

template<typename T>
BinaryHeap<T>::BinaryHeap() {

}

template<typename T>
BinaryHeap<T>::BinaryHeap(std::vector<T> nodes) {
  for (unsigned i = 0; i < nodes.size(); ++i) {
    Insert(nodes[i]);
  }
}

template<typename T>
BinaryHeap<T>::~BinaryHeap() {

}

template<typename T>
T BinaryHeap<T>::GetNode(unsigned nodeId) {
  if (mappingArray.size() <= nodeId)
    return 0;
  int index = mappingArray[nodeId];
  if (index == -1)
    return 0;
  else
    return heap[index].first;
}

template<typename T>
bool BinaryHeap<T>::Search(unsigned nodeId) {
  if (nodeId < mappingArray.size()) {
    unsigned index = mappingArray[nodeId]+1;
    if (index != 0 && index <= heap.size() && heap[index-1].first->id == nodeId) {
      return true;
    }
  }
  return false;
}

template<typename T>
int BinaryHeap<T>::getNewId() {
  int size = availableIdsList.size();
  if (size != 0) {
    int id = availableIdsList[size-1];
    availableIdsList.pop_back();
    return id;
  }
  else {
    return mappingArray.size();
  }
}

template<typename T>
void BinaryHeap<T>::Insert(T node) {
  int newId;
  try {
    newId = node->id;
  }
  catch(const std::exception& e) {
    newId = getNewId();
  }
  std::pair<T, int> heapElement = {node, newId};
  heap.push_back(heapElement);

  int lastIndex = heap.size()-1;
  if (newId >= mappingArray.size())
    mappingArray.resize(newId+1);

  mappingArray[newId] = lastIndex;

  SiftUp(lastIndex);
}

template<typename T>
void BinaryHeap<T>::SiftUp(int index) {
  while(heap[index].first->weight < heap[index/2].first->weight) {
    Swap(index, index/2);
    index = index/2;
  }
}

template<typename T>
void BinaryHeap<T>::SiftDown(int index) {
  while(true) {
  // std::cout << heap.size() << std::endl;
    int nodeValue = heap[index].first->weight;
    int child1Value = (2*index+2 < heap.size()) ? heap[2*index+1].first->weight : 9999999;
    int child2Value = (2*index+2 < heap.size()) ? heap[2*index+2].first->weight : 9999999;
    if (child1Value < child2Value && child1Value < nodeValue) {
      Swap(index, 2*index+1);
      index = 2*index+1;
    }
    else if (child2Value < nodeValue) {
      Swap(index, 2*index+2);
      index = 2*index+2;
    }
    else
      break;
  }
}

template<typename T>
void BinaryHeap<T>::Remove(int nodeId) {
  int index = mappingArray[nodeId];
  Swap(index, heap.size()-1);
  heap.pop_back();
  // std::cout << "rem" << mappingArray.size() << std::endl;
  mappingArray[nodeId] = -1;
  // std::cout << "rem" << mappingArray.size() << std::endl;
  availableIdsList.push_back(nodeId);

  SiftDown(index);
  // std::cout << "rem" << mappingArray.size() << std::endl;
}

template<typename T>
void BinaryHeap<T>::Swap(int index1, int index2) {
  mappingArray[heap[index1].second] = index2;
  mappingArray[heap[index2].second] = index1;

  std::pair<T, int> temp = heap[index1];
  heap[index1] = heap[index2];
  heap[index2] = temp;

}

template<typename T>
void BinaryHeap<T>::Display() {
  if (!heap.size()) {
    std::cout << "heap is empty." << std::endl;
    return;
  }
  for (int i = 0; i < heap.size(); ++i) {
    std::cout << heap[i].second;
    if (i != heap.size()-1) {
      std::cout << ", ";
    }
    if ((i+1 % 8 == 0)) {
      std::cout << std::endl;
    }
  }
  std::cout  << std::endl;
}

template<typename T>
T BinaryHeap<T>::PeekAtTop() {
  if (!heap.size())
    return 0;
  return heap.front().first;
}

template<typename T>
void BinaryHeap<T>::DecreaseKey(unsigned nodeId, int value) {
  int index = mappingArray[nodeId];
  if (heap[index].first->weight <= value)
    return;
  heap[index].first->weight = value;
  SiftUp(index);
}

template<typename T>
void BinaryHeap<T>::IncreaseKey(unsigned nodeId, int value) {
  int index = mappingArray[nodeId];
  if (heap[index].first->weight >= value)
    return;
  heap[index].first->weight = value;
  SiftDown(index);
}
