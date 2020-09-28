// Core Windows kernel definitions
#include <ntddk.h>
// WDF
#include <wdf.h>

// Declarations
// Entry point
DRIVER_INITIALIZE DriverEntry;
// OnCreate handle
DRIVER_DISPATCH OnCreate;
// OnClose handle
DRIVER_DISPATCH OnClose;
// Driver close handle
DRIVER_UNLOAD OnDrvUnload;

// Constants
// Device type constant
const int SIMPLE_2_DEVICE_TYPE = 32769;
// Device characteristics
const int SIMPLE_2_DEVICE_CHARACTERISTICS = 0;
// Device exclusiveness policy
const BOOLEAN SIMPLE_2_DEVICE_EXCLUSIVE = FALSE;

// Objects
PDEVICE_OBJECT deviceObject;

//Entry point
NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ PUNICODE_STRING    RegistryPath
)
{
	// Status variable
	auto status = STATUS_SUCCESS;

	// Device name
	PUNICODE_STRING devName();

	// Initialize device name with \Device\Simple2
	RtlInitUnicodeString(devName, L"\\Device\\Simple2");

	// Create device
	status = IoCreateDevice(
		DriverObject,
		0,
		devName,
		SIMPLE_2_DEVICE_TYPE,
		SIMPLE_2_DEVICE_CHARACTERISTICS,
		SIMPLE_2_DEVICE_EXCLUSIVE,
		&deviceObject
	);

	if(status != STATUS_SUCCESS)
		return status;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = OnCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = OnClose;
	DriverObject->DriverUnload = OnDrvUnload;

	return status;

}

// OnCreate handle
NTSTATUS OnCreate(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
)
{
	// Set status to sucessful
	Irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	Irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		Irp,
		IO_NO_INCREMENT
	);

	return STATUS_SUCCESS;

}

// OnCreate handle
NTSTATUS OnClose(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
)
{
	// Set status to sucessful
	Irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	Irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		Irp,
		IO_NO_INCREMENT
	);

	return STATUS_SUCCESS;

}

VOID OnDrvUnload(PDRIVER_OBJECT DriverObject) {

	// Delete device
	IoDeleteDevice(DriverObject->DeviceObject);

}
