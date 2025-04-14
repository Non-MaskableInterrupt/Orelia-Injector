#include <impl/includes.h>

unsigned __stdcall entry_point( void* ) {
    MessageBoxA( 0, 0, 0, 0 );

    sdk::init::init_classes( );

    MessageBoxA( 0, 0, 0, 0 );

    sdk::init::init_libraries( );

    MessageBoxA( 0, 0, 0, 0 );

    sdk::init::init_keys( );

    MessageBoxA( 0, 0, 0, 0 );

    sdk::u_world world;
    auto front_end = world.get_front_end( );
    if ( !front_end ) return 0;

    auto game_instance = front_end->game_instance( );
    if ( !game_instance ) return 0;

    auto localplayer = game_instance->get_localplayer( );
    if ( !localplayer ) return 0;

    auto viewport_client = localplayer->viewport_client( );
    if ( !viewport_client ) return 0;

    MessageBoxA( 0, 0, 0, 0 );

    static vmt::c_vmt vmt;
    if ( !vmt.setup( viewport_client, offsets::draw_transition ) )
        return 0;

    return vmt.create_hook(
        game::draw_transition,
        &game::draw_transition_original
    );
}

volatile LONG g_initialized = 0;
extern "C" __declspec( dllexport ) BOOL APIENTRY DllMain( HMODULE module_base,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch ( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH: {
        if ( InterlockedCompareExchange( &g_initialized, 1, 0 ) != 0 ) {
            return TRUE;
        }

        MessageBoxA( 0, 0, 0, 0 );

        //pe::g_module_base = reinterpret_cast< std::uintptr_t >(
        //    module_base
        //    );

        //auto handle = _beginthreadex( 0, 0, entry_point, 0, 0, 0 );
        //CloseHandle( reinterpret_cast< HANDLE >( handle ) );
    } break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return 1;
}