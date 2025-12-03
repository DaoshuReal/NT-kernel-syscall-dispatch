#include <ntifs.h>

VOID driverUnload(_In_ PDRIVER_OBJECT driverObject)
{
	UNREFERENCED_PARAMETER(driverObject);
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
	UNREFERENCED_PARAMETER(registryPath);

	driverObject->DriverUnload = driverUnload;

	return STATUS_SUCCESS;
}
