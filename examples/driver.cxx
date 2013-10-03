#include <iostream>
#include <memory>

#include "plugin.hxx"
#include "dynlib.hxx"


int main()
{
	std::string libname;
	dl::handle lib;

	// Resolve symbols when they are referenced instead of on load.
	lib.resolve_policy(dl::resolve::lazy);

	// Make symbols available for resolution in subsequently loaded libraries.
	lib.set_options(dl::options::global | dl::options::no_delete);

	std::cout << "Enter the name of the library you wish to load: ";
	while (std::cin >> libname)
	{
		// Load the library specified.
		lib.load(libname);

		// Attempt to create a plugin instance using the "create_plugin"
		// factory method from the dynamic library.
		std::shared_ptr<plugin> p = lib.create<plugin>("create_plugin");

		// Only call render if the object could successfully be created.
		if (p)
			p->render();
		else
			std::cout << "Could not create object.\n";

		// Attempt to resolve the "bin_op" symbol.
		auto bin_op_sym = lib.lookup<int (int, int)>("bin_op");

		// Only call the function if symbol lookup was successful.
		if (bin_op_sym)
		{
			std::function<int(int,int)> bin_op = bin_op_sym.get();

			std::cout << bin_op(1, 2) << std::endl;

			lib.close();

			// Function can still be called after closing the library due to
			// no_delete option that was set before loading the DL.
			std::cout << bin_op(1, 2) << std::endl;
		}
		else
		{
			std::cerr << "That symbol cannot be found.\n";
		}

		std::cout << "Enter the name of the library you wish to load: ";
	}
}
