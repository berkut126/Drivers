// Core Windows kernel definitions
#include <ntddk.h>
// WDF
#include <wdf.h>

// DebugPrint declaration
#if DBG
#define DebugPrint(x) DbgPrint x
#else
#define DebugPrint(x)
#endif

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
const int SIMPLE_3_DEVICE_TYPE = 42769;
// Device characteristics
const int SIMPLE_3_DEVICE_CHARACTERISTICS = 0;
// Device exclusiveness policy
const BOOLEAN SIMPLE_3_DEVICE_EXCLUSIVE = FALSE;

// Objects
PDEVICE_OBJECT deviceObject;

//Entry point
NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ PUNICODE_STRING    RegistryPath
)
{
	DebugPrint(("simple3: Entering DriverEntry\n"));
	// Status variable
	auto status = STATUS_SUCCESS;

	// Device name
	UNICODE_STRING devName;

	// Symbolic name
	UNICODE_STRING symbolicName;

	// Initialize device name with \Device\Simple2
	RtlInitUnicodeString(&devName, L"\\Device\\Simple3");

	// Initialize symbolic name with \DosDevices\Simple3Link
	RtlInitUnicodeString(&symbolicName, L"\\DosDevices\\Simple3Link");

	// Create device
	status = IoCreateDevice(
		DriverObject,
		0,
		&devName,
		SIMPLE_3_DEVICE_TYPE,
		SIMPLE_3_DEVICE_CHARACTERISTICS,
		SIMPLE_3_DEVICE_EXCLUSIVE,
		&deviceObject
	);

	if (status != STATUS_SUCCESS) {

		DebugPrint(("simple3: Exiting DriverEntry\n"));

		return status;

	}

	// Create symbolic link
	status = IoCreateSymbolicLink(&symbolicName, &devName);
	
	// Delete evice if link init failed
	if (status != STATUS_SUCCESS) {

		IoDeleteDevice(deviceObject);

		DebugPrint(("simple3: Exiting DriverEntry\n"));

		return status;

	}

	// Set handles
	DriverObject->MajorFunction[IRP_MJ_CREATE] = OnCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = OnClose;
	DriverObject->DriverUnload = OnDrvUnload;

	DebugPrint(("simple3: Exiting DriverEntry\n"));

	return status;
}

// OnCreate handle
NTSTATUS OnCreate(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
)
{
	DebugPrint(("simple3: Entering OnCreate\n"));
	// Set status to sucessful
	Irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	Irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		Irp,
		IO_NO_INCREMENT
	);

	DebugPrint(("simple3: Exiting OnCreate\n"));

	return STATUS_SUCCESS;

}

// OnCreate handle
NTSTATUS OnClose(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
)
{
	DebugPrint(("simple3: Entering OnClose\n"));

	// Set status to sucessful
	Irp->IoStatus.Status = STATUS_SUCCESS;
	// No additional info
	Irp->IoStatus.Information = 0;

	// Complete request without incrementing priority
	IoCompleteRequest(
		Irp,
		IO_NO_INCREMENT
	);

	DebugPrint(("simple3: Exiting OnClose\n"));

	return STATUS_SUCCESS;

}

VOID OnDrvUnload(PDRIVER_OBJECT DriverObject) {

	DebugPrint(("simple3: Entering OnDrvUnload\n"));

	// Symbolic name
	UNICODE_STRING symbolicName;

	// Initialize symbolic name with \DosDevices\Simple3Link
	RtlInitUnicodeString(&symbolicName, L"\\DosDevices\\Simple3Link");

	// Delete symbolic link
	IoDeleteSymbolicLink(&symbolicName);

	// Delete device
	IoDeleteDevice(DriverObject->DeviceObject);

	DebugPrint(("simple3: Exiting OnDrvUnload\n"));

}
