# (slh) simple_linear_hash
A simple and fast hash table that uses linear probing.

https://en.wikipedia.org/wiki/Linear_probing

slh is a simple implementation that is less than 300 lines of code written in C89. It contains one .c and .h file so it is extremely easy to integrate into one's project.

```
#include <stdio.h>
#include <assert.h>
#include "slh.h"

int main(void) {
    // allocate a new hash table context
    slh_ctx ht = slh_new();

    int i1 = 5;

    int* ret = slh_insert(ht, djb2("testing"), &i1);
    int * ret = slh_get(ht, djb2("testing"));

    assert(*ret == 5);

    // free the ht context
    slh_free(ht);
  return 0
}

```
compile this like so:

```
clang example.c slh.c -o example
```

I have also included extensive unit testing which stresses the slh's functionality. I have built this using Scons. To run, download scons via pip and simply

```
slh> scons
scons: Reading SConscript files ...
scons: done reading SConscript files.
scons: Building targets ...
gcc -o slh.o -c -g slh.c
gcc -o test.o -c -g test.c
gcc -o hash_test.exe slh.o test.o
hash_test.exe
Test stress test...                             [ OK ]
Test overwrite...                               [ OK ]
Test remove test...                             [ OK ]
Test test collision mid...                      [ OK ]
Test test collision edge...                     [ OK ]
SUCCESS: All unit tests have passed.
scons: done building targets.
```
