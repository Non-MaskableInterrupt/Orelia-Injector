
namespace pdb
{
    BOOL CALLBACK SymEnumCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext) {
        logging::print(encrypt("Found symbol: %s at 0x%llx"), pSymInfo->Name, pSymInfo->Address);
        return TRUE; // Continue enumeration
    }

    template<typename T>
    [[nodiscard]]
    std::uint8_t* get_symbol_address( const char* file_path, T symbol_name ) {
        auto result = SymInitialize(
            reinterpret_cast< HANDLE >( -1 ),
            "http://msdl.microsoft.com/download/symbols",
            false
        );

        if ( !result ) {
            logging::print( encrypt( "SymInitialize failed with error: %d" ), GetLastError( ) );
            return nullptr;
        }

        SymSetOptions( SYMOPT_ALLOW_ABSOLUTE_SYMBOLS |
                       SYMOPT_DEBUG |
                       SYMOPT_LOAD_LINES |
                       SYMOPT_UNDNAME );

        const auto module_base = SymLoadModule64(
            reinterpret_cast< HANDLE >( -1 ),
            nullptr,
            file_path,
            nullptr,
            0,
            0
        );

        if ( !module_base ) {
            logging::print( encrypt( "SymLoadModule64 failed with error: %d" ), GetLastError( ) );
            SymCleanup( reinterpret_cast< HANDLE >( -1 ) );
            return nullptr;
        }

        IMAGEHLP_MODULEW64 module_info{};
        module_info.SizeOfStruct = sizeof( IMAGEHLP_MODULEW64 );
        if ( !SymGetModuleInfoW64( reinterpret_cast< HANDLE >( -1 ), module_base, &module_info ) ) {
            logging::print( encrypt( "SymGetModuleInfoW64 failed with error: %d" ), GetLastError( ) );
            SymUnloadModule64( reinterpret_cast< HANDLE >( -1 ), module_base );
            SymCleanup( reinterpret_cast< HANDLE >( -1 ) );
            return nullptr;
        }

        char symbol_name_full[ 64 ]{};
        crt::str_cpy( symbol_name_full, module_info.ModuleName );
        crt::str_cat( symbol_name_full, "!" );
        crt::str_cat( symbol_name_full, symbol_name );

        auto* symbol_info = reinterpret_cast< PSYMBOL_INFO_PACKAGE >( _alloca( sizeof( SYMBOL_INFO ) ) );
        symbol_info->si.MaxNameLen = sizeof( symbol_info->name );
        symbol_info->si.SizeOfStruct = sizeof( SYMBOL_INFO );

        // Enumerate all symbols
        if ( !SymEnumSymbols(
            reinterpret_cast< HANDLE >( -1 ),
            module_base,
            "",  // Wildcard to find all symbols containing "Mi"
            SymEnumCallback,
            nullptr ) ) {
            logging::print( encrypt( "SymEnumSymbols failed with error: %d" ), GetLastError( ) );
        }

        std::uint8_t* resolved_address = nullptr;
        if ( SymFromName( reinterpret_cast< HANDLE >( -1 ), symbol_name_full, &symbol_info->si ) ) {
            resolved_address = reinterpret_cast< std::uint8_t* >(
                symbol_info->si.Address - module_base
                );
        }
        else {
            logging::print( encrypt( "Failed to parse PDB for %s with error: %d" ), symbol_name, GetLastError( ) );
        }

        SymUnloadModule64( reinterpret_cast< HANDLE >( -1 ), module_base );
        SymCleanup( reinterpret_cast< HANDLE >( -1 ) );

        return 0;
    }
}