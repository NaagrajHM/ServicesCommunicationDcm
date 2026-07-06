# ServicesCommunicationDcm
Diagnostic communication manager

# 2026.07.05:
      #26 2026.07.05: branch_server_v0.0.5 - FEATURE - Update software version numbers.

# 2026.05.05:
      #13 2026.05.05: branch_server_v0.0.5 - FEATURE - Implement (a dummy) request download service.
   ,  #14 2026.05.05: branch_server_v0.0.5 - BUGFIX - Unknown service request send negative response code - service not supported.
   ,  #15 2026.05.05: branch_server_v0.0.5 - BUGFIX - Unknown sub-function request send negative response code - sub-function not supported - Diagnostic session change.
   ,  #16 2026.05.05: branch_server_v0.0.5 - BUGFIX - Unknown sub-function request send negative response code - sub-function not supported - ECU Reset.
   ,  #17 2026.05.05: branch_server_v0.0.5 - BUGFIX - Unknown sub-function request send negative response code - sub-function not supported - Security access.
   ,  #18 2026.05.05: branch_server_v0.0.5 - BUGFIX - Unknown sub-function request send negative response code - sub-function not supported - Tester present.
   ,  #19 2026.05.05: branch_server_v0.0.5 - BUGFIX - Unknown local identifier request send negative response code - sub-function not supported - Read data by local identifier.
   ,  #20 2026.05.05: branch_server_v0.0.5 - BUGFIX - Diagnostic session change to re-programming is allowed only through extended session.
   ,  #21 2026.05.05: branch_server_v0.0.5 - BUGFIX - Diagnostic session change from re-programming to default must go through reset-to-application.
   ,  #22 2026.05.05: branch_server_v0.0.5 - BUGFIX - Read bootloader version request when in application mode send negative response code - request out of range.
   ,  #23 2026.05.05: branch_server_v0.0.5 - BUGFIX - Read application version request when in bootloader mode send negative response code - request out of range.
   ,  #24 2026.05.05: branch_server_v0.0.5 - BUGFIX - Upgrade diagnostic session change logic with ServicesSystemEcuM_geModeCurrent in consideration.
   ,  #25 2026.05.05: branch_server_v0.0.5 - BUGFIX - not implemented - suppress positive response - Tester present.

# 2026.04.27: branch_server_v0.0.4
Feature:
      #9  Type define uint32.
   ,  #10 Implement ASCII to DWORD(uint32) conversion.
   ,  #11 Implement SecurityAccess service and required infrastructure.

Known issue:
      #12 Optimize ASCII to HEX conversions.

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

