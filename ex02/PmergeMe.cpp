// PmergeMe.cpp
#include "PmergeMe.hpp"

#include <iostream>
#include <iomanip>
#include <list>
#include <vector>
#include <sys/time.h>
#include <algorithm> // for std::lower_bound

// Global counter for comparisons during insertions
static unsigned long comparisonCount = 0;

// Functor to wrap comparisons and increment the counter
template <typename NodeType>
struct CompareNode
{
  bool operator()(NodeType const &a, NodeType const &b) const
  {
    ++comparisonCount;
    return a < b;
  }
};

unsigned long getTime()
{
  timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000000 + t.tv_usec;
}

unsigned long jacobsthal(unsigned long i)
{
  static std::vector<unsigned long> buf;
  for (std::size_t k; (k = buf.size()) <= i;)
  {
    if (k == 0)
      buf.push_back(0);
    else if (k == 1)
      buf.push_back(1);
    else
    {
      unsigned long tmp;
      if (__builtin_mul_overflow(2, buf[k - 2], &tmp) ||
          __builtin_add_overflow(tmp, buf[k - 1], &tmp))
        throw std::overflow_error("jacobsthal");
      buf.push_back(tmp);
    }
  }
  return buf[i];
}

template <typename T>
void sort(std::vector<T> &data)
{
  if (data.size() < 2)
    return;
  typedef typename TypeSelector<T>::type Type;
  std::vector<Node<Type> > large, small;
  large.reserve(data.size() / 2);
  small.reserve(data.size() / 2 + data.size() % 2);

  // Pairing phase
  for (typename std::vector<T>::iterator it = data.begin(); it != data.end(); ++it)
  {
    typename std::vector<T>::iterator pre = it++;
    if (it == data.end())
    {
      small.push_back(&*pre);
      break;
    }
    bool isLess = *pre < *it;
    large.push_back(isLess ? &*it : &*pre);
    small.push_back(isLess ? &*pre : &*it);
    large.back().push(&small.back());
  }

  // Recursive sort on 'large'
  sort(large);

  // Merge-insert phase
  std::vector<Node<Type> > tmp;
  tmp.reserve(data.size());
  {
    bool finished = false;
    typename std::vector<Node<Type> >::iterator it = large.begin();
    tmp.push_back(*it->pop());
    tmp.push_back(*it);
    ++it;

    for (std::size_t n = 1; !finished; ++n)
    {
      // Take the next 2*J(n) elements from 'large'
      for (std::size_t j = 2 * jacobsthal(n); j > 0; --j)
      {
        if (it == large.end())
        {
          // If odd count, insert the leftover from 'small'
          if (data.size() % 2)
          {
            Node<Type> &node = small.back();
            tmp.insert(
                std::lower_bound(tmp.begin(), tmp.end(), node, CompareNode<Node<Type> >()),
                node);
          }
          finished = true;
          break;
        }
        tmp.push_back(*it);
        ++it;
      }
      // Now insert paired elements back into tmp
      for (typename std::vector<Node<Type> >::reverse_iterator jt = tmp.rbegin();
           jt != tmp.rend();)
      {
        if (jt->hasPair())
        {
          Node<Type> &node = *jt->pop();
          tmp.insert(
              std::lower_bound(tmp.begin(), --jt.base(), node, CompareNode<Node<Type> >()),
              node);
        }
        else
        {
          ++jt;
        }
      }
    }
  }

  // Extract sorted values
  std::vector<T> res;
  res.reserve(data.size());
  for (typename std::vector<Node<Type> >::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    T *value;
    it->getValue(value);
    res.push_back(*value);
  }
  data.swap(res);
}

template <typename T>
void sort(std::list<T> &data)
{
  if (data.size() < 2)
    return;
  typedef typename TypeSelector<T>::type Type;
  std::list<Node<Type> > large, small;

  // Pairing phase
  for (typename std::list<T>::iterator it = data.begin(); it != data.end(); ++it)
  {
    typename std::list<T>::iterator pre = it++;
    if (it == data.end())
    {
      small.push_back(&*pre);
      break;
    }
    bool isLess = *pre < *it;
    large.push_back(isLess ? &*it : &*pre);
    small.push_back(isLess ? &*pre : &*it);
    large.back().push(&small.back());
  }

  // Recursive sort on 'large'
  sort(large);

  // Merge-insert phase
  std::list<Node<Type> > tmp;
  {
    bool finished = false;
    typename std::list<Node<Type> >::iterator it = large.begin();
    tmp.push_back(*it->pop());
    tmp.push_back(*it);
    ++it;

    for (std::size_t n = 1; !finished; ++n)
    {
      // Take the next 2*J(n) elements from 'large'
      for (std::size_t j = 2 * jacobsthal(n); j > 0; --j)
      {
        if (it == large.end())
        {
          if (data.size() % 2)
          {
            Node<Type> &node = small.back();
            tmp.insert(
                std::lower_bound(tmp.begin(), tmp.end(), node, CompareNode<Node<Type> >()),
                node);
          }
          finished = true;
          break;
        }
        tmp.push_back(*it);
        ++it;
      }
      // Now insert paired elements back into tmp
      for (typename std::list<Node<Type> >::reverse_iterator jt = tmp.rbegin();
           jt != tmp.rend(); ++jt)
      {
        if (jt->hasPair())
        {
          Node<Type> &node = *jt->pop();
          tmp.insert(
              std::lower_bound(tmp.begin(), --jt.base(), node, CompareNode<Node<Type> >()),
              node);
        }
      }
    }
  }

  // Extract sorted values
  std::list<T> res;
  for (typename std::list<Node<Type> >::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    T *value;
    it->getValue(value);
    res.push_back(*value);
  }
  data.swap(res);
}

void PmergeMe(int *data, std::size_t size)
{
  // Vector-based sort
  comparisonCount = 0;
  unsigned long start = getTime();
  std::vector<int> v1(data, data + size);
  sort(v1);
  unsigned long middle = getTime();
  unsigned long cmpVec = comparisonCount;

  // List-based sort
  comparisonCount = 0;
  std::list<int> v2(data, data + size);
  sort(v2);
  unsigned long end = getTime();
  unsigned long cmpList = comparisonCount;

  // Output
  std::cout << "Before:\t";
  printData(data, data + size);
  std::cout << "After (vector):\t";
  printData(v1.begin(), v1.end());
  // std::cout << "After (list):\t";
  // printData(v2.begin(), v2.end());

  std::cout << "Time to process a range of " << std::setw(3)
            << std::setfill(' ') << size
            << " elements with std::vector : " << (middle - start) << " us"
            << std::endl
            << "Time to process a range of " << std::setw(3)
            << std::setfill(' ') << size
            << " elements with std::list   : " << (end - middle) << " us"
            << std::endl
            << "Comparisons (vector)        : " << cmpVec << std::endl
            << "Comparisons (list)          : " << cmpList << std::endl;
}

// #include "PmergeMe.hpp"

// #include <iomanip>
// #include <list>
// #include <sys/time.h>
// #include <vector>

// unsigned long getTime() {
//   timeval t;
//   gettimeofday(&t, NULL);
//   return t.tv_sec * 1000000 + t.tv_usec;
// }

// unsigned long jacobsthal(unsigned long i) {
//   static std::vector<unsigned long> buf;
//   for (std::size_t k; (k = buf.size()) <= i;) {
//     if (k == 0)
//       buf.push_back(0);
//     else if (k == 1)
//       buf.push_back(1);
//     else {
//       unsigned long tmp;
//       if (__builtin_mul_overflow(2, buf[k - 2], &tmp) ||
//           __builtin_add_overflow(tmp, buf[k - 1], &tmp))
//         throw std::overflow_error("jacobsthal");
//       buf.push_back(tmp);
//     }
//   }
//   return buf[i];
// }

// template <typename T> void sort(std::vector<T> &data);
// template <typename T> void sort(std::list<T> &data);

// void PmergeMe(int *data, std::size_t size) {
//   unsigned long start = getTime();
//   std::vector<int> v1(data, data + size);
//   sort(v1);
//   unsigned long middle = getTime();
//   std::list<int> v2(data, data + size);
//   sort(v2);
//   unsigned long end = getTime();
//   std::cout << "Before:\t";
//   printData(data, data + size);
//   std::cout << "After:\t";
//   printData(v1.begin(), v1.end());
// //   std::cout << "After:\t";
// //   printData(v2.begin(), v2.end());
//   std::cout << "Time to process a range of " << std::setw(3)
//             << std::setfill(' ') << size
//             << " elements with std::vector : " << middle - start << " us"
//             << std::endl
//             << "Time to process a range of " << std::setw(3)
//             << std::setfill(' ') << size
//             << " elements with std::list : " << end - middle << " us"
//             << std::endl;
// }

// template <typename T> void sort(std::vector<T> &data) {
//   if (data.size() < 2)
//     return;
//   typedef typename TypeSelector<T>::type Type;
//   std::vector<Node<Type> > large, small;
//   large.reserve(data.size() / 2);
//   small.reserve(data.size() / 2 + data.size() % 2);
//   for (typename std::vector<T>::iterator it = data.begin(); it != data.end();
//        ++it) {
//     typename std::vector<T>::iterator pre = it++;
//     if (it == data.end()) {
//       small.push_back(&*pre);
//       break;
//     }
//     bool isLess = *pre < *it;
//     large.push_back(isLess ? &*it : &*pre);
//     small.push_back(isLess ? &*pre : &*it);
//     large.back().push(&small.back());
//   }
//   sort(large);
//   std::vector<Node<Type> > tmp;
//   tmp.reserve(data.size());
//   {
//     bool finished = false;
//     typename std::vector<Node<Type> >::iterator it = large.begin();
//     {
//       tmp.push_back(*it->pop());
//       tmp.push_back(*it);
//       ++it;
//     }
//     for (std::size_t n = 1; !finished; n++) {
//       for (std::size_t j = 2 * jacobsthal(n); 0 < j; j--) {
//         if (it == large.end()) {
//           if (data.size() % 2) {
//             Node<Type> &node = small.back();
//             tmp.insert(std::lower_bound(tmp.begin(), tmp.end(), node), node);
//           }
//           finished = true;
//           break;
//         }
//         tmp.push_back(*it);
//         ++it;
//       }
//       for (typename std::vector<Node<Type> >::reverse_iterator jt = tmp.rbegin();
//            jt != tmp.rend();) {
//         if (jt->hasPair()) {
//           Node<Type> &node = *jt->pop();
//           tmp.insert(std::lower_bound(tmp.begin(), --jt.base(), node), node);
//         } else
//           ++jt;
//       }
//     }
//   }
//   std::vector<T> res;
//   res.reserve(data.size());
//   for (typename std::vector<Node<Type> >::iterator it = tmp.begin();
//        it != tmp.end(); ++it) {
//     T *value;
//     it->getValue(value);
//     res.push_back(*value);
//   }
//   data.swap(res);
// }

// template <typename T> void sort(std::list<T> &data) {
//   if (data.size() < 2)
//     return;
//   typedef typename TypeSelector<T>::type Type;
//   std::list<Node<Type> > large, small;
//   for (typename std::list<T>::iterator it = data.begin(); it != data.end();
//        ++it) {
//     typename std::list<T>::iterator pre = it++;
//     if (it == data.end()) {
//       small.push_back(&*pre);
//       break;
//     }
//     bool isLess = *pre < *it;
//     large.push_back(isLess ? &*it : &*pre);
//     small.push_back(isLess ? &*pre : &*it);
//     large.back().push(&small.back());
//   }
//   sort(large);
//   std::list<Node<Type> > tmp;
//   {
//     bool finished = false;
//     typename std::list<Node<Type> >::iterator it = large.begin();
//     {
//       tmp.push_back(*it->pop());
//       tmp.push_back(*it);
//       ++it;
//     }
//     for (std::size_t n = 1; !finished; n++) {
//       for (std::size_t j = 2 * jacobsthal(n); 0 < j; j--) {
//         if (it == large.end()) {
//           if (data.size() % 2) {
//             Node<Type> &node = small.back();
//             tmp.insert(std::lower_bound(tmp.begin(), tmp.end(), node), node);
//           }
//           finished = true;
//           break;
//         }
//         tmp.push_back(*it);
//         ++it;
//       }
//       for (typename std::list<Node<Type> >::reverse_iterator jt = tmp.rbegin();
//            jt != tmp.rend();) {
//         if (jt->hasPair()) {
//           Node<Type> &node = *jt->pop();
//           tmp.insert(std::lower_bound(tmp.begin(), --jt.base(), node), node);
//         }
//         ++jt;
//       }
//     }
//   }
//   std::list<T> res;
//   for (typename std::list<Node<Type> >::iterator it = tmp.begin();
//        it != tmp.end(); ++it) {
//     T *value;
//     it->getValue(value);
//     res.push_back(*value);
//   }
//   data.swap(res);
// }

///////////////////////////////////////////////////////////////////////////

// #include <iostream>

// class Integer {
// private:
//   int _value;

// public:
//   static std::size_t count;
//   Integer(int value) : _value(value) {};
//   bool operator<(Integer const &rhs) const {
//     std::cout << ++count << ": " << _value << " vs " << rhs._value << std::endl;
//     return _value < rhs._value;
//   };
//   friend std::ostream &operator<<(std::ostream &os, Integer const &rhs) {
//     os << rhs._value;
//     return os;
//   };
// };

// std::size_t Integer::count = 0;

// int main() {
//   int data[] = {4, 2, 5, 11, 3, 7, 6, 23, 46, 1, 56, 14, 88, 34, 51, 33, 78, 22};
//   std::vector<Integer> v1(data, data + sizeof(data) / sizeof(data[0]));
//   Integer::count = 0;
//   sort(v1);
//   for (std::vector<Integer>::iterator it = v1.begin(); it != v1.end(); ++it)
//     std::cout << *it << " ";
//   std::cout << std::endl;
//   return 0;
// }
/*
21の時はどうなるか考えてみましょう。
まずペアを作ります
a a a a a a a a a a
b b b b b b b b b b b
この時比較が10回行われます。
 a a a a a
 b b b b b
 この時比較が5回行われます。
  a a
  b b b
  この時比較が2回行われます。
   a
   b
   この時比較が1回行われます。
   挿入を行います
   c c
  挿入を行います
 c c a
     b|b
 先に、余りの挿入が行われます。比較回数は2回です。
 比較回数2回の挿入が1回行われます。
c c a a a a
    b b,b b
比較回数2回の挿入が2回行われます。
比較回数3回の挿入が2回行われます。
c c a a a a a a a a a
    b b,b b,b b b b b|b
比較回数2回の挿入が2回行われます。
比較回数3回の挿入が2回行われます。
先に、余りの挿入が行われます。比較回数は4回です。
比較回数4回の挿入が5回行われます。

よって10+5+2+1+2+2+2+2+3+3+2+2+3+3+4+4+4+4+4+4=66
の比較が必要になります
*/