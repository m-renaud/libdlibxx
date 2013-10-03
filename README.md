# Dynlib

Dynlib is a generalized C++ wrapper around the dl functions
from `<dlfcn.h>` to handle the loading and symbol lookup in dynamic
libraries.

### Dependencies

Until `optional` is introduced into the standard, this code relies on
Boost.Optional.

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
auto func_symbol = lib.lookup<void(int)>("function_name");
if (func_symbol)
{
  std::function<void(int)> f = func_symbol.get();
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

If the instance could not be created, a `shared_ptr` containing
`nullptr` will be returned.

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
default is to bind all symbols when the library is loaded (through the
use of `dl::resolve::now`), but lazy binding is also available.

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

#### Options

In addition to the open policy, the options available for the `flag`
argument to `dlopen` are also available. They can be set with the
`set_options` member function before opening the library by ORing the
enum values together. The values available are:

```cpp
dl::options::none      = 0,
dl::options::global    = RTLD_GLOBAL,
dl::options::local     = RTLD_LOCAL,
dl::options::no_delete = RTLD_NODELETE,
dl::options::no_load   = RTLD_NOLOAD,
dl::options::deep_bind = RTLD_DEEPBIND
```

(See `man dlopen` for information on options.)

```cpp
dl::handle lib;
lib.set_options(dl::options::global | dl::options::no_load);
```

### Test Code

- The `dynlib.hxx` file has been symlinked to the test directory.

Build the plugins:

```bash
g++ -std=c++11 -shared -fPIC plugin-a.cxx -o a.so
g++ -std=c++11 -shared -fPIC plugin-b.cxx -o b.so
g++ -std=c++11 -shared -fPIC plugin-c.cxx -o c.so
```

Build the driver class:

```bash
g++ -std=c++11 driver.cxx -o driver -ldl
```

Run the driver code with `./driver` and when prompted enter:

- `./a.so`
- `./b.so`
- `./c.so`

and observe the results.
