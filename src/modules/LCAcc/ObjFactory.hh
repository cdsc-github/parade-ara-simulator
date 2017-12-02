#ifndef OBJFACTORY_H
#define OBJFACTORY_H

#include <map>
#include <cassert>

namespace LCAcc
{
template <class Key, class T>
class ObjFactory
{
public:
  class Generator
  {
  public:
    virtual ~Generator() {}
    virtual T* CreateNew() = 0;
  };
  template <class Derived>
  class TemplateGenerator : public Generator
  {
  public:
    virtual T* CreateNew()
    {
      return new Derived();
    }
  };
private:
  typedef std::map<Key, Generator*> GenSet;
  GenSet genSet;
public:
  T* Create(const Key& key)
  {
    assert(genSet.find(key) != genSet.end());
    return genSet[key]->CreateNew();
  }
  void RegisterGenerator(Key k, Generator* gen)
  {
    genSet[k] = gen;
  }
};
}

#endif
