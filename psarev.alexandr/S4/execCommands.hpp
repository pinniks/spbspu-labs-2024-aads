#ifndef EXEC_COMMANDS_HPP
#define EXEC_COMMANDS_HPP
#include <iostream>
#include "avlTree.hpp"

namespace psarev
{
  using base_t = psarev::avlTree< int, std::string >;

  std::ostream& outError(std::ostream& out, const std::string& errText);
  void fillTree(std::istream& in, avlTree< std::string, base_t >& dataSets);
}

#endif