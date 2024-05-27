#ifndef BINARYSEARCHTREE_HPP
#define BINARYSEARCHTREE_HPP
#include <algorithm>
#include <utility>
#include <functional>
#include <iterator>
#include <cassert>
#include <memory>

#include <iostream> //

//может быть updateHeight не надо использовать?
// и height_ тоже??

namespace kovshikov
{
  template< typename Key, typename Value, typename Compare = std::less< Key > >
 // class BinarySearchTree
  class Tree
  {
  public:

    Tree();
    Tree(const Tree& tree);
    Tree(Tree&& tree);

    ~Tree();

    class Node;
    class Iterator;
    class ConstIterator;

    using Pair = std::pair< Key, Value >;
    using Range = std::pair< Iterator, Iterator >;
    Compare comp;

    bool isEmpty() const noexcept;
    size_t getSize() const noexcept;

    Iterator find(const Key& key) const;
    size_t count(const Key& key) const;
    Range getEqualRange(const Key& key) const;

    void insert(const Key& key, const Value& value); //have Error
    void insert(Pair& pair);
    Value& operator[](const Key& key);
    Value& at(const Key& key);

    void clear();
    size_t erase(const Key& key);
    Iterator erase(Iterator iterator);

    void swap(Tree& other);

    Iterator end() const noexcept;
    Iterator begin() const noexcept;

    ConstIterator cend() const noexcept;
    ConstIterator cbegin() const noexcept;

  private:
    Node* root_;

    size_t getHeight(Node* node);
    long long getDifference(Node* node);
    void updateHeight(Node* node);
    void RightRight(Node* node);
    void LeftLeft(Node* node);

    Node* checkBalance(Node* node);
    void balance(Node* node);

    void clear(Node* node);
  };
 // using Tree = BinarySearchTree;
}


template< typename Key, typename Value, typename Compare >
class kovshikov::Tree< Key, Value, Compare >::Node
{
public:
  friend class Tree< Key, Value, Compare >;
  Node(Key key, Value value, Node* father = nullptr, size_t height = 0, Node* left = nullptr, Node* right = nullptr);

private:
  Node* left_;
  Node* right_;
  Node* father_;
  size_t height_;
  Pair element_;
};

template< typename Key, typename Value, typename Compare >
kovshikov::Tree< Key, Value, Compare >::Node::Node(Key key, Value value, Node* father, size_t height, Node* left, Node* right):
  left_(left),
  right_(right),
  father_(father),
  height_(height),
  element_(std::make_pair(key, value))
{
  std::cout << "Node()\n"; //
}

template< typename Key, typename Value, typename Compare >
class kovshikov::Tree< Key, Value, Compare >::Iterator : public std::iterator< std::bidirectional_iterator_tag, Pair >
{
public:

  friend class Tree< Key, Value, Compare >;
  using this_t = Iterator;

  Iterator(): node_(nullptr), root_(nullptr) {};
  Iterator(Node* node, Node* root): node_(node), root_(root) {};
  Iterator(const this_t&) = default;
  ~Iterator() = default;

  this_t& operator=(const this_t&) = default;

  this_t& operator++();
  this_t operator++(int);

  this_t& operator--();
  this_t operator--(int);

  bool operator==(const this_t& other) const;
  bool operator!=(const this_t& other) const;

  Pair& operator*() const;
  Pair* operator->() const;

private:
  Node* node_;
  Node* root_;
};

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator::this_t& kovshikov::Tree< Key, Value, Compare >::Iterator::operator++()
{
  assert(node_ != nullptr);
  Node* current = node_;
  if(current -> right)
  {
    current = current -> right;
    while(current -> left)
    {
      current = current -> left;
    }
  }
  else
  {
    Node* currentFather = current -> father;
    while(currentFather && current == currentFather -> right)
    {
      current = currentFather;
      currentFather = currentFather -> father;
    }
    current = currentFather;
  }
  node_ = current;
  return *this;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator::this_t kovshikov::Tree< Key, Value, Compare >::Iterator::operator++(int)
{
  assert(node_ != nullptr);
  this_t currentIterator = *this;
  ++(*this);
  return currentIterator;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator::this_t& kovshikov::Tree< Key, Value, Compare >::Iterator::operator--()
{
  assert(node_ != nullptr);
  Node* current = node_;
  if(current -> left)
  {
    current = current -> left;
    while(current -> right)
    {
      current = current -> right;
    }
  }
  else
  {
    Node* currentFather = current -> father;
    while(currentFather && current == currentFather -> left)
    {
      current = currentFather;
      currentFather = currentFather -> father;
    }
    current = currentFather;
  }
  node_ = current;
  return *this;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator::this_t kovshikov::Tree< Key, Value, Compare >::Iterator::operator--(int)
{
  assert(node_ != nullptr);
  this_t currentIterator = *this;
  --(*this);
  return currentIterator;
}

template< typename Key, typename Value, typename Compare >
bool kovshikov::Tree< Key, Value, Compare >::Iterator::operator==(const this_t& other) const
{
  return node_ == other.node_;
}

template< typename Key, typename Value, typename Compare >
bool kovshikov::Tree< Key, Value, Compare >::Iterator::operator!=(const this_t& other) const
{
  return !(*this == other);
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Pair& kovshikov::Tree< Key, Value, Compare >::Iterator::operator*() const
{
  assert(node_ != nullptr);
  return node_ -> element_;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Pair* kovshikov::Tree< Key, Value, Compare >::Iterator::operator->() const
{
  assert(node_ != nullptr);
  return std::addressof(node_ -> element_);
}

template< typename Key, typename Value, typename Compare >
class kovshikov::Tree< Key, Value, Compare >::ConstIterator : public std::iterator< std::bidirectional_iterator_tag, Pair >
{
public:

  friend class Tree< Key, Value, Compare >;
  using this_t = ConstIterator;

  ConstIterator(): iterator_(Iterator()) {};
  ConstIterator(Iterator iterator): iterator_(iterator) {};
  ConstIterator(const this_t&) = default;
  ~ConstIterator() = default;

  this_t& operator=(const this_t&) = default;

  this_t& operator++();
  this_t operator++(int);

  this_t& operator--();
  this_t operator--(int);

  bool operator==(const this_t& other) const;
  bool operator!=(const this_t& other) const;

  const Pair& operator*() const;
  const Pair* operator->() const;

private:
  Iterator iterator_;
};

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::ConstIterator& kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator++()
{
  assert(iterator_.node_ != nullptr);
  iterator_++;
  return *this;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::ConstIterator kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator++(int)
{
  assert(iterator_.node_ != nullptr);
  ++iterator_;
  return *this;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::ConstIterator& kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator--()
{
  assert(iterator_.node_ != nullptr);
  iterator_--;
  return *this;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::ConstIterator kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator--(int)
{
  assert(iterator_.node_ != nullptr);
  --iterator_;
  return *this;
}

template< typename Key, typename Value, typename Compare >
bool kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator==(const this_t& other) const
{
  return iterator_ == other.iterator_;
}

template< typename Key, typename Value, typename Compare >
bool kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator!=(const this_t& other) const
{
  return !(*this == other);
}

template< typename Key, typename Value, typename Compare >
const typename kovshikov::Tree< Key, Value, Compare >::Pair& kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator*() const
{
  assert(iterator_.node_ != nullptr);
  return iterator_.node_ -> element_;
}

template< typename Key, typename Value, typename Compare >
const typename kovshikov::Tree< Key, Value, Compare >::Pair* kovshikov::Tree< Key, Value, Compare >::ConstIterator::operator->() const
{
  assert(iterator_.node_ != nullptr);
  return std::addressof(iterator_.node_ -> element_);
}

template< typename Key, typename Value, typename Compare >
kovshikov::Tree< Key, Value, Compare >::Tree():
  comp(Compare()),
  root_(nullptr)
{
  std::cout << "Tree()\n";
}

template< typename Key, typename Value, typename Compare >
kovshikov::Tree< Key, Value, Compare >::Tree(const Tree& tree)
{
  root_ = nullptr;
  comp = tree.comp_;
  Iterator iterator = tree.begin();
  while(iterator != tree.end())
  {
    insert(iterator -> first, iterator -> second);
    iterator++;
  }
}

template< typename Key, typename Value, typename Compare >
kovshikov::Tree< Key, Value, Compare >::Tree(Tree&& tree):
  root_(tree.root_),
  comp(std::move(tree.comp))
{
  tree.root_ = nullptr;
}

template< typename Key, typename Value, typename Compare >
kovshikov::Tree< Key, Value, Compare >::~Tree()
{
  clear();
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator kovshikov::Tree< Key, Value, Compare >::end() const noexcept
{
  Node* current = root_;
  if(current == nullptr)
  {
    return Iterator(nullptr, nullptr);
  }
  while(current != nullptr)
  {
    current = current -> right_;
  }
  current += 1;
  return Iterator(current, root_);
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator kovshikov::Tree< Key, Value, Compare >::begin() const noexcept
{
  Node* current = root_;
  if(current == nullptr)
  {
    return end();
  }
  while(current != nullptr)
  {
    current = current -> left_;
  }
  return Iterator(current, root_);
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::ConstIterator kovshikov::Tree< Key, Value, Compare >::cend() const noexcept
{
  Iterator iterator = this->end();
  return ConstIterator(iterator);
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::ConstIterator kovshikov::Tree< Key, Value, Compare >::cbegin() const noexcept
{
  Iterator iterator = this->begin();
  return ConstIterator(iterator);
}

template< typename Key, typename Value, typename Compare >
bool kovshikov::Tree< Key, Value, Compare >::isEmpty() const noexcept
{
  return (root_ == nullptr) ? true : false;
}

template< typename Key, typename Value, typename Compare >
size_t kovshikov::Tree< Key, Value, Compare >::getSize() const noexcept
{
  ConstIterator cstart = this->cbegin();
  ConstIterator cfinish = this->cend();
  size_t size = 0;
  while(cstart != cfinish)
  {
    size += 1;
  }
  return size;
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator kovshikov::Tree< Key, Value, Compare >::find(const Key& key) const
{
  Node* current = root_;
  while(current != nullptr)
  {
    if(current -> element_.first == key)
    {
      return Iterator(current, root_);
    }
    else if(comp(key, current -> element_.first))
    {
      current = current -> left_;
    }
    else
    {
      current = current -> right_;
    }
  }
  return end();
}

template< typename Key, typename Value, typename Compare >
size_t kovshikov::Tree< Key, Value, Compare >::count(const Key& key) const
{
  if(find(key) == end())
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Range kovshikov::Tree< Key, Value, Compare >::getEqualRange(const Key& key) const
{
  if(find(key) != begin())
  {
    Iterator start = find(key);
    Iterator finish = start + 1;
    return std::make_pair(start, finish);
  }
  else
  {
    return std::make_pair(end(), end());
  }
}

template< typename Key, typename Value, typename Compare >
size_t kovshikov::Tree< Key, Value, Compare >::getHeight(Node* node)
{
  std::cout << "START GETHEIGHT\n"; //
  if(node == nullptr)
  {
    return 0;
  }
  return std::max(getHeight(node -> left_), getHeight(node -> right_)) + 1;
}

template< typename Key, typename Value, typename Compare >
long long kovshikov::Tree< Key, Value, Compare >::getDifference(Node* node)
{
  std::cout << "START GETDIFFERENCE\n"; //
  return getHeight(node -> left_) - getHeight(node -> right_);
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::updateHeight(Node* node)
{
  std::cout << "START UPDATE\n"; //
  while(node != nullptr)
  {
    node -> height_ = std::max(getHeight(node -> left_), getHeight(node -> right_)) + 1;
    node = node -> father_;
  }
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::RightRight(Node* node)
{
  std::cout << "START RIGHTRIGHT\n"; //
  Node* bigFather = node -> father_;
  Node* newFather = node -> left_;
  Node* lastRight = newFather -> right_;

  node -> father_ = newFather;
  if(bigFather != nullptr)
  {
    if(comp(node -> element_.first, bigFather -> element_.first))
    {
      bigFather -> left_ = newFather;
    }
    else
    {
      bigFather -> right_ = newFather;
    }
  }
  else
  {
    root_ = newFather;
  }
  newFather -> father_ = bigFather;
  newFather -> right_ = node;
  node ->left_ = lastRight; //странно
  if(lastRight != nullptr)
  {
    lastRight -> father_ = node; //nullptr
  }
  updateHeight(node);
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::LeftLeft(Node* node)
{
  std::cout << "START LEFTLEFT\n"; //
  Node* bigFather = node -> father_;
  Node* newFather = node -> right_;
  Node* lastLeft = newFather -> left_;

  if(bigFather != nullptr)
  {
    node -> father_ = newFather;
    if(comp(node -> element_.first, bigFather -> element_.first))
    {
      bigFather -> left_ = newFather;
    }
    else
    {
      bigFather -> right_ = newFather;
    }
  }
  else
  {
    root_ = newFather;
  }

  newFather -> father_ = bigFather;
  newFather -> left_ = node;
  node -> right_ = lastLeft;
  if(lastLeft != nullptr)
  {
    lastLeft -> father_ = node; //проверка на nullptr;
  }
  updateHeight(node);
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Node* kovshikov::Tree< Key, Value, Compare >::checkBalance(Node* node)
{
  std::cout << "START CHECKBALANCE\n"; //
  while(node != nullptr)
  {
    if(std::abs(getDifference(node)) > 1)
    {
      return node;
    }
    else
    {
      node = node -> father_;
    }
  }
  std::cout << "FINISH CHECKBALANCE\n"; //
  return node;
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::balance(Node* node)
{
  std::cout << "START BALANCE\n"; //
  Node* toBalance = node; //
  bool isBalance = false;
  while(isBalance == false)
  {
    toBalance = checkBalance(toBalance); // заменил node на checkBalance и переприсваивал
    if(toBalance == nullptr)
    {
      isBalance = true;
    }
    else
    {
      if(getDifference(toBalance) < -1 && getDifference(toBalance -> right_) <= 0)
      {
        LeftLeft(toBalance);
      }
      else if(getDifference(toBalance) > 1 && getDifference(toBalance -> left_) >= 0)
      {
        RightRight(toBalance);
      }
      else if(getDifference(toBalance) < -1 && getDifference(toBalance -> right_) > 0)
      {
        RightRight(toBalance -> right_);
        LeftLeft(toBalance);
      }
      else if(getDifference(toBalance) > 1 && getDifference(toBalance -> left_) < 0)
      {
        LeftLeft(toBalance -> left_);
        RightRight(toBalance);
      }
      toBalance = toBalance -> father_;
    }
  }
  std::cout << "FINISH BALANCE\n"; //
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::insert(const Key& key, const Value& value)
{
  std::cout << "INSERT\n"; //
  if(find(key) == end())
  {
    Node* newNode = new Node(key, value);
    if(isEmpty())
    {
      root_ = newNode;
      std::cout << "GET ROOT\n"; //
    }
    else
    {
      std::cout << "NO ROOT\n"; //
      Node* current = root_;
      Node* father = nullptr;
      while(current != nullptr)
      {
        father = current;
        if(comp(key, current -> element_.first))
        {
          current = current -> left_;
        }
        else
        {
          current = current -> right_;
        }
      }
      newNode -> father_ = father;
      if(comp(key, father -> element_.first))
      {
        father -> left_ = newNode;
      }
      else
      {
        father -> right_ = newNode;
      }
    }
    std::cout << "INSERT2\n"; //
    balance(newNode); //можно засунуть в else
    std::cout << "END INSERT\n"; //
  }
  std::cout << "END END\n"; //
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::insert(Pair& pair)
{
  insert(pair.first, pair.second);
}

template< typename Key, typename Value, typename Compare >
Value& kovshikov::Tree< Key, Value, Compare >::operator[](const Key& key)
{
  if(find(key) == end())
  {
    insert(key, Value());
    return Value();
  }
  else
  {
    return find(key) -> second;
  }
}

template< typename Key, typename Value, typename Compare >
Value& kovshikov::Tree< Key, Value, Compare >::at(const Key& key)
{
  if(find(key) == end())
  {
    throw std::out_of_range("");
  }
  else
  {
    return find(key) -> second;
  }
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::clear(Node* node)
{
  if(node != nullptr)
  {
    clear(node -> left_);
    clear(node -> right_);
    delete node;
  }
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::clear()
{
  if(!isEmpty())
  {
    clear(root_);
  }
  root_ = nullptr;
}

template< typename Key, typename Value, typename Compare >
void kovshikov::Tree< Key, Value, Compare >::swap(Tree& other)
{
  std::swap(comp, other.comp);
  std::swap(root_, other.root_);
}

template< typename Key, typename Value, typename Compare >
typename kovshikov::Tree< Key, Value, Compare >::Iterator kovshikov::Tree< Key, Value, Compare >::erase(Iterator iterator)
{
  Node* toDelete = iterator.node_;
  Node* bigFather = toDelete -> father;
  Node* changer = nullptr;
  Node* toBalance = nullptr;
  if(toDelete == nullptr)
  {
    return end();
  }
  if(toDelete -> left_ == nullptr && toDelete -> right_ == nullptr)
  {
    if(bigFather != nullptr)
    {
      delete toDelete;
      balance(bigFather);
      return Iterator(bigFather, root_);
    }
    else
    {
      delete toDelete;
      return end();
    }
  }
  else if(toDelete -> left_ == nullptr && toDelete -> right_ != nullptr)
  {
    changer = toDelete -> right_;
    while(changer != nullptr)
    {
      changer = changer -> left_;
    }
    toBalance = changer -> father_;
  }
  else
  {
    changer = toDelete -> left_;
    while(changer != nullptr)
    {
      changer = changer -> right_;
    }
    toBalance - changer -> father_;
  }
  if(comp(toDelete -> element_.first, bigFather -> element_.first))
  {
    bigFather -> left_ = changer;
  }
  else
  {
    bigFather -> right_ = changer;
  }
  changer -> father_ = bigFather;
  changer -> left_ = toDelete -> left_;
  changer -> right_ = toDelete -> right_;
  toDelete -> left_ -> father_ = changer;
  toDelete -> right_ -> father_ = changer;

  delete toDelete;
  updateHeight(root_);
  balance(toBalance);
  return Iterator(changer, root_);
}

template< typename Key, typename Value, typename Compare >
size_t kovshikov::Tree< Key, Value, Compare >::erase(const Key& key)
{
  Iterator iterator = find(key);
  if(iterator == end())
  {
    return 0;
  }
  else
  {
    erase(iterator);
    return 1;
  }
}

#endif
