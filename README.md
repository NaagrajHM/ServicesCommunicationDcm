# ServicesCommunicationDcm
Diagnostic communication manager

# 2026.04.22: branch_server_v0.0.3
Feature:
      #1 Implement Ascii to Hex word conversion function.
   ,  #2 System variable to hold Diagnostic Session information.
   ,  #3 Change diagnostic session service handles default session, reprogramming session and extended diagnostic session.
   ,  #4 ECU reset service handles hard reset (but at present it is implemented as reset to application) and ECU shutdown.
   ,  #5 Implement ReadDataByLocalIdentifier service.
   ,  #6 Implement local identifier 0xF180 - To read bootloader software version.
   ,  #7 Implement local identifier 0xF181 - To read application software version.
   ,  #8 Implement local identifier 0xF186 - To read active diagnostic session.

# 2026.04.03: branch_server_v0.0.2
Feature:
      Service-0x10-DSC
   ,  Service-0x11-EcuReset
   ,  Common response transmit logic
   ,  De-Initialize API offered to initiate shutdown sequence.

Bug-fix:
       Null pointer check for empty ring buffer pop usecase.

# 2026.04.02: branch_server_v0.0.1
Feature: modular development.

