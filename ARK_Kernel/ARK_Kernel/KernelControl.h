#pragma once

#include "DriverBase.h"

NTSTATUS DriverEntry(
	DRIVER_OBJECT* DriverObject,
	PUNICODE_STRING  RegistryPath
);

VOID Unload(
	DRIVER_OBJECT* DriverObject
);

NTSTATUS DispatchCreate(
	DEVICE_OBJECT* DeviceObject,
	IRP* Irp
);

NTSTATUS DispatchClose(
	DEVICE_OBJECT* DeviceObject,
	IRP* Irp
);


NTSTATUS DispatchControl(
	DEVICE_OBJECT* DeviceObject,
	IRP* Irp
);

#pragma alloc_text( "INIT", DriverEntry)			// INIT节初始化完即被释放
#pragma alloc_text( "PAGE", Unload)
#pragma alloc_text( "PAGE", DispatchCreate)
#pragma alloc_text( "PAGE", DispatchClose)
#pragma alloc_text( "PAGE", DispatchControl)