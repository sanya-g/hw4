#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* current);
    void removeFix(AVLNode<Key, Value>* current, int diff);
    void rotateLeft(AVLNode<Key, Value>* current);
    void rotateRight(AVLNode<Key, Value>* current);
    AVLNode<Key, Value>* internalFind(const Key& k) const; 
    static AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current); 
    static AVLNode<Key, Value>* successor(AVLNode<Key, Value>* current);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    bool goesLeft=0;

    AVLNode<Key, Value> *AVLRoot = static_cast<AVLNode<Key, Value>*>(this->root_);

    if(AVLRoot == nullptr)
    {
      AVLNode<Key, Value>* newPair= new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
      newPair->setRight(nullptr);
      newPair->setLeft(nullptr);
      newPair->setBalance(0); 

      this->root_ = newPair;
      return;
    }
    AVLNode<Key, Value>* current=AVLRoot;
    AVLNode<Key, Value>* parent=nullptr;

    while (current)
    {
      parent=current;
      if(new_item.first < current->getKey())
      {
        current=current->getLeft();
        goesLeft=1;
      }
      else if(new_item.first > current->getKey())
      {
        current=current->getRight();
        goesLeft=0;
      }
      else if (new_item.first == current->getKey())
      {
        current->setValue(new_item.second);
        return;
      }
    }
    
    if(goesLeft)
    {
      AVLNode<Key, Value>* newPair= new AVLNode<Key,Value>(new_item.first, new_item.second, parent);
      newPair->setRight(nullptr);
      newPair->setLeft(nullptr);
      newPair->setBalance(0);
      parent->setLeft(newPair);
      current=newPair;
    }
    else if(!goesLeft)
    {
      AVLNode<Key, Value>* newPair= new AVLNode<Key,Value>(new_item.first, new_item.second, parent);
      newPair->setRight(nullptr);
      newPair->setLeft(nullptr);
      newPair->setBalance(0);
      parent->setRight(newPair);
      current=newPair;
    }

    if(parent->getBalance() == -1 || parent->getBalance() == 1)
    {
        parent->setBalance(0);
    }
    else if(parent->getBalance() == 0)
    {
      if(goesLeft)  
      {
        parent->updateBalance(-1);
      }
      else 
      {
        parent->updateBalance(1);
      }
      
      insertFix(parent, current);
      
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* current)
{
    if(parent==nullptr) 
    {
        return;
    }
    AVLNode<Key, Value>* grandparent=parent->getParent();
    if(grandparent == nullptr)
    {
      return;
    }
    else if(grandparent->getLeft() != nullptr && grandparent->getLeft() ==parent)
    {
      grandparent->updateBalance(-1);
      if(grandparent->getBalance() ==0)
      {
          return;
      }
      else if (grandparent->getBalance() ==-1)
      {
          insertFix(grandparent, parent);
      }
      else if (grandparent->getBalance() == -2)
      {
        if(parent->getLeft() !=nullptr && parent->getLeft() == current)
        {
            rotateRight(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);
        }
        else if(parent->getRight() !=nullptr && parent->getRight() ==current)
        {
          rotateLeft(parent);
          rotateRight(grandparent);
          if(current->getBalance() == -1)
          {
              parent->setBalance(0);
              grandparent->setBalance(1);
              current->setBalance(0);
          }
          else if(current->getBalance() == 0)
          {
              parent->setBalance(0);
              grandparent->setBalance(0);
              current->setBalance(0);
          }
          else if (current->getBalance() == 1)
          {
              parent->setBalance(-1);
              grandparent->setBalance(0);
              current->setBalance(0);
          }
        }
      }
    }
    else if(grandparent->getRight() != nullptr && grandparent->getRight() ==parent)
    {
      grandparent->updateBalance(1);
      if(grandparent->getBalance() ==0)
      {
          return;
      }
      else if (grandparent->getBalance() ==1)
      {
          insertFix(grandparent, parent);
      }
      else if (grandparent->getBalance() == 2)
      {
        if(parent->getRight() !=nullptr && parent->getRight() == current)
        {
            rotateLeft(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);
        }
        else if(parent->getLeft() !=nullptr && parent->getLeft() ==current)
        {
          rotateRight(parent);
          rotateLeft(grandparent);
          if(current->getBalance() == 1)
          {
              parent->setBalance(0);
              grandparent->setBalance(-1);
              current->setBalance(0);
          }
          else if(current->getBalance() == 0)
          {
              parent->setBalance(0);
              grandparent->setBalance(0);
              current->setBalance(0);
          }
          else if (current->getBalance() == -1)
          {
              parent->setBalance(1);
              grandparent->setBalance(0);
              current->setBalance(0);
          }
        }
      }
    }

}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* current=internalFind(key);
    if(current==nullptr) return;
    AVLNode<Key, Value>* currParent=current->getParent();
    AVLNode<Key, Value>* currLeft=current->getLeft();
    AVLNode<Key, Value>* currRight=current->getRight();
    AVLNode<Key, Value>* currPred=predecessor(current);
    AVLNode<Key, Value> *AVLRoot = static_cast<AVLNode<Key, Value>*>(this->root_);

    int diff;

    if(current==AVLRoot) //is root, no parent
    {
      if(current->getLeft() == nullptr && current->getRight() == nullptr)
      {
        this->root_=nullptr;
      }
      else if((current->getLeft() != nullptr && current->getRight() == nullptr))
      {
        currLeft->setParent(nullptr);
        current->setLeft(nullptr);

      
        this->root_=currLeft;
      }
      else if((current->getLeft() == nullptr && current->getRight() != nullptr))
      {
        currRight->setParent(nullptr);
        current->setRight(nullptr);

        this->root_=currRight;
      }
      else if (current->getLeft() != nullptr && current->getRight() != nullptr) 
      {
        nodeSwap(current, currPred);
        currParent=current->getParent();
        if(current->getLeft() == nullptr)
        {
          if(current->getParent()->getLeft() != nullptr && 
            current->getParent()->getLeft() ==current)
          {
            diff=1;
            current->getParent()->setLeft(nullptr);    
          }
          else if(current->getParent()->getRight() != nullptr && 
            current->getParent()->getRight() ==current)
          {
            diff=-1;
            current->getParent()->setRight(nullptr);
          }
        }   

        else if(current->getLeft() != nullptr)
        {
          current->getLeft()->setParent(current->getParent());
          if(current->getParent()->getLeft() != nullptr && 
            current->getParent()->getLeft() ==current)
          {
            diff=1;
            current->getParent()->setLeft(current->getLeft());          
          }
          else if(current->getParent()->getRight() != nullptr && 
            current->getParent()->getRight() ==current)
          {
            diff=-1;
            current->getParent()->setRight(current->getLeft());
          }
        }

        this->root_=currPred;
        
        current->setParent(nullptr);
        current->setLeft(nullptr);
        current->setRight(nullptr);
      }
    }
    else if(current->getParent() != nullptr)
    {
      //is left child
      if(currParent->getLeft() != nullptr && currParent->getLeft() == current)
      {
        diff=1;
        if(current->getLeft() == nullptr && current->getRight() == nullptr)
        {
            currParent->setLeft(nullptr);
        }
        else if (current->getLeft() != nullptr && current->getRight() != nullptr)
        {
          nodeSwap(current, currPred);
          currParent=current->getParent();
          if(current->getLeft() == nullptr)
          {
            if(current->getParent()->getLeft() != nullptr && 
              current->getParent()->getLeft() ==current)
            {
              diff=1;
              current->getParent()->setLeft(nullptr);
            }
            else if(current->getParent()->getRight() != nullptr && 
              current->getParent()->getRight() ==current)
            {
              diff=-1;
              current->getParent()->setRight(nullptr);
            }
          }
          else if(current->getLeft() != nullptr)
          {
            current->getLeft()->setParent(current->getParent());
            if(current->getParent()->getLeft() != nullptr && 
              current->getParent()->getLeft() == current)
            {
              diff=1;
              current->getParent()->setLeft(current->getLeft());
            }
            else if(current->getParent()->getRight() != nullptr && 
              current->getParent()->getRight() == current)
            {              
              diff=-1;
              current->getParent()->setRight(current->getLeft());
            }
          }
        }
        else if (current->getLeft() != nullptr && current->getRight() == nullptr) 
        {
          currParent->setLeft(currLeft);
          currLeft->setParent(currParent);
        }
        else if (current->getLeft() == nullptr && current->getRight() != nullptr)
        {
          currParent->setLeft(currRight);
          currRight->setParent(currParent);
        }
        
      }
      if(currParent->getRight() != nullptr && currParent->getRight() == current)
      {
        diff=-1;
        if(current->getLeft() == nullptr && current->getRight() == nullptr)
        {
          currParent->setRight(nullptr);
        }
        else if (current->getLeft() != nullptr && current->getRight() != nullptr)
        {
          nodeSwap(current, currPred);
          currParent=current->getParent();
          if(current->getLeft() == nullptr)
          {
            if(current->getParent()->getLeft() != nullptr && 
              current->getParent()->getLeft() ==current)
            {
              diff=1;
              current->getParent()->setLeft(nullptr);         
            }
            else if(current->getParent()->getRight() != nullptr && 
              current->getParent()->getRight() ==current)
            {
              diff=-1;
              current->getParent()->setRight(nullptr);
            }
          }
          else if(current->getLeft() != nullptr)
          {
            current->getLeft()->setParent(current->getParent());
            if(current->getParent()->getLeft() != nullptr && 
              current->getParent()->getLeft() ==current)
            {
              diff=1;
              current->getParent()->setLeft(current->getLeft());       
            }
            else if(current->getParent()->getRight() != nullptr &&  
              current->getParent()->getRight() ==current)
            {
              diff=-1;
              current->getParent()->setRight(current->getLeft());
            }
          }
        }
        else if (current->getLeft() != nullptr && current->getRight() == nullptr) 
        {
          currParent->setRight(currLeft);
          currLeft->setParent(currParent);
        }
        else if (current->getLeft() == nullptr && current->getRight() != nullptr)
        {
          currParent->setRight(currRight);
          currRight->setParent(currParent);
        }
        
      }
      current->setParent(nullptr);
      current->setLeft(nullptr);
      current->setRight(nullptr);
    }  
    delete current; 
    removeFix(currParent, diff);
    
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* current, int diff)
{
  if(current ==nullptr)
  {
    return;
  }
  AVLNode<Key, Value>* parent=current->getParent();
  int ndiff=0;
  
  if(parent != nullptr)
  { 
    if(parent->getLeft() != nullptr && parent->getLeft() == current)
    {
      ndiff=1;
    }
    else if(parent->getRight() != nullptr && parent->getRight() == current)
    {
      ndiff=-1;
    }
  }

  if(diff==-1)
  {
    if(current->getBalance() + diff ==-2)
    {
      AVLNode<Key, Value>* tallChild=current->getLeft();
      if(tallChild->getBalance() == -1)
      {
        rotateRight(current);
        current->setBalance(0);
        tallChild->setBalance(0);
        removeFix(parent, ndiff);
      }
      else if(tallChild->getBalance() == 0)
      {
        rotateRight(current);
        current->setBalance(-1);
        tallChild->setBalance(1);
      }
      else if (tallChild->getBalance() == 1)
      {
        AVLNode<Key, Value>* grandChild=tallChild->getRight();
        rotateLeft(tallChild);
        rotateRight(current);
        if(grandChild->getBalance() == 1)
        {
          current->setBalance(0);
          tallChild->setBalance(-1);
          grandChild->setBalance(0);
        }
        else if(grandChild->getBalance() == 0)
        {
          current->setBalance(0);
          tallChild->setBalance(0);
          grandChild->setBalance(0);
        }
        else if(grandChild->getBalance() == -1)
        {
          current->setBalance(1);
          tallChild->setBalance(0);
          grandChild->setBalance(0);
        }
        removeFix(parent, ndiff);
      }
    }
    else if (current->getBalance() + diff ==-1)
    {
      current->setBalance(-1);
    }
    else if (current->getBalance() + diff == 0)
    {
      current->setBalance(0);
      removeFix(parent, ndiff);
    }
  }
  else if(diff==1)
  {
    if(current->getBalance() + diff ==2)
    {
      AVLNode<Key, Value>* tallChild=current->getRight();
      if(tallChild->getBalance() == 1)
      {
        rotateLeft(current);
        current->setBalance(0);
        tallChild->setBalance(0);
        removeFix(parent, ndiff);
      }
      else if(tallChild->getBalance() == 0)
      {
        rotateLeft(current);
        current->setBalance(1);
        tallChild->setBalance(-1);
      }
      else if (tallChild->getBalance() == -1)
      {
        AVLNode<Key, Value>* grandChild=tallChild->getLeft();
        rotateRight(tallChild);
        rotateLeft(current);
        if(grandChild->getBalance() == -1)
        {
          current->setBalance(0);
          tallChild->setBalance(1);
          grandChild->setBalance(0);
        }
        else if(grandChild->getBalance() == 0)
        {
          current->setBalance(0);
          tallChild->setBalance(0);
          grandChild->setBalance(0);
        }
        else if(grandChild->getBalance() == 1)
        {
          current->setBalance(-1);
          tallChild->setBalance(0);
          grandChild->setBalance(0);
        }
        removeFix(parent, ndiff);
      }
    }
    else if (current->getBalance() + diff ==1)
    {
      current->setBalance(1);
    }
    else if (current->getBalance() + diff == 0)
    {
      current->setBalance(0);
      removeFix(parent, ndiff);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* parent)
{
  AVLNode<Key, Value>* current=parent->getRight();
  AVLNode<Key, Value>* grandParent=parent->getParent();
  if(grandParent == nullptr)
  {
    this->root_=current;
  }
  else
  {
    if(grandParent->getLeft() != nullptr && grandParent->getLeft()==parent )
    {
      grandParent->setLeft(current);
    }
    else if(grandParent->getRight() != nullptr && grandParent->getRight()==parent )
    {
      grandParent->setRight(current);
    }
  }
  parent->setRight(current->getLeft());
  if(current->getLeft() != nullptr)
  {
    current->getLeft()->setParent(parent);
  }
  parent->setParent(current);

  current->setParent(grandParent);
  current->setLeft(parent);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* parent)
{
  AVLNode<Key, Value>* current=parent->getLeft();
  AVLNode<Key, Value>* grandParent=parent->getParent();
  if(grandParent == nullptr)
  {
    this->root_=current;
  }
  else
  {
    if(grandParent->getLeft() != nullptr && grandParent->getLeft()==parent )
    {
      grandParent->setLeft(current);
    }
    else if(grandParent->getRight() != nullptr && grandParent->getRight()==parent )
    {
      grandParent->setRight(current);
    }
  }

  parent->setLeft(current->getRight());
  if(current->getRight() != nullptr)
  {
    current->getRight()->setParent(parent);
  }
  parent->setParent(current);

  current->setParent(grandParent);
  current->setRight(parent);
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::internalFind(const Key& key) const
{
    // TODO
    AVLNode<Key, Value> *current = static_cast<AVLNode<Key, Value>*>(this->root_);
    while(current!=nullptr)
    {
      if(current->getKey()==key)
      {
        return current;
      }
      else if(key < current->getKey())
      {
        current=current->getLeft();
      }
      else if(key >= current->getKey())
      {
        current=current->getRight();
      }
    }
    return nullptr;
}


template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current)
{
    // TODO

    AVLNode<Key, Value>* pred=current;
    if(pred->getLeft() !=nullptr)
    {
      pred=pred->getLeft();
      while(pred->getRight() != nullptr)
      {
        pred=pred->getRight();
      }
      return pred;
    }
    else{
      bool found =0;
      while(pred->getParent() != nullptr)
      {
        if( pred->getParent()->getRight() != nullptr && pred == pred->getParent()->getRight()) 
        {
          pred=pred->getParent();
          found=1;
          break;          
        }
        pred=pred->getParent();
      }
      if(!found)
      {
        pred=nullptr;
      }
      return pred;
    }
    
}

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::successor(AVLNode<Key, Value>* current)
{
  AVLNode<Key, Value>* succ=current;
  if(succ->getRight() !=nullptr)
  {
    succ=succ->getRight();
    while(succ->getLeft() != nullptr)
    {
      succ=succ->getLeft();
    }
    return succ;
  }
  else
  {
    bool found =0;
    while(succ->getParent() != nullptr)
    {
      if( succ->getParent()->getLeft() != nullptr && succ == succ->getParent()->getLeft()) 
      {
        succ=succ->getParent();
        found=1;
        break;
      }
      succ=succ->getParent();
    }
    if(!found)
    {
      succ=nullptr;
    }
    return succ;
  }
}

#endif