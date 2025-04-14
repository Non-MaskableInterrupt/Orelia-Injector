
#define in_range(x, a, b) (x >= a && x <= b) 
#define get_bits(x) (in_range(x, '0', '9') ? (x - '0') : ((x - 'A') + 0xA))
#define get_byte(x) ((BYTE)(get_bits(x[0]) << 4 | get_bits(x[1])))

namespace utility
{
	[[nodiscard]]
	void gen_rnd_str( wchar_t* random_str ) {
		auto length = ( crt::rand( ) % 12 ) + 8;
		for ( auto i = 0ull; i < length; i++ ) {
			switch ( crt::rand( ) % 3 ) {
			case 0:
				random_str[ i ] = static_cast< wchar_t > ( 'A' + crt::rand( ) % 26 );
				break;

			case 1:
				random_str[ i ] = static_cast< wchar_t > ( 'a' + crt::rand( ) % 26 );
				break;

			case 2:
				random_str[ i ] = static_cast< wchar_t > ( '0' + crt::rand( ) % 10 );
				break;
			}
		}

		random_str[ length ] = 0;
	}

	const wchar_t* find_driver_path( ) {
		wchar_t* cmd = GetCommandLineW( );
		int cmd_len = crt::str_len( cmd ) - 1;
		if ( cmd_len == -1 )
			return nullptr;

		if ( cmd[ cmd_len-- ] == L'\"' ) {
			cmd[ cmd_len + 1 ] = 0;
			for ( ; cmd_len >= 0; cmd_len-- ) {
				if ( cmd[ cmd_len ] == L'\"' ) {
					cmd[ cmd_len++ ] = 0;
					return &cmd[ cmd_len ];
				}
			}
		}
		else {
			for ( ; cmd_len >= 0; cmd_len-- ) {
				if ( cmd[ cmd_len ] == L' ' ) {
					return &cmd[ ++cmd_len ];
				}
			}
		}

		return nullptr;
	}

	[[nodiscard]]
	void open_file( const std::string& file, std::vector<uint8_t>& data ) {
		std::ifstream fstr( file, std::ios::binary );
		fstr.unsetf( std::ios::skipws );
		fstr.seekg( 0, std::ios::end );

		const auto file_size = fstr.tellg( );

		fstr.seekg( NULL, std::ios::beg );
		data.reserve( static_cast< uint32_t >( file_size ) );
		data.insert( data.begin( ), std::istream_iterator<uint8_t>( fstr ), std::istream_iterator<uint8_t>( ) );
	}

	[[nodiscard]]
	UNICODE_STRING make_unicode_string( PWSTR buffer ) {
		UNICODE_STRING str{};
		if ( !buffer ) return str;

		size_t length = 0;
		while ( buffer[ length ] ) length++;

		str.Length = USHORT( length * 2 );
		str.MaximumLength = USHORT( ( length + 1 ) * 2 );
		str.Buffer = buffer;
		return str;
	}

	[[nodiscard]]
	void decrypt_bytes( PBYTE buffer, DWORD size, PBYTE out ) {
		for ( DWORD i = 0; i < size; i++ )
			out[ i ] = ( BYTE ) ( buffer[ i ] ^ ( ( i + 32 * i + 78 ) + 45 + i ) );
	}

	bool is_aligned( void* ptr, size_t alignment ) {
		return reinterpret_cast< std::uintptr_t >( ptr ) % alignment == 0;
	}
}