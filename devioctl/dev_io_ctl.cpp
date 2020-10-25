/*
* This is a personal academic project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
*/
// Core Windows kernel definitions
#include "Utility.h"

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
// OnRead handle
DRIVER_DISPATCH on_read;
// OnWrite handle
DRIVER_DISPATCH on_write;
// OnDeviceControl handle
DRIVER_DISPATCH on_ctl;
// Driver close handle
DRIVER_UNLOAD on_drv_unload;


// Device objects
PDEVICE_OBJECT dev_io_ctl_device_object;

constexpr device dev_io_ctl = {
	L"\\Device\\devioctl",
	L"\\DosDevices\\devioctl",
	42769,
	0,
	FALSE,
	1,
	's5d', // NOLINT(clang-diagnostic-multichar)
	MSG_CREATING,
	MSG_CREATE_SUCCESS,
	MSG_CREATE_FAILED
};

//Entry point
NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ [[maybe_unused]] PUNICODE_STRING    RegistryPath
)
{
	DEBUG_PRINT(("devioctl: Entering DriverEntry\n"));
	write_event(MSG_DRIVER_ENTRY, DriverObject, nullptr);

	// Status variable
	const auto status = device_init(&dev_io_ctl, DriverObject, dev_io_ctl_device_object);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}
	// Set handles
	DriverObject->MajorFunction[IRP_MJ_CREATE] = on_create;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = on_close;
	DriverObject->MajorFunction[IRP_MJ_READ] = on_read;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = on_ctl;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = on_write;
	DriverObject->DriverUnload = on_drv_unload;

	DEBUG_PRINT(("devioctl: Exiting DriverEntry\n"));

	return status;
}

// OnCreate handle
NTSTATUS on_create(
	[[maybe_unused]] PDEVICE_OBJECT DeviceObject,
	const PIRP irp
)
{
	write_event(MSG_CREATE, DeviceObject->DriverObject, irp);
	DEBUG_PRINT(("devioctl: Entering OnCreate\n"));
	// Set status to successful
	irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	DEBUG_PRINT(("devioctl: Exiting OnCreate\n"));

	return STATUS_SUCCESS;

}

// OnCreate handle
NTSTATUS on_close(
	[[maybe_unused]] PDEVICE_OBJECT DeviceObject,
	const PIRP irp
)
{
	write_event(MSG_CLOSE, DeviceObject->DriverObject, irp);
	DEBUG_PRINT(("devioctl: Entering OnClose\n"));

	// Set status to successful
	irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	DEBUG_PRINT(("devioctl: Exiting OnClose\n"));

	return STATUS_SUCCESS;

}

VOID on_drv_unload(PDRIVER_OBJECT DriverObject) {

	DEBUG_PRINT(("devioctl: Entering OnDrvUnload\n"));

	// Symbolic name
	UNICODE_STRING symbolicName;

	// Initialize symbolic name with \DosDevices\devioctlLink
	RtlInitUnicodeString(&symbolicName, L"\\DosDevices\\BufferedIO");

	// Delete symbolic link
	IoDeleteSymbolicLink(&symbolicName);

	// Delete device
	IoDeleteDevice(DriverObject->DeviceObject);

	DEBUG_PRINT(("devioctl: Exiting OnDrvUnload\n"));

}

// OnRead handle
NTSTATUS on_read(
	const PDEVICE_OBJECT device,  // NOLINT(misc-misplaced-const)
	const PIRP irp  // NOLINT(misc-misplaced-const)
)
{
	write_event(MSG_READ, device->DriverObject, irp);
	DEBUG_PRINT(("devioctl: Entering OnRead\n"));

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

	DEBUG_PRINT(("devioctl: Exiting OnRead\n"));

	return STATUS_SUCCESS;

}

// OnWrite handle
NTSTATUS on_write(
	[[maybe_unused]] PDEVICE_OBJECT device,
	const PIRP irp  // NOLINT(misc-misplaced-const)
)
{
	write_event(MSG_WRITE, device->DriverObject, irp);
	DEBUG_PRINT(("devioctl: Entering OnWrite\n"));

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

	DEBUG_PRINT(("devioctl: Exiting OnWrite\n"));

	return STATUS_SUCCESS;

}

NTSTATUS on_ctl(
	const PDEVICE_OBJECT device,
	const PIRP irp
)
{
	write_event(MSG_CTL, device->DriverObject, irp);
	DEBUG_PRINT(("devioctl: Entering OnDeviceControl\n"));

	auto* stack = IoGetCurrentIrpStackLocation(irp);

	const auto code = stack->Parameters.DeviceIoControl.IoControlCode & 0x03L;

	switch (code)
	{
		case METHOD_BUFFERED:
			ctl_buffered(irp, stack);
		break;
		case METHOD_IN_DIRECT:
			ctl_in_direct(irp, stack);
		break;
		case METHOD_OUT_DIRECT:
			ctl_out_direct(irp, stack);
			break;
		case METHOD_NEITHER:
			ctl_neither(irp, stack);
			break;
		default:
			return STATUS_FAIL_CHECK;
	}

	return STATUS_SUCCESS;
	
}