# avllib
a simple stand alone C++ avl library.

## Example

Two full examples are in `avl_test.cc` and `avlmap_test.cc`.

Simple example
```
#include "avlmap.h"
#include <iostream>
class A {
public:
  A(int i): n(i) {}
  int n;
};
int main () {
  A a(1);	
  Map<A*,int> map;
  map.insert(&a,1);
  return 1 - *(map.get(&a)); // returns 0
}
```
