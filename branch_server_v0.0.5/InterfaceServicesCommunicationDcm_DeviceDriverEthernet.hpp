#pragma once

#include "Message.hpp"

class InterfaceServicesCommunicationDcm_DeviceDriverEthernet{
   public:
      virtual     ~InterfaceServicesCommunicationDcm_DeviceDriverEthernet ()                  = default;
      virtual void vRxIndication                                          (Message& message)  = 0;
};

extern InterfaceServicesCommunicationDcm_DeviceDriverEthernet* InterfaceServicesCommunicationDcm_DeviceDriverEthernet_ptr;
