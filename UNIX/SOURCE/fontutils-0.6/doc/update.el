;;;; Driver to update all the node pointers and menus in the Texinfo source.

(setq load-path (append  '("/usr/local/gnu/lib/emacs/elisp") load-path))
(require 'texinfo)
(texinfo-multiple-files-update "fontu.texi" t t)
(save-some-buffers t)
