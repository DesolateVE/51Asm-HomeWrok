#include "MyPortEnum.h"

//PVOID EnumPortInformation(OUT PULONG	OutLength, IN USHORT	PortType)
//{
//	ULONG	BufLen = PAGE_SIZE;
//	PVOID	pInputBuff = NULL;
//	PVOID	pOutputBuff = NULL;
//	PVOID	pOutBuf = NULL;
//	NTSTATUS status = STATUS_SUCCESS;
//	HANDLE FileHandle = NULL;
//	UNICODE_STRING	DeviceName;
//	PFILE_OBJECT pFileObject = NULL;
//	PDEVICE_OBJECT pDeviceObject = NULL;
//	KEVENT	Event;
//	IO_STATUS_BLOCK StatusBlock;
//	PIRP		pIrp;
//	PIO_STACK_LOCATION StackLocation;
//	ULONG		NumOutputBuffers;
//	ULONG		i;
//	TCP_REQUEST_QUERY_INFORMATION_EX		TdiId;
//
//	RtlZeroMemory(&TdiId, sizeof(TCP_REQUEST_QUERY_INFORMATION_EX));
//
//	if (TCPPORT == PortType)
//	{
//		TdiId.ID.toi_entity.tei_entity = CO_TL_ENTITY;
//	}
//
//	if (UDPPORT == PortType)
//	{
//		TdiId.ID.toi_entity.tei_entity = CL_TL_ENTITY;
//	}
//
//	TdiId.ID.toi_entity.tei_instance = ENTITY_LIST_ID;
//	TdiId.ID.toi_class = INFO_CLASS_PROTOCOL;
//	TdiId.ID.toi_type = INFO_TYPE_PROVIDER;
//	TdiId.ID.toi_id = 0x102;
//
//	pInputBuff = (PVOID)&TdiId;
//
//	__try
//	{
//		if (UDPPORT == PortType)
//		{
//			BufLen *= 3;
//		}
//		pOutputBuff = ExAllocatePool(NonPagedPool, BufLen);
//		if (NULL == pOutputBuff)
//		{
//			KdPrint(("����������ڴ����ʧ�ܣ�\n"));
//			*OutLength = 0;
//			__leave;
//		}
//
//		if (TCPPORT == PortType)
//		{
//			status = GetObjectByName(&FileHandle, &pFileObject, L"\\Device\\Tcp");
//		}
//
//		if (UDPPORT == PortType)
//		{
//			status = GetObjectByName(&FileHandle, &pFileObject, L"\\Device\\Udp");
//		}
//		if (!NT_SUCCESS(status))
//		{
//			KdPrint(("��ȡ�豸��ʧ�ܣ�\n"));
//			*OutLength = 0;
//			__leave;
//		}
//
//		pDeviceObject = IoGetRelatedDeviceObject(pFileObject);
//		if (NULL == pDeviceObject)
//		{
//			KdPrint(("��ȡ�豸����ʧ�ܣ�\n"));
//			*OutLength = 0;
//			__leave;
//		}
//
//		KdPrint(("Tcpip Driver Object:%08lX\n", pDeviceObject->DriverObject));
//		KeInitializeEvent(&Event, 0, FALSE);
//
//		pIrp = IoBuildDeviceIoControlRequest(IOCTL_TCP_QUERY_INFORMATION_EX, \
//			pDeviceObject, pInputBuff, sizeof(TCP_REQUEST_QUERY_INFORMATION_EX), \
//			pOutputBuff, BufLen, FALSE, &Event, &StatusBlock);
//		if (NULL == pIrp)
//		{
//			KdPrint(("IRP����ʧ�ܣ�\n"));
//			*OutLength = 0;
//			__leave;
//		}
//
//		StackLocation = IoGetNextIrpStackLocation(pIrp);
//		StackLocation->FileObject = pFileObject;//���������������
//		StackLocation->DeviceObject = pDeviceObject;
//
//		status = IoCallDriver(pDeviceObject, pIrp);
//
//		KdPrint(("STATUS:%08lX\n", status));
//
//		if (STATUS_BUFFER_OVERFLOW == status)
//		{
//			KdPrint(("������̫С��%d\n", StatusBlock.Information));
//		}
//
//		if (STATUS_PENDING == status)
//		{
//			KdPrint(("STATUS_PENDING"));
//			status = KeWaitForSingleObject(&Event, 0, 0, 0, 0);
//		}
//
//		if (STATUS_CANCELLED == status)
//		{
//			KdPrint(("STATUS_CANCELLED"));
//		}
//
//		if (status == STATUS_SUCCESS)
//		{
//			*OutLength = StatusBlock.Information;
//			pOutBuf = pOutputBuff;
//		}
//	}
//	__finally
//	{
//		SafeObDereferenceObject(pFileObject);
//		if (FileHandle)
//		{
//			ZwClose(FileHandle);
//		}
//	}
//	return pOutBuf;
//}