// Core Windows kernel definitions for all drivers
#include <ntddk.h>

// Driver entry point
NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ PUNICODE_STRING    RegistryPath
)
{
	
	auto status = STATUS_SUCCESS;

	return status;

}
