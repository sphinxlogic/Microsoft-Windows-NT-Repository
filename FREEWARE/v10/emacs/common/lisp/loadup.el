;;; loadup.el --- load up standardly loaded Lisp files for Emacs.

;; Copyright (C) 1985, 1986, 1992 Free Software Foundation, Inc.

;; Maintainer: FSF
;; Keywords: internal

;; This file is part of GNU Emacs.

;; GNU Emacs is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to
;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

;;; Commentary:

;; This is loaded into a bare Emacs to make a dumpable one.

;;; Code:

(message "Using load-path %s" load-path)
(sleep-for 1)

;;; We don't want to have any undo records in the dumped Emacs.
(buffer-disable-undo "*scratch*")

(load "subr")
(garbage-collect)
(load "byte-run")
(garbage-collect)
(load "map-ynp")
(garbage-collect)
(load "loaddefs.el")  ;Don't get confused if someone compiled loaddefs by mistake.
(garbage-collect)
(load "simple")
(garbage-collect)
(load "help")
(garbage-collect)
(load "files")
(garbage-collect)
(load "indent")
(garbage-collect)
(load "window")
(garbage-collect)
(if (fboundp 'delete-frame)
    (progn
      (load "frame")
      (load "mouse")
      (garbage-collect)
      (load "scroll-bar")
      (load "select")))
(garbage-collect)
(load "paths.el")  ;Don't get confused if someone compiled paths by mistake.
(garbage-collect)
(load "startup")
(garbage-collect)
(load "lisp")
(garbage-collect)
(load "page")
(garbage-collect)
(load "register")
(garbage-collect)
(load "paragraphs")
(garbage-collect)
(load "lisp-mode")
(garbage-collect)
(load "text-mode")
(garbage-collect)
(load "fill")
(garbage-collect)
(load "c-mode")
(garbage-collect)
(load "isearch")
(garbage-collect)
(load "replace")
(if (eq system-type 'vax-vms)
    (progn
      (garbage-collect)
      (load "vmsproc")))
(garbage-collect)
(load "abbrev")
(garbage-collect)
(load "buff-menu")
(if (eq system-type 'vax-vms)
    (progn
      (garbage-collect)
      (load "vms-patch")))
(if (fboundp 'atan)	; preload some constants and 
    (progn		; floating pt. functions if 
      (garbage-collect)	; we have float support.
      (load "float-sup")))
(garbage-collect)
(load "vc-hooks")

;If you want additional libraries to be preloaded and their
;doc strings kept in the DOC file rather than in core,
;you may load them with a "site-load.el" file.
;But you must also cause them to be scanned when the DOC file
;is generated.  For VMS, you must edit [-.vms]makedoc.com or
;[-.src]descrip.mms_in_in.  For other systems, you must edit
;../src/ymakefile.
(if (load "site-load" t)
    (garbage-collect))

(load "version.el")  ;Don't get confused if someone compiled version.el by mistake.

;Now, the vms patches are dated.
(if (eq system-type 'vax-vms)
    (load "vms-date.el"))

;; Note: all compiled Lisp files loaded above this point
;; must be among the ones parsed by make-docfile
;; to construct DOC.  Any that are not processed
;; for DOC will not have doc strings in the dumped Emacs.

(if (or (and (equal (nth 3 command-line-args) "dump")
	     (not (equal (nth 4 command-line-args) "nodoc")))
	(and (equal (nth 4 command-line-args) "dump")
	     (not (equal (nth 5 command-line-args) "nodoc"))))
    (progn
      (message "Finding pointers to doc strings...")
      (if (fboundp 'dump-emacs)
	  (let ((name emacs-version))
	    (while (string-match "[^-+_.a-zA-Z0-9]+" name)
	      (setq name (concat (downcase (substring name
						      0 (match-beginning 0)))
				 "-"
				 (substring name (match-end 0)))))
	    (setq name (concat (expand-file-name "../etc/DOC-") name))
	    (if (file-exists-p name)
		(delete-file name))
	    (copy-file (expand-file-name "../etc/DOC") name t)
	    (Snarf-documentation (file-name-nondirectory name)))
	(Snarf-documentation "DOC"))
      (message "Finding pointers to doc strings...done")))

;Note: You can cause additional libraries to be preloaded
;by writing a site-init.el that loads them.
;See also "site-load" above.
(load "site-init" t)
(garbage-collect)

;;; At this point, we're ready to resume undo recording for scratch.
(buffer-enable-undo "*scratch*")

(if (or (equal (nth 3 command-line-args) "dump")
	(equal (nth 4 command-line-args) "dump"))
    (if (eq system-type 'vax-vms)
	(let ((fn (invocation-name))
	      (d (invocation-directory)))
	  (message (concat "Dumping data as file " (downcase d) fn ".dump"))
	  (dump-emacs (concat d fn ".dump") fn)
	  (kill-emacs))
      (let ((name (concat "emacs-" emacs-version)))
	(while (string-match "[^-+_.a-zA-Z0-9]+" name)
	  (setq name (concat (downcase (substring name 0 (match-beginning 0)))
			     "-"
			     (substring name (match-end 0)))))
	(message "Dumping under names emacs and %s" name))
      (condition-case ()
	  (delete-file "emacs")
	(file-error nil))
      ;; We used to dump under the name xemacs, but that occasionally
      ;; confused people installing Emacs (they'd install the file
      ;; under the name `xemacs'), and it's inconsistent with every
      ;; other GNU product's build process.
      (dump-emacs "emacs" "temacs")
      ;; Recompute NAME now, so that it isn't set when we dump.
      (let ((name (concat "emacs-" emacs-version)))
	(while (string-match "[^-+_.a-zA-Z0-9]+" name)
	  (setq name (concat (downcase (substring name 0 (match-beginning 0)))
			     "-"
			     (substring name (match-end 0)))))
	(add-name-to-file "emacs" name t))
      (kill-emacs)))

;; Avoid error if user loads some more libraries now.
(setq purify-flag nil)

;; For machines with CANNOT_DUMP defined in config.h,
;; this file must be loaded each time Emacs is run.
;; So run the startup code now.

(or (fboundp 'dump-emacs)
    (eval top-level))

;;; loadup.el ends here
