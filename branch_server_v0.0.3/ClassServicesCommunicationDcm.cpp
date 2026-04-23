#include "InterfaceServicesCommunicationDcm_DeviceDriverEthernet.hpp"
#include "InterfaceServicesCommunicationDcm_ServicesSystemEcuM.hpp"
#include "InterfaceServicesCommunicationDcm_ServicesSystemSchM.hpp"
#include "InterfaceClientDeviceDriverEthernet.hpp"

#include <string.h>

#include "InterfaceRingBuffer.hpp"
using TypeRingBuffer = InterfaceRingBuffer<Message, SIZE_RING_BUFFER>;
unique_ptr<TypeRingBuffer> ringBuffer = TypeRingBuffer::Instantiate();

typedef unsigned short uint16;
typedef unsigned int   uint8;

uint8 u8Ascii2HexNibble(char chAscii){
   uint8 lu8ValueReturn;
        if(chAscii >= '0' && chAscii <= '9'){lu8ValueReturn = chAscii - '0'     ;}
   else if(chAscii >= 'a' && chAscii <= 'f'){lu8ValueReturn = chAscii - 'a' + 10;}
   else if(chAscii >= 'A' && chAscii <= 'F'){lu8ValueReturn = chAscii - 'A' + 10;}
   else                                     {lu8ValueReturn = 0                 ;}
   return lu8ValueReturn;
}

uint8 u8Ascii2HexByte(char* lptri8Ascii){
   return(
         u8Ascii2HexNibble(lptri8Ascii[0]) * 16
      +  u8Ascii2HexNibble(lptri8Ascii[1])
   );
}

uint16 u16Ascii2HexWord(char* lptri8Ascii){
   return(
         u8Ascii2HexNibble(lptri8Ascii[0]) * 4096
      +  u8Ascii2HexNibble(lptri8Ascii[1]) *  256
      +  u8Ascii2HexNibble(lptri8Ascii[2]) *   16
      +  u8Ascii2HexNibble(lptri8Ascii[3])
   );
}

uint8 u8DiagnosticSession = 0x01;
#include "InterfaceServicesSystemEcuM_ServicesCommunicationDcm.hpp"
class ClassServicesCommunicationDcm:
      public InterfaceServicesCommunicationDcm_DeviceDriverEthernet
   ,  public InterfaceClientServicesSystemEcuM
   ,  public InterfaceClientServicesSystemSchM
{
   private:
      void vDiagnosticSessionChange(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u8Ascii2HexByte(ptrchBuffer)){
            case 0x01: u8DiagnosticSession = 0x01;                                                                                                             memcpy(messageResponse.data, "025001", 7); break;
            case 0x02: u8DiagnosticSession = 0x02; InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_RequestReprogramming); memcpy(messageResponse.data, "025002", 7); break;
            case 0x03: u8DiagnosticSession = 0x03;                                                                                                             memcpy(messageResponse.data, "025003", 7); break;
            default:                                                                                                                                                                                      break;
         }
      }

      void vEcuReset(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u8Ascii2HexByte(ptrchBuffer)){
            case 0x01: InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_RequestResetAppl); memcpy(messageResponse.data, "025101", 7); break;
            case 0x04: InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_InitShutdown);     memcpy(messageResponse.data, "025104", 7); break;
            default:                                                                                                                                                      break;
         }
      }

      void vReadDataByLocalIdentifier(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u16Ascii2HexWord(ptrchBuffer)){
            case 0xF180:
               if(
                     eModeEcuM_Boot
                  == ServicesSystemEcuM_eGetModeCurrent()
               ){
                  memcpy(messageResponse.data, "0762F180424C5631", 17);
               }
               else{
                  memcpy(messageResponse.data, "037F2231", 9);
               }
               break;
            case 0xF181:
               if(
                     eModeEcuM_Appl
                  == ServicesSystemEcuM_eGetModeCurrent()
               ){
                  memcpy(messageResponse.data, "0762F18141503230", 17);
               }
               else{
                  memcpy(messageResponse.data, "037F2231", 9);
               }
               break;
            case 0xF186:
               switch(
                  u8DiagnosticSession
               ){
                  case 0x01: memcpy(messageResponse.data, "0462F18601", 11); break;
                  case 0x02: memcpy(messageResponse.data, "0462F18602", 11); break;
                  case 0x03: memcpy(messageResponse.data, "0462F18603", 11); break;
               }
               break;
            default: break;
         }
      }

      void vTesterPresent(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u8Ascii2HexByte(ptrchBuffer)){
            case 0x01: memcpy(messageResponse.data, "027E01", 7); break;
            default:                                              break;
         }
      }

      void vExecuteService(char* ptrchBuffer){
         Message messageResponse;
         switch(u8Ascii2HexByte(&ptrchBuffer[2])){
            case 0x10: vDiagnosticSessionChange   (&ptrchBuffer[4], messageResponse); break;
            case 0x11: vEcuReset                  (&ptrchBuffer[4], messageResponse); break;
            case 0x22: vReadDataByLocalIdentifier (&ptrchBuffer[4], messageResponse); break;
            case 0x3E: vTesterPresent             (&ptrchBuffer[4], messageResponse); break;
            default:                                                                  break;
         }
         InterfaceClientDeviceDriverEthernet_ptr->vTransmit(messageResponse);
      }

   public:
      void vFunctionDeInit(void){
      }
      void vFunctionInit(void){
      }
      void vFunctionMain(void){
         if(
            ringBuffer->isEmpty()
         ){
         }
         else{
            vExecuteService(ringBuffer->pop()->data);
         }
      }
      void vRxIndication(
         Message& message
      ){
         ringBuffer->push(message);
      }
};

static ClassServicesCommunicationDcm                           InstanceServicesCommunicationDcm;
       InterfaceServicesCommunicationDcm_DeviceDriverEthernet* InterfaceServicesCommunicationDcm_DeviceDriverEthernet_ptr    = &InstanceServicesCommunicationDcm;
       InterfaceClientServicesSystemEcuM*                      InterfaceClientServicesSystemEcuM_ptrServicesCommunicationDcm = &InstanceServicesCommunicationDcm;
       InterfaceClientServicesSystemSchM*                      InterfaceClientServicesSystemSchM_ptrServicesCommunicationDcm = &InstanceServicesCommunicationDcm;
