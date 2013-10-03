#include <string>

#include "plugin.hxx"

struct plugin_b : plugin
{
	virtual std::string name() override
	{
		return "plugin_b";
	}

	virtual void render() override
	{
		std::cout << "plugin_b::render()\n";
	}
};

extern "C" plugin* create_plugin()
{
	return new plugin_b;
}
