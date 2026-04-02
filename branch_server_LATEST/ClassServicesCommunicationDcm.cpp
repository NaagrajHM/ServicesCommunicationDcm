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

class ClassServicesCommunicationDcm:
      public InterfaceServicesCommunicationDcm_DeviceDriverEthernet
   ,  public InterfaceClientServicesSystemEcuM
   ,  public InterfaceClientServicesSystemSchM
{
   private:
      void vTesterPresent(char* ptrchBuffer){
         Message message;
         switch(Ascii2HexByte(ptrchBuffer)){
            case 0x01: memcpy(message.data, "027E01", 7); break;
            default:                                      break;
         }
         InterfaceClientDeviceDriverEthernet_ptr->vTransmit(message);
      }

      void vExecuteService(char* ptrchBuffer){
         switch(Ascii2HexByte(&ptrchBuffer[2])){
            case 0x3E: vTesterPresent(&ptrchBuffer[4]); break;
            default:                                    break;
         }
      }

   public:
      void vFunctionInit(void){
      }
      void vFunctionMain(void){
         vExecuteService(ringBuffer->pop()->data);
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
