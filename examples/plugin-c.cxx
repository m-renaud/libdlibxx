#include <string>

// No plugin class or factory method.
// Instance will fail to be created.

extern "C" int bin_op(int a, int b)
{
	return a + b;
}
