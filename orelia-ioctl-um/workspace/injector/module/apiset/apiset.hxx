#include <winnt.h>

namespace resolver {
    struct api_set_value_entry_10_t {
        ULONG m_flags;
        ULONG m_name_offset;
        ULONG m_name_length;
        ULONG m_value_offset;
        ULONG m_value_length;
    };
    using papi_set_value_entry_10_t = api_set_value_entry_10_t*;

    struct api_set_value_array_10_t {
        ULONG m_flags;
        ULONG m_name_offset;
        ULONG m_unk;
        ULONG m_name_length;
        ULONG m_data_offset;
        ULONG m_count;

        inline papi_set_value_entry_10_t entry( void* p_api_set, DWORD i ) {
            return ( papi_set_value_entry_10_t )( ( BYTE* )p_api_set + m_data_offset + i * sizeof( api_set_value_entry_10_t ) );
        }
    };
    using papi_set_value_array_10_t = api_set_value_array_10_t*;

    struct api_set_namespace_entry_10_t {
        ULONG m_limit;
        ULONG m_size;
    };
    using papi_set_namespace_entry_10_t = api_set_namespace_entry_10_t*;

    struct api_set_namespace_array_10_t {
        ULONG m_version;
        ULONG m_size;
        ULONG m_flags;
        ULONG m_count;
        ULONG m_start;
        ULONG m_end;
        ULONG m_unk[ 2 ];

        inline papi_set_namespace_entry_10_t entry( DWORD i ) {
            return ( papi_set_namespace_entry_10_t )( ( BYTE* )this + m_end + i * sizeof( api_set_namespace_entry_10_t ) );
        }

        inline papi_set_value_array_10_t val_array( papi_set_namespace_entry_10_t p_entry ) {
            return ( papi_set_value_array_10_t )( ( BYTE* )this + m_start + sizeof( api_set_value_array_10_t ) * p_entry->m_size );
        }

        inline ULONG api_name( papi_set_namespace_entry_10_t p_entry, wchar_t* output ) {
            auto p_array = val_array( p_entry );
            memcpy( output, ( char* )this + p_array->m_name_offset, p_array->m_name_length );
            return p_array->m_name_length;
        }
    };
    using papi_set_namespace_array_10_t = api_set_namespace_array_10_t*;

    class c_apiset_resolver {
    private:
        std::map<std::wstring, std::vector<std::wstring>> m_api_schema;
        bool m_initialized = false;

        bool initialize( ) {
            if ( !m_api_schema.empty( ) )
                return true;

            PEB* peb = NtCurrentTeb( )->ProcessEnvironmentBlock;
            auto set_map = reinterpret_cast< api_set_namespace_array_10_t* >( peb->Reserved9[ 0 ] );
            if ( set_map->m_count == 0 ) {
                logging::print( encrypt( "Invalid API set map version or count" ) );
                return false;
            }

            logging::print( encrypt( "API Set Map version: %u, count: %u" ), set_map->m_version, set_map->m_count );

            for ( DWORD i = 0; i < set_map->m_count; i++ ) {
                auto pDescriptor = set_map->entry( i );

                std::vector<std::wstring> vhosts;
                wchar_t dllName[ MAX_PATH ] = { 0 };

                auto nameSize = set_map->api_name( pDescriptor, dllName );
                std::transform( dllName, dllName + nameSize / sizeof( wchar_t ), dllName, ::towlower );

                auto pHostData = set_map->val_array( pDescriptor );

                for ( DWORD j = 0; j < pHostData->m_count; j++ ) {
                    auto pHost = pHostData->entry( set_map, j );
                    std::wstring hostName(
                        reinterpret_cast< wchar_t* >( reinterpret_cast< uint8_t* >( set_map ) + pHost->m_value_offset ),
                        pHost->m_value_length / sizeof( wchar_t )
                    );

                    if ( !hostName.empty( ) )
                        vhosts.emplace_back( std::move( hostName ) );
                }

                m_api_schema.emplace( dllName, std::move( vhosts ) );
            }

            m_initialized = true;
            return !m_api_schema.empty( );
        }

    public:
        std::wstring resolve_api_set( const std::wstring& name ) {
            if ( !m_initialized && !initialize( ) )
                return name;

            std::wstring lower_name = name;
            std::transform( lower_name.begin( ), lower_name.end( ), lower_name.begin( ), ::tolower );

            if ( lower_name.find( L"ext-ms-" ) == 0 ) {
                lower_name.replace( 0, 7, L"api-ms-" );
            }

            for ( const auto& entry : m_api_schema ) {
                if ( lower_name.find( entry.first ) != std::wstring::npos ) {
                    if ( !entry.second.empty( ) ) {
                        return entry.second.front( );
                    }
                }
            }

            return name;
        }
    };
}