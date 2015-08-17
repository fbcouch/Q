# Q

A simple implementation of Q in C++. If you love promises, you'll love this.

## Usage:

```c++
#include "Q.h"

Promise *my_promise_method() {
  Deferred *deferred = Q::defer();

  // do something...
  if (/* we want to resolve the promise */) {
    deferred->resolve(PromiseVal::value((void*)true));
  } else {
    // we want to reject the promise
    deferred->reject((void*)"Something bad happened");
  }

  // return the promise
  return deferred->promise();
}

// call then
my_promise_method()->then([](PromiseVal *value) -> PromiseVal * {
  // success

  // extract your data from the value
  bool my_bool = value->getData<bool>();

  //...

  return PromiseVal::null(); // return a PromiseVal (for chainability)
}, [](void *error) {
  // error
}, []() {
  // finally
});

```

Promise.then returns a promise, so you can chain then methods. You can also return a promise value with PromiseVal::promise, and Q will wait for that promise to be resolved before continuing down the chain.
