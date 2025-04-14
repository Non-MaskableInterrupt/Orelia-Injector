#include <impl/includes.h>

struct dpc_context {
	std::uint64_t image_base;
	std::uint64_t entry_point;
	std::uint32_t image_size;
	std::uint32_t status;
};

nt_status_t entry_point( void* a1, dpc_context* ctx ) {
	if ( !nt::g_resolver.setup( ) )
		return nt_status_t::not_supported;

	if ( !mm::g_paging.setup( ) )
		return nt_status_t::not_supported;

	device::init_ioctl_name( );
	auto result = nt::create_driver( dispatch::intialize_driver );
	ctx->status = static_cast< std::uint32_t >( nt_status_t::success );
	return nt_status_t::success;
}