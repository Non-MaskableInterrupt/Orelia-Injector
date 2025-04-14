#pragma once

namespace resolver {
    namespace pdb {
        struct module_mapping_t {
            std::string original_name;
            std::string mapped_name;
        };

        struct export_info_t {
            std::string name;
            std::uintptr_t rva;
        };

        BOOL CALLBACK ModuleMappingCallback( PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext ) {
            auto mappings = reinterpret_cast< std::vector<module_mapping_t>* >( UserContext );

            const char* symbol_name = pSymInfo->Name;
            const char* separator = strchr( symbol_name, '_' );
            if ( separator ) {
                const char* next_separator = strchr( separator + 1, '_' );
                if ( next_separator ) {
                    std::string from( separator + 1, next_separator - ( separator + 1 ) );
                    std::string to( next_separator + 1 );

                    if ( from.length( ) > 2 && to.length( ) > 2 &&
                        from.find( "::" ) == std::string::npos &&
                        to.find( "::" ) == std::string::npos ) {

                        mappings->push_back( { from, to } );
                    }
                }
            }

            return TRUE;
        }

        BOOL CALLBACK ExportInfoCallback( PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext ) {
            auto context = reinterpret_cast< std::pair<std::vector<export_info_t>*, const char*>* >( UserContext );
            auto exports = context->first;
            const char* export_name_to_find = context->second;

            const char* symbol_name = pSymInfo->Name;
            if ( strcmp( symbol_name, export_name_to_find ) == 0 ||
                strcmp( symbol_name, export_name_to_find + 1 ) == 0 ||
                ( symbol_name[ 0 ] == 'o' && strcmp( symbol_name + 1, export_name_to_find ) == 0 ) ||
                ( strncmp( symbol_name, "o_", 2 ) == 0 && strcmp( symbol_name + 2, export_name_to_find ) == 0 ) ) {

                exports->push_back( { export_name_to_find, pSymInfo->Address } );
                return TRUE;
            }

            if ( strstr( symbol_name, export_name_to_find ) ) {
                exports->push_back( { export_name_to_find, pSymInfo->Address } );
            }

            return TRUE;
        }

        std::vector<module_mapping_t> get_module_mappings( const char* module_path ) {
            std::vector<module_mapping_t> mappings;

            auto result = SymInitialize(
                reinterpret_cast< HANDLE >( -1 ),
                "http://msdl.microsoft.com/download/symbols",
                false
            );

            if ( !result ) {
                logging::print( encrypt( "SymInitialize failed with error: %d" ), GetLastError( ) );
                return mappings;
            }

            SymSetOptions( SYMOPT_ALLOW_ABSOLUTE_SYMBOLS |
                SYMOPT_DEBUG |
                SYMOPT_LOAD_LINES |
                SYMOPT_UNDNAME );

            const auto module_base = SymLoadModule64(
                reinterpret_cast< HANDLE >( -1 ),
                nullptr,
                module_path,
                nullptr,
                0,
                0
            );

            if ( !module_base ) {
                logging::print( encrypt( "SymLoadModule64 failed for %s with error: %d" ),
                    module_path, GetLastError( ) );
                SymCleanup( reinterpret_cast< HANDLE >( -1 ) );
                return mappings;
            }

            if ( !SymEnumSymbols(
                reinterpret_cast< HANDLE >( -1 ),
                module_base,
                "*",
                ModuleMappingCallback,
                &mappings ) ) {
                logging::print( encrypt( "SymEnumSymbols failed for %s with error: %d" ),
                    module_path, GetLastError( ) );
            }

            SymUnloadModule64( reinterpret_cast< HANDLE >( -1 ), module_base );
            SymCleanup( reinterpret_cast< HANDLE >( -1 ) );

            return mappings;
        }

        std::vector<module_mapping_t> get_all_module_mappings( ) {
            std::vector<module_mapping_t> all_mappings;
            auto modules = g_driver->get_process_modules( );

            logging::print( encrypt( "Scanning %zu modules for mappings" ), modules.size( ) );
            for ( const auto& module : modules ) {
                char module_path[ MAX_PATH ] = { 0 };
                char module_name[ MAX_PATH ] = { 0 };

                WideCharToMultiByte( CP_ACP, 0, module.second.c_str( ), -1, module_name, MAX_PATH, NULL, NULL );
                GetSystemDirectoryA( module_path, MAX_PATH );
                strcat_s( module_path, "\\" );
                strcat_s( module_path, module_name );

                if ( GetFileAttributesA( module_path ) == INVALID_FILE_ATTRIBUTES ) {
                    GetSystemWow64DirectoryA( module_path, MAX_PATH );
                    strcat_s( module_path, "\\" );
                    strcat_s( module_path, module_name );

                    if ( GetFileAttributesA( module_path ) == INVALID_FILE_ATTRIBUTES ) {
                        logging::print( encrypt( "Could not find path for module %s" ), module_name );
                        continue;
                    }
                }

                auto mappings = get_module_mappings( module_path );
                all_mappings.insert( all_mappings.end( ), mappings.begin( ), mappings.end( ) );

                logging::print( encrypt( "Found %zu mappings in module %s" ),
                    mappings.size( ), module_path );
            }

            logging::print( encrypt( "Found a total of %zu module mappings" ), all_mappings.size( ) );
            return all_mappings;
        }

        std::uintptr_t find_export( const char* module_path, const char* export_name ) {
            std::vector<export_info_t> exports;

            auto result = SymInitialize(
                reinterpret_cast< HANDLE >( -1 ),
                "http://msdl.microsoft.com/download/symbols",
                false
            );

            if ( !result ) {
                logging::print( encrypt( "SymInitialize failed with error: %d" ), GetLastError( ) );
                return 0;
            }

            SymSetOptions( SYMOPT_ALLOW_ABSOLUTE_SYMBOLS |
                SYMOPT_DEBUG |
                SYMOPT_LOAD_LINES |
                SYMOPT_UNDNAME );

            const auto module_base = SymLoadModule64(
                reinterpret_cast< HANDLE >( -1 ),
                nullptr,
                module_path,
                nullptr,
                0,
                0
            );

            if ( !module_base ) {
                logging::print( encrypt( "SymLoadModule64 failed for %s with error: %d" ),
                    module_path, GetLastError( ) );
                SymCleanup( reinterpret_cast< HANDLE >( -1 ) );
                return 0;
            }

            std::pair<std::vector<export_info_t>*, const char*> context( &exports, export_name );
            if ( !SymEnumSymbols(
                reinterpret_cast< HANDLE >( -1 ),
                module_base,
                "*",
                ExportInfoCallback,
                &context ) ) {
                logging::print( encrypt( "SymEnumSymbols failed for %s with error: %d" ),
                    module_path, GetLastError( ) );
            }

            SymUnloadModule64( reinterpret_cast< HANDLE >( -1 ), module_base );
            SymCleanup( reinterpret_cast< HANDLE >( -1 ) );

            return exports.empty( ) ? 0 : exports[ 0 ].rva;
        }
    }

    std::wstring resolve_module_name( const char* module_name ) {
        std::wstring wide_module_name_original = L"";
        {
            wchar_t wide_name[ MAX_PATH ] = { 0 };
            MultiByteToWideChar( CP_ACP, 0, module_name, -1, wide_name, MAX_PATH );
            wide_module_name_original = wide_name;

            if ( wide_module_name_original.find( L".dll" ) == std::wstring::npos &&
                wide_module_name_original.find( L".DLL" ) == std::wstring::npos ) {
                wide_module_name_original += L".dll";
            }

            if ( g_driver->get_process_module( wide_module_name_original.c_str( ) ) ) {
                return wide_module_name_original;
            }
        }

        auto apiset_resolver = std::make_unique< c_apiset_resolver >( );
        auto resolved_name = apiset_resolver->resolve_api_set( wide_module_name_original );
        if ( resolved_name != wide_module_name_original ) {
            if ( g_driver->get_process_module( resolved_name.c_str( ) ) ) {
                return resolved_name;
            }
        }

        char module_path[ MAX_PATH ] = { 0 };
        GetSystemDirectoryA( module_path, MAX_PATH );
        strcat_s( module_path, "\\" );
        strcat_s( module_path, module_name );

        static std::map<std::string, std::string> module_mappings;
        std::vector<pdb::module_mapping_t> all_mappings;
        auto mappings = pdb::get_module_mappings( module_path );
        all_mappings.insert( all_mappings.end( ), mappings.begin( ), mappings.end( ) );

        for ( const auto& mapping : all_mappings ) {
            if ( mapping.original_name.length( ) > 2 && mapping.mapped_name.length( ) > 2 ) {
                std::string lower_original = mapping.original_name;
                std::transform( lower_original.begin( ), lower_original.end( ), lower_original.begin( ), ::tolower );

                std::string lower_mapped = mapping.mapped_name;
                std::transform( lower_mapped.begin( ), lower_mapped.end( ), lower_mapped.begin( ), ::tolower );

                module_mappings[ lower_original ] = lower_mapped;
            }
        }

        logging::print( encrypt( "Initialized %zu module mappings from PDB information" ),
            module_mappings.size( ) );

        std::string module_name_str = module_name;
        std::transform( module_name_str.begin( ), module_name_str.end( ), module_name_str.begin( ), ::tolower );

        const std::string dll_ext = ".dll";
        if ( module_name_str.length( ) > dll_ext.length( ) &&
            module_name_str.substr( module_name_str.length( ) - dll_ext.length( ) ) == dll_ext ) {
            module_name_str = module_name_str.substr( 0, module_name_str.length( ) - dll_ext.length( ) );
        }

        for ( const auto& mapping : module_mappings ) {
            std::string key_without_ext = mapping.first;
            if ( key_without_ext.length( ) > dll_ext.length( ) &&
                key_without_ext.substr( key_without_ext.length( ) - dll_ext.length( ) ) == dll_ext ) {
                key_without_ext = key_without_ext.substr( 0, key_without_ext.length( ) - dll_ext.length( ) );
            }

            if ( key_without_ext == module_name_str ) {
                std::string mapped_name = mapping.second;

                wchar_t wide_name[ MAX_PATH ] = { 0 };
                MultiByteToWideChar( CP_ACP, 0, mapped_name.c_str( ), -1, wide_name, MAX_PATH );
                std::wstring result = wide_name;

                if ( result.find( L".dll" ) == std::wstring::npos &&
                    result.find( L".DLL" ) == std::wstring::npos ) {
                    result += L".dll";
                }

                if ( g_driver->get_process_module( result.c_str( ) ) != 0 ) {
                    logging::print( encrypt( "Mapped module %s to %s using PDB information" ),
                        module_name, mapped_name.c_str( ) );
                    return result;
                }
                else {
                    logging::print( encrypt( "Mapped module %s to %s but target not found, skipping" ),
                        module_name, mapped_name.c_str( ) );
                }
            }
        }

        logging::print( encrypt( "No valid mapping found for module %s, using original name" ), module_name );
        return wide_module_name_original;
    }

    std::uintptr_t resolve_export_pdb( std::uintptr_t module_base, const char* export_name, const char* module_path ) {
        static std::map<std::string, std::map<std::string, std::uintptr_t>> export_cache;

        if ( export_name[ 0 ] == '#' )
            return g_driver->get_module_export( module_base, export_name );

        std::string cache_key = std::string( module_path ) + ":" + export_name;
        if ( export_cache.find( module_path ) != export_cache.end( ) ) {
            auto& module_exports = export_cache[ module_path ];
            if ( module_exports.find( export_name ) != module_exports.end( ) ) {
                std::uintptr_t rva = module_exports[ export_name ];
                return module_base + rva;
            }
        }

        std::uintptr_t rva = pdb::find_export( module_path, export_name );
        if ( rva ) {
            if ( export_cache.find( module_path ) == export_cache.end( ) ) {
                export_cache[ module_path ] = std::map<std::string, std::uintptr_t>( );
            }
            export_cache[ module_path ][ export_name ] = rva;

            return module_base + rva;
        }

        return g_driver->get_module_export( module_base, export_name );
    }
}