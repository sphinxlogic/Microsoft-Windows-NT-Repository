ALIGNFAULT, UTILITIES, Alignment Fault Cata Collector

Tool to monitor OpenVMS Alpha Alignment Faults...

  - Define logical name ALIGN$TOOLS to point to the directory
    containing these tools.
  - Set default to directory where you want to run your test
  - Copy ALIGN$TOOLS:SYSFAULTS.DAT to this directory
  - Edit the copy as necessary to collect data of interest
    over time period of interest
  - Ensure that CMKRNL and SYSPRV privileges are enabled
  - Invoke ALIGN$TOOLS:ALIGN_SETUP.COM to initiate the
    alignment fault data collection
  - Run your application
  - Allow subprocess ALIGN_FAULT to complete
  - Invoke ALIGN$TOOLS:ALIGN_FAULTS.COM to process the data
  - Type or print the two versions of ALIGNMENT_FAULTS.RPT. 
    File version ;0 is sorted in descending hit-rate order, 
    and file version ;-1 is sorted by PC.

