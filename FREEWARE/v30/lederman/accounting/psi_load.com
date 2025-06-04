$ sql$ @psi
$ rmu/load/rms=file=psi psi psi_calls psi_intran
$ set prot=ow:rwed *.*;*
$ purge
$ exit
