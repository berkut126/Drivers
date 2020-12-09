#include <ntddk.h>
#include <wdf.h>
#include "Trace.h"
#include <Source.tmh>

extern "C"{
// ReSharper disable once CppInconsistentNaming
	DRIVER_INITIALIZE DriverEntry;
}

EVT_WDF_DRIVER_DEVICE_ADD kmdf_example_device_add;
EVT_WDF_DRIVER_UNLOAD kmdf_example_device_unload;
EVT_WDF_IO_QUEUE_IO_READ kmdf_example_queue_read;
EVT_WDF_IO_QUEUE_IO_WRITE kmdf_example_queue_write;

WDFQUEUE read_queue, write_queue;

NTSTATUS create_device() noexcept;

constexpr int buffer_size = 16;

struct example_context
{
	unsigned char image[buffer_size];
};

WDF_DECLARE_CONTEXT_TYPE(example_context);

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ PUNICODE_STRING    RegistryPath
)
{

	WPP_INIT_TRACING(DriverObject, RegistryPath);

	WDF_DRIVER_CONFIG config;
	
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KMDF example: DriverEntry\n"));
	TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "DriverEntry\n");

	WDF_DRIVER_CONFIG_INIT(&config, kmdf_example_device_add);
	config.EvtDriverUnload = kmdf_example_device_unload;
	config.DriverPoolTag = 'S3AS';  // NOLINT(clang-diagnostic-four-char-constants)

	auto status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);  // NOLINT(clang-diagnostic-zero-as-null-pointer-constant
	
	if(!NT_SUCCESS(status))
	{

		WPP_CLEANUP(DriverObject);
		
	}

	TraceEvents(TRACE_LEVEL_VERBOSE, DBG_CREATE, "DriverEntry\n");
	status = create_device();

	if(!NT_SUCCESS(status))
	{

		WPP_CLEANUP(DriverObject);
		
	}
	
	return status;
	
}

NTSTATUS kmdf_example_device_add(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
{

	UNREFERENCED_PARAMETER(Driver);

	WDFDEVICE device;

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KMDF example: DeviceAdd\n"));
	TraceEvents(TRACE_LEVEL_VERBOSE, DBG_CREATE, "DeviceAdd\n");

	return WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);
	
}

void kmdf_example_device_unload(
	WDFDRIVER Driver
)
{

	WdfObjectDelete(read_queue);

	WdfObjectDelete(write_queue);

	
	
	WPP_CLEANUP(WdfDriverWdmGetDriverObject(Driver));
	
}

void kmdf_example_queue_read(
	WDFQUEUE Queue,
	WDFREQUEST Request,
	size_t Length
)
{

	UNREFERENCED_PARAMETER(Queue);

	WDFREQUEST write_request;
	
	auto status = WdfIoQueueRetrieveNextRequest(write_queue, &write_request);

	if(status == STATUS_NO_MORE_ENTRIES)
	{

		WdfRequestForwardToIoQueue(Request, read_queue);
		return;
		
	}

	WDFMEMORY output;
	PVOID input;
	size_t length;

	status = WdfRequestRetrieveOutputMemory(Request, &output);

	if(!NT_SUCCESS(status))
	{

		TraceEvents(TRACE_LEVEL_FATAL, DBG_READ_OUTPUT_FAIL, "Could not get output memory, status: %u", status);
		WdfRequestCompleteWithInformation(Request, STATUS_UNSUCCESSFUL, 0);
		WdfRequestCompleteWithInformation(write_request, STATUS_UNSUCCESSFUL, 0);
		
	}
	
	status = WdfRequestRetrieveInputBuffer(write_request, 0, &input, &length);

	if (!NT_SUCCESS(status))
	{

		TraceEvents(TRACE_LEVEL_FATAL, DBG_READ_INPUT_FAIL, "Could not get output memory, status: %u", status);
		WdfRequestCompleteWithInformation(Request, STATUS_UNSUCCESSFUL, 0);
		WdfRequestCompleteWithInformation(write_request, STATUS_UNSUCCESSFUL, 0);

	}

	const auto copied = min(Length, length);

	status = WdfMemoryCopyFromBuffer(output, 0, input, copied);

	if (!NT_SUCCESS(status))
	{

		TraceEvents(TRACE_LEVEL_FATAL, DBG_READ_INPUT_FAIL, "Could not get input memory, status: %u", status);
		WdfRequestCompleteWithInformation(Request, STATUS_UNSUCCESSFUL, 0);
		WdfRequestCompleteWithInformation(write_request, STATUS_UNSUCCESSFUL, 0);

	}

	WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, copied);
	WdfRequestCompleteWithInformation(write_request, STATUS_SUCCESS, copied);
	
}

void kmdf_example_queue_write(
	WDFQUEUE Queue,
	WDFREQUEST Request,
	size_t Length
)
{

	UNREFERENCED_PARAMETER(Queue);

	WDFREQUEST read_request;

	auto status = WdfIoQueueRetrieveNextRequest(read_queue, &read_request);

	if (status == STATUS_NO_MORE_ENTRIES)
	{

		WdfRequestForwardToIoQueue(Request, write_queue);
		return;

	}

	WDFMEMORY input;
	PVOID output;
	size_t length;

	status = WdfRequestRetrieveInputMemory(Request, &input);

	if (!NT_SUCCESS(status))
	{

		TraceEvents(TRACE_LEVEL_FATAL, DBG_WRITE_INPUT_FAIL, "Could not get input memory, status: %u", status);
		WdfRequestCompleteWithInformation(Request, STATUS_UNSUCCESSFUL, 0);
		WdfRequestCompleteWithInformation(read_request, STATUS_UNSUCCESSFUL, 0);

	}

	status = WdfRequestRetrieveOutputBuffer(read_request, 0, &output, &length);

	if (!NT_SUCCESS(status))
	{

		TraceEvents(TRACE_LEVEL_FATAL, DBG_WRITE_OUTPUT_FAIL, "Could not get output memory, status: %u", status);
		WdfRequestCompleteWithInformation(Request, STATUS_UNSUCCESSFUL, 0);
		WdfRequestCompleteWithInformation(read_request, STATUS_UNSUCCESSFUL, 0);

	}

	const auto copied = min(Length, length);

	status = WdfMemoryCopyToBuffer(input, 0, output, copied);

	if (!NT_SUCCESS(status))
	{

		TraceEvents(TRACE_LEVEL_FATAL, DBG_READ_INPUT_FAIL, "Could not get output memory, status: %u", status);
		WdfRequestCompleteWithInformation(Request, STATUS_UNSUCCESSFUL, 0);
		WdfRequestCompleteWithInformation(read_request, STATUS_UNSUCCESSFUL, 0);

	}

	WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, copied);
	WdfRequestCompleteWithInformation(read_request, STATUS_SUCCESS, copied);

}

NTSTATUS create_device() noexcept
{

	auto* device_init = WdfControlDeviceInitAllocate(WdfGetDriver(), &SDDL_DEVOBJ_SYS_ALL_ADM_ALL);

	WdfDeviceInitSetIoType(device_init, WdfDeviceIoBuffered);

	WdfDeviceInitSetExclusive(device_init, false);

	UNICODE_STRING name, symlink;

	RtlInitUnicodeString(&name, L"\\Device\\Simple3Async");

	RtlInitUnicodeString(&symlink, L"\\DosDevices\\MyDevice");

	auto status = WdfDeviceInitAssignName(device_init, &name);

	if (!NT_SUCCESS(status))
	{

		WdfDeviceInitAssignName(device_init, nullptr);

	}

	WdfDeviceInitSetDeviceType(device_init, 42769);

	WDF_OBJECT_ATTRIBUTES attributes;

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, example_context);

	attributes.ParentObject = nullptr;

	WDFDEVICE device;

	status = WdfDeviceCreate(&device_init, &attributes, &device);

	DbgBreakPoint();

	if(!NT_SUCCESS(status))
	{

		return status;
		
	}

	status = WdfDeviceCreateSymbolicLink(device, &symlink);

	DbgBreakPoint();

	if (!NT_SUCCESS(status))
	{

		return status;

	}

	TraceEvents(TRACE_LEVEL_VERBOSE, DBG_QUEUE_CREATE, "CreateDevice\n");

	WDF_IO_QUEUE_CONFIG queue_config;

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queue_config, WdfIoQueueDispatchSequential);

	queue_config.DefaultQueue = true;

	queue_config.EvtIoRead = kmdf_example_queue_read;
	queue_config.EvtIoWrite = kmdf_example_queue_write;

	queue_config.PowerManaged = WdfUseDefault;

	WDFQUEUE queue;
	
	status = WdfIoQueueCreate(
		device,
		&queue_config,
		WDF_NO_OBJECT_ATTRIBUTES,  // NOLINT(clang-diagnostic-zero-as-null-pointer-constant)
		&queue
	);

	if(!NT_SUCCESS(status))
	{

		DbgBreakPoint();
		
	}

	WDF_IO_QUEUE_CONFIG write_queue_config;

	WDF_IO_QUEUE_CONFIG_INIT(&write_queue_config, WdfIoQueueDispatchManual);

	write_queue_config.DefaultQueue = false;
	write_queue_config.PowerManaged = WdfUseDefault;

	status = WdfIoQueueCreate(
		device,
		&write_queue_config,
		WDF_NO_OBJECT_ATTRIBUTES,  // NOLINT(clang-diagnostic-zero-as-null-pointer-constant)
		&write_queue
	);

	if (!NT_SUCCESS(status))
	{

		DbgBreakPoint();

	}

	WDF_IO_QUEUE_CONFIG read_queue_config;

	WDF_IO_QUEUE_CONFIG_INIT(&read_queue_config, WdfIoQueueDispatchManual);

	read_queue_config.DefaultQueue = false;
	read_queue_config.PowerManaged = WdfUseDefault;

	status = WdfIoQueueCreate(
		device,
		&read_queue_config,
		WDF_NO_OBJECT_ATTRIBUTES,  // NOLINT(clang-diagnostic-zero-as-null-pointer-constant)
		&read_queue
	);

	if (!NT_SUCCESS(status))
	{

		DbgBreakPoint();

	}

	return status;
	
}