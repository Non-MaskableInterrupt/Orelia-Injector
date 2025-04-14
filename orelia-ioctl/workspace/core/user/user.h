namespace user {
	enum class e_command_type : std::uint32_t {
		open_process,
		attach_process,
		detach_process,
		get_process_peb,
		read_memory,
		write_memory,
		get_eprocess,
		get_base_address,
		translate_linear,
		protect_virtual,
		allocate_virtual,
		free_virtual,
		map_process_pte,
		map_physical,
		get_dtb
	};

	struct s_command_data {
		std::uint32_t m_pid{};
		std::uintptr_t m_address{};
		std::uintptr_t m_address2{};
		eprocess_t* m_process{};
		std::size_t m_size{};
		int m_new_protection{};
		int m_old_protection{};
		void* m_buffer{};
		peb_t* m_peb{};
		kapc_state_t m_apc_state{};
	};

	class c_command {
	public:
		c_command( ) = default;
		c_command(
			e_command_type type,
			s_command_data data,
			bool completed = false )
			: m_type( type ), m_data( data ), m_completed( completed ), m_status( nt_status_t::unsuccessful ) {
		}

		bool is_completed( ) const { return m_completed; }
		void set_completed( bool state ) { m_completed = state; }

		nt_status_t get_status( ) const { return m_status; }
		void set_status( nt_status_t status_code ) {
			m_status = status_code;
		}

		e_command_type get_type( ) const { return m_type; }
		const s_command_data& get_data( ) const { return m_data; }

	private:
		e_command_type m_type;
		s_command_data m_data;
		bool m_completed;
		std::uint64_t m_timestamp;
		nt_status_t m_status;
	};
}