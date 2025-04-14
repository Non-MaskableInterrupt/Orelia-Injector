#pragma once

namespace shellcode {
    using namespace asmjit;
    using namespace asmjit::x86;

#pragma pack(push, 8) 
    struct inject_data_t {
        volatile std::uint32_t m_status;
        std::uint32_t m_return;
    };
#pragma pack(pop)
}