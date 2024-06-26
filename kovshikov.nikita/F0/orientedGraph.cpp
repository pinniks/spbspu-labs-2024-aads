#include "orientedGraph.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include "outMessage.hpp"
#include "sort.hpp"

kovshikov::Graph& kovshikov::Graph::operator=(const Graph& graph)
{
  auto current = graph.tree.begin();
  while(current != graph.tree.end())
  {
    tree.insert(*current);
    current++;
  }
  return *this;
}

kovshikov::Graph::Node& kovshikov::Graph::Node::operator=(const Node& node)
{
  auto current = node.edges.begin();
  while(current != node.edges.end())
  {
    edges.insert(*current);
    current++;
  }
  value = node.value;
  return *this;
}

void kovshikov::Graph::haveThisVertex(size_t key)
{
  if(tree.find(key) == tree.end())
  {
    throw std::logic_error("This key does not exist");
  }
}

void kovshikov::Graph::haveNot(size_t keyWho, size_t keyWith)
{
  bool haveWho = tree.find(keyWho) == tree.end();
  bool haveWith = tree.find(keyWith) == tree.end();
  if(haveWho || haveWith)
  {
    throw std::logic_error("At least one key does not exist");
  }
}

bool kovshikov::Graph::haveThisKey(size_t key)
{
  return tree.find(key) != tree.end();
}

bool kovshikov::Graph::isDouble(size_t key1, size_t key2)
{
  bool have1 = tree.at(key1).edges.find(key2) != tree.at(key1).edges.end();
  bool have2 = tree.at(key2).edges.find(key1) != tree.at(key2).edges.end();
  return have1 && have2;
}

bool kovshikov::noThis(size_t whoKey, size_t randomKey)
{
  return whoKey != randomKey;
}

void kovshikov::Graph::getConnectKeys(DoubleList< size_t >& connectKeys, size_t whoKey)
{
   DoubleList< size_t > keys;
   std::transform(tree.begin(), tree.end(), std::back_inserter(keys), getKey);
   std::copy_if(keys.begin(), keys.end(), std::back_inserter(connectKeys), std::bind(noThis, whoKey, std::placeholders::_1));
}

std::string kovshikov::Graph::getVertex(std::pair< size_t, Node > vertex)
{
  return vertex.second.value;
}

size_t kovshikov::Graph::getCountEdge(std::pair< size_t, Node > vertex)
{
  return vertex.second.edges.size();
}

size_t kovshikov::getKey(std::pair< size_t, Graph::Node > vertex)
{
  return vertex.first;
}

size_t kovshikov::getWith(std::pair< size_t, size_t > edge)
{
  return edge.first;
}

size_t kovshikov::getWeightEdge(std::pair< size_t, size_t > edge)
{
  return edge.second;
}

void kovshikov::Graph::addVertex(size_t key, std::string str)
{
  tree[key] = Node(str);
}

void kovshikov::Graph::deleteVertex(size_t key)
{
  try
  {
    haveThisVertex(key);
  }
  catch(const std::logic_error& e)
  {
    throw;
  }
  DoubleList< size_t > allKeys;
  std::transform(tree.begin(), tree.end(), std::back_inserter(allKeys), getKey);
  DoubleList< size_t > keys;
  auto current = keys.begin();
  std::copy_if(allKeys.begin(), allKeys.end(), std::back_inserter(keys), std::bind(noThis, key, std::placeholders::_1));
  while(current != keys.end())
  {
    if(tree[*current].edges.find(key) != tree[*current].edges.end())
    {
      tree[*current].edges.erase(key);
    }
    current++;
  }
  tree.erase(key);
}

void kovshikov::Graph::createEdge(size_t keyWho, size_t keyWith, size_t weight)
{
  try
  {
    haveNot(keyWho, keyWith);
  }
  catch(const std::logic_error& e)
  {
    throw;
  }
  tree.at(keyWho).edges[keyWith] = weight;
}

void kovshikov::Graph::deleteEdge(size_t keyWho, size_t keyWith)
{
  if(getWeight(keyWho, keyWith) == 0)
  {
    throw std::logic_error("<The edge does not exist>");
  }
  else
  {
    tree.at(keyWho).edges.erase(keyWith);
  }
}

void kovshikov::Graph::increaseWeight(size_t keyWho, size_t keyWith, size_t increase)
{
  try
  {
    haveNot(keyWho, keyWith);
  }
  catch(const std::logic_error& e)
  {
    throw;
  }
  if(getWeight(keyWho, keyWith) == 0)
  {
    createEdge(keyWho, keyWith, increase);
  }
  else
  {
    tree.at(keyWho).edges[keyWith] += increase;
  }
}

void kovshikov::Graph::decreaseWeight(size_t keyWho, size_t keyWith, size_t decrease)
{
  if(tree.at(keyWho).edges[keyWith] < decrease)
  {
    throw std::logic_error("<The edge weight is less than the entered value>");
  }
  else
  {
    tree.at(keyWho).edges[keyWith] -= decrease;
    if(tree.at(keyWho).edges[keyWith] == 0)
    {
      tree.at(keyWho).edges.erase(keyWith);
    }
  }
}

size_t kovshikov::Graph::getWeight(size_t keyWho, size_t keyWith)
{
  if(tree.find(keyWho) != tree.end())
  {
    if(tree.at(keyWho).edges.find(keyWith) != tree.at(keyWho).edges.end())
    {
      return tree.at(keyWho).edges.at(keyWith);
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

void kovshikov::Graph::change(size_t count, size_t who, size_t with)
{
  if(tree.at(who).edges.find(with) == tree.at(who).edges.end())
  {
    throw std::logic_error("There is no connection");
  }
  if(tree.at(who).edges.at(with) < count)
  {
    throw std::logic_error("links are less than the requested number");
  }
  tree.at(who).edges.at(with) -= count;
  if(tree.at(with).edges.find(who) == tree.at(with).edges.end())
  {
    createEdge(with, who, count);
  }
  else
  {
    tree.at(with).edges.at(who) += count;
  }
}

void kovshikov::Graph::connect(size_t whoKey, size_t count, size_t weight)
{
  size_t size = getSize() - 1;
  size_t remainder = count % size;
  size_t numCircules = (remainder == 0)?(count / size):(count / size + 1);
  DoubleList< size_t > connectKeys;
  getConnectKeys(connectKeys, whoKey);
  for(size_t i = 1; i <= numCircules; i++)
  {
    if(i == numCircules)
    {
      auto current = connectKeys.begin();
      current++;
      for(size_t j = 1; j <= remainder; j++)
      {
        increaseWeight(whoKey, *current, weight);
        current++;
      }
    }
    else
    {
      auto current = connectKeys.begin();
      for(size_t k = 0; k < size; k++)
      {
        increaseWeight(whoKey, *current, weight);
        current++;
      }
    }
  }
}

size_t kovshikov::Graph::getVertexWeight(size_t key)
{
  try
  {
    haveThisVertex(key);
  }
  catch(const std::logic_error& e)
  {
    throw;
  }
  DoubleList< size_t > sum;
  std::transform(tree.at(key).edges.begin(), tree.at(key).edges.end(), std::back_inserter(sum), getWeightEdge);
  return std::accumulate(sum.begin(), sum.end(), 0);
}

size_t kovshikov::Graph::getDegree(size_t key)
{
  try
  {
    haveThisVertex(key);
  }
  catch(const std::logic_error& e)
  {
    throw;
  }
  size_t degree = 0;
  degree += tree.at(key).edges.size();
  DoubleList< size_t > allKeys;
  std::transform(tree.begin(), tree.end(), std::back_inserter(allKeys), getKey);
  DoubleList< size_t > keys;
  std::copy_if(allKeys.begin(), allKeys.end(), std::back_inserter(keys), std::bind(noThis, key, std::placeholders::_1));
  auto current = keys.begin();
  while(current != keys.end())
  {
    if(tree[*current].edges.find(key) != tree[*current].edges.end())
    {
      degree += 1;
    }
    current++;
  }
  return degree;
}

size_t kovshikov::Graph::getOwn(size_t key)
{
  try
  {
    haveThisVertex(key);
  }
  catch(const std::logic_error& e)
  {
    throw;
  }
  size_t own = 0;
  DoubleList< size_t > allKeys;
  std::transform(tree.begin(), tree.end(), std::back_inserter(allKeys), getKey);
  DoubleList< size_t > keys;
  std::copy_if(allKeys.begin(), allKeys.end(), std::back_inserter(keys), std::bind(noThis, key, std::placeholders::_1));
  auto current = keys.begin();
  while(current != keys.end())
  {
    if(tree[*current].edges.find(key) != tree[*current].edges.end())
    {
      own += tree[*current].edges[key];
    }
    current++;
  }
  return own;
}

size_t kovshikov::Graph::getEdges()
{
  DoubleList< size_t > count;
  std::transform(tree.begin(), tree.end(), std::back_inserter(count), getCountEdge);
  return std::accumulate(count.begin(), count.end(), 0);
}

void kovshikov::Graph::outGraph(std::ostream& out) const
{
  auto outEmpty = std::bind(outMessage, "This graph is empty", std::placeholders::_1);
  if(isEmpty())
  {
    outEmpty(out);
  }
  else
  {
    DoubleList< size_t > keysList;
    std::transform(tree.begin(), tree.end(), std::back_inserter(keysList), getKey);
    auto curKeysList = keysList.begin();
    while(curKeysList != keysList.end())
    {
      out << *curKeysList << " " << tree.at(*curKeysList).value << " ";
      if(tree.at(*curKeysList).edges.empty())
      {
        out << 0 << "\n";
      }
      else
      {
        DoubleList< size_t > keysWith;
        std::transform(tree.at(*curKeysList).edges.begin(), tree.at(*curKeysList).edges.end(), std::back_inserter(keysWith), getWith);
        auto curKeysWith = keysWith.begin();
        size_t count = tree.at(*curKeysList).edges.size();
        for(size_t j = 0; j < count; j++)
        {
          if(j == count - 1)
          {
            out << *curKeysWith << " : " << tree.at(*curKeysList).edges.at(*curKeysWith) << "\n";
          }
          else
          {
            out << *curKeysWith << " : " << tree.at(*curKeysList).edges.at(*curKeysWith) << "  ";
          }
          curKeysWith++;
        }
      }
      curKeysList++;
    }
  }
}

bool kovshikov::Graph::isEmpty() const noexcept
{
  return tree.empty();
}


size_t kovshikov::Graph::getSize() const noexcept
{
  return tree.size();
}

bool kovshikov::Graph::comp(std::pair< size_t, Node > left, std::pair< size_t, Node > right, Graph& graph)
{
  return graph.getDegree(left.first) > graph.getDegree(right.first);
}
void kovshikov::Graph::getMax(std::ostream& out)
{
  using namespace std::placeholders;
  DoubleList< std::pair< size_t, Node > > tempVector;
  std::copy(tree.begin(), tree.end(), std::back_inserter(tempVector));
  selectionSort(tempVector.begin(), tempVector.end(), std::bind(comp, _1, _2, *this));
  size_t max = tempVector.front().first;
  DoubleList< size_t > keys;
  std::transform(tree.begin(), tree.end(), std::back_inserter(keys), getKey);
  DoubleList< size_t > keysWithout;
  std::copy_if(keys.begin(), keys.end(), std::back_inserter(keysWithout), std::bind(noThis, max, _1));
  auto iterator = keysWithout.begin();
  while(iterator != keysWithout.end())
  {
    if(getDegree(max) == getDegree(*iterator))
    {
      out << *iterator << " ";
    }
    iterator++;
  }
  out << max << "\n";
}
