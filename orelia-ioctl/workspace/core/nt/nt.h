#include <workspace/core/nt/resolver/resolver.hxx>

namespace nt {
	c_resolver g_resolver;
	void* g_thread_handle = nullptr;
}

#include <workspace/core/nt/symbols/symbols.hxx>
#include <workspace/core/nt/custom/custom.hxx>