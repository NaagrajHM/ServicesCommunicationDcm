#include "InterfaceServicesCommunicationDcm_DeviceDriverEthernet.hpp"
#include "InterfaceServicesCommunicationDcm_ServicesSystemEcuM.hpp"
#include "InterfaceServicesCommunicationDcm_ServicesSystemSchM.hpp"
#include "InterfaceClientDeviceDriverEthernet.hpp"

#include <string.h>

#include "InterfaceRingBuffer.hpp"
using TypeRingBuffer = InterfaceRingBuffer<Message, SIZE_RING_BUFFER>;
unique_ptr<TypeRingBuffer> ringBuffer = TypeRingBuffer::Instantiate();

typedef int uint8;

uint8 Ascii2HexNibble(char chAscii){
   uint8 lu8ValueReturn;
        if(chAscii >= '0' && chAscii <= '9'){lu8ValueReturn = chAscii - '0'     ;}
   else if(chAscii >= 'a' && chAscii <= 'f'){lu8ValueReturn = chAscii - 'a' + 10;}
   else if(chAscii >= 'A' && chAscii <= 'F'){lu8ValueReturn = chAscii - 'A' + 10;}
   else                                     {lu8ValueReturn = 0                 ;}
   return lu8ValueReturn;
}

uint8 Ascii2HexByte(char* lptri8Ascii){
   return(
         Ascii2HexNibble(lptri8Ascii[0]) * 16
      +  Ascii2HexNibble(lptri8Ascii[1])
   );
}

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
         switch(Ascii2HexByte(ptrchBuffer)){
            case 0x01: memcpy(messageResponse.data, "025001", 7); break;
            default:                                              break;
         }
      }

      void vEcuReset(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(Ascii2HexByte(ptrchBuffer)){
            case 0x04: InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_InitShutdown); memcpy(messageResponse.data, "025104", 7); break; // Enable Rapid Power Shutdown
            default:                                                                                                                                                  break;
         }
      }

      void vTesterPresent(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(Ascii2HexByte(ptrchBuffer)){
            case 0x01: memcpy(messageResponse.data, "027E01", 7); break;
            default:                                              break;
         }
      }

      void vExecuteService(char* ptrchBuffer){
         Message messageResponse;
         switch(Ascii2HexByte(&ptrchBuffer[2])){
            case 0x10: vDiagnosticSessionChange (&ptrchBuffer[4], messageResponse); break;
            case 0x11: vEcuReset                (&ptrchBuffer[4], messageResponse); break;
            case 0x3E: vTesterPresent           (&ptrchBuffer[4], messageResponse); break;
            default:                                                                break;
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
