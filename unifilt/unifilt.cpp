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
// Universal driver dispatch
DRIVER_DISPATCH filter_drv_dispatch;
// Mocked driver dispatch
DRIVER_DISPATCH mocked_dispatch;
// GUI driver dispatch
DRIVER_DISPATCH gui_dispatch;
// Driver close handle
DRIVER_UNLOAD on_drv_unload;
// Driver call callback
IO_COMPLETION_ROUTINE filter_device_dispatch_complete;

// Device objects
PDEVICE_OBJECT unifilt_device_object;
device_extension buffer;

constexpr device unifilt_dev = {
	L"\\Device\\GuiDevice",
	L"\\DosDevices\\GuiDevice",
	42769,
	0,
	FALSE,
	1,
	's6uf', // NOLINT(clang-diagnostic-four-char-constants)
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
	DEBUG_PRINT(("unifilt: Entering DriverEntry\n"));
	write_event(MSG_DRIVER_ENTRY, DriverObject, nullptr);

	// Status variable
	const auto status = device_init(&unifilt_dev, DriverObject, &unifilt_device_object, &buffer);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}

	for(auto i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = filter_drv_dispatch;
	}

	// Set handles
	DriverObject->DriverUnload = on_drv_unload;

	DEBUG_PRINT(("unifilt: Exiting DriverEntry\n"));

	return status;
}

VOID on_drv_unload(PDRIVER_OBJECT DriverObject) {

	if (buffer.target_device_object != nullptr) {

		ObDereferenceObject(buffer.file_object);

		IoDetachDevice(buffer.target_device_object);

		IoAcquireRemoveLock(&buffer.lock, nullptr);
		IoReleaseRemoveLockAndWait(&buffer.lock, nullptr);

		IoDeleteDevice(buffer.target_device_object);

		buffer.file_object = nullptr;
		buffer.target_device_object = nullptr;

	}

	DEBUG_PRINT(("unifilt: Entering OnDrvUnload\n"));

	// Symbolic name
	UNICODE_STRING symbolic_name;

	// Initialize symbolic name with \DosDevices\unifiltLink
	RtlInitUnicodeString(&symbolic_name, L"\\DosDevices\\BufferedIO");

	// Delete symbolic link
	IoDeleteSymbolicLink(&symbolic_name);

	// Delete device
	IoDeleteDevice(DriverObject->DeviceObject);

	DEBUG_PRINT(("unifilt: Exiting OnDrvUnload\n"));

}

NTSTATUS filter_drv_dispatch(
	const PDEVICE_OBJECT device,
	const PIRP irp
)
{

	DEBUG_PRINT(("unifilt: Entering universal dispatch\n"));
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "unifilt: device = %p, unifilt_device_object = %p\n", device, unifilt_device_object);

	if (device == unifilt_device_object)
	{
		// The device is gui
		return gui_dispatch(device, irp);
	}
	else
	{
		// Process mocked device
		return mocked_dispatch(device, irp);
	}
	
}

NTSTATUS mocked_dispatch(
	const PDEVICE_OBJECT device,
	const PIRP irp
)
{
	DEBUG_PRINT(("unifilt: Entering mocked dipatch\n"));
	write_event(MSG_MOCKED, device, irp);
	auto status = STATUS_SUCCESS;

	const auto* const stack = IoGetCurrentIrpStackLocation(irp);

	switch(stack->MajorFunction)
	{

		case IRP_MJ_CREATE:
			DEBUG_PRINT(("Filtered: IRP_MJ_CREATE\n"));
			break;
		case IRP_MJ_CLOSE:
			DEBUG_PRINT(("Filtered: IRP_MJ_CLOSE\n"));
			break;
		case IRP_MJ_READ:
			DEBUG_PRINT(("Filtered: IRP_MJ_READ\n"));
			break;
		case IRP_MJ_WRITE:
			DEBUG_PRINT(("Filtered: IRP_MJ_WRITE\n"));
			break;
		case IRP_MJ_DEVICE_CONTROL:
			DEBUG_PRINT(("Filtered: IRP_MJ_DEVICE_CONTROL\n"));
			break;
		case IRP_MJ_INTERNAL_DEVICE_CONTROL:
			DEBUG_PRINT(("Filtered: IRP_MJ_INTERNAL_DEVICE_CONTROL\n"));
			break;

		default:
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Filtered: MajorFunction: %u\n", stack->MajorFunction);
	}

	//auto* const next_stack = IoGetNextIrpStackLocation(irp);
	//*next_stack = *stack;
	IoAcquireRemoveLock(&buffer.lock, nullptr);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Filtered: StackSize: %d, irp->CurrentLocation: %d, StackSize: %d\n", device->StackSize, irp->CurrentLocation, buffer.target_device_object->StackSize);
	irp->CurrentLocation++;
	irp->Tail.Overlay.CurrentStackLocation++;
	IoSetCompletionRoutine(irp, filter_device_dispatch_complete, device, TRUE, TRUE, TRUE);
	status = IoCallDriver(buffer.target_device_object, irp);

	return status;

}

NTSTATUS gui_dispatch(
	const PDEVICE_OBJECT device,
	const PIRP irp
)
{
	
	DEBUG_PRINT(("unifilt: Entering gui dipatch\n"));

	auto status = STATUS_SUCCESS;

	const auto* const stack = IoGetCurrentIrpStackLocation(irp);

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "unifilt: function = %u, ctl_code = %u\n", stack->Parameters.DeviceIoControl.IoControlCode, unifilt_my_create);

	// Create or delete device should be passed via DeviceIoControl
	if(stack->MajorFunction == IRP_MJ_DEVICE_CONTROL)
	{
		// Create or delete device
		if(stack->Parameters.DeviceIoControl.IoControlCode == unifilt_my_create)
		{

			if (buffer.target_device_object == nullptr) {

				IoInitializeRemoveLock(&buffer.lock, 0, 0, 0);

				DEBUG_PRINT(("unifilt: Creating device\n"));
				write_event(MSG_MOCKED_CREATE, device, irp);

				auto* input = irp->AssociatedIrp.SystemBuffer;

				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "unifilt: got length: %u\n", stack->Parameters.DeviceIoControl.InputBufferLength);

				const auto length = stack->Parameters.DeviceIoControl.InputBufferLength;

				const auto* const data = static_cast<PCWSTR>(input);

				UNICODE_STRING name;

				RtlInitUnicodeString(&name, data);

				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "unifilt: name = %wZ\n", &name);

				FILE_OBJECT* object;

				DEVICE_OBJECT* device_to_attach_to;

				status = IoGetDeviceObjectPointer(&name, FILE_ALL_ACCESS, &object, &device_to_attach_to);

				RtlFreeUnicodeString(&name);

				if (status != STATUS_SUCCESS)
				{

					DEBUG_PRINT(("unifilt: Failed to get device pointer\n"));
					return status;

				}

				DEVICE_OBJECT* new_device;

				status = IoCreateDevice(device->DriverObject, sizeof(device_to_attach_to->DeviceExtension), nullptr, device_to_attach_to->DeviceType, device_to_attach_to->Characteristics, FALSE, &new_device);
				if (status != STATUS_SUCCESS)
				{

					DEBUG_PRINT(("unifilt: Failed to create filtered device\n"));
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "unifilt: name = %d\n", status);
					return status;

				}

				new_device->Flags |= get_io_flag(device_to_attach_to);
				new_device->Flags &= ~DO_DEVICE_INITIALIZING;

				auto* saved_device = IoAttachDeviceToDeviceStack(new_device, device_to_attach_to);

				if (!saved_device)
				{

					DEBUG_PRINT(("unifilt: Failed to attach device\n"));
					return status;

				}

				buffer.target_device_object = saved_device;
				buffer.file_object = object;

				DEBUG_PRINT(("unifilt: Created mocked device\n"));
				return status;

			}
			
		}
		else if(stack->Parameters.DeviceIoControl.IoControlCode == unifilt_my_delete)
		{

			DEBUG_PRINT(("unifilt: Deleting device\n"));
			write_event(MSG_MOCKED_DELETE, device, irp);

			if (buffer.target_device_object != nullptr) {

				ObDereferenceObject(buffer.file_object);

				IoDetachDevice(buffer.target_device_object);

				IoAcquireRemoveLock(&buffer.lock, nullptr);
				IoReleaseRemoveLockAndWait(&buffer.lock, nullptr);
				
				IoDeleteDevice(buffer.target_device_object);

				buffer.file_object = nullptr;
				buffer.target_device_object = nullptr;

			}

			DEBUG_PRINT(("unifilt: Deleted device\n"));
			return STATUS_SUCCESS;
			
		}
	}
	else
	{

		return STATUS_SUCCESS;
		
	}

	return STATUS_SUCCESS;
	
}

NTSTATUS filter_device_dispatch_complete([[maybe_unused]] PDEVICE_OBJECT device_object, const PIRP irp, [[maybe_unused]] PVOID context)
{

	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}

	IoReleaseRemoveLock(&buffer.lock, nullptr);
	
	return STATUS_SUCCESS;
	
}