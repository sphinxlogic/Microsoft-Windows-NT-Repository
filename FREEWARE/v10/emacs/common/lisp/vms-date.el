;; Record creation date of the Emacs patches for VMS.
;; Copyright (C) 1985 Free Software Foundation, Inc.

;; This file is part of GNU Emacs.

;; GNU Emacs is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 1, or (at your option)
;; any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to
;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

(defconst vms-patch-date "Saturday August 6, 1994" "\
Creation date of this version of the GNU Emacs for VMS patches.")

(defun vms-patch-date () "\
Return string describing this version of the GNU Emacs for VMS patches."
  (interactive)
  (if (interactive-p)
      (message "%s" (vms-patch-date))
    (format "VMS patches created on %s" vms-patch-date)))

;;Local variables:
;;version-control: never
;;End:
