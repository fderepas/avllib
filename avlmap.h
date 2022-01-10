// -*- c++ -*-
#ifndef _AVLMAP_H_
#define _AVLMAP_H_

#include "avl.h"

#include <exception>

class MapException: public std::exception {
  virtual const char* what() const throw() {
    return "My exception happened";
  }
};

/**
 * This class represents a pair which will be used
 * to store data in the map.
 * General philosophy is to throw exceptions when trying to
 * read a key/value which has not been initilized.
 */
template <class Key, class Value>
class MapPair {
public :
  MapPair(): key (), value() {noValue=true;noKey=true;}
  MapPair(Key k): key(k), value() { noValue=true;noKey=false;}
  MapPair (Key k, Value & v) : key(k), value(v) {
    noValue=false;noKey=false;
  }
  MapPair(const MapPair<Key,Value>& mp) : key(mp.key), value (mp.value) {
    noValue=mp.noValue;noKey=mp.noKey;
  }
  const Key & getKey() const { 
    if (noKey) throw MapException();
    return key;
  }
  Key & getKey()  {
    if (noKey) throw MapException();
    return key;
  }
  const Value & getValue() const {
    if (noValue) throw MapException();
    return value;
  }
  Value & getValue() { 
    if (noValue) throw MapException();
    return value;
  }
  void setValue(Value & v) { value=v;}
  bool hasValue() { return !noValue;}
  bool hasKey() {return !noKey;}
private :
  Key key;
  Value value;
  bool noValue;
  bool noKey;
};

/**
 * This class enables to compare two pairs
 * by just looking at the first element.
 * This class is used to order pairs in the Avl
 * tree representing the map.
 */
template<class Key, class Value, class Compare>
class PairCompare {
public:
  /**
   * Function to compare two pairs.
   * @param v1 a pair.
   * @param v2 a pair
   * @return 1 if the key of v1
   * is stictly lower than the key of v2.
   * Returns 0 otherwise.
   */
  int operator() (const MapPair<Key,Value>& v1,
                  const MapPair<Key,Value>& v2) const
  {
    Compare cmp;
    return cmp(v1.getKey(),v2.getKey());
  }
private:
  
};

template<class Key, class Value>
class RefCompare {
public:
  //::std::less<const Key > compareFromStl;

  /**
   * Function to compare two pairs.
   * @param v1 a pair.
   * @param v2 a pair
   * @return 1 if first element of v1
   * is stictly lower than first element of v2.
   * Returns 0 otherwise.
   */
  int operator() (const MapPair<Key,Value>& v1,
                  const MapPair<Key,Value>& v2) const
  {
    //return compareFromStl(v1.getKey(),v2.getKey());
    return &(v1.getKey().get())<&(v2.getKey().get());
  }
};

/**
 * Class which enables to iterate on objects in a map.
 * This class is basically a wrapper of an iterator on an 
 * Avl.
 */
template<class Key, class Value>
class MapIterator {
public:
  typedef AvlIterator<MapPair<Key, Value> > AvlIter;
  MapIterator(AvlIter _avlIter) : avlIter(_avlIter) { }
  MapIterator() : avlIter(nullptr) { }
  /**
   * Returns the current value associated with
   * the iterator.
   */
  Value value() {
    if (avlIter.isLast() || !hasValue()) {
      throw MapException();
    }
    return (*avlIter).getValue();
  }
  /**
   * Returns the current key associated with the 
   * iterator.
   */
  Key key() {
    if (avlIter.isLast()) throw MapException();
    if (!hasKey()) throw MapException();
    return (*avlIter).getKey();
  }
  /**
   * Returns the current key associated with the 
   * iterator.
   */
  Key& getKey() {
    if (avlIter.isLast()) throw MapException();
    if (!hasKey()) throw MapException();
    return (*avlIter).getKey();
  }
  /**
   * Tells if this iterator is the last one or not.
   */
  int isLast() {
    return avlIter.isLast();
  }
  /**
   * Switch to next key/value pair.
   * Nothing is done if isLast() returns true.
   */
  MapIterator & operator++() { // prefix
    ++avlIter;
    return *this;
  }
  /** 
   * tells if a value is stored in this iterator.
   */
  bool hasValue() {
    return (*avlIter).hasValue();
  }
  /** 
   * tells if a key is stored in this iterator.
   */
  bool hasKey() {
    return (*avlIter).hasKey();
  }
  /**
   * Returns the pair associated with this iterator.
   */
  MapPair<Key, Value> operator*() {
    return avlIter.operator*();
  }
private :
  AvlIter avlIter;
};

/**
 * This class represents a map.
 * Key the class used as a key, and retreived
 * values will be instances of class Value.
 * Here is a small example which create a 
 * <tt>phoneNumber</tt> map:
 * <pre>
 *  Map<string,string> phoneNumber;
 *  phoneNumber.insert("robert","456 12 23");
 *  phoneNumber.insert("alan","345 31 44");
 *  cout << "Robert phone is " 
 *       << *(phoneNumber.get("robert")) 
 *       << endl;
 * </pre>
 * To display all entries in a map:
 * <pre>
 * for (Map<string,string>::iterator i = phoneNumber.begin();
 *      !i.isLast();++i) {
 *   cout << i.key() << " " << i.value() << endl;
 * }
 * </pre>
 */
template<class Key, class Value, class Compare=::std::less<const Key > >
class Map {
public :
  typedef MapPair<Key,Value> MyMapPair;
  typedef Avl< MyMapPair, PairCompare < Key,Value, Compare > > MapAvl;
  
  typedef MapIterator<Key,Value> iterator;
  /**
   * Builds an empty map.
   */
  Map() {}
  /**
   * Release memory allocated for the map.
   */
  ~Map() {
    clear();
  }
  /**
   * Insert a key/value pair in the map.
   * @param k a key,
   * @param v a value.
   */
  void insert(Key k, Value v) {
    const MapPair<Key,Value> pi (k,v);
    MapPair<Key,Value> * p = mapAvl.get(pi);
    if (p==nullptr) {
      mapAvl.insert(pi);
    } else {
      p->setValue(v);
    }
  }
  /**
   * Remove a key/value pair in the map.
   * @param k a key,
   * @param v a value.
   */
  void remove(Key k) {
    MapPair<Key,Value> pi (k);
    mapAvl.remove(pi);
  }
  /**
   * Tells if a key is present.
   */

  bool has(const Key & k) const {
    MapPair<Key,Value> pi (k);
    MapPair<Key,Value> * p = mapAvl.get(pi);
    if (p==nullptr) return false;
    return true;
  }
  /**
   * Retreive a value associated with a key.
   */
  Value * get(const Key & k) const {
    MapPair<Key,Value> pi (k);
    MapPair<Key,Value> * p = mapAvl.get(pi);
    if (p==nullptr) return nullptr;
    if (!(p->hasValue())) return nullptr;
    return &(p->getValue());
  }
  
  /**
   * Retreive a value associated with a key.
   * Raises an exception when not found
   */
  Value & operator[] (const Key & k) const {
    MapPair<Key,Value> pi (k);
    MapPair<Key,Value> * p = mapAvl.get(pi);
    if (p==nullptr) AVL_EXCEPTION("not found");
    if (!(p->hasValue())) AVL_EXCEPTION("not found");
    return p->getValue();
  }
  
  /**
   * Erase all elements in the map.
   */
  void clear() {
    if(mapAvl.size()>0) {
      mapAvl.clear();
    }
  }
  /**
   * Returns the number of associations in the map.
   */
  int size() const { return mapAvl.size();}
  /**
   * Returns an iterator which start at the first
   * element of the Map.
   */
  iterator begin() const {
    if (mapAvl.size()==0) return MapIterator<Key,Value>();
    return MapIterator<Key,Value>(mapAvl.begin()); 
  }

  Map<Key,Value,Compare> & operator=(const Map<Key,Value,Compare> & m) { 
    mapAvl.clear();
    MapIterator<Key,Value> mi = m.begin();
    while (!mi.isLast()) {
      mapAvl.insert(*mi);
      ++mi;
    }
    return *this;
  }
protected :
  /**
   * Avl tree in which the map is stored.
   */
  MapAvl mapAvl;
};


#endif
