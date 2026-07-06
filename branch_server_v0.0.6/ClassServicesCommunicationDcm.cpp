#include "InterfaceServicesCommunicationDcm_DeviceDriverEthernet.hpp"
#include "InterfaceServicesCommunicationDcm_ServicesSystemEcuM.hpp"
#include "InterfaceServicesCommunicationDcm_ServicesSystemSchM.hpp"
#include "InterfaceClientDeviceDriverEthernet.hpp"

#include <string.h>

#include "InterfaceRingBuffer.hpp"
using TypeRingBuffer = InterfaceRingBuffer<Message, SIZE_RING_BUFFER>;
unique_ptr<TypeRingBuffer> ringBuffer = TypeRingBuffer::Instantiate();

typedef unsigned int   uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;

uint8 u8Ascii2HexNibble(char chAscii){
   uint8 lu8ValueReturn;
        if(chAscii >= '0' && chAscii <= '9'){lu8ValueReturn = chAscii - '0'     ;}
   else if(chAscii >= 'a' && chAscii <= 'f'){lu8ValueReturn = chAscii - 'a' + 10;}
   else if(chAscii >= 'A' && chAscii <= 'F'){lu8ValueReturn = chAscii - 'A' + 10;}
   else                                     {lu8ValueReturn = 0                 ;}
   return lu8ValueReturn;
}

// TBD: Optimize conversions
uint8 u8Ascii2HexByte(char* lptri8Ascii){
   return(
         u8Ascii2HexNibble(lptri8Ascii[0]) * 16
      +  u8Ascii2HexNibble(lptri8Ascii[1])
   );
}

uint16 u16Ascii2HexWORD(char* lptri8Ascii){
   return(
         u8Ascii2HexNibble(lptri8Ascii[0]) * 4096
      +  u8Ascii2HexNibble(lptri8Ascii[1]) *  256
      +  u8Ascii2HexNibble(lptri8Ascii[2]) *   16
      +  u8Ascii2HexNibble(lptri8Ascii[3])
   );
}

uint32 u16Ascii2HexDWORD(char* lptri8Ascii){
   return(
         u8Ascii2HexNibble(lptri8Ascii[0]) * 268435456
      +  u8Ascii2HexNibble(lptri8Ascii[1]) *  16777216
      +  u8Ascii2HexNibble(lptri8Ascii[2]) *   1048576
      +  u8Ascii2HexNibble(lptri8Ascii[3]) *     65536
      +  u8Ascii2HexNibble(lptri8Ascii[4]) *      4096
      +  u8Ascii2HexNibble(lptri8Ascii[5]) *       256
      +  u8Ascii2HexNibble(lptri8Ascii[6]) *        16
      +  u8Ascii2HexNibble(lptri8Ascii[7])
   );
}

#include "InterfaceServicesSystemEcuM_ServicesCommunicationDcm.hpp"
class ClassServicesCommunicationDcm:
      public InterfaceServicesCommunicationDcm_DeviceDriverEthernet
   ,  public InterfaceClientServicesSystemEcuM
   ,  public InterfaceClientServicesSystemSchM
{
   private:
      uint8 u8DiagnosticSession = 0x01;
      void vDiagnosticSessionChange(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u8Ascii2HexByte(ptrchBuffer)){ // TBD: Upgrade logic with ServicesSystemEcuM_geModeCurrent in consideration.
            case 0x01:
               if(0x02 == u8DiagnosticSession){
                  InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_RequestResetAppl);
               }
               u8DiagnosticSession = 0x01;
                  memcpy(messageResponse.data, "025001",   7);
               break;
            case 0x02:
               if(0x03 == u8DiagnosticSession){
                  u8DiagnosticSession = 0x02;
                  InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_RequestReprogramming);
                  memcpy(messageResponse.data, "025002",   7);
               }
               else{
                  memcpy(messageResponse.data, "037F107E", 9);
               }
               break;
            case 0x03:
               u8DiagnosticSession = 0x03;
                  memcpy(messageResponse.data, "025003",   7);
               break;
            default:
                  memcpy(messageResponse.data, "037F1012", 9);
               break;
         }
      }

      void vEcuReset(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u8Ascii2HexByte(ptrchBuffer)){
            case 0x01: InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_RequestResetAppl); memcpy(messageResponse.data, "025101",   7); break;
            case 0x04: InterfaceServicesSystemEcuM_ServicesCommunicationDcm_ptr->vSetStatusEcuM(eStatusEcuM_InitShutdown);     memcpy(messageResponse.data, "025104",   7); break;
            default:                                                                                                           memcpy(messageResponse.data, "037F1112", 9); break;
         }
      }

      void vReadDataByLocalIdentifier(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u16Ascii2HexWORD(ptrchBuffer)){
            case 0xF180:
               if(
                     eModeEcuM_Boot
                  == ServicesSystemEcuM_eGetModeCurrent()
               ){
                             memcpy(messageResponse.data, "0762F180424C5632", 17);
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
                             memcpy(messageResponse.data, "0762F18141503232", 17);
               }
               else{
                             memcpy(messageResponse.data, "037F2231",    9);
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
            default:         memcpy(messageResponse.data, "037F2212",    9); break;
         }
      }

      enum EnumStateSecA{
            eStateSecA_Locked = 0
         ,  eStateSecA_RequestSequenceError
         ,  eStateSecA_Denied
         ,  eStateSecA_ExceedNumberOfAttempts
         ,	eStateSecA_RequiredTimeDelayNotExpired
         ,  eStateSecA_SeedRequested
         ,  eStateSecA_Unlocked
      };
      enum EnumEventSecA{
            eEventSecA_Reserved = 0
         ,  eEventSecA_RequestSeed
         ,  eEventSecA_VerifyKey
      };
      void vSecurityAccess(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         static char          lstrSeedSent[]      = "AABBCCDD";
         static uint32        lu32KeyEvaluated    = 0xABBCCDDE;
         static EnumStateSecA leStateSecA         = eStateSecA_Locked;
         static uint8         lu8NumberOfAttempts = 0;
                EnumEventSecA leEventSecA         = (EnumEventSecA)u8Ascii2HexByte(ptrchBuffer);
                uint32        lu32KeyReceived;
         switch(leStateSecA){
            case eStateSecA_Locked:
            case eStateSecA_RequestSequenceError:
            case eStateSecA_Denied:
            case eStateSecA_ExceedNumberOfAttempts:
            case eStateSecA_RequiredTimeDelayNotExpired:
               switch(leEventSecA){
                  case eEventSecA_RequestSeed:
                                                if(
                                                      3
                                                   >  lu8NumberOfAttempts
                                                ){
                                                      memcpy(messageResponse.data, "066701AABBCCDD", 15); leStateSecA = eStateSecA_SeedRequested;               break;
                                                }
                                                else{
                                                      memcpy(messageResponse.data, "037F2737",        9); leStateSecA = eStateSecA_RequiredTimeDelayNotExpired; break;
                                                }
                  case eEventSecA_VerifyKey:          memcpy(messageResponse.data, "037F2724",        9); leStateSecA = eStateSecA_RequestSequenceError;        break;
                  default:                            memcpy(messageResponse.data, "037F2712",        9);                                                       break;
               }
               break;
            case eStateSecA_SeedRequested:
               switch(leEventSecA){
                  case eEventSecA_VerifyKey:   lu32KeyReceived = u16Ascii2HexDWORD(&ptrchBuffer[2]);
                                                if(
                                                      lu32KeyEvaluated
                                                   == lu32KeyReceived
                                                ){
                                                   lu8NumberOfAttempts = 0;
                                                      memcpy(messageResponse.data, "026702",          7); leStateSecA = eStateSecA_Unlocked;                    break;
                                                }
                                                else{
                                                   lu8NumberOfAttempts++;
                                                   if(
                                                         3
                                                      >  lu8NumberOfAttempts
                                                   ){
                                                      memcpy(messageResponse.data, "037F2735",        9); leStateSecA = eStateSecA_Denied;                      break;
                                                   }
                                                   else{
                                                      memcpy(messageResponse.data, "037F2736",        9); leStateSecA = eStateSecA_ExceedNumberOfAttempts;      break;
                                                   }
                                                }
                  case eEventSecA_RequestSeed:        memcpy(messageResponse.data, "037F2724",        9); leStateSecA = eStateSecA_RequestSequenceError;        break;
                  default:                            memcpy(messageResponse.data, "037F2712",        9);                                                       break;
               }
               break;
            case eStateSecA_Unlocked:
               switch(leEventSecA){
                  case eEventSecA_RequestSeed:
                  case eEventSecA_VerifyKey:          memcpy(messageResponse.data, "037F2724",        9); leStateSecA = eStateSecA_RequestSequenceError;        break;
                  default:                            memcpy(messageResponse.data, "037F2712",        9);                                                       break;
               }
               break;
            default:                                  memcpy(messageResponse.data, "037F2710",        9); leStateSecA = eStateSecA_RequestSequenceError;        break;
         }
      }

      void vRequestDownload(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u8Ascii2HexByte(ptrchBuffer)){
            case 0x00: memcpy(messageResponse.data, "027400",   7); break;
            default:   memcpy(messageResponse.data, "037F3412", 9); break;
         }
      }

      void vTesterPresent(
            char*    ptrchBuffer
         ,  Message& messageResponse
      ){
         switch(u8Ascii2HexByte(ptrchBuffer)){
            case 0x01: memcpy(messageResponse.data, "027E01",   7); break;
            default:   memcpy(messageResponse.data, "037F3E12", 9); break; // TBD: Supress positive response!
         }
      }

      void vExecuteService(char* ptrchBuffer){
         Message messageResponse;
         switch(u8Ascii2HexByte(&ptrchBuffer[2])){
            case 0x10: vDiagnosticSessionChange   (&ptrchBuffer[4], messageResponse); break;
            case 0x11: vEcuReset                  (&ptrchBuffer[4], messageResponse); break;
            case 0x22: vReadDataByLocalIdentifier (&ptrchBuffer[4], messageResponse); break;
            case 0x27: vSecurityAccess            (&ptrchBuffer[4], messageResponse); break;
            case 0x34: vRequestDownload           (&ptrchBuffer[4], messageResponse); break;
            case 0x3E: vTesterPresent             (&ptrchBuffer[4], messageResponse); break;
            default:
               memcpy( messageResponse.data,    "037F??11",      9);
               memcpy(&messageResponse.data[4], &ptrchBuffer[2], 2);
               break;
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
