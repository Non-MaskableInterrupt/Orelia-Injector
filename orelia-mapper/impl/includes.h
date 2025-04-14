#include <chrono>
#include <ctime>
#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <fstream>
#include <vector>
#include <winternl.h>
#include <cstdint>
#include <DbgHelp.h>
#include <thread>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")

typedef enum _KPROFILE_SOURCE {
    ProfileTime,
    ProfileAlignmentFixup,
    ProfileTotalIssues,
    ProfilePipelineDry,
    ProfileLoadInstructions,
    ProfilePipelineFrozen,
    ProfileBranchInstructions,
    ProfileTotalNonissues,
    ProfileDcacheMisses,
    ProfileIcacheMisses,
    ProfileCacheMisses,
    ProfileBranchMispredictions,
    ProfileStoreInstructions,
    ProfileFpInstructions,
    ProfileIntegerInstructions,
    Profile2Issue,
    Profile3Issue,
    Profile4Issue,
    ProfileSpecialInstructions,
    ProfileTotalCycles,
    ProfileIcacheIssues,
    ProfileDcacheAccesses,
    ProfileMemoryBarrierCycles,
    ProfileLoadLinkedIssues,
    MaximumProfileSource
} KPROFILE_SOURCE, * PKPROFILE_SOURCE;

NTSTATUS NtQueryIntervalProfile(
    _In_ KPROFILE_SOURCE ProfileSource,
    _Out_ PULONG Interval
) {
    return reinterpret_cast< NTSTATUS( NTAPI* )( KPROFILE_SOURCE, PULONG ) >(
        GetProcAddress( GetModuleHandleA( "ntdll.dll" ), "NtQueryIntervalProfile" )
        )( ProfileSource, Interval );
}

#include <dependencies/bytes/bytes.hxx>
#include <dependencies/skcrypt/skcrypter.h>

#include <impl/crt/crt.hxx>
#include <impl/log/log.hxx>
#include <impl/pdb/pdb.hxx>
#include <impl/ia32/ia32.h>

#include <workspace/utility.hxx>
#include <workspace/mapper/service/service.hxx>
#include <workspace/mapper/driver/driver.hxx>
driver::c_driver* g_driver = new driver::c_driver;

#include <workspace/mapper/module/module.hxx>
#include <workspace/mapper/driver/kernel/kernel.hxx>
kernel::c_kernel* g_kernel = new kernel::c_kernel;

#include <workspace/exception/exception.hxx>
#include <workspace/mapper/module/gadgets/gadgets.hxx>
#include <workspace/mapper/module/exports/exports.hxx>
#include <workspace/mapper/image/image.hxx>
#include <workspace/mapper/mapper.hxx>