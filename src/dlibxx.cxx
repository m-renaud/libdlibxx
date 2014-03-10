#include <dlibxx.hxx>

namespace dlibxx {

handle::handle(std::string const& name)
	: name_(name)
{
	this->load(name_);
}

handle::~handle()
{
	this->close();
}

void handle::resolve_policy(::dlibxx::resolve rt)
{
	resolve_time_ = rt;
}

void handle::set_options(::dlibxx::options opts)
{
	resolve_options_ = opts;
}

void handle::load(std::string const& name)
{
	this->close();
	int resolve_flag =
		static_cast<int>(resolve_time_) | static_cast<int>(resolve_options_);

	// Clear previous errors.
	::dlerror();

	if (name.size() == 0)
		handle_ = ::dlopen(NULL, resolve_flag);
	else
		handle_ = ::dlopen(name.c_str(), resolve_flag);

	char* error_message = ::dlerror();
	if (error_message != NULL)
	{
		error_ = error_message;
		return;
	}

	name_ = name;
}

std::string const& handle::get_lib_name() const
{
	return name_;
}

std::string const& handle::error() const
{
	return error_;
}

void handle::close()
{
	if (handle_)
		::dlclose(handle_);

	handle_ = nullptr;
}

} // namespace dlibxx
