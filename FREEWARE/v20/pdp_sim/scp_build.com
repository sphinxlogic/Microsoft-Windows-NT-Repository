$ cc/debug/optim=noinline scp
$ cc/debug/optim=noinline scp_tty
$ link/debug/exec=pdp8 pdp8_cpu,pdp8_sys,pdp8_pt,pdp8_tt,pdp8_lp,pdp8_clk,-
  pdp8_rk,pdp8_rx,pdp8_rf,scp,scp_tty
