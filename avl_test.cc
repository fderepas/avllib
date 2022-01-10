//#define DEBUG

#include "avl.h"
#include <functional>
#include <sstream>
#include <iostream>

class A {
public:
  A(int i) { a=i;}
  int a;
  friend ostream & operator<<(ostream & os,const A & r) {
    os << r.a;
    return os;
  }
};


/**
 * compare two references.
 */

class RefCompare {
public:
  RefCompare() {}
  ::std::less<const A*  > compareFromStl;

  int operator()(const A& p1,
		 const A& p2)
  {
    return compareFromStl(&p1,&p2);
  }
};

template<class T>
ostream & operator<<(ostream & os,const A & r) {
  os << r.a ;
  return os;
}

#define ERROR(x)  { std::cerr << __FILE__ << ":" << __LINE__ << ": " << x << std::endl; exit(1);}


bool contentIs(Avl<std::reference_wrapper<A>,RefCompare > & avl,
	       A &a) {
  if (avl.size()!=1) ERROR("should not happen.");
  if (avl.get(a)==nullptr) ERROR("should not happen.");
  int count=0;
  for (AvlIterator<std::reference_wrapper<A> > i = avl.begin();
       !i.isLast();++i) {
    count++;
  }
  if (count!=avl.size()) ERROR("should not happen.");
  return true;
}
bool contentIs(Avl<std::reference_wrapper<A>,RefCompare > & avl,
	       A &a,A&b) {
  if (avl.size()!=2) ERROR("should not happen.");
  if (avl.get(a)==nullptr) ERROR("should not happen.");
  if (avl.get(b)==nullptr) ERROR("should not happen.");
  int count=0;
  for (AvlIterator<std::reference_wrapper<A> > i = avl.begin();
       !i.isLast();++i) {
    count++;
  }
  if (count!=avl.size()) ERROR("should not happen.");
  return true;
}
bool contentIs(Avl<std::reference_wrapper<A>,RefCompare > & avl,
	       A &a,A&b,A&c) {
  if (avl.size()!=3) ERROR("should not happen.");
  if (avl.get(a)==nullptr) ERROR("should not happen.");
  if (avl.get(b)==nullptr) ERROR("should not happen.");
  if (avl.get(c)==nullptr) ERROR("should not happen.");
  int count=0;
  for (AvlIterator<std::reference_wrapper<A> > i = avl.begin();
       !i.isLast();++i) {
    count++;
  }
  if (count!=avl.size()) ERROR("should not happen.");
  return true;
}
bool contentIs(Avl<std::reference_wrapper<A>,RefCompare > & avl,
	       A &a,A&b,A&c,A&d) {
  if (avl.size()!=4) ERROR("should not happen.");
  if (avl.get(a)==nullptr) ERROR("should not happen.");
  if (avl.get(b)==nullptr) ERROR("should not happen.");
  if (avl.get(c)==nullptr) ERROR("should not happen.");
  if (avl.get(d)==nullptr) ERROR("should not happen.");
  int count=0;
  for (AvlIterator<std::reference_wrapper<A> > i = avl.begin();
       !i.isLast();++i) {
    count++;
  }
  if (count!=avl.size()) ERROR("should not happen.");
  
  return true;
}
bool contentIs(Avl<std::reference_wrapper<A>,RefCompare > & avl,
	       A &a,A&b,A&c,A&d,A&e) {
  if (avl.size()!=5) ERROR("should not happen.");
  if (avl.get(a)==nullptr) ERROR("should not happen.");
  if (avl.get(b)==nullptr) ERROR("should not happen.");
  if (avl.get(c)==nullptr) ERROR("should not happen.");
  if (avl.get(d)==nullptr) ERROR("should not happen.");
  if (avl.get(e)==nullptr) ERROR("should not happen.");
  int count=0;
  for (AvlIterator<std::reference_wrapper<A> > i = avl.begin();
       !i.isLast();++i) {
    count++;
  }
  if (count!=avl.size()) ERROR("should not happen.");
  return true;
}
void testReferences() {
  A a(1);
  A b(2);
  A c(3);
  A d(4);
  A e(5);
  /*
  cout << "&a="  << &a << endl;
  cout << "&b="  << &b << endl;
  cout << "&c="  << &c << endl;
  cout << "&d="  << &d << endl;
  cout << "&e="  << &e << endl;
  */
  {
    Avl<A*> avl;
    avl.insert(&a);
    avl.insert(&b);
    avl.insert(&c);
    avl.insert(&d);
    avl.insert(&e);
  }
  if (1) {
    Avl<std::reference_wrapper<A>,RefCompare > avl;
    if (avl.size()!=0) ERROR("should not happen.");
    avl.insert(std::reference_wrapper<A>(a));
    contentIs(avl,a);
    avl.insert(std::reference_wrapper<A>(a));
    contentIs(avl,a);
    avl.insert(std::reference_wrapper<A>(b));
    contentIs(avl,a,b);
    //avl.display();
    avl.insert(std::reference_wrapper<A>(b));
    contentIs(avl,a,b);
    avl.insert(std::reference_wrapper<A>(c));
    contentIs(avl,a,b,c);
    
    //avl.display();
    avl.insert(std::reference_wrapper<A>(d));
    contentIs(avl,a,b,c,d);
    avl.insert(std::reference_wrapper<A>(e));
    //avl.display();
    contentIs(avl,a,b,c,d,e);
    //cout << "removing a" << endl;
    avl.remove(std::reference_wrapper<A>(a));
    //avl.display();
    cout << "XXXX" << endl;
    contentIs(avl,b,c,d,e);
    cout << "XXXX" << endl;
    cout << "removing b" << endl;
    avl.remove(std::reference_wrapper<A>(b));
    //avl.display();
    contentIs(avl,c,d,e);
    cout << "removing c" << endl;
    avl.remove(std::reference_wrapper<A>(c));
    //avl.display();
    contentIs(avl,d,e);
    avl.remove(std::reference_wrapper<A>(d));
    //avl.display();
    contentIs(avl,e);
    avl.remove(std::reference_wrapper<A>(e));
    if (avl.size()!=0) ERROR("should not happen.");
    
  }
}


string generateRandomName(int min, int max) {
  if (min>=max) {
    ERROR("Min should be lower than max");
  }
  ostringstream oss;
  int len = random()%(max-min)+min;
  for (int i=0;i<len;++i) {
    oss << (char) ('a'+random()%26);
  }
  return oss.str();
}

void testString(int strMax=100) {
  string strTab[strMax];
  Avl<string,::std::less<string> > a;
  for (int i=0;i<strMax;++i) {
    strTab[i]=generateRandomName(10,20);
    a.insert(strTab[i]);
    if (a.size()!=i+1) ERROR("should not happen.");
  }
  for (int i=0;i<strMax;++i) {
    if (a.get(strTab[i])==nullptr) {
      ERROR("should not happen.");
    }
  }
  for (int i=0;i<strMax;++i) {
    a.remove(strTab[i]);
    if (a.size()!=strMax-i-1) ERROR("should not happen.");
  }
}

int main() {
  testReferences();
  testString();
  return 0;
}
