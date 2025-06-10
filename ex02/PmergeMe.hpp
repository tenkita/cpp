
#pragma once
#ifndef __PMERGEME_HPP__
#define __PMERGEME_HPP__

#include <iostream>
#include <list>
#include <vector>

unsigned long getTime();
unsigned long jacobsthal(unsigned long);

void PmergeMe(int *data, std::size_t size);

template <typename T> class Node {
private:
  bool _isLeaf;
  union Value {
    T *leaf;
    Node<T> *inner;
  } _value;
  Node<T> *_pair;

public:
  Node(T *value = NULL) : _isLeaf(true), _pair(NULL) {
    _value.leaf = value;
  };
  Node(Node<T> *value) : _isLeaf(false), _pair(NULL) {
    _value.inner = value;
  };
  Node(Node<T> const &rhs) : _isLeaf(rhs._isLeaf), _pair(rhs._pair) {
    if (rhs._isLeaf)
      _value.leaf = rhs._value.leaf;
    else
      _value.inner = rhs._value.inner;
  };
  ~Node() {};
  Node<T> &operator=(Node<T> const &rhs) {
    if (this != &rhs) {
      _isLeaf = rhs._isLeaf;
      _pair = rhs._pair;
      if (rhs._isLeaf)
        _value.leaf = rhs._value.leaf;
      else
        _value.inner = rhs._value.inner;
    }
    return *this;
  };
  bool hasPair() const {
    return _pair != NULL;
  };
  void push(Node<T> *node) {
    _pair = node;
  };
  Node<T> *pop() {
    Node<T> *tmp = _pair;
    _pair = NULL;
    return tmp;
  };
  T &getTypical() const {
    if (_isLeaf)
      return *_value.leaf;
    else
      return _value.inner->getTypical();
  };
  bool operator<(Node<T> const &rhs) const {
    return getTypical() < rhs.getTypical();
  };

  void getValue(T *&value) {
    value = _value.leaf;
  }
  void getValue(Node<T> *&value) {
    value = _value.inner;
  }
};

template <typename T> struct TypeSelector {
  typedef T type;
};

template <typename T> struct TypeSelector<Node<T> > {
  typedef T type;
};

template <typename ConstIterator>
void printData(ConstIterator begin, ConstIterator end) {
  if (begin != end) {
    std::cout << *begin;
    while (++begin != end)
      std::cout << " " << *begin;
  }
  std::cout << std::endl;
}

#endif
