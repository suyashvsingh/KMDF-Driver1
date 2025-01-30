#include <ntddk.h>
#include <wdf.h>
#include "trace.h"
#include "Driver.tmh"

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFDriver1EvtDeviceAdd;

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
)
{
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    // NTSTATUS variable to record success or failure
    NTSTATUS status = STATUS_SUCCESS;

    // Allocate the driver configuration object
    WDF_DRIVER_CONFIG config;

    // Initialize the driver configuration object to register the
    // entry point for the EvtDeviceAdd callback, KMDFDriver1EvtDeviceAdd
    WDF_DRIVER_CONFIG_INIT(&config,
        KMDFDriver1EvtDeviceAdd
    );

    // Finally, create the driver object
    status = WdfDriverCreate(DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

NTSTATUS
KMDFDriver1EvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    // We're not using the driver object,
    // so we need to mark it as unreferenced
    UNREFERENCED_PARAMETER(Driver);

    NTSTATUS status;

    // Allocate the device object
    WDFDEVICE hDevice;

    // Create the device object
    status = WdfDeviceCreate(&DeviceInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &hDevice
    );

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDeviceCreate failed with status 0x%x\n", status);
		return status;
	}

    WDFKEY sKey;

    status = WdfDeviceOpenRegistryKey(
        hDevice,
        PLUGPLAY_REGKEY_DRIVER,
        KEY_READ,
        NULL,
        &sKey
    );

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDeviceOpenRegistryKey failed with status 0x%x\n", status);
		return status;
	}

    ULONG  length, valueType,value;
    DECLARE_CONST_UNICODE_STRING(valueName, L"SampleValue");
    status = WdfRegistryQueryValue(
        sKey,
        &valueName,
        sizeof(ULONG),
        &value,
        &length,
        &valueType
    );

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfRegistryQueryValue failed with status 0x%x\n", status);
		return status;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Value: %d\n", value);
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return status;
}