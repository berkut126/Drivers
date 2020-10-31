#pragma once
#include <ntddk.h>
#include "Messages.h"

// DebugPrint declaration
#if DBG
#define DEBUG_PRINT(x) DbgPrintEx (DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, x)
#else
#define DEBUG_PRINT(x)
#endif

// Buffer
struct device_extension
{
	PDEVICE_OBJECT target_device_object;
	PFILE_OBJECT file_object;
};

constexpr int buffer_size = sizeof(device_extension);

struct device
{
	const unsigned short* name;
	const unsigned short* symlink;
	int type;
	int characteristics;
	BOOLEAN exclusiveness;
	int flag;
	int tag;
	NTSTATUS init;
	NTSTATUS success;
	NTSTATUS failure;
};

constexpr int file_device_unifilt = 0x00008301;
constexpr unsigned int unifilt_my_create = CTL_CODE(file_device_unifilt, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)   ;
constexpr unsigned int unifilt_my_delete = CTL_CODE(file_device_unifilt, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS);

BOOLEAN write_event(IN const NTSTATUS error_code, IN const PVOID io_object, IN const PIRP irp) noexcept {

	constexpr UCHAR packet_size = sizeof(IO_ERROR_LOG_PACKET);

	auto* const packet = static_cast<PIO_ERROR_LOG_PACKET>(IoAllocateErrorLogEntry(io_object, packet_size));
	if (packet == nullptr) return FALSE;

	packet->ErrorCode = error_code;
	packet->UniqueErrorValue = 0;
	packet->RetryCount = 0;
	packet->SequenceNumber = 0;
	packet->IoControlCode = 0;
	packet->DumpDataSize = 0;

	if (irp != nullptr) {
		auto* const irp_stack = IoGetCurrentIrpStackLocation(irp);
		packet->MajorFunctionCode = irp_stack->MajorFunction;
		packet->FinalStatus = irp->IoStatus.Status;
	}
	else {
		packet->MajorFunctionCode = 0;
		packet->FinalStatus = 0;
	}

	IoWriteErrorLogEntry(packet);
	return TRUE;
}

inline unsigned long get_io_flag(const DEVICE_OBJECT* const device) noexcept
{

	if(device->Flags & DO_BUFFERED_IO)
	{
		return DO_BUFFERED_IO;
	}
	else if(device->Flags & DO_DIRECT_IO)
	{
		return DO_DIRECT_IO;
	}
	else
	{
		return 0;
	}
	
}

NTSTATUS device_init(device const* device, const PDRIVER_OBJECT driver_object, PDEVICE_OBJECT device_object, device_extension* buffer) noexcept
{

	auto status = STATUS_SUCCESS;

	// Device name
	UNICODE_STRING dev_name;

	// Symbolic name
	UNICODE_STRING symbolic_name;

	// Initialize device name with \Device\BufferedIO
	RtlInitUnicodeString(&dev_name, device->name);

	// Initialize symbolic name with \DosDevices\BufferedIO
	RtlInitUnicodeString(&symbolic_name, device->symlink);

	// Create device
	status = IoCreateDevice(
		driver_object,
		buffer_size,
		&dev_name,
		device->type,
		device->characteristics,
		device->exclusiveness,
		&device_object
	);

	write_event(device->init, driver_object, nullptr);

	if (status != STATUS_SUCCESS) {

		DEBUG_PRINT(("devioctl: Exiting DriverEntry\n"));
		write_event(device->failure, driver_object, nullptr);

		return status;

	}

	write_event(device->success, driver_object, nullptr);

	device_object->Flags |= device->flag;

	device_object->DeviceExtension = ExAllocatePoolWithTag(NonPagedPool, buffer_size, device->tag);

	RtlZeroMemory(
		device_object->DeviceExtension,
		buffer_size);

	buffer = static_cast<device_extension*>(device_object->DeviceExtension);

	// Create symbolic link
	status = IoCreateSymbolicLink(&symbolic_name, &dev_name);

	// Delete device if link init failed
	if (status != STATUS_SUCCESS) {

		IoDeleteDevice(device_object);

		DEBUG_PRINT(("unifilt: Exiting DriverEntry\n"));

		return status;

	}

	DEBUG_PRINT(("devioctl: Exiting DriverEntry\n"));

	return status;

}
