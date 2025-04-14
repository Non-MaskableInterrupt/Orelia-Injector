
namespace injector {
    bool inject_dll( const char* path_name ) {
        logging::print( encrypt( "Starting payload mapping process" ) );

        auto pe_image = std::make_unique<module::c_module>( );
        if ( !pe_image->load_file( path_name ) ) {
            logging::print( encrypt( "Failed to parse pe headers." ) );
            return false;
        }

        if ( !pe_image->is_dll( ) ) {
            logging::print( encrypt( "Image is not a dll or corrupted PE Headers." ) );
            return false;
        }

        auto image_size = pe_image->get_image_size( );
        auto image_va = g_driver->allocate_virtual( image_size );
        if ( !image_va ) {
            logging::print( encrypt( "Failed to allocate virtual memory." ) );
            return false;
        }

        auto [image_pa, page_size] = g_driver->translate_linear( image_va );
        if ( !image_pa || !page_size ) {
            logging::print( encrypt( "Failed to map virtual memory." ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        logging::print( encrypt( "PE allocated at: 0x%llx, Size=0x%x" ), image_va, image_size );
        logging::print( encrypt( "PE page at: 0x%llx, Size=0x%x" ), image_pa, page_size );

        if ( !pe_image->copy_headers( image_va ) ) {
            logging::print( encrypt( "Failed to copy headers" ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        if ( !pe_image->relocate( image_va ) ) {
            logging::print( encrypt( "Failed to relocate image" ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        if ( !pe_image->map_imports( ) ) {
            logging::print( encrypt( "Failed to map imports" ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        if ( !pe_image->map_sections( image_va ) ) {
            logging::print( encrypt( "Failed to write sections" ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        if ( !pe_image->verify_module_integrity( image_va ) ) {
            logging::print( encrypt( "Target image headers or code corrupted" ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        auto export_rva = pe_image->find_export( encrypt( "DllMain" ) );
        if ( !export_rva ) {
            logging::print( encrypt( "Failed to DllMain RVA" ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        logging::print( encrypt( "Module preparation completed successfully\n" ) );

        auto shellcode = std::make_unique<shellcode::c_shellcode>( image_va, export_rva );
        if ( !shellcode->setup( ) ) {
            logging::print( encrypt( "Failed to setup shellcode." ) );
            g_driver->free_virtual( image_va );
            return false;
        }

        if ( !shellcode->update( ) ) {
            logging::print( encrypt( "Failed to compile shellcode." ) );
            g_driver->free_virtual( image_va );
            shellcode->cleanup( );
            return false;
        }

        auto result = shellcode->run( );
        if ( !result )
            g_driver->free_virtual( image_va );

        shellcode->cleanup( );
        return result;
    }
}