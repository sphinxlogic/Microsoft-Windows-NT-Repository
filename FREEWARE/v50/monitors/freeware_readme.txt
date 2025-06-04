MONITORS, SYSTEM_MGMT, System Monitors for OpenVMS Alpha

Monitors for OpenVMS Alpha

CPU-Monitor (CPUMON.EXE)

Shows the CPU-Ticks for each Mode. Following modes can be set or cleaned.
	Interrupt
	MPsync
	Kernel
	Executive
	Supervisor
	User

The interval is chooseable in seconds. Default is 3 seconds.
The foreground-color will be set to brown for the primary cpu. (label
widget)

The background color shows in which RAD the cpu belongs to (if there are
RADs).


Disk-Monitor (DSKMON.EXE)

IO-Count for each disk, where the top counter shows the IOs per second
and the bottom counter shows the accumulated IO-Count. The disks are 
sorted by the accumulated counter.

There are 3 entries in the Option-Menu.

	Rescale 100	because a disk can have more than 100 IOs per
			second the scale expand automatically. This action
			resets the scale to 100.
	All disk	lists all disk available so you can simply lock 
                        on or more disk (locking directly on display is 
                        also possible)
	Reset total	resets the accumulated counter to zero for all
                        disks. 

The interval is chooseable in seconds. Default is 3 seconds.


Process-Monitor (PRCMON.EXE)

Shows the top 16 processes on system, sorted by.
	CPU
	IO
	MEM
	Processname
	Username

There are 3 graphs per process.

	1. Graph	CPU-Ticks for each mode (see legend right above
                        corner)
	2. Graph	IO (Options are DIO, BIO, BOTH)
	3. Graph	Memory (Options are Private, Global, BOTH)

To see following process data, double-click on graph
	Processname
	Username
	Processid, UIC-Group
	Image
	Affinity	can be modified
	Priority	can be modified
	Suspend/Resume	can be modified

The following filters are availabe for collecting processes

	System		take each process
	Group		take process with the same UIC-Group as entered
	Process		take process with the same Processname as entered
	User		take process with the same Username as entered

Wilcard-Input is valid.

Locking processes on display is also possible thru simple click on the
process description.


	Shadow-Monitor (SHDMON.EXE)

You see all shadowsets which perform an shadow-merge or shadow-copy and on
which node that happens.

The interval is chooseable in seconds. Default is 5 seconds.

