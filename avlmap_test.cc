#include "avlmap.h"
#include <sstream>
#include <iostream>
#include <algorithm>

#define ERR(x) { cerr << __FILE__ << ":" << __LINE__ << ": " << x << endl; exit(1); }
using namespace std;

class A {
public:
  A(const A&a) { this->a=a.a;}
  A(int i) { a=i;}
  A() { a=0;}
  int a;
  bool operator==(const A &b) const {
    return a==b.a;
  }
  bool operator!=(const A &b) const {
    return a!=b.a;
  }
};

class CompareAStar {
public:
  bool operator()(const A*l, const A*r) const  {
    return l->a < r->a;
  }
};

// class B has private constructors
// to check that we really use references
// the "create" static method should be used
// to create new instances
class B {
public:
  static B& create(int i) {
    return *(new B(i));
  }
  int a;
  bool operator==(const A &b) const {
    return a==b.a;
  }
  bool operator!=(const A &b) const {
    return a!=b.a;
  }
private:
  B(const B&a) { this->a=a.a;}
  B(int i) { a=i;}
  B() { a=0;}
  
};
ostream & operator<<(ostream & os,const A & r) {
  //os << r.balance << "-" << r.value;
  os << "A(" << r.a  << ")";
  return os;
}

void testinit() {
  /* initialize random seed: */
  srand (time(NULL));
}
void testinit(int s) {
  /* initialize random seed: */
  srand (s);
}

int myrand (int max) {
  return rand() % max;
}

string randString() {
  ostringstream oss;
  for (int i=0;i<8;++i) {
    oss << ((char)(myrand(26)+'a'));
  }
  return oss.str();
}

#define MAX_CHILDS 10

class Nn {
public:
  // create random nodes
  static Nn * random(int proba=100) {
    if (myrand(100)<proba) {
      Nn * nn = new Nn(randString());
      Nn * answer = new Nn (nn);
      int imax=myrand(5);
      float factor=0.85;
      if (proba<50) {
	factor=0.4;
      }
      for (int i=0;i<imax;++i) {
	answer->childs[i]=random(((int)proba*factor));
      }
      return answer;
    }
    return nullptr;
  }
  
  Nn * childs[MAX_CHILDS];
  string getName() { return name;}
  Nn * getParent() { return parent; }
  // class to compare nodes
  class ValueCompare {
  public:
    // crashes if p1 or p2 is null
    // or if p1->parent or p2->parent is null
    bool operator()(const Nn * p1,
		    const Nn * p2)
    {
      if (stringLexicographic(p1->parent->name,
			      p2->parent->name))
	return true;
      if (stringLexicographic(p2->parent->name,
			      p1->parent->name))
	return false;
      for (int i=0;i<MAX_CHILDS;++i) {
	if ((p1->childs[i]==nullptr) &&
	    (p2->childs[i]==nullptr)) {
	  // keep checking
	} else if (p1->childs[i]==nullptr) {
	  // so p2 is not null and we consider
	  // nullptr < "valid string"
	  return true;
	} else if (p2->childs[i]==nullptr) {
	  // so p1 is not null and we consider
	  // "valid string" > nullptr
	  return false;
	} else {
	  // there are childs in both nodes
	  if (this->operator()(p1->childs[i],p2->childs[i]))
	    return true;
	  if (this->operator()(p2->childs[i],p1->childs[i]))
	    return false;
	}
      }
      return false;
    }
  private :
    ::std::less<string> stringLexicographic;
  };
  Nn* copy() {
    Nn* answer = new Nn(parent);
    for (int i=0;i<MAX_CHILDS;++i) {
      if (childs[i]!=nullptr) {
	answer->childs[i]=childs[i]->copy();
      }
    }
    return answer;
  }
  friend ostream & operator<<(ostream & os,const Nn & nn) {
    if (nn.parent==nullptr) {
      if (nn.name=="") {
	os << "z" << &nn;
      } else {
	os << nn.name;
      }
    } else {
      os << *(nn.parent);
    }
    bool hasChild=false;
    for (int i=0;i<MAX_CHILDS;++i) {
      if (nn.childs[i]!=nullptr) {
	if (!hasChild) {
	  os << "(";
	  hasChild=true;
	} else {
	  os << ",";
	}
	os << *(nn.childs[i]);
      }
    }
    if (hasChild) {
      os << ")";
    } else {
    }
    return os;
  }

  int size() {
    int answer = 1;
    for (int i=0;i<MAX_CHILDS;++i) {
      if (childs[i]!=nullptr) {
	answer+=childs[i]->size();
      }
    }
    return answer;
  }
private:
  // create a new node type corresponding to string s
  Nn(string s) {
    name=s;
    parent=nullptr;
    std::fill( std::begin( childs ), std::end( childs ), nullptr );
  }
  // create a new instance of node *p
  Nn(Nn * p) {
    parent=p;
    std::fill( std::begin( childs ), std::end( childs ), nullptr );
  }
  ~Nn() {}
  Nn * parent;
  string name;
};


// simple test
int test1 () {
  A a(1);
  A b(2);
  A c(3);
  A d(4);
  A e(5);
  Map<A*,int> map;
  map.insert(&a,1);
  map.insert(&b,2);
  map.insert(&c,3);
  map.insert(&d,4);
  map.insert(&e,5);
  //cout << *(map.get(&c)) << endl;
  return 0;
}

void permutArray(int arraySz,int *permut) {
  for (int i=0;i<arraySz;++i) {
    int j1 = myrand(arraySz);
    int j2 = myrand(arraySz);
    int tmp=permut[j1];
    permut[j1]=permut[j2];
    permut[j2]=tmp;
  }
}

// more intensive test 
int test2 () {
  int arraySz = myrand(100);
  cout << "Creating an array with " << arraySz << " elements." << endl;
  int permut[arraySz];
  for (int i=0;i<arraySz;++i) {
    permut[i]=i;
  }
  permutArray(arraySz,permut);
  A a[arraySz];
  for (int i=0;i<arraySz;++i) {
    a[i]=A(i);
  }
  Map<A*,int> map;
  for (int i=0;i<arraySz;++i) {
    map.insert(&a[permut[i]],permut[i]);
    if (map.size()!=i+1) {
      ERR("error in test");
    }
    // check the i elements of the hash table
    for (int k=0;k<=i;++k) {
      int *j = map.get(&a[permut[k]]);
      if (j==nullptr) {
	ERR("error in test");
      }
      if (*j!=permut[k]) {
	ERR("error in test");
      }
    }
  }
  // remove in a different order
  permutArray(arraySz,permut);
  for (int i=0;i<arraySz;++i) {
    map.remove(&a[permut[i]]);
    if (map.size()!=arraySz-i-1) {
      ERR("error in test");
    }
    // check the arraySz-i elements of the hash table
    for (int k=i+1;k<arraySz;++k) {
      int *j = map.get(&a[permut[k]]);
      if (j==nullptr) {
	ERR("error in test");
      }
      if (*j!=permut[k]) {
	ERR("error in test");
      }
    }
  }
  return 0;
}

// comparison test
int test3 () {
  int arraySz = 100;
  cout << "Creating an array with " << arraySz << " elements." << endl;
  int permut[arraySz];
  for (int i=0;i<arraySz;++i) {
    permut[i]=i;
  }
  permutArray(arraySz,permut);
  A a[arraySz];
  for (int i=0;i<arraySz;++i) {
    a[i]=A(i%10);
  }
  Map<A*,Avl<A*>*,CompareAStar> map;
  for (int i=0;i<arraySz;++i) {
    Avl<A*>**v = map.get(&a[permut[i]]);
    if (v==nullptr || *v==nullptr) {
      map.insert(&a[permut[i]],new Avl<A*>());
      v = map.get(&a[permut[i]]);
    }
    (*v)->insert(&a[permut[i]]);
  }

  for (Map<A*,Avl<A*>*,CompareAStar>::iterator i = map.begin();!i.isLast();++i) {
    if (10 != i.value()->size()) {
      ERR("error in test");
    }
  }
  
  return 0;
}

// comparison test
int test4 () {
  int arraySz = 100;
  cout << "Creating an array with " << arraySz << " elements." << endl;
  int permut[arraySz];
  for (int i=0;i<arraySz;++i) {
    permut[i]=i;
  }
  permutArray(arraySz,permut);
  Nn* a[arraySz];
  for (int i=0;i<arraySz;++i) {
    a[i]=Nn::random();
    cout << (a[i])->size() << endl;
  }
  Map<Nn*,Avl<Nn*>*,Nn::ValueCompare> map;
  for (int i=0;i<arraySz;++i) {
    Avl<Nn*>**v = map.get(a[permut[i]]);
    if (v==nullptr || *v==nullptr) {
      cout << "i=" <<i << endl; // breakpoint set --file avlmap_test.cc --line 339
      map.insert(a[permut[i]],new Avl<Nn*>());
      v = map.get(a[permut[i]]); 
    } else {
      ERR("error in test" << a[permut[i]]);
    }
    (*v)->insert(a[permut[i]]);
  }
  for (int k=0;k<10;++k) {
    for (int i=0;i<arraySz;++i) {
      Nn * tmp = a[permut[i]]->copy();
      Avl<Nn*>**v = map.get(tmp);
      if (v==nullptr || *v==nullptr) {
	ERR("error in test");
      } else {
	(*v)->insert(tmp);
      }
    }
  }

  for (Map<Nn*,Avl<Nn*>*,Nn::ValueCompare>::iterator i = map.begin();!i.isLast();++i) {
    if (11 != i.value()->size()) {
      ERR("error in test, got size " << i.value()->size());
    }
  }
  
  return 0;
}

int main () {
  testinit(0);
  if (test1()) { ERR("error in test"); }
  for (int i=0;i<100;++i) {
    if (test2()) { ERR("error in test2 step "<< i); }
  }
  if (test3()) { ERR("error in test"); }
  if (test4()) { ERR("error in test"); }
  return 0;
}
