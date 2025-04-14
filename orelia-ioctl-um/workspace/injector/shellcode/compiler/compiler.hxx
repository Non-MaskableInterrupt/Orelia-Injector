namespace compiler {
    using namespace asmjit;
    using namespace asmjit::x86;

    class c_compiler {
    public:
        void setup( ) {
            m_code = std::make_unique<CodeHolder>( );
            m_code->init( m_runtime.environment( ) );

            m_assembler = std::make_unique<Assembler>( m_code.get( ) );
            m_data_label = m_assembler->newLabel( );
        }

        void save_registers( ) {
            m_assembler->push( rax );
            m_assembler->push( rcx );
            m_assembler->push( rdx );
            m_assembler->push( r8 );
            m_assembler->push( r9 );
            m_assembler->push( r11 );
            m_assembler->push( r10 );
            m_assembler->push( rbx );
            m_assembler->push( rsi );
            m_assembler->push( rdi );
            m_assembler->push( rbp );
        }

        void restore_registers( ) {
            m_assembler->pop( rbp );
            m_assembler->pop( rdi );
            m_assembler->pop( rsi );
            m_assembler->pop( rbx );
            m_assembler->pop( r10 );
            m_assembler->pop( r11 );
            m_assembler->pop( r9 );
            m_assembler->pop( r8 );
            m_assembler->pop( rdx );
            m_assembler->pop( rcx );
            m_assembler->pop( rax );
        }

        void set_shadow_space( std::uint32_t space ) {
            m_assembler->sub( rsp, Imm( space ) );
        }

        void restore_stack( std::uint32_t space ) {
            m_assembler->add( rsp, Imm( space ) );
        }

        void set_rcx( std::uint64_t a1 ) {
            m_assembler->mov( rcx, Imm( a1 ) );
        }

        void set_rdx( std::uint32_t a2 ) {
            m_assembler->mov( rdx, Imm( a2 ) );
        }

        void set_r8( std::uint64_t a3 ) {
            m_assembler->mov( r8, Imm( a3 ) );
        }

        void set_rax( std::uint64_t addr ) {
            m_assembler->mov( rax, Imm( addr ) );
        }

        void call_rax( ) {
            m_assembler->call( rax );
            m_assembler->mov( rbx, rax );
        }

        void jmp_rax( ) {
            m_assembler->jmp( rax );
        }

        void set_status( int status ) {
            m_assembler->mov( rax, qword_ptr( m_data_label ) );
            m_assembler->mov( dword_ptr( rax, offsetof( shellcode::inject_data_t, m_status ) ), Imm( status ) );
        }

        void store_return( ) {
            m_assembler->mov( rax, qword_ptr( m_data_label ) );
            m_assembler->mov( dword_ptr( rax, offsetof( shellcode::inject_data_t, m_return ) ), ebx );
        }

        void add_random_nops( int min_count = 1, int max_count = 5 ) {
            int nop_count = min_count + ( std::rand( ) % ( max_count - min_count + 1 ) );
            for ( int i = 0; i < nop_count; i++ ) {
                m_assembler->nop( );
            }
        }

        bool set_data_ref( std::uint64_t* data_va ) {
            shellcode::inject_data_t inject_data{ };
            inject_data.m_return = 0;
            inject_data.m_status = 0;

            *data_va = g_driver->allocate_virtual( sizeof( inject_data ) );
            if ( !*data_va ) {
                logging::print( encrypt( "Failed to allocate data buffer" ) );
                return false;
            }

            m_assembler->bind( m_data_label );
            m_assembler->dq( *data_va );

            return g_driver->write_memory( *data_va, &inject_data, sizeof( inject_data ) );
        }

        bool generate( std::uint64_t* shellcode_va ) {
            auto err = m_assembler->finalize( );
            if ( err ) {
                logging::print( encrypt( "AsmJit finalize error: %d" ), err );
                return false;
            }

            auto& buffer = m_code->sectionById( 0 )->buffer( );
            auto size = buffer.size( );
            if ( size == 0 ) {
                logging::print( encrypt( "AsmJit generated empty buffer" ) );
                return false;
            }

            *shellcode_va = g_driver->allocate_virtual( size );
            if ( !*shellcode_va ) {
                logging::print( encrypt( "Failed to allocate shellcode buffer" ) );
                return false;
            }

            return g_driver->write_memory( *shellcode_va, buffer.data( ), size );
        }

    private:
        asmjit::JitRuntime m_runtime;
        std::unique_ptr<CodeHolder> m_code;
        std::unique_ptr<Assembler> m_assembler;
        Label m_data_label;
    };
}