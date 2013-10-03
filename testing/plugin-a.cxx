#include <string>

#include "plugin.hxx"

struct plugin_a : plugin
{
	virtual std::string name() override
	{
		return "plugin_a";
	}

	virtual void render() override
	{
		std::cout << "plugin_a::render() <<<<<<<<<<<<<<\n";
	}
};

int bin_op(int a, int b)
{
	return a + b;
}

extern "C" plugin* create_plugin()
{
	return new plugin_a;
}
