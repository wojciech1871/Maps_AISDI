#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  struct Node
  {
    value_type data;
    Node* prev;
    Node* next;

    Node()
    : data(std::make_pair(0,0)), prev(nullptr), next(nullptr) {}
    Node(key_type k, mapped_type m)
    : data(std::make_pair(k,m)), prev(nullptr), next(nullptr) {}
    Node(value_type v)
    : Node(v.first, v.second) {}

    ~Node() {}
  };
  size_type TAB_SIZE;
  Node** table;
  size_type numOfNodes;

  void insert(Node *n)
  {
    key_type index=n->data.first%TAB_SIZE;
    Node* currentPtr=table[index];
    if(currentPtr!=nullptr)
    {
      while(currentPtr->next!=nullptr)
      {
        if(currentPtr->next->data.first<n->data.first)
        {
          n->next=currentPtr->next;
          currentPtr->next->prev=n;
          break;
        }
        currentPtr=currentPtr->next;
      }
      currentPtr->next=n;
      n->prev=currentPtr;
    }
    else
    {
      table[index]=n;
    }
    numOfNodes++;
  }

  void removeNode(Node *remNode)
  {
    key_type index=remNode->data.first%TAB_SIZE;
    if(remNode->prev!=nullptr)
    {
      remNode->prev->next=remNode->next;
    }
    if(remNode->next!=nullptr)
    {
      remNode->next->prev=remNode->prev;
    }
    if(table[index]==remNode)
    {
      table[index]=remNode->next;
    }
    remNode->next=nullptr;
    remNode->prev=nullptr;
    delete remNode;
    numOfNodes--;
  }

  Node* findNode(key_type key) const
  {
    key_type index=key%TAB_SIZE;
    Node* currentPtr=table[index];
    while(currentPtr!=nullptr)
    {
      if(currentPtr->data.first==key)
      {
        return currentPtr;
      }
      currentPtr=currentPtr->next;
    }
    return nullptr;
  }

  void deleteHash()
  {
    for(size_type index=0; index<TAB_SIZE; index++)
    {
      if(table[index]!=nullptr)
      {
        Node* currentDel=table[index];
        Node* nextDel;
        while(currentDel!=nullptr)
        {
          nextDel=currentDel->next;
          currentDel->prev=nullptr;
          currentDel->next=nullptr;
          delete currentDel;
          currentDel=nextDel;
        }
        table[index]=nullptr;
      }
    }
    numOfNodes=0;
  }

public:
  HashMap(size_type tabSize=1000)
  : TAB_SIZE(tabSize), numOfNodes(0)
  {
    table=new Node* [TAB_SIZE]{nullptr};
  }

  HashMap(std::initializer_list<value_type> list)
  : HashMap()
  {
    for(auto it=list.begin(); it!=list.end(); it++)
    {
      insert(new Node(*it));
    }
  }

  HashMap(const HashMap& other)
  : HashMap()
  {
    for(auto it=other.begin(); it!=other.end(); it++)
    {
      insert(new Node(*it));
    }
  }

  HashMap(HashMap&& other)
  {
    this->table=other.table;
    this->numOfNodes=other.numOfNodes;
    this->TAB_SIZE=other.TAB_SIZE;

    other.table=nullptr;
    other.numOfNodes=0;
    other.TAB_SIZE=0;
  }

  ~HashMap()
  {
    deleteHash();
    delete[] table;
  }

  HashMap& operator=(const HashMap& other)
  {
    if(this!=&other)
    {
      this->deleteHash();
      for(auto it=other.begin(); it!=other.end(); it++)
      {
        insert(new Node(*it));
      }
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if(this!=&other)
    {
      this->deleteHash();
      this->table=other.table;
      this->numOfNodes=other.numOfNodes;
      this->TAB_SIZE=other.TAB_SIZE;

      other.table=nullptr;
      other.numOfNodes=0;
      other.TAB_SIZE=0;
    }
    return *this;
  }

  bool isEmpty() const
  {
    if(numOfNodes)
    {
      return false;
    }
    return true;
  }

  mapped_type& operator[](const key_type& key)
  {
    auto it=find(key);
    if(it!=end())
    {
      return it->second;
    }
    Node* newNode=new Node(key,mapped_type());
    insert(newNode);
    return newNode->data.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    if(table==nullptr)
    {
      throw std::out_of_range("Tree is empty");
    }
    auto it=find(key);
    if(it==end())
    {
      throw std::out_of_range("No such key");
    }
    return it->second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    if(table==nullptr)
    {
      throw std::out_of_range("Tree is empty");
    }
    auto it=find(key);
    if(it==end())
    {
      throw std::out_of_range("No such key");
    }
    return it->second;
  }

  const_iterator find(const key_type& key) const
  {
    return ConstIterator(this,findNode(key), key%TAB_SIZE);
  }

  iterator find(const key_type& key)
  {
    return Iterator(this,findNode(key), key%TAB_SIZE);
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(table==nullptr)
    {
      throw std::out_of_range("Tree is empty");
    }
    if(it==end())
    {
      throw std::out_of_range("No key found in hash");
    }
    removeNode(it.getPtr());
  }

  size_type getSize() const
  {
    return numOfNodes;
  }

  bool operator==(const HashMap& other) const
  {
    if(this->numOfNodes!=other.numOfNodes)
    {
      return false;
    }
    for(auto it=begin();it!=end();it++)
    {
      if(other.findNode(it->first)==nullptr)
      {
        return false;
      }
      if(other.valueOf(it->first)!=it->second)
      {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    Node* n=nullptr;
    size_type index=0;
    while(index<TAB_SIZE && table[index]==nullptr)
    {
      index++;
    }
    if(index<TAB_SIZE)
    {
      n=table[index];
    }

    return Iterator(this,n,index);
  }

  iterator end()
  {
    return Iterator(this,nullptr,TAB_SIZE);
  }

  const_iterator cbegin() const
  {
    Node* n=nullptr;
    size_type index=0;
    while(index<TAB_SIZE && table[index]==nullptr)
    {
      index++;
    }
    if(index<TAB_SIZE)
    {
      n=table[index];
    }
    return ConstIterator(this,n,index);
  }

  const_iterator cend() const
  {
    return ConstIterator(this,nullptr,TAB_SIZE);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

  size_type getTabSize() const
  {
    return TAB_SIZE;
  }

  Node** getTabPtr() const
  {
    return table;
  }

};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

private:
  const HashMap* mapPtr;
  Node* nodePtr;
  size_type index;
public:
  explicit ConstIterator(const HashMap* h, Node* n, size_type i=0)
  : mapPtr(h), nodePtr(n), index(i)
  { /*
    if(nodePtr==nullptr &&  mapPtr!=nullptr)
    {
      index=mapPtr->getTabSize();
    } */
  }

  ConstIterator(const ConstIterator& other)
  : mapPtr(other.mapPtr), nodePtr(other.nodePtr), index(other.index)
  {
    if(nodePtr==nullptr && mapPtr!=nullptr)
    {
      index=mapPtr->getTabSize();
    }
  }

  ConstIterator& operator++()
  {
    if(nodePtr==nullptr)
    {
      throw std::out_of_range("Cannot increment");
    }
    else if(nodePtr->next!=nullptr)
    {
      nodePtr=nodePtr->next;
    }
    else
    {
      index++;
      while(index<mapPtr->getTabSize() && mapPtr->getTabPtr()[index]==nullptr)
      {
        index++;
      }
      if(index<mapPtr->getTabSize())
      {
        nodePtr=mapPtr->getTabPtr()[index];
      }
      else
      {
        nodePtr=nullptr;
      }
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator temp(*this);
    ConstIterator::operator++();
    return temp;
  }

  ConstIterator& operator--()
  {
    if(mapPtr==nullptr)
    {
      throw std::out_of_range("Cannot increment");
    }

    else if(nodePtr!=nullptr)
    {
      if(nodePtr->prev!=nullptr)
      {
        nodePtr=nodePtr->prev;
      }
    }

    if(nodePtr==nullptr || nodePtr==mapPtr->getTabPtr()[index])
    {
      index--;
      while(index>0 && mapPtr->getTabPtr()[index]==nullptr)
      {
        index--;
      }
      if(index==0 && mapPtr->getTabPtr()[index]==nullptr)
      {
        throw std::out_of_range("Cannot decrement");
      }
      nodePtr=mapPtr->getTabPtr()[index];
      while(nodePtr->next!=nullptr)
      {
        nodePtr=nodePtr->next;
      }
    }
    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator temp(*this);
    ConstIterator::operator--();
    return temp;
  }

  reference operator*() const
  {
    if(nodePtr==nullptr)
    {
      throw std::out_of_range("Cannot dereference");
    }
    return nodePtr->data;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    if(mapPtr==other.mapPtr&&nodePtr==other.nodePtr)
    {
      return true;
    }
    return false;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }

  Node* getPtr() const
  {
    return nodePtr;
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator(HashMap* h, Node* n, size_type i=0)
  : ConstIterator(h,n,i)
  {}

  Iterator(const ConstIterator& other)
  : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
