// ListComPorts.cpp 
// A Simple Example how to List COM Ports in Windows.
// Lists the COM Ports that you can see in Device Manager. 

#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#include <vector>

int _tmain(int argc, _TCHAR* argv[])
{

  unsigned int uReturn                               = ERROR_SUCCESS;
  HDEVINFO     devInfo;
  std::vector<BYTE> memSpc;
  std::vector<BYTE> memSpc2;

  PSP_INTERFACE_DEVICE_DETAIL_DATA pDevIfcDetailData = NULL;

  // This statement returns a device information set that contains all devices
  // of a specified class, with the last parameter not set to DIGCF_DEVICEINTERFACE,
  // the first parameter specifies a setup class, the last parameter causes this
  // function to return only devices that are currently present.
  devInfo = SetupDiGetClassDevs((CONST LPGUID) &GUID_DEVINTERFACE_COMPORT,
    (LPCTSTR) NULL,
    (HWND)   NULL,
    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

  if(devInfo == INVALID_HANDLE_VALUE)
  {
    uReturn = GetLastError();
  }
  else
  {
    SP_DEVICE_INTERFACE_DATA devIfcData;
    devIfcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    unsigned int uDevIdx = 0;

    for( ; ; uDevIdx++)
    {
      if(uReturn != ERROR_SUCCESS)
      {
        break;
      }

      // The next statement returns a context structure for a device information
      // element of a device information set. Each call returns information about
      // one device, the function can be called repeatedly to get information
      // about several devices.
      if(!SetupDiEnumDeviceInterfaces(devInfo,
        NULL,
        (CONST LPGUID) &GUID_DEVINTERFACE_COMPORT,
        uDevIdx,
        &devIfcData))
      {
        uReturn = GetLastError(); // 259 = NO_MORE_ITEMS
        _ASSERTE(uReturn == ERROR_NO_MORE_ITEMS);
        if(uReturn == ERROR_NO_MORE_ITEMS)
        {
          uReturn = ERROR_SUCCESS;
        }
        break;
      }

      DWORD uReqLength = 0;
      DWORD uPreLength = 0;
      if(!SetupDiGetDeviceInterfaceDetail(devInfo,
        &devIfcData,
        NULL,
        0,
        &uReqLength,
        NULL))
      {
        uReturn = GetLastError();
        _ASSERTE(uReturn == ERROR_INSUFFICIENT_BUFFER);
        if(uReturn == ERROR_INSUFFICIENT_BUFFER)
        {
          uReturn = ERROR_SUCCESS;
        }
        else
        {
          break;
        }
      }
      uPreLength = uReqLength;
      memSpc.resize(uPreLength);
      void* pLint = &(memSpc[0]);
      pDevIfcDetailData = reinterpret_cast<PSP_INTERFACE_DEVICE_DETAIL_DATA> (pLint);
      pDevIfcDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
      if(!SetupDiGetDeviceInterfaceDetail(devInfo,
        &devIfcData,
        pDevIfcDetailData,
        uPreLength,
        &uReqLength,
        &devInfoData))
      {
        uReturn = GetLastError();
        break;
      }
      //std::wstring strDeviceInterface = pDevIfcDetailData->DevicePath;
      //wprintf(L"%s\n", strDeviceInterface.c_str());

      DWORD PropertyRegDataType = 0;
      DWORD RequiredSize = 0;
      BOOL bRet = SetupDiGetDeviceRegistryProperty(
          devInfo,
          &devInfoData,
        SPDRP_FRIENDLYNAME ,
        &PropertyRegDataType,
        0,
        0,
        &RequiredSize
        );
      memSpc2.resize(RequiredSize);
      BYTE* pPropertyBuffer = &memSpc2[0];
      bRet = SetupDiGetDeviceRegistryProperty(
        devInfo,
        &devInfoData,
        SPDRP_FRIENDLYNAME ,
        &PropertyRegDataType,
        pPropertyBuffer,
        RequiredSize,
        &RequiredSize
        );
      pPropertyBuffer[RequiredSize-1] = 0;
      pPropertyBuffer[RequiredSize-2] = 0;
      wprintf(L"%s\n", pPropertyBuffer);

    } // end for

    // cleanup
    SetupDiDestroyDeviceInfoList(devInfo);
  } 

  return 0;
}
