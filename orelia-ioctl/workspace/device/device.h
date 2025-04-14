#define ioctl_device L"\\Device\\i-am-ioctl"
#define ioctl_dos_device L"\\GLOBAL??\\i-am-ioctl"
#define irp_mj_create 0x00
#define irp_mj_close 0x02
#define irp_mj_device_control 0x0E
#define irp_mj_maximum_function 0x1B
#define irp_mj_cleanup 18
#define do_device_initializing 0x00000080
#define do_buffered_io 0x00000004
#define file_device_secure_open 0x00000100

namespace device {
	unicode_string_t device_name;
	unicode_string_t dos_device_name;

	void init_ioctl_name( ) {
		nt::rtl_init_unicode_string( &device::device_name, ioctl_device );
		nt::rtl_init_unicode_string( &device::dos_device_name, ioctl_dos_device );

		nt::dbg_print( "[orelia] device name: %wZ\n", &device_name );
		nt::dbg_print( "[orelia] dos device name: %wZ\n", &dos_device_name );
	}
}

#include <workspace/device/controller/controller.hxx>
#include <workspace/device/dispatch/dispatch.hxx>