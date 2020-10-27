#pragma once
#include <ntddk.h>
#include "Messages.h"

// DebugPrint declaration
#if DBG
#define DEBUG_PRINT(x) DbgPrintEx (DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, x)
#else
#define DEBUG_PRINT(x)
#endif

constexpr int buffer_size = 16;

// Buffer
struct device_extension
{
	unsigned char image[buffer_size];
};

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

constexpr int file_device_ioctl  = 0x00008301;
constexpr int ioctl_my_neither   = CTL_CODE(file_device_ioctl, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS);
constexpr int ioctl_my_buffered  = CTL_CODE(file_device_ioctl, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS);
constexpr int ioctl_my_in_direct  = CTL_CODE(file_device_ioctl, 0x802, METHOD_IN_DIRECT, FILE_ANY_ACCESS);
constexpr int ioctl_my_out_direct = CTL_CODE(file_device_ioctl, 0x803, METHOD_OUT_DIRECT, FILE_ANY_ACCESS);


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

NTSTATUS device_init(device const* device, const PDRIVER_OBJECT driver_object, PDEVICE_OBJECT device_object) noexcept
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

	// Create symbolic link
	status = IoCreateSymbolicLink(&symbolic_name, &dev_name);

	// Delete device if link init failed
	if (status != STATUS_SUCCESS) {

		IoDeleteDevice(device_object);

		DEBUG_PRINT(("devioctl: Exiting DriverEntry\n"));

		return status;

	}

	return status;

}

NTSTATUS read_buffered(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("devioctl: entering buffered read\n"));

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	//Get User Output Buffer & Length 
	const auto length = stack->Parameters.Read.Length > buffer_size ? buffer_size : stack->Parameters.Read.Length;
	auto* user_buffer = irp->AssociatedIrp.SystemBuffer;

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlCopyMemory(user_buffer, driver_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("devioctl: exiting buffered read\n"));
	return STATUS_SUCCESS;
}

NTSTATUS write_buffered(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("devioctl: entering buffered write\n"));

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	//Get User Input Buffer & Length
	const auto length = stack->Parameters.Write.Length > buffer_size ? buffer_size : stack->Parameters.Read.Length;
	auto* user_buffer = irp->AssociatedIrp.SystemBuffer;

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlZeroMemory(driver_buffer, buffer_size);

	RtlCopyMemory(driver_buffer, user_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("devioctl: exiting buffered read\n"));
	return STATUS_SUCCESS;
}
NTSTATUS read_direct(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("devioctl: entering buffered read\n"));

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	if (irp->MdlAddress == nullptr)
	{

		DEBUG_PRINT(("devioctl: Mdl is null\n"));
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	// Get User Output Buffer & Length 
	const auto length = MmGetMdlByteCount(irp->MdlAddress) > buffer_size ? buffer_size : MmGetMdlByteCount(irp->MdlAddress);
	auto* user_buffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);

	if (user_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlCopyMemory(user_buffer, driver_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("devioctl: exiting buffered read\n"));
	return STATUS_SUCCESS;

}

NTSTATUS write_direct(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("devioctl: entering buffered write\n"));

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	if (irp->MdlAddress == nullptr)
	{

		DEBUG_PRINT(("devioctl: Mdl is null\n"));
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	//Get User Output Buffer & Length 
	const auto length = stack->Parameters.Write.ByteOffset.LowPart > buffer_size ? buffer_size : stack->Parameters.Write.ByteOffset.LowPart;
	// DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Mdl equals: %f", irp->MdlAddress);
	auto* user_buffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority);

	if (user_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlZeroMemory(driver_buffer, buffer_size);

	RtlCopyMemory(driver_buffer, user_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("devioctl: exiting buffered read\n"));
	return STATUS_SUCCESS;
}

NTSTATUS read_neither(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("devioctl: entering neither read\n"));
	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);
	//Get Driver Data Buffer & Length
	auto* const driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}
	_try
	{

		// User buffer
		auto * user_buffer = irp->UserBuffer;

	// Size (can't be more than buffer_size [16] bytes)
	const auto length = stack->Parameters.Write.Length > buffer_size ? buffer_size : stack->Parameters.Write.Length;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "devioctl: Probe started, user_buffer: %s, driver_buffer: %s\n", user_buffer, driver_buffer);

	// Test write access
	ProbeForRead(user_buffer, length, sizeof(unsigned char));

	DEBUG_PRINT(("devioctl: Probe done\n"));

	// Move memory
	RtlCopyMemory(user_buffer, driver_buffer, length);

	irp->IoStatus.Information = length;

	}
		_except(EXCEPTION_EXECUTE_HANDLER)
	{
		// ProbeForWrite failed
		irp->IoStatus.Status = STATUS_ACCESS_VIOLATION;
		DEBUG_PRINT(("devioctl: Oops!\n"));
		irp->IoStatus.Information = 0;
		IoCompleteRequest(
			irp,
			IO_NO_INCREMENT
		);
		return STATUS_ACCESS_VIOLATION;
	}
	DEBUG_PRINT(("devioctl: exiting neither read\n"));
	return STATUS_SUCCESS;
}

NTSTATUS write_neither(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("devioctl: entering neither write\n"));
	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);
	//Get Driver Data Buffer & Length
	auto* const driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}
	_try
	{

		// User buffer
		auto * user_buffer = irp->UserBuffer;

		// Size (can't be more than buffer_size [16] bytes)
		const auto length = stack->Parameters.Read.Length > buffer_size ? buffer_size : stack->Parameters.Read.Length;

		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "devioctl: Probe started, user_buffer: %s, driver_buffer: %s\n", user_buffer, driver_buffer);

		// Test write access
		ProbeForWrite(user_buffer, length, sizeof(unsigned char));

		DEBUG_PRINT(("devioctl: Probe done\n"));

		// Move memory
		RtlCopyMemory(driver_buffer, user_buffer, length);

		irp->IoStatus.Information = length;

	}
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		// ProbeForWrite failed
		irp->IoStatus.Status = STATUS_ACCESS_VIOLATION;
		irp->IoStatus.Information = 0;
		DEBUG_PRINT(("devioctl: Oops!\n"));
		IoCompleteRequest(
			irp,
			IO_NO_INCREMENT
		);
		return STATUS_ACCESS_VIOLATION;
	}

	DEBUG_PRINT(("devioctl: exiting neither write\n"));
	return STATUS_SUCCESS;
}

NTSTATUS ctl_in_direct(IN const PIRP irp, IN const PIO_STACK_LOCATION stack) noexcept
{
	
	auto* in_buffer = irp->AssociatedIrp.SystemBuffer;
	const auto in_length = stack->Parameters.DeviceIoControl.InputBufferLength;
	auto* out_buffer = MmGetSystemAddressForMdl(irp->MdlAddress);
	const auto out_length = stack->Parameters.DeviceIoControl.OutputBufferLength;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IOCTL.SYS:  InB=%08lx len=%lx, OutB=%08lx, len=%lx\n", reinterpret_cast<ULONG>(in_buffer), in_length, reinterpret_cast<ULONG>(out_buffer), out_length);

	return STATUS_SUCCESS;
	
}

NTSTATUS ctl_out_direct(IN const PIRP irp, IN const PIO_STACK_LOCATION stack) noexcept
{

	auto* in_buffer = MmGetSystemAddressForMdl(irp->MdlAddress);
	const auto in_length = stack->Parameters.DeviceIoControl.InputBufferLength;
	auto* out_buffer = irp->AssociatedIrp.SystemBuffer;
	const auto out_length = stack->Parameters.DeviceIoControl.OutputBufferLength;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IOCTL.SYS:  InB=%08lx len=%lx, OutB=%08lx, len=%lx\n", reinterpret_cast<ULONG>(in_buffer), in_length, reinterpret_cast<ULONG>(out_buffer), out_length);

	return STATUS_SUCCESS;
	
}

NTSTATUS ctl_buffered(IN const PIRP irp, IN const PIO_STACK_LOCATION stack) noexcept
{

	auto* in_buffer = irp->AssociatedIrp.SystemBuffer;
	const auto in_length = stack->Parameters.DeviceIoControl.InputBufferLength;
	auto* out_buffer = irp->AssociatedIrp.SystemBuffer;
	const auto out_length = stack->Parameters.DeviceIoControl.OutputBufferLength;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IOCTL.SYS:  InB=%08lx len=%lx, OutB=%08lx, len=%lx\n", reinterpret_cast<ULONG>(in_buffer), in_length, reinterpret_cast<ULONG>(out_buffer), out_length);

	return STATUS_SUCCESS;
	
}

NTSTATUS ctl_neither(IN const PIRP irp, IN const PIO_STACK_LOCATION stack) noexcept
{

	auto* in_buffer = stack->Parameters.DeviceIoControl.Type3InputBuffer;
	const auto in_length = stack->Parameters.DeviceIoControl.InputBufferLength;
	auto* out_buffer = irp->UserBuffer;
	const auto out_length = stack->Parameters.DeviceIoControl.OutputBufferLength;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IOCTL.SYS:  InB=%08lx len=%lx, OutB=%08lx, len=%lx\n", reinterpret_cast<ULONG>(in_buffer), in_length, reinterpret_cast<ULONG>(out_buffer), out_length);

	return STATUS_SUCCESS;
	
}