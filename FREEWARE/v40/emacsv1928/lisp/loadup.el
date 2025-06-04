;;; loadup.el --- load up standardly loaded Lisp files for Emacs.

;; Copyright (C) 1985, 1986, 1992, 1994 Free Software Foundation, Inc.

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
      (load "faces")
      (garbage-collect)
      (load "menu-bar")
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
(if (eq system-type 'ms-dos)
    (progn
      (load "ls-lisp")
      (garbage-collect)
      (load "mouse")
      (garbage-collect)
      (load "dos-fns")
      (garbage-collect)
      (load "disp-table") ; needed to setup ibm-pc char set, see internal.el
      (garbage-collect)))
(if (fboundp 'atan)	; preload some constants and 
    (progn		; floating pt. functions if 
      (garbage-collect)	; we have float support.
      (load "float-sup")))
(garbage-collect)
(load "vc-hooks")

;; We specify .el in case someone compiled version.el by mistake.
(load "version.el")

;If you want additional libraries to be preloaded and their
;doc strings kept in the DOC file rather than in core,
;you may load them with a "site-load.el" file.
;But you must also cause them to be scanned when the DOC file
;is generated.  For VMS, you must edit [-.vms]makedoc.com or
;[-.src]descrip.mms_in_in.  For other systems, you must edit
;../src/Makefile.in.in.
(if (load "site-load" t)
    (garbage-collect))

;; Determine which last version number to use
;; based on the executables that now exist.
(setq new-version
      (or (and (equal (nth 3 command-line-args) "dump")
	       (not (equal (nth 4 command-line-args) "nonewversion"))
	       (not (equal (nth 5 command-line-args) "nonewversion")))
	  (and (equal (nth 4 command-line-args) "dump")
	       (not (equal (nth 5 command-line-args) "nonewversion"))
	       (not (equal (nth 6 command-line-args) "nonewversion")))))
(if (and (or (equal (nth 3 command-line-args) "dump")
	     (equal (nth 4 command-line-args) "dump"))
	 (not (eq system-type 'ms-dos)))
    (let ((d (if (memq system-type '(vax-vms axp-vms))
		 (invocation-directory)
	       default-directory))
	  (base (if (memq system-type '(vax-vms axp-vms))
		    (concat "temacs-" emacs-version)
		  (concat "emacs-" emacs-version)))
	  ;; VMS only...
	  (base2 (if (memq system-type '(vax-vms axp-vms))
		     (concat "temacs_d-" emacs-version))))
      (and (memq system-type '(vax-vms axp-vms))
	   (while (string-match "[^-_a-zA-Z0-9]+" base)
	     (setq base (concat (downcase (substring base
						     0 (match-beginning 0)))
				"_"
				(substring base (match-end 0))))))
      (and (memq system-type '(vax-vms axp-vms))
	   (while (string-match "[^-_a-zA-Z0-9]+" base2)
	     (setq base2 (concat (downcase (substring base2
						      0 (match-beginning 0)))
				"_"
				(substring base2 (match-end 0))))))
      (let* ((files (file-name-all-completions base d))
	     (files2 (and base2 (file-name-all-completions base2 d)))
	     (versions (mapcar (function (lambda (name)
					   (if (and (memq system-type
							  '(vax-vms axp-vms))
						    (not (string-match ".DUMP$"
								       name)))
					       0
					     (string-to-int (substring name (1+ (length base)))))))
			       files))
	     (versions2 (append versions
				(mapcar (function
					 (lambda (name)
					   (if (and (memq system-type
							  '(vax-vms axp-vms))
						    (not (string-match
							  ".DUMP$"
							  name)))
					       0
					     (string-to-int
					      (substring name
							 (1+
							  (length base2)))))))
					files2))))
	(setq versions versions2)
	(setq emacs-version (format "%s.%d"
				    emacs-version
				    (if versions
					(if new-version
					    (1+ (apply 'max versions))
					  (apply 'max versions))
				      1))))))

(message "Emacs version is %s" emacs-version)

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
      (let ((name emacs-version))
	(while (string-match "[^-+_.a-zA-Z0-9]+" name)
	  (setq name (concat (downcase (substring name 0 (match-beginning 0)))
			     "-"
			     (substring name (match-end 0)))))
	;; It's more consistent with VMS version numbering to have version
	;; numbers with the form <major>.<minor>-<edit>, and it avoids docs
	;; from other compilations to be purged, if several architectures
	;; share the same installation directory.
	(if (memq system-type '(vax-vms axp-vms))
	    (while (string-match "\\(\\..*\\)\\." name)
	      (setq name (concat (downcase (substring name 0 (match-end 1)))
				 "-"
				 (substring name (match-end 0))))))
	(if (eq system-type 'ms-dos)
	    (setq name (expand-file-name "../etc/DOC"))
	  (setq name (concat (expand-file-name "../etc/DOC-") name))
	  (if (file-exists-p name)
	      (delete-file name))
	  (copy-file (expand-file-name "../etc/DOC") name t))
	(Snarf-documentation (file-name-nondirectory name))))
  (Snarf-documentation "DOC"))
(message "Finding pointers to doc strings...done")

;Note: You can cause additional libraries to be preloaded
;by writing a site-init.el that loads them.
;See also "site-load" above.
(load "site-init" t)
(setq current-load-list nil)
(garbage-collect)

;;; At this point, we're ready to resume undo recording for scratch.
(buffer-enable-undo "*scratch*")

(if (or (equal (nth 3 command-line-args) "dump")
	(equal (nth 4 command-line-args) "dump"))
    ;;    (if (eq system-type 'vax-vms)
    ;;	(let ((fn (invocation-name))
    ;;	      (d (invocation-directory)))
    ;;	  (message (concat "Dumping data as file " (downcase d) fn ".dump"))
    ;;	  (dump-emacs (concat d fn ".dump") fn)
    ;;	  (kill-emacs))
    (let ((d (if (memq system-type '(vax-vms axp-vms))
		 (invocation-directory)
	       ""))
	  (fn (invocation-name)))
      (let ((name (concat (if (memq system-type '(vax-vms axp-vms))
			      fn
			    "emacs") "-" emacs-version)))
	(while (string-match (if (memq system-type '(vax-vms axp-vms))
				 "[^-_a-zA-Z0-9]+"
			       "[^-+_.a-zA-Z0-9]+") name)
	  (setq name (concat (downcase (substring name 0 (match-beginning 0)))
			     (if (memq system-type '(vax-vms axp-vms)) "_" "-")
			     (substring name (match-end 0)))))
	(cond ((eq system-type 'ms-dos)
	       (message "Dumping under the name emacs"))
	      ((memq system-type '(vax-vms axp-vms))
	       (message "Dumping under names emacs and %s%s.dump" d name))
	      (t
	       (message "Dumping under names emacs and %s" name))))
      (condition-case ()
	  (delete-file (if (memq system-type '(vax-vms axp-vms))
			   (concat d fn ".dump")
			 "emacs"))
	(file-error nil))
      ;; We used to dump under the name xemacs, but that occasionally
      ;; confused people installing Emacs (they'd install the file
      ;; under the name `xemacs'), and it's inconsistent with every
      ;; other GNU product's build process.
      (let ((ident (progn (string-match
			   "\\([0-9][0-9]*\\)\\.\\([0-9][0-9]*\\)\\.\\([0-9][0-9]*\\)"
			   emacs-version)
			  (cons (string-to-number (substring emacs-version
							     (match-beginning 1)
							     (match-end 1)))
				(cons (string-to-number (substring emacs-version
								   (match-beginning 2)
								   (match-end 2)))
				      (string-to-number (substring emacs-version
								   (match-beginning 3)
								   (match-end 3))))))))
	(dump-emacs (if (memq system-type '(vax-vms axp-vms))
			(concat d fn ".dump")
		      "emacs") "temacs" ident))
      ;; Recompute NAME now, so that it isn't set when we dump.
      (if (not (eq system-type 'ms-dos))
	  (let ((name (concat (if (memq system-type '(vax-vms axp-vms))
				  fn
				"emacs") "-" emacs-version)))
	    (while (string-match (if (memq system-type '(vax-vms axp-vms))
				     "[^-_a-zA-Z0-9]+"
				   "[^-+_.a-zA-Z0-9]+") name)
	      (setq name (concat (downcase (substring name
						      0 (match-beginning 0)))
				 (if (memq system-type '(vax-vms axp-vms))
				     "_" "-")
				 (substring name (match-end 0)))))
	    (if (memq system-type '(vax-vms axp-vms))
		(add-name-to-file (concat d fn ".dump")
				  (concat d name ".dump")
				  t)
	      (add-name-to-file "emacs" name t))))
      (kill-emacs)))

;; Avoid error if user loads some more libraries now.
(setq purify-flag nil)

;; For machines with CANNOT_DUMP defined in config.h,
;; this file must be loaded each time Emacs is run.
;; So run the startup code now.

(or (or (equal (nth 3 command-line-args) "dump")
	(equal (nth 4 command-line-args) "dump"))
    (eval top-level))

;;; loadup.el ends here
