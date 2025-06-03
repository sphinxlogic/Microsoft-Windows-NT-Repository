DISKM010, system management, Disk and File Fragmentation management tool

DISKM010 is a useful tool for system managers concerned with the performance 
impact of fragmented disks and files. This tool is NOT a disk defragmenter.This
vmsinstall-able kit produces reports at several different levels: A list of 
disks requiring defragmenting, A summary of why the disks are in need of 
defragmenting, Detail fragmentation reports for every disk including a list of
fragmented files and a report showing the fragmentation level of hotfiles 
(decps and vpa versions only). History files showing the state of disks and 
files over time are updated as reports are produced. See the Postscript User 
Guide and Install Guide for details.

Save set:          DISKM010.A (common code and sources)

DISKM010_IG.PS		Install guide (moved to sys$manager during install)
DISKM010_UG.PS		User guide (moved to sys$manager during install)
DISKMANAGEMENTIVP.COM	Installation verification procedure
DISK_HISTORY.DAT	Empty Disk Fragmentation History file
DM_MAIL.DIS		Mail distribution list for Summary reports
FILE_HISTORY.DAT	Empty File Fragmentation History file
KITINSTAL.COM		Vmsinstal required (this file does the install)
REVIEW_DISK_DECPS.CLD	DECps command language definition
REVIEW_DISK_DECPS.FOR	DECps version fortran source code
REVIEW_DISK_NOVPA.CLD	Non-DECps/VPA command language definition
REVIEW_DISK_NOVPA.FOR	Non-DECps/VPA version fortran source code
REVIEW_DISK_VPA.CLD	VPA command language definition
REVIEW_DISK_VPA.FOR	VPA fortran version source code

Save set:          DISKM010.B (DECps version of the code)

FRAG_SUMMARY.COM	Procedure to analyze disk detail
PSPA$LIB.FOR		DECps fortran library 
REVIEW_ALL_DISKS.COM	Weekly procedure to analyze all disks
REVIEW_DISK.COM		Produce a fragmentation report for one disk
REVIEW_DISK_DECPS.OBJ	DECps object code used during install
WEEKLY_HOT_FILES.COM	Procedure to produce hotfiles report

Save set:          DISKM010.C (VPA version of the code -- this is old)

FRAG_SUMMARY.COM	Procedure to analyze disk detail
REVIEW_ALL_DISKS.COM	Weekly procedure to analyze all disks
REVIEW_DISK.COM		Produce a fragmentation report for one disk
REVIEW_DISK_VPA.OBJ	VPA object code used during install
VPA$LIB.FOR		VPA fortran library
WEEKLY_HOT_FILES.COM	Procedure to produce hotfiles report

Save set:          DISKM010.D (NON DECps or VPA version of code -- no hot files)

FRAG_SUMMARY.COM	Procedure to analyze disk detail
REVIEW_ALL_DISKS.COM	Weekly procedure to analyze all disks
REVIEW_DISK.COM		Produce a fragmentation report for one disk
REVIEW_DISK_NOVPA.OBJ	Non-DECps/VPA object code used during install 
