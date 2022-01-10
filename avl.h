// -*- c++ -*-
#ifndef _AVL_H_
#define _AVL_H_
#define DEBUG
#include <stdlib.h>
#include <assert.h>
#include <functional> // for std::less
#ifdef DEBUG
#include <iostream>
#include <fstream>
using namespace std;
#endif

struct AvlException : public std::exception {
  const char* file;
  int line;
  const char* msg;
public:
  AvlException(const char* file_, int line_,const char * msg_) :
    file(file_),line(line_), msg(msg_) { } 
};

#define AVL_EXCEPTION(x) { throw AvlException(__FILE__,__LINE__,x);}

/**
 * The maximum depth an AVL should have.
 */
#define MAX_AVL_DEPTH 100
#ifdef DEBUG
/**
 * In debug mode this macro enables to locate an internal error 
 */
#define AVL_INTERNAL_ERROR { cerr << __FILE__ << ":" << __LINE__ << ": INTERNAL ERROR IN AVL LIB." << endl; exit(1); } 
#else
/**
 * In non debug mode this macro generates an assert when an 
 * internal error occurs.
 */
#define AVL_INTERNAL_ERROR assert(0);
#endif



/**
 * This class is used for its operator() method
 * which enables to compare memory location of two pointers.
 * This is used in Avl trees which holds pointers.
 * For instance to create an Avl tree which store elements
 * of type <tt>Foo*</tt>, write:
 * <pre>
 * Avl<Foo*,PtrCompare> myTree;
 * myTree.insert(new Foo());
 * </pre>
 */
class PtrCompare {
public:
  int operator()(const void * const & p1,
		 const void * const & p2) {
    
    return u(p1,p2);
  }
private:
  ::std::less<const void* const > u;
};


/**
 * A node in the avl tree.
 * Template parameter T represents the type of value of nodes.
 */
template<class T>
class AvlNode {
 public :
  /**
   * Constructor from a given value.
   */
  AvlNode(const T& t) : value(t) {
    balance=0;
    left=nullptr;
    right=nullptr;
  }
  /**
   * Copy constructor
   */
  AvlNode(const AvlNode<T> & avl) {
    value=avl.value;
    balance=avl.balance;
    left=nullptr; right=nullptr;
  }
  /**
   * The destructor which does nothing, since no memory
   * was allocated.
   */
  ~AvlNode() { }
  AvlNode<T> & operator=(const AvlNode<T> & a) {
    value=a.value;
    balance=a.balance;
    left=nullptr;
    right=nullptr;
    return *this;
  }
  /**
   * Returns the value held by the node.
   */
  const T& getValue() const { return value;}
  /**
   * The value held by the node.
   */
  T value;
  /**
   * The balance factor.
   */
  int balance;
  /**
   * Pointer to the left child.
   */
  AvlNode<T> * left;
  /**
   * Pointer to the right child.
   */
  AvlNode<T> * right;
#ifdef DEBUG
  /**
   * In debug this variable is used to check that leftDepth-rightDepth
   * is in the range [-1;1].
   */
  int leftDepth;
  /**
   * In debug this variable is used to check that leftDepth-rightDepth
   * is in the range [-1;1].
   */
  int rightDepth;
#endif
};

/**
 * This clas enables to iterate on all elements located in an Avl tree.
 * In the following example, we call <tt>method1</tt> on all elements
 * of tree <tt>myTree</tt>.
 * <pre>
 * Avl<Foo*,PtrCompare> myTree;
 * //...
 * for (AvlIterator<Foo*> i = myTree.begin();!i.isLast();++i) {
 *   (*i)->method1();
 * }
 * </pre>
 */
template<class T>
class AvlIterator {
 public:
  /**
   * This builds an iterator on a given node.
   * The Avl::begin method is equivalent to 
   * calling this constructor with the root of the tree.
   */
  AvlIterator(AvlNode<T> * c) {
    current=c;
    tableIndex=0;
    for (int i=0;i<MAX_AVL_DEPTH;++i) st[i]=nullptr;
  }
  /**
   * this is the copy constructor on iterators.
   */
  AvlIterator(const AvlIterator & i) {
    for (int ii=0;ii<MAX_AVL_DEPTH;++ii) st[ii]=i.st[ii];
    tableIndex=i.tableIndex;
    current=i.current;
  }
  /**
   * Returns true if there is no more values to look.
   */
  int isLast() {
    return current==nullptr;
  }
  /**
   * Returns the value pointed to by the iterator.
   * Returned result is only valid if AvlIterator::isLast()
   * is false.
   */
  T operator*() {
    return current->value;
  }
  /**
   * Go to next value for this iterator.
   * If there is no more values (e.g., AvlIterator::isLast()
   * is false) then this operator does nothing.
   */
  AvlIterator & operator++() { // prefix
    if (current==nullptr) return *this;
    if (current->right==nullptr) {
      if (current->left==nullptr) {
	if (tableIndex<1) {
	  // this is last argument.
	  current=nullptr;
	} else {
	  current=st[tableIndex-1]->right;
	  tableIndex--;
	}
      } else {
	// right==nullptr and left!=nullptr
	current=current->left;
      }
    } else {
      if (current->left==nullptr) {
	current=current->right;
      } else {
	// rember to explore right branch
	st[tableIndex]=current;
	++tableIndex;
	current=current->left;
      }
    }
    return *this;
  }
 private :
  /**
   * An array which store the current position in the Avl tree.
   */
  AvlNode<T> * st[MAX_AVL_DEPTH];
  /**
   * Points to the current node.
   * This is nullptr when isLast returns true.
   */
  AvlNode<T> * current;
  /**
   * Index to read the AvlIterator::st table.
   */
  int tableIndex;
};


#ifdef DEBUG
/**
 * Display an avl node on a stream
 */
template<class T>
ostream & operator<<(ostream & os,const AvlNode<T> & r) {
  os << r.balance << "-" << r.value.get();
  //os << r.balance ;
  return os;
}
#endif

#define AvlCompareEquals(x,y) ((!compare(x,y)) && (!compare(y,x)))

/**
 * An avl tree.
 * Template parameter T represents the type of value of nodes.
 * Template parameter Compare is a comparison class which
 * has an operartor() member, an example is the PtrCompare
 * class.
 */
template<class ValueType, class Compare = PtrCompare>
class Avl {
 private :
  /**
   * Private type used for name of nodes.
   */
  typedef AvlNode<ValueType> AvlNodeType;
 public :
  /**
   * Type used to iterate on nodes in the avl tree.
   */
  typedef AvlIterator<ValueType> iterator;

  /**
   * Initializes an empty avl structure.
   */
  Avl() {
    head=nullptr;
    mySize=0;
    /*
    ValueType t = ValueType();
    head = new AvlNode<ValueType>(t);
    */
  }

  /**
   * Copy constructor.
   */
  Avl(const Avl<ValueType,Compare> & a) {
    head=copyNodeRec(a.head);
    mySize=a.mySize;
  }

  /**
   * Deletes nodes required to store values.
   */
  ~Avl() { 
    deleteFromNode(head);
    head=nullptr;
    mySize=0;
  }

  /**
   * Copies an avl tree in another one.
   * @param a tree to copy
   * @return reference to this instance.
   */
  Avl<ValueType,Compare> &
  operator=(const Avl<ValueType,Compare> & a) {
    deleteFromNode(head);
    mySize=a.mySize;
    head=copyNodeRec(a.head);
    return *this;
  }

  /**
   * Insert a new value in the avl.
   * Source of this method comes from page 462 of the art
   * of computer programming volume 3, 2nd edition,
   * by Donald E. Knuth.
   * @param t value to insert in Avl tree.
   */
  void insert(const ValueType& t) {
    if (head==nullptr) {
      head = new AvlNode<ValueType>(t);
      mySize=1;
      return;
    }
    if (AvlCompareEquals(head->getValue(),t)) {
      return; // already inserted
    }
    if (head->right==nullptr) {
      head->right = new AvlNode<ValueType> (t);
      mySize++;
      return;
    }
    // notation a1..a10 and i, ii, iii refer to page 462 of the art
    // of computer programming volume 3 second edition.

    // a1
    AvlNodeType * T = head;
    AvlNodeType * S = head->right;
    AvlNodeType * P = head->right;
    AvlNodeType * Q = nullptr;
    bool locationFound=false;
    while (!locationFound) {
    // a2
      if (AvlCompareEquals(P->getValue(),t)) return; // already inserted.
      if (compare(t,P->getValue())) {
    // a3
	Q=P->left;
	if (Q==nullptr) {
	  locationFound=true;
	  Q= new AvlNode<ValueType>(t);
	  mySize++;
	  P->left=Q;
	}
      } else {
     // a4
	Q=P->right;
	if (Q==nullptr) {
	  locationFound=true;
	  Q= new AvlNode<ValueType>(t);
	  mySize++;
	  P->right=Q;
	}
      }
      if (!locationFound) {
	if (Q->balance!=0) {
	  T=P;
	  S=Q;
	}
	P=Q;
      }
    }
    // a5 insert (done in constructor) 
    // a6 adjuste balanced factors
    AvlNode<ValueType> * R = nullptr;
    if (compare(t,S->getValue())) {
      R=P=S->left;
    } else {
      R=P=S->right;
    }
    while (P!=Q) {
      if (compare(t,P->getValue())) {
	P->balance=-1;
	P=P->left;
      } else if (compare(P->getValue(),t)) {
	P->balance=1;
	P=P->right;
      } else {
	AVL_INTERNAL_ERROR;
      }
    }
    // a7 balancing act
    int a;
    if (compare(t,S->getValue())) {
      a=-1;
    } else {
      a=1;
    }
    // case -i-
    if (S->balance==0) {
      S->balance=a;
      return;
    }
    // case -ii-
    if (S->balance==-a) {
      S->balance=0;
      return;
    }
    // case -iii-
    if (S->balance!=a) {
      AVL_INTERNAL_ERROR;
    }
    if (R->balance==a) {
    // a8 single rotation
      P=R;
      if (a==1) {
	S->right=R->left;
	R->left=S;
      } else {
	S->left=R->right;
	R->right=S;
      }
      S->balance=R->balance=0;
    } else if (R->balance==-a) {
    // a9 double rotation
      if (a==1) {
	P=R->left;
	R->left=P->right;
	P->right=R;
	S->right=P->left;
	P->left=S;
      } else {
	P=R->right;
	R->right=P->left;
	P->left=R;
	S->left=P->right;
	P->right=S;
      }
      if (P->balance==a) {
	S->balance=-a; R->balance=0;
      } else if (P->balance==0) {
	S->balance=0; R->balance=0;
      } else if (P->balance==-a) {
	S->balance=0; R->balance=a;
      } else {
	AVL_INTERNAL_ERROR;
      }
      P->balance=0;
    } else {
	AVL_INTERNAL_ERROR;
    }
    // a10
    if (S==T->right) {
      T->right=P;
    } else {
      T->left=P;
    }
  }

  /**
   * Removes a value from the avl tree.
   * Source of this method comes from 
   * <a href="https://benpfaff.org/avl/algorithm.ps">https://benpfaff.org/avl/algorithm.ps</a>
   * @param t value to remove from the Avl tree.
   */
  void remove(const ValueType & t) {
    if (head==nullptr) {
      // empty avl, nothing to remove
      return;
    }
    if (AvlCompareEquals(head->value,t)) {
      cout << "we are going to remove value at head" << endl;
      if (head->right==nullptr) {
	// this is the last node, so delete head
	delete head;
	head=nullptr;
	mySize=0;
	return;
      } else {
	cout << "there is another value in the avl, so swap it" << endl;
	cout << "so that we can just apply standard algo" << endl;
	//cout << "head->value=" << (head->value) << endl;
	//cout << "head->right->value=" << (head->right->value) << endl; 
	/*
	ValueType & tmp = head->right->value;
	cout << "tmp=" << tmp << endl; 
	//	std::cout << " ZZZZZZZ " << head->right->value << std::endl;
	//remove(tmp);
        head->right->value=head->value;
	head->value=tmp;
	*/
	AvlNodeType * tmp = head->right;
	AvlNodeType * tmp_r = head->right->right;
	AvlNodeType * tmp_l = head->right->left;
	head->right->right=head;
	head->right->left=nullptr;
	head->right=tmp_r;
	head->left=tmp_l;
	head = tmp;
	//cout << "head->value=" << (head->value) << endl;
	//cout << "head->right->value=" << (head->right->value) << endl;
      }
    }
    // notation d1..d13 refer to https://benpfaff.org/avl/algorithm.ps
    if (head->right==nullptr) return;
    int tableSize=MAX_AVL_DEPTH;
    AvlNodeType * st[tableSize];
    int a[tableSize];
    // d1: initialize
    st[0]=head;
    a[0]=1;
    int k=1;
    AvlNodeType * P = head->right;
    AvlNodeType * S = nullptr;
    AvlNodeType * R = nullptr;
    // d2: compare
    bool found=false;
    while (!found) {
      if (AvlCompareEquals(P->value,t)) {
	found=true;
	mySize--;
      } else {
	st[k]=P;
	if (compare(t,P->value)) {
	  // d3
	  a[k]=-1;
	  P=P->left;
	} else {
	  // d4
	  a[k]=1;
	  P=P->right;
	}
	++k;
	if (k==tableSize) { AVL_INTERNAL_ERROR; }
	if (P==nullptr) return; // element not in tree
      }
    }
    cout << "element is in tree" << endl;
    bool adjustBalanceNow=false;
    // d5: is rlink null ?
    AvlNodeType ** Q = nullptr;
    if (a[k-1]==1) {
      Q=&(st[k-1]->right);
    } else {
      Q=&(st[k-1]->left);
    }
    if (P->right==nullptr) {
      *Q=P->left;
      if (*Q!=nullptr) {
	(*Q)->balance=0;
      }
      adjustBalanceNow=true;
    }
    if (!adjustBalanceNow) {
      // d6: find successor
      R = P->right;
      if (R->left==nullptr) {
	R->left=P->left;
	*Q=R;
	R->balance=P->balance;
	a[k]=1;
	st[k]=R;
	++k;
	if (k==tableSize) { AVL_INTERNAL_ERROR; }
      } else {
	// d7: set up to find null left link
	S=R->left;
	int l=k;
	++k;
	if (k==tableSize) {
	  AVL_INTERNAL_ERROR;
	}
	a[k]=-1;
	st[k]=R;
	++k;
	if (k==tableSize) { AVL_INTERNAL_ERROR; }
	// d8: find null left link
	while (S->left!=nullptr) {
	  R=S;
	  S=R->left;
	  a[k]=-1;
	  st[k]=R;
	  ++k;
	  if (k==tableSize) { AVL_INTERNAL_ERROR; }
	}
	// d9: fix up
	a[l]=1;
	st[l]=S;
	S->left=P->left;
	R->left=S->right;
	S->right=P->right;
	S->balance=P->balance;
	*Q=S;
      }
    }
    // d10: adjust balance factors
    while (1) {
      --k;
      if (k==0) {
	delete P;
	return;
      }
      S=st[k];
      if (S->balance==0) { 
	// step -i-
	S->balance=-a[k]; 
	delete P;
	return;
      } else if (S->balance==a[k]) {
	// step -ii-
	S->balance=0;
      } else if (S->balance==-a[k]) {
	// step -iii-
	R=link(-a[k],S);
	if (R->balance==0) {
	  // d11: single rotation with balanced R
	  if (-a[k]==1) {
	    S->right= R->left;
	    R->left=S;
	  } else {
	    S->left= R->right;
	    R->right=S;
	  }
	  R->balance=a[k];
	  if (a[k-1]==1) {
	    st[k-1]->right=R;
	  } else {
	    st[k-1]->left=R;
	  }
	  delete P;
	  return;
	} else if (R->balance==-a[k]) {
	  // d12: single rotation with unbalanced R
	  if (-a[k]==1) {
	    S->right= R->left;
	    R->left=S;
	  } else {
	    S->left= R->right;
	    R->right=S;
	  }
	  S->balance=R->balance=0;
	  if (a[k-1]==1) {
	    st[k-1]->right=R;
	  } else {
	    st[k-1]->left=R;
	  }
	} else if (R->balance==a[k]) {
	  // d13: double rotation
	  AvlNodeType * PP=link(a[k],R);
	  if (a[k]==1) {
	    R->right=PP->left;
	    PP->left=R;
	    S->left=PP->right;
	    PP->right=S;
	  } else {
	    R->left=PP->right;
	    PP->right=R;
	    S->right=PP->left;
	    PP->left=S;
	  }
	  if (PP->balance==-a[k]) {
	    S->balance=a[k];
	    R->balance=0;
	  } else if (PP->balance==0) {
	    S->balance=0;
	    R->balance=0;
	  } else if (PP->balance==a[k]) {
	    R->balance=-a[k];
	    S->balance=0;
	  } else {
	    AVL_INTERNAL_ERROR;
	  }
	  PP->balance=0;
	  if (a[k-1]==1) {
	    st[k-1]->right=PP;
	  } else {
	    st[k-1]->left=PP;
	  }
	} else {
	  AVL_INTERNAL_ERROR;
	}
      } else {
	AVL_INTERNAL_ERROR;
      }
    }
    delete P;
  }

  /**
   * Returns a value in the avl tree.
   * @param t the value we are looking for.
   * @return nullptr if no node in tree has value t,
   * or a pointer to the value equals to t which
   * is present in the tree.
   */
  ValueType * get(const ValueType& t) const {
    if (head==nullptr) return nullptr;
    if (AvlCompareEquals(head->value,t)) {
      return &(head->value);
    }
    AvlNodeType * P = head->right;
    while (P!=nullptr) {
      if (AvlCompareEquals(t,P->value)) return &(P->value);
      else if (compare(P->value,t)) P=P->right;
      else if (compare(t,P->value))  P=P->left;
    }
    return nullptr;
  }
  /**
   * Same as <tt>ValueType * get(const ValueType& t) const</tt>
   */
  ValueType * get(const ValueType& t) {
    if (head==nullptr) return nullptr;
    if (AvlCompareEquals(head->value,t)) {
      return &(head->value);
    }
    AvlNodeType * P = head->right;
    while (P!=nullptr) {
      if (AvlCompareEquals(t,P->value)) return &(P->value);
      else if (compare(P->value,t)) P=P->right;
      else if (compare(t,P->value))  P=P->left;
    }
    return nullptr;
  }
  
#ifdef DEBUG
  /**
   * Display the tree on stdout.
   */
  void display() { display(head); cout << endl;}
  /**
   * Display the tree using dot.
   */
  void toDot(string fileName) {
    ofstream ofs (fileName.c_str());
    ofs << "digraph \"" << fileName << "\" {\n";
    toDot(ofs,head->right);
    ofs << "}\n";
    ofs.close();
    char buf[1000];
    sprintf(buf,"dotty %s",fileName.c_str());
    system(buf);
  }
  /**
   * Check balance values in the tree.
   */
  void check() {
    if (head==nullptr) return;
    updateDepth(head->right);
  }
#endif
  /**
   * Returns the number of values currently in the tree.
   */
  int size() const { return mySize;}
  /**
   * Return an AvlIterator which will enable to walk
   * through all elements currently in the tree.
   * Here is a typical usage:
   * <pre>
   * Avl<Foo*,PtrCompare> myTree;
   * //...
   * for (AvlIterator<Foo*> i = myTree.begin();!i.isLast();++i) {
   *   (*i)->method1();
   * }
   * </pre>
   */
  iterator begin() const {
    return AvlIterator<ValueType>(head);
  }
  /**
   * Removes all elements in this tree.
   */
  void clear() {
    if (head==nullptr) return;
    deleteFromNode(head->right);
    mySize=0;
    head->right=nullptr;
    head=nullptr;
  }
  /**
   * calls delete on all elements of the avl
   */
  void deleteAll() {
    for (AvlIterator<ValueType> i = begin();!i.isLast();++i) {
      delete *i;
    }
  }
 protected:
  /**
   * Holds the number of elements currently in the tree.
   */
  int mySize;
  /**
   * Somehting which holds the root of the tree in
   * head->right.
   */
  AvlNodeType * head;
  /**
   * An instance of the class used to compare elements.
   */
  Compare compare;
  /**
   * Removes recursively a node and its sub nodes.
   * @param n node to remove.
   */
  void deleteFromNode(AvlNodeType * n) {
    if (n==nullptr) return;
    if (n->right!=nullptr) {
      deleteFromNode(n->right);
      n->right=nullptr;
    }
    if (n->left!=nullptr) {
      deleteFromNode(n->left);
      n->left=nullptr;
    }
    delete n;
  }
  /**
   * Returns P->right if a==1,
   * and P->left if a==-1.
   */
  AvlNodeType * link(int a,AvlNodeType *P) {
    if (P==nullptr) {AVL_INTERNAL_ERROR;}
    if (a==1) { return P->right;}
    if (a==-1) { return P->left;}
    AVL_INTERNAL_ERROR;
    return nullptr;
  }
  /**
   * Copies recursively a node of an Avl tree.
   * @param n node to copy.
   * @return copy of node n.
   */
  AvlNodeType * copyNodeRec(AvlNodeType * n) {
    if (n==nullptr) return nullptr;
    AvlNodeType * answer = new AvlNode<ValueType>(*n);
    answer->right=copyNodeRec(n->right);
    answer->left=copyNodeRec(n->left);
    return answer;
  }
#ifdef DEBUG
  /**
   * Display a node on stdout.
   */
  void display(AvlNodeType * n) {
    if (n==nullptr) return;
    cout << " " << n << "[" << *n  << "]" << "(" << n->balance;
    display(n->right);
    cout << ",";
    display(n->left);
    cout << ")";
  }
  /**
   * Generate a dot file, to display the tree.
   */
  void toDot(ostream & os, AvlNodeType * n) {
    if (n==nullptr) return;
    if (n->right!=nullptr) {
      os << " \"" << *n << "\" -> \"" << *(n->right) << "\"\n";
      toDot(os,n->right);
    }
    if (n->left!=nullptr) {
      os << " \"" << *n << "\" -> \"" << *(n->left) << "\"\n";
      toDot(os,n->left);
    }
  }
  /**
   * Perform a computation of the left and right
   * depth to validate the balance value.
   */
  void updateDepth(AvlNodeType * n) {
    if (n==nullptr) return;
    n->rightDepth=0;
    n->leftDepth=0;
    if (n->right!=nullptr) {
      updateDepth(n->right);
      if (n->right->leftDepth > n->right->rightDepth) {
	n->rightDepth=n->right->leftDepth+1;
      } else {
	n->rightDepth=n->right->rightDepth+1;
      }
    }
    if (n->left!=nullptr) {
      updateDepth(n->left);
      if (n->left->leftDepth > n->left->rightDepth) {
	n->leftDepth=n->left->leftDepth+1;
      } else {
	n->leftDepth=n->left->rightDepth+1;
      }
    }
    if (n->balance!=n->rightDepth-n->leftDepth) {
      AVL_INTERNAL_ERROR;
    }
    if (n->balance>1 || n->balance<-1) {
      AVL_INTERNAL_ERROR;
    }
  }
#endif
};

#undef AvlCompareEquals


#endif


