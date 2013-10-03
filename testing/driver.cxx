#include <iostream>
#include <memory>

#include "plugin.hxx"
#include "dynlib.hxx"


int main()
{
	std::string libname;
	dl::handle lib;

	lib.resolve_policy(dl::resolve::lazy);
	lib.set_options(dl::options::global | dl::options::no_delete);

	std::cout << "Enter the name of the library you wish to load: ";
	while (std::cin >> libname)
	{
		lib.load(libname);
		std::shared_ptr<plugin> p = lib.create<plugin>("create_plugin");
		if (p)
			p->render();
		else
			std::cout << "Could not create object.\n";

		auto bin_op_sym = lib.lookup<int (int, int)>("bin_op");
		if (bin_op_sym)
		{
			std::function<int(int,int)> bin_op = bin_op_sym.get();

			std::cout << bin_op(1, 2) << std::endl;

			lib.close();

			std::cout << bin_op(1, 2) << std::endl;
		}
		else
		{
			std::cerr << "That symbol cannot be found.\n";
		}


		std::cout << "Enter the name of the library you wish to load: ";
	}
}
