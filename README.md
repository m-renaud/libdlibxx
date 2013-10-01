# Dynlib

Dynlib is a generalized C++ wrapper around the dl functions
from `<dlfcn.h>` to handle the loading and symbol lookup in dynamic
libraries.

### Usage

#### Loading Dynamic Libraries

	// Construct a DL handle with a string.
    dl::handle lib("library-name.so");

	// Given an already constructed DL handle, replace it with a new DL.
	dl::handle lib;
	lib.load("library-name.so");


#### Symbol Lookup

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


#### Class Instance Creation

	class base_type
	{
	  virtual void foo() = 0;
	};

	std::shared_ptr<base_type> p = lib.create<base_type>("factory_function");
	p->foo();



