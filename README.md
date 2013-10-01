# Dynlib

Dynlib is a generalized C++ wrapper around the dl functions
from `<dlfcn.h>` to handle the loading and symbol lookup in dynamic
libraries.

### Usage

#### Loading Dynamic Libraries

```cpp
// Construct a DL handle with a string.
dl::handle lib("library-name.so");

// Given an already constructed DL handle, replace it with a new DL.
dl::handle lib;
lib.load("library-name.so");
```

#### Symbol Lookup

Symbol lookup is performed with the `lookup` member function. You must
pass in the type of the function as the template parameter and the name
of the function in the dynamic library as the function argument.

```cpp
// Get an optional<function<Signature>> to the function.
auto func_symbol = lib.lookup<int()>("function_name");
if (func_symbol)
{
  std::function<int()> f = func_symbol.get();
  f(5);
}
else
{
  std::cout << "Symbol lookup failed.\n";
}
```

#### Class Instance Creation

Due to the frequency of which dynamic libraries are used to create an
instance of an abstract class satisfying some interface, a simple interface
for creating objects from factory methods is provided.

```cpp
class base_type
{
  virtual void foo() = 0;
};

std::shared_ptr<base_type> p = lib.create<base_type>("factory_function");
if (p)
  p->foo();
else
  std::cout << "Unable to create instance.\n";
```

#### Symbol Resolution Policy

When loading the dynamic library, it can be specified whether all symbols
should be bound on opening or only when they are referenced. The
default is to bind all symbols when the library is loaded, but lazy
binding is also available.

It can be specified in the constructor as follows:

```cpp
dl::handle lib("library-name.so", dl::resolve::lazy);
	
```

Additionally, if you already have a handle, you can set the resolution
policy for future library loads like so:

```cpp
dl::handle lib;
lib.resolve_policy(dl::resolve::lazy);
```

