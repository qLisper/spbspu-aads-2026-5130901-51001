#ifndef MUSORIN_DATASET_OPS_HPP
#define MUSORIN_DATASET_OPS_HPP

#include "bstree.hpp"

namespace musorin
{

BSTree<int, std::string> complementTrees(const BSTree<int, std::string>& a,
                                         const BSTree<int, std::string>& b)
{
  BSTree<int, std::string> result;
  for (auto it = a.cbegin(); it != a.cend(); ++it)
  {
    if (!b.has((*it).first))
    {
      result.push((*it).first, (*it).second);
    }
  }
  return result;
}

BSTree<int, std::string> intersectTrees(const BSTree<int, std::string>& a,
                                        const BSTree<int, std::string>& b)
{
  BSTree<int, std::string> result;
  for (auto it = a.cbegin(); it != a.cend(); ++it)
  {
    if (b.has((*it).first))
    {
      result.push((*it).first, (*it).second);
    }
  }
  return result;
}

BSTree<int, std::string> unionTrees(const BSTree<int, std::string>& a,
                                    const BSTree<int, std::string>& b)
{
  BSTree<int, std::string> result = a;
  for (auto it = b.cbegin(); it != b.cend(); ++it)
  {
    if (!result.has((*it).first))
    {
      result.push((*it).first, (*it).second);
    }
  }
  return result;
}

} 

#endif
