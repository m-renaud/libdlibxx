# Libdlibxx

Libdlibxx is a generalized C++11 wrapper library around the dl
functions from `<dlfcn.h>` to handle the loading and symbol lookup in
dynamic libraries.

### Dependencies

- Until `optional` is introduced into the standard, this code relies on
  Boost.Optional.
- A C++11 compiler.

### Licence

This library is licensed under the FreeBSD license.

### Usage

#### Linking to libdlibxx

To link to this library, pass the command line argument `-ldlibxx`.

#### Loading Dynamic Libraries

```cpp
// Construct a DL handle with a string.
dlibxx::handle lib("library-name.so");

// Given an already constructed DL handle, replace it with a new DL.
dlibxx::handle lib;
lib.load("library-name.so");
```

#### Library Loading Error Handling

If the library was loaded successfully, a call to the `loaded()`
function will return `true`. Error handling code could be performed as
follows after a library has been loaded.

```cpp
if (!lib.loaded())
{
  std::cerr << lib.error() << std::endl;
  return 1;
}
```

#### Symbol Lookup

Symbol lookup is performed with the `lookup` member function. You must
pass in the type of the function as the template parameter and the name
of the function in the dynamic library as the function argument.

If the function was successfully loaded, then the `optional` when
cast to bool will return `true`, and `false` otherwise. After a
successful load, the function can be retrieved via a call to `get()`.

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
struct base_type
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
use of `dlibxx::resolve::now`), but lazy binding is also available.

It can be specified in the constructor as follows:

```cpp
dlibxx::handle lib("library-name.so", dlibxx::resolve::lazy);

```

Additionally, if you already have a handle, you can set the resolution
policy for future library loads like so:

```cpp
dlibxx::handle lib;
lib.resolve_policy(dlibxx::resolve::lazy);
```

#### Options

In addition to the open policy, the options available for the `flag`
argument to `dlopen` are also available. They can be set with the
`set_options` member function before opening the library by ORing the
enum values together. The values available are:

```cpp
dlibxx::options::none      = 0,
dlibxx::options::global    = RTLD_GLOBAL,
dlibxx::options::local     = RTLD_LOCAL,
dlibxx::options::no_delete = RTLD_NODELETE,
dlibxx::options::no_load   = RTLD_NOLOAD,
dlibxx::options::deep_bind = RTLD_DEEPBIND
```

(See `man dlopen` for information on options.)

```cpp
dlibxx::handle lib;
lib.set_options(dlibxx::options::global | dlibxx::options::no_load);
```

### Example Code

Run `make` and change to the `examples/` directory.

Run the demo code with `./demo` and when prompted enter:

- `./liba.so`
- `./libb.so`
- `./libc.so`

and observe the results.

### Testing

To enable and run the unit tests you first need to ensure that you have
initialized the submodules so that gtest/gmock are available:

```bash
git submodule update --init
```

Enable testing:
```bash
cd build
cmake -DDLIBXX_ENABLE_TESTS=ON ..
```

Running `make` will now build the unit tests as well. Run the tests with one of
the following:
* `make test`
* `ctest`
* `ctest -V` - to see the output from gtest

### Contributing

In order to successfully contribute code there must be corresponding test code
that fully demonstrates that the contribution works as intended. See previous
section on enabling and running the unit test suite.
