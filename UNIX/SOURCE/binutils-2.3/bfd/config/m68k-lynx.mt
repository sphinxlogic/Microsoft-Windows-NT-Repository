# Target:  Motorola 680x0 running LynxOS (a.out and coff, defaulting to coff)
TDEFINES= -DFPRINTF_ALREADY_DECLARED
DEFAULT_VECTOR=m68klynx_coff_vec
SELECT_VECS=m68klynx_aout_vec
SELECT_ARCHITECTURES=bfd_m68k_arch
