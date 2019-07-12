#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <queue>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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
    Node* parent;
    Node* left;
    Node* right;

    Node()
    : data(std::make_pair(0,0)), parent(nullptr), left(nullptr), right(nullptr) {}
    Node(key_type k, mapped_type m)
    : data(std::make_pair(k,m)),  parent(nullptr), left(nullptr), right(nullptr) {}
    Node(value_type v)
    : Node(v.first, v.second) {}

    ~Node() {}
  };

  Node* root;
  size_type numOfNodes;

  void insert(Node* n)
  {
      Node** tmp=&root;
      Node* parentTemp=nullptr;

      while(*tmp!=nullptr)
      {
          parentTemp=*tmp;
          if(n->data.first<(*tmp)->data.first)
          {
              tmp=&((*tmp)->left);
          }
          else
          {
              tmp=&((*tmp)->right);
          }
      }
      *tmp=n;
      (*tmp)->parent=parentTemp;
      numOfNodes++;
      return;
  }

  void replace(Node* delNode, Node* repNode)
  {
    if(delNode==root)
    {
      root=repNode;
    }
    else
    {
      if(delNode->parent->left==delNode)
      {
        delNode->parent->left=repNode;
      }
      else
      {
        delNode->parent->right=repNode;
      }
    }
    if(repNode!=nullptr)
    {
      repNode->parent=delNode->parent;
      if(delNode->left!=nullptr && delNode->left!=repNode)
      {
        repNode->left=delNode->left;
        delNode->left->parent=repNode;
      }
      if(delNode->right!=nullptr && delNode->right!=repNode)
      {
        repNode->right=delNode->right;
        delNode->right->parent=repNode;
      }
    }
    delNode->left=nullptr;
    delNode->right=nullptr;
    delNode->parent=nullptr;
  }

  void removeNode(Node* remNode)
  {
    if(remNode->right==nullptr)
    {
      replace(remNode,remNode->left);
    }
    else if (remNode->left==nullptr)
    {
      replace(remNode,remNode->right);
    }
    else
    {
      auto tmp=remNode->right;
      while(tmp->left!=nullptr)
      {
        tmp=tmp->left;
      }
      replace(tmp,tmp->right);
      replace(remNode,tmp);
    }
    delete remNode;
    numOfNodes--;
  }

Node* findNode(key_type key) const
{
  Node* temp=root;
  while(temp!=nullptr)
  {
    if(key>temp->data.first)
    {
      temp=temp->right;
    }
    else if(key<temp->data.first)
    {
      temp=temp->left;
    }
    else break;
  }
  return temp;
}

  void deleteTree()
  {
    if(root!=nullptr)
    {
      std::queue<Node*> q;
      Node* node;

      q.push(root);
      while(!q.empty())
      {
        node = q.front();
        q.pop();
        if(node!=nullptr)
        {
          q.push(node->left);
          q.push(node->right);
          delete node;
        }
      }
    }
    root=nullptr;
    numOfNodes=0;
  }

public:
  TreeMap()
  : root(nullptr), numOfNodes(0)
  {}

  TreeMap(std::initializer_list<value_type> list)
  : TreeMap()
  {
    for(auto it=list.begin(); it!=list.end(); it++)
    {
      insert(new Node(*it));
    }
  }

  TreeMap(const TreeMap& other)
  : TreeMap()
  {
    if(other.root!=nullptr)
    {
      std::queue<Node*> q;
      Node* node;

      q.push(other.root);
      while(!q.empty())
      {
        node = q.front();
        q.pop();
        if(node!=nullptr)
        {
          q.push(node->left);
          q.push(node->right);
          insert(new Node(node->data));
        }
      }
    }
  }

  TreeMap(TreeMap&& other)
  {
    this->root=other.root;
    this->numOfNodes=other.numOfNodes;

    other.root=nullptr;
    other.numOfNodes=0;
  }

  ~TreeMap()
  {
    deleteTree();
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(this!=&other)
    {
      this->deleteTree();
      if(other.root!=nullptr)
      {
        std::queue<Node*> q;
        Node* node;

        q.push(other.root);
        while(!q.empty())
        {
          node = q.front();
          q.pop();
          if(node!=nullptr)
          {
            q.push(node->left);
            q.push(node->right);
            insert(new Node(node->data));
          }
        }
      }
    }
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if(this!=&other)
    {
      this->deleteTree();
      this->root=other.root;
      this->numOfNodes=other.numOfNodes;

      other.root=nullptr;
      other.numOfNodes=0;
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
    if(root==nullptr)
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
    if(root==nullptr)
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
    return ConstIterator(this,findNode(key));
  }

  iterator find(const key_type& key)
  {
    return Iterator(this,findNode(key));
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(root==nullptr)
    {
      throw std::out_of_range("Tree is empty");
    }
    if(it==cend())
    {
      throw std::out_of_range("No key found in tree");
    }
    removeNode(it.getPtr());
  }

  size_type getSize() const
  {
    return numOfNodes;
  }

  bool operator==(const TreeMap& other) const
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

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    Node* temp=root;
    if(temp!=nullptr)
    {
      while(temp->left!=nullptr)
      {
        temp=temp->left;
      }
    }
    return Iterator(this,temp);
  }

  iterator end()
  {
    return Iterator(this,nullptr);
  }

  const_iterator cbegin() const
  {
    Node* temp=root;
    if(temp!=nullptr)
    {
      while(temp->left!=nullptr)
      {
        temp=temp->left;
      }
    }
    return ConstIterator(this,temp);
  }

  const_iterator cend() const
  {
    return ConstIterator(this,nullptr);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

  Node* getRoot() const
  {
    return root;
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
  const TreeMap* treePtr;
  Node* nodePtr;
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

  explicit ConstIterator(const TreeMap* t, Node* n)
  : treePtr(t), nodePtr(n)
  {}

  ConstIterator(const ConstIterator& other)
  : ConstIterator(other.treePtr, other.nodePtr)
  {}

  ConstIterator& operator++()
  {
    if(nodePtr==nullptr)
    {
      throw std::out_of_range("Cannot increment");
    }
    else if(nodePtr->right!=nullptr)
    {
      nodePtr=nodePtr->right;
      while(nodePtr->left!=nullptr)
      {
        nodePtr=nodePtr->left;
      }
    }
    else
    {
      while(true)
      {
        if(nodePtr->parent==nullptr)
        {
          nodePtr=nullptr;
          break;
        }
        if(nodePtr->parent->right!=nodePtr)
        {
          nodePtr=nodePtr->parent;
          break;
        }
        nodePtr=nodePtr->parent;
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
    if(treePtr->getRoot()==nullptr)
    {
      throw std::out_of_range("Cannot decrement");
    }
    else if(nodePtr==nullptr)
    {
      nodePtr=treePtr->getRoot();
      while(nodePtr->right!=nullptr)
      {
        nodePtr=nodePtr->right;
      }
    }
    else
    {
      if(nodePtr->left!=nullptr)
      {
        nodePtr=nodePtr->left;
        while(nodePtr->right!=nullptr)
        {
          nodePtr=nodePtr->right;
        }
      }
      else
      {
        while(true)
        {
          if(nodePtr->parent==nullptr)
          {
            throw std::out_of_range("Cannot decrement");
          }
          if(nodePtr->parent->left!=nodePtr)
          {
            nodePtr=nodePtr->parent;
            break;
          }
          nodePtr=nodePtr->parent;
        }
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
    if(treePtr==other.treePtr&&nodePtr==other.nodePtr)
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
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator(TreeMap* t, Node* n)
  : ConstIterator(t,n)
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

#endif /* AISDI_MAPS_MAP_H */
