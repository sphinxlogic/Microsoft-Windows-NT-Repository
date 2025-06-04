$ Write Sys$Output "Setting up the Freeware Demo environment for XPDF"
$
$ Bin_Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + ".XPDF-0_91.XPDF]"
$
$ VMS_Version := 'F$GetSYI (""Version"")'
$ Length = F$Length (VMS_Version)
$ If ("''F$GetSYI (""Arch_Name"")'" .eqs. "Alpha")
$   Then
$       If (F$Locate ("V6.2", VMS_Version) .le. Length) Then Version = "ALP62"
$       If (F$Locate ("V7.", VMS_Version) .le. Length) Then Version = "ALP71"
$   Else
$       Write Sys$Output "Sorry.  VAX version not available in this DEMO..."
$       Exit
$ EndIf
$
$ xpdf        :== $ 'Bin_Path'xpdf.'Version'_exe
$ pdfinfo     :== $ 'Bin_Path'pdfinfo.'Version'_exe
$ pdfimages   :== $ 'Bin_Path'pdfimages.'Version'_exe
$ pdftopbm    :== $ 'Bin_Path'pdftopbm.'Version'_exe
$ pdftops     :== $ 'Bin_Path'pdftops.'Version'_exe
$ pdftotext   :== $ 'Bin_Path'pdftotext.'Version'_exe
$
$ Type /NoPage Sys$Input

Defining the following PDF-related foreign symbols:

    xpdf
    pdftopbm
    pdfimages
    pdfinfo
    pdftops
    pdftotext

Starting XPDF without a document.  Right-click to open a PDF file...
$
$ xpdf
$
$ Exit
