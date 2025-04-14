#include <impl/std/std.h>
#include <impl/ia32/ia32.h>
#include <impl/crt/crt.hxx>
extern "C" std::uintptr_t get_nt_base( );

#define kernel_mode 0
#define user_mode 1

#define copy_physical 1
#define copy_virtual 2

#include <workspace/core/nt/nt.h>
#include <workspace/core/mm/mm.h>
#include <workspace/core/eac/eac.h>
#include <workspace/core/user/user.h>
#include <workspace/device/handler/mem/mem.hxx>
#include <workspace/device/handler/proc/proc.hxx>
#include <workspace/device/handler/sys/sys.hxx>
#include <workspace/device/device.h>