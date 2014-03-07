#include <iostream>
#include <memory>

#include "plugin.hxx"
#include "dlibxx.hxx"

int main()
{
	std::string libname;
	dlibxx::handle lib;

	std::cout << "Enter the name of the library you wish to load: ";
	while (std::cin >> libname)
	{
		std::cout << "Loading library " << libname << std::endl;
		lib.load(libname);

		std::cout << "Creating plugin instance...\n";
		std::shared_ptr<plugin> p = lib.create<plugin>("create_plugin");

		std::cout << "Rendering using plugin...\n>>>>  ";
		p->render();

		std::cout << "\n\nEnter the name of the library you wish to load: ";
	}
}
