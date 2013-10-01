#ifndef DYNLIB_HXX__
#define DYNLIB_HXX__

#include <dlfcn.h>
#include <functional>
#include <memory>

#include <boost/optional.hpp> // Will be replaced with std::optional in C++14.

namespace dl {

namespace util {

//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Trick to convert a void* to a std::function.
// Since casting from a void* to a function pointer is undefined,
// we have to do a workaround. I am not positive if this is completely
// portable or not.
template <typename Ret, typename... Args>
auto fptr_cast(void* fptr)
	-> std::function<Ret(Args...)>
{
	using function_type = Ret (*)(Args...);
	using ptr_size_type = std::conditional<
		sizeof(fptr) == 4, long, long long
	>::type;

	return reinterpret_cast<function_type>(
		reinterpret_cast<ptr_size_type>(fptr)
	);
}

} // namespace util


// Enum to hold the symbol resolution policies.
enum class resolve { lazy = RTLD_LAZY, now = RTLD_NOW };

// Enum to hold the options to OR with the resolve policy.
enum class options
{
	none = 0,
	global = RTLD_GLOBAL,
	local = RTLD_LOCAL,
	no_delete = RTLD_NODELETE,
	no_load = RTLD_NOLOAD,
	deep_bind = RTLD_DEEPBIND
};

// Allow ORing two options together.
options operator | (options lhs, options rhs)
{
	return static_cast<options>(
		static_cast<int>(lhs) | static_cast<int>(rhs)
	);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Wrapper around the handle for a dynamic library.
class handle
{
public:
	handle() = default;

	handle(std::string const& name)
		: name_(name)
	{
		this->load(name_);
	}

	~handle()
	{
		this->close();
	}

	// Return true if is a valid handle.
	operator bool () const
	{
		return handle_;
	}

	// Set the policy for when symbols will be resolved.
	void resolve_policy(::dl::resolve rt)
	{
		resolve_time_ = rt;
	}

	// Set additional options for the behaviour of the handle.
	void set_options(::dl::options opts)
	{
		resolve_options_ = opts;
	}

	// Load the dynamic library named NAME.
	void load(std::string const& name)
	{
		this->close();
		int resolve_flag =
			static_cast<int>(resolve_time_) | static_cast<int>(resolve_options_);

		if (name.size() == 0)
			handle_ = ::dlopen(NULL, resolve_flag);
		else
			handle_ = ::dlopen(name.c_str(), resolve_flag);
	}

	// Return the last error that occured for this handle..
	std::string const& error() const
	{
		return error_;
	}

	// Close this handle to the dynamic library.
	void close()
	{
		if (handle_)
			::dlclose(handle_);
	}


	//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	// Create an instance of a type using the FNAME factory function.
	// Pass any arguments necessary to the factory method as well.
	template <typename T, typename... Args>
	inline auto create(char const* fname, Args&&... args) const
		-> std::shared_ptr<T>
	{
		auto factory_symbol = this->symbol_lookup_impl<T*>(fname);
		if (factory_symbol)
			return std::shared_ptr<T>(factory_symbol.get()(std::forward<Args>(args)...));
		else
			return std::shared_ptr<T>(nullptr);
	}

	template <typename T, typename... Args>
	inline auto create(std::string const& fname, Args&&... args) const
		-> std::shared_ptr<T>
	{
		return this->create<T>(fname.c_str(), std::forward<Args>(args)...);
	}


	//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	// Lookup the function named FNAME in the dynamic library.
	// This simply forwards the call to an internal class to allow
	// for function pointer syntax in the template parameter.
	template <typename Prototype>
	inline auto lookup(char const* fname) const
		-> boost::optional< std::function<Prototype> >
	{
		return symbol_lookup_wrapper<Prototype>::lookup(*this, fname);
	}

	template <typename Prototype>
	inline auto lookup(std::string const& fname) const
		-> boost::optional< std::function<Prototype> >
	{
		return this->lookup<Prototype>(fname.c_str());
	}


private:
	// Member data.
	std::string name_ = "";
	::dl::resolve resolve_time_ = ::dl::resolve::now;
	::dl::options resolve_options_ = ::dl::options::none;
	void* handle_ = nullptr;
	mutable std::string error_ = "";


	//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	// Implementation of symbol lookup in the dynamic library.
	// If there was an error, an uninitialized "optional" will be returned,
	// otherwise, an optional containing a std::function object will be
	// returned which can be retrieved with get().
	template <typename Ret, typename... Args>
	auto symbol_lookup_impl(char const* fname) const
		-> boost::optional< std::function<Ret (Args...)> >
	{
		// If the handle is invalid, return an uninitialized optional object.
		if (handle_ == nullptr)
		{
			error_ = "Handle not open.";
			return boost::optional< std::function<Ret (Args...)> >();
		}

		// Clear previous errors.
		::dlerror();

		// Lookup the symbol from the dynamic library.
		auto fptr = ::dl::util::fptr_cast<Ret, Args...>(
			::dlsym(handle_, fname)
		);

		// If there was an error, return an uninitialized optional.
		char* error_message;
		if ((error_message = ::dlerror()) != NULL)
		{
			error_ = error_message;
			return boost::optional< std::function<Ret (Args...)> >();
		}

		return fptr;
	}

	template <typename Ret, typename... Args>
	inline auto symbol_lookup_impl(std::string const& fname) const
		-> boost::optional< std::function<Ret (Args...)> >
	{
		return this->symbol_lookup_impl<Ret, Args...>(fname.c_str());
	}


	//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	// Helper class to allow for function pointer syntax in template parameter.
	// To allow a call such as lib.lookup<int(int)>("name"), a class template
	// with partial specialization must be used since function templates
	// cannot be partially specialized.
	template <typename Prototype>
	struct symbol_lookup_wrapper;

	template <typename Ret, typename... Args>
	struct symbol_lookup_wrapper<Ret(Args...)>
	{
		static inline auto lookup(handle const& lib, char const* fname)
			-> boost::optional< std::function<Ret(Args...)> >
		{
			return lib.symbol_lookup_impl<Ret, Args...>(fname);
		}
	};

}; // struct handle

} // namespace dl

#endif // #ifndef DYNLIB_HXX__
