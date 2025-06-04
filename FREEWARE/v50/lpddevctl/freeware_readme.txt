LPDDEVCTL, UTILITIES, LPD Print Symbiont Setup Modules

LPDDEVCTL is a set of text (postscript) modules for use with UCX$LPD_SMB
(untested on other lpd symbionts).

These modules are used as setups to form a postscript prefix to a text file for
printing: while many printers can autosense postscript or plain text/pcl,
there's no control over the layout of text. These modules allow you to specify
66 lines, 80 chars, portrait; 66 lines, 132 chars, landscape; various others,
or to create your own .stub file(s) with your own spec.

They provide a way to get text layout comparable to Pathworks (Mac) MSAP, or
the newer DCPS, but via the ubiquitous lpd protocol which neither supports.

Instructions: Unzip the kit. If you wish, look at the stub files & create your
own variations (& add them to LPDDEVCTL.COM). Execute LPDDEVCTL.COM to create
the text library. Create forms as necessary (assign your own numbers) - see
QUEUE-SETUP.TXT for details - if you use MSAP some may already exist. Modify
your lpd queues to use the library & forms.

If you know how to get the vms file/job/username or any other details from
postscript please tell me, or if you have any problems/complaints/suggestions.

Chris.Sharman@ccagroup.co.uk
