/*
* This is a personal academic project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
*/
// Core Windows kernel definitions
#include <ntddk.h>

// Messages
#include "Messages.h"

// DebugPrint declaration
#if DBG
#define DEBUG_PRINT(x) DbgPrintEx (DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, x)
#else
#define DEBUG_PRINT(x)
#endif

// Declarations
//
// Entry point
//
extern "C" {
	DRIVER_INITIALIZE DriverEntry;
}
//
// Handles
// 
// OnCreate handle
DRIVER_DISPATCH on_create;
// OnClose handle
DRIVER_DISPATCH on_close;
//OnRead handle
DRIVER_DISPATCH on_read;
//OnWrite handle
DRIVER_DISPATCH on_write;
// Driver close handle
DRIVER_UNLOAD on_drv_unload;
//
// Helper functions
// 
// Write to event log helper function
BOOLEAN write_event(IN NTSTATUS, IN PVOID, IN PIRP) noexcept;
// BufferedIO read helper function
NTSTATUS read_buffered(IN PDEVICE_OBJECT, IN PIRP, PIO_STACK_LOCATION stack) noexcept;
// BufferedIO write helper function
NTSTATUS write_buffered(IN PDEVICE_OBJECT, IN PIRP, PIO_STACK_LOCATION stack) noexcept;
// DirectIO read helper function
NTSTATUS read_direct(IN PDEVICE_OBJECT, IN PIRP, PIO_STACK_LOCATION stack) noexcept;
// DirectIO write helper function
NTSTATUS write_direct(IN PDEVICE_OBJECT, IN PIRP, PIO_STACK_LOCATION stack) noexcept;
// NeitherIO read helper function
NTSTATUS read_neither(IN PDEVICE_OBJECT, IN PIRP, PIO_STACK_LOCATION stack) noexcept;
// NeitherIO write helper function
NTSTATUS write_neither(IN PDEVICE_OBJECT, IN PIRP, PIO_STACK_LOCATION stack) noexcept;


// Device objects
PDEVICE_OBJECT simple_4_device_object;
PDEVICE_OBJECT simple_5_device_object;
PDEVICE_OBJECT simple_6_device_object;

// Buffer size
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

constexpr device simple4 = {
	L"\\Device\\BufferedIO",
	L"\\DosDevices\\BufferedIO",
	42769,
	0,
	FALSE,
	DO_BUFFERED_IO,
	's4b', // NOLINT(clang-diagnostic-multichar)
	MSG_CREATE_BUFFERED,
	MSG_CREATE_BUFFERED_SUCCESS,
	MSG_CREATE_BUFFERED_FAILED
};

constexpr device simple5 = {
	L"\\Device\\DirectIO",
	L"\\DosDevices\\DirectIO",
	42769,
	0,
	FALSE,
	DO_DIRECT_IO,
	's5d', // NOLINT(clang-diagnostic-multichar)
	MSG_CREATE_DIRECT,
	MSG_CREATE_DIRECT_SUCCESS,
	MSG_CREATE_DIRECT_FAILED
};

constexpr device simple6 = {
	L"\\Device\\NeitherIO",
	L"\\DosDevices\\NeitherIO",
	42769,
	0,
	FALSE,
	1,
	's6n', // NOLINT(clang-diagnostic-multichar)
	MSG_CREATE_NEITHER,
	MSG_CREATE_NEITHER_SUCCESS,
	MSG_CREATE_NEITHER_FAILED
};

// Device init helper function
NTSTATUS device_init(device const*, PDRIVER_OBJECT, PDEVICE_OBJECT) noexcept;

//Entry point
NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ [[maybe_unused]] PUNICODE_STRING    RegistryPath
)
{
	DEBUG_PRINT(("simple4: Entering DriverEntry\n"));
	write_event(MSG_DRIVER_ENTRY, DriverObject, nullptr);

	// Status variable
	auto status = device_init(&simple4, DriverObject, simple_4_device_object);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}
	status = device_init(&simple5, DriverObject, simple_5_device_object);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}
	status = device_init(&simple6, DriverObject, simple_6_device_object);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}
	// Set handles
	DriverObject->MajorFunction[IRP_MJ_CREATE] = on_create;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = on_close;
	DriverObject->MajorFunction[IRP_MJ_READ] = on_read;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = on_write;
	DriverObject->DriverUnload = on_drv_unload;

	DEBUG_PRINT(("simple4: Exiting DriverEntry\n"));

	return status;
}

// OnCreate handle
NTSTATUS on_create(
	[[maybe_unused]] PDEVICE_OBJECT DeviceObject,
	const PIRP irp
)
{
	write_event(MSG_CREATE, DeviceObject->DriverObject, irp);
	DEBUG_PRINT(("simple4: Entering OnCreate\n"));
	// Set status to successful
	irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	DEBUG_PRINT(("simple4: Exiting OnCreate\n"));

	return STATUS_SUCCESS;

}

// OnCreate handle
NTSTATUS on_close(
	[[maybe_unused]] PDEVICE_OBJECT DeviceObject,
	const PIRP irp
)
{
	write_event(MSG_CLOSE, DeviceObject->DriverObject, irp);
	DEBUG_PRINT(("simple4: Entering OnClose\n"));

	// Set status to successful
	irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	DEBUG_PRINT(("simple4: Exiting OnClose\n"));

	return STATUS_SUCCESS;

}

VOID on_drv_unload(PDRIVER_OBJECT DriverObject) {

	DEBUG_PRINT(("simple4: Entering OnDrvUnload\n"));

	// Symbolic name
	UNICODE_STRING symbolicName;

	// Initialize symbolic name with \DosDevices\simple4Link
	RtlInitUnicodeString(&symbolicName, L"\\DosDevices\\BufferedIO");

	// Delete symbolic link
	IoDeleteSymbolicLink(&symbolicName);

	// Delete device
	IoDeleteDevice(DriverObject->DeviceObject);

	DEBUG_PRINT(("simple4: Exiting OnDrvUnload\n"));

}

// OnRead handle
NTSTATUS on_read(
	const PDEVICE_OBJECT device,  // NOLINT(misc-misplaced-const)
	const PIRP irp  // NOLINT(misc-misplaced-const)
)
{
	write_event(MSG_READ, device->DriverObject, irp);
	DEBUG_PRINT(("simple4: Entering OnRead\n"));

	auto status = STATUS_SUCCESS;

	const auto flags = device->Flags;

	auto* stack = IoGetCurrentIrpStackLocation(irp);

	// Buffered I/O
	if (flags & DO_BUFFERED_IO)
	{

		// Check that we're reading
		if (stack->MajorFunction == IRP_MJ_READ) {

			status = read_buffered(device, irp, stack);

		}

	}

	// Direct I/O
	else if (flags & DO_DIRECT_IO)
	{

		// Check that we're reading
		if (stack->MajorFunction == IRP_MJ_READ) {

			status = read_direct(device, irp, stack);

		}

	}

	// Neither Buffered nor Direct I/O
	else
	{
		// Check that we're reading
		if (stack->MajorFunction == IRP_MJ_READ)
		{

			status = read_neither(device, irp, stack);

		}


	}

	irp->IoStatus.Status = status;

	// Complete request without incrementing priority
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	DEBUG_PRINT(("simple4: Exiting OnRead\n"));

	return STATUS_SUCCESS;

}

// OnWrite handle
NTSTATUS on_write(
	[[maybe_unused]] PDEVICE_OBJECT device,
	const PIRP irp  // NOLINT(misc-misplaced-const)
)
{
	write_event(MSG_WRITE, device->DriverObject, irp);
	DEBUG_PRINT(("simple4: Entering OnWrite\n"));

	NTSTATUS status = STATUS_SUCCESS;

	const auto flags = device->Flags;

	auto* stack = IoGetCurrentIrpStackLocation(irp);

	// Buffered I/O
	if (flags & DO_BUFFERED_IO)
	{

		// Check that we're writing
		if (stack->MajorFunction == IRP_MJ_WRITE) {

			status = write_buffered(device, irp, stack);

		}

	}

	// Direct I/O
	else if (flags & DO_DIRECT_IO)
	{

		// Check that we're writing
		if (stack->MajorFunction == IRP_MJ_WRITE) {

			status = write_direct(device, irp, stack);

		}

	}

	// Neither Buffered nor Direct I/O
	else
	{
		// Check that we're writing
		if (stack->MajorFunction == IRP_MJ_WRITE)
		{

			status = write_neither(device, irp, stack);

		}

	}

	irp->IoStatus.Status = status;

	// Complete request without incrementing priority
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	DEBUG_PRINT(("simple4: Exiting OnWrite\n"));

	return STATUS_SUCCESS;

}

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

		DEBUG_PRINT(("simple4: Exiting DriverEntry\n"));
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

		DEBUG_PRINT(("simple4: Exiting DriverEntry\n"));

		return status;

	}

	return status;

}

NTSTATUS read_buffered(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("simple4: entering buffered read\n"));
	write_event(MSG_BUFFERED_READ, device_object, irp);

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	//Get User Output Buffer & Length 
	const auto length = stack->Parameters.Read.Length > buffer_size ? buffer_size : stack->Parameters.Read.Length;
	auto* user_buffer = irp->AssociatedIrp.SystemBuffer;

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		write_event(MSG_BUFFERED_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlCopyMemory(user_buffer, driver_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("simple4: exiting buffered read\n"));
	return STATUS_SUCCESS;
}

NTSTATUS write_buffered(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("simple4: entering buffered write\n"));
	write_event(MSG_BUFFERED_WRITE, device_object, irp);

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	//Get User Input Buffer & Length
	const auto length = stack->Parameters.Write.Length > buffer_size ? buffer_size : stack->Parameters.Read.Length;
	auto* user_buffer = irp->AssociatedIrp.SystemBuffer;

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		write_event(MSG_BUFFERED_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlZeroMemory(driver_buffer, buffer_size);

	RtlCopyMemory(driver_buffer, user_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("simple4: exiting buffered write\n"));
	return STATUS_SUCCESS;
}
NTSTATUS read_direct(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("simple4: entering direct read\n"));
	write_event(MSG_DIRECT_READ, device_object, irp);

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	if (irp->MdlAddress == nullptr)
	{

		DEBUG_PRINT(("simple4: Mdl is null\n"));
		write_event(MSG_DIRECT_READ_MDL_EMPTY, device_object, irp);
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	// Get User Output Buffer & Length 
	const auto length = MmGetMdlByteCount(irp->MdlAddress) > buffer_size ? buffer_size : MmGetMdlByteCount(irp->MdlAddress);
	auto* user_buffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);

	if (user_buffer == nullptr)
	{
		write_event(MSG_DIRECT_USER_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		write_event(MSG_DIRECT_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlCopyMemory(user_buffer, driver_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("simple4: exiting direct read\n"));
	return STATUS_SUCCESS;

}

NTSTATUS write_direct(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("simple4: entering direct write\n"));
	write_event(MSG_DIRECT_WRITE, device_object, irp);

	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);

	if (irp->MdlAddress == nullptr)
	{

		DEBUG_PRINT(("simple4: Mdl is null\n"));
		write_event(MSG_DIRECT_READ_MDL_EMPTY, device_object, irp);
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	//Get User Output Buffer & Length 
	const auto length = stack->Parameters.Write.ByteOffset.LowPart > buffer_size ? buffer_size : stack->Parameters.Write.ByteOffset.LowPart;
	// DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Mdl equals: %f", irp->MdlAddress);
	auto* user_buffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority);

	if (user_buffer == nullptr)
	{
		write_event(MSG_DIRECT_USER_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//Get Driver Data Buffer & Length
	auto* driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		write_event(MSG_BUFFERED_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}

	RtlZeroMemory(driver_buffer, buffer_size);

	RtlCopyMemory(driver_buffer, user_buffer, length);

	irp->IoStatus.Information = length;

	DEBUG_PRINT(("simple4: exiting direct write\n"));
	return STATUS_SUCCESS;
}

NTSTATUS read_neither(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("simple4: entering neither read\n"));
	write_event(MSG_NEITHER_READ, device_object, irp);
	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);
	//Get Driver Data Buffer & Length
	auto* const driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		write_event(MSG_BUFFERED_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}
	_try
	{

		// User buffer
		auto* user_buffer = irp->UserBuffer;

		// Size (can't be more than buffer_size [16] bytes)
		const auto length = stack->Parameters.Write.Length > buffer_size ? buffer_size : stack->Parameters.Write.Length;

		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "simple4: Probe started, user_buffer: %s, driver_buffer: %s\n", user_buffer, driver_buffer);
		
		// Test write access
		ProbeForRead(user_buffer, length, sizeof(unsigned char));

		DEBUG_PRINT(("simple4: Probe done\n"));

		// Move memory
		RtlCopyMemory(user_buffer, driver_buffer, length);

		irp->IoStatus.Information = length;

	}
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		// ProbeForWrite failed
		irp->IoStatus.Status = STATUS_ACCESS_VIOLATION;
		DEBUG_PRINT(("simple4: Oops!\n"));
		irp->IoStatus.Information = 0;
		IoCompleteRequest(
			irp,
			IO_NO_INCREMENT
		);
		return STATUS_ACCESS_VIOLATION;
	}
	DEBUG_PRINT(("simple4: exiting neither read\n"));
	return STATUS_SUCCESS;
}

NTSTATUS write_neither(IN const PDEVICE_OBJECT device_object, IN const PIRP irp, [[maybe_unused]] const PIO_STACK_LOCATION stack) noexcept
{

	DEBUG_PRINT(("simple4: entering neither write\n"));
	write_event(MSG_DIRECT_WRITE, device_object, irp);
	auto* device_extension_object = static_cast<device_extension*>(device_object->DeviceExtension);
	//Get Driver Data Buffer & Length
	auto* const driver_buffer = device_extension_object->image;
	if (driver_buffer == nullptr)
	{

		write_event(MSG_BUFFERED_BUFFER_EMPTY, device_object, irp);
		irp->IoStatus.Information = 0;
		return STATUS_INSUFFICIENT_RESOURCES;

	}
	_try
	{

		// User buffer
		auto * user_buffer = irp->UserBuffer;

		// Size (can't be more than buffer_size [16] bytes)
		const auto length = stack->Parameters.Read.Length > buffer_size ? buffer_size : stack->Parameters.Read.Length;

		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "simple4: Probe started, user_buffer: %s, driver_buffer: %s\n", user_buffer, driver_buffer);
		
		// Test write access
		ProbeForWrite(user_buffer, length, sizeof(unsigned char));

		DEBUG_PRINT(("simple4: Probe done\n"));

		// Move memory
		RtlCopyMemory(driver_buffer, user_buffer, length);

		irp->IoStatus.Information = length;

	}
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		// ProbeForWrite failed
		irp->IoStatus.Status = STATUS_ACCESS_VIOLATION;
		irp->IoStatus.Information = 0;
		DEBUG_PRINT(("simple4: Oops!\n"));
		IoCompleteRequest(
			irp,
			IO_NO_INCREMENT
		);
		return STATUS_ACCESS_VIOLATION;
	}

	DEBUG_PRINT(("simple4: exiting neither write\n"));
	return STATUS_SUCCESS;
}