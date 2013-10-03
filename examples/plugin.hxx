#ifndef PLUGIN_HXX__
#define PLUGIN_HXX__

#include <string>
#include <iostream>

// A rendering plugin interface.
struct plugin
{
	virtual ~plugin()
	{
	}

	virtual std::string name () = 0;
	virtual void render() {
		std::cout << "Default rendering\n";
	}
};

#endif // #ifndef PLUGIN_HXX__
