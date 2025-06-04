;;; vms-patch.el --- override parts of files.el for VMS.

;; Copyright (C) 1986, 1992 Free Software Foundation, Inc.

;; Maintainer: FSF
;; Keywords: vms

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

;;; Code:

;;; Functions that need redefinition

;;; VMS file names are upper case, but buffer names are more
;;; convenient in lower case.

(defun create-file-buffer (filename)
  "Create a suitably named buffer for visiting FILENAME, and return it.
FILENAME (sans directory) is used unchanged if that name is free;
otherwise a string <2> or <3> or ... is appended to get an unused name."
  (generate-new-buffer (downcase (file-name-nondirectory filename))))

;;; Given a string FN, return a similar name which is a legal VMS filename.
;;; This is used to avoid invalid auto save file names.
;;; As a matter of fact, this might be used elsewhere, so we need a flag
;;; telling how dots should be processed:
;;;   'leave-dots     - don't touch the dots
;;;   'take-all-dots  - convert all the dots, even the first.
;;;   nil or other    - convert all but the first to underscore.
(defun make-legal-file-name (fn &optional process-dots)
  (setq fn (copy-sequence fn))
  (let ((dot nil) (indx 0) (len (length fn)) chr)
    (while (< indx len)
      (setq chr (aref fn indx))
      (cond
       ((eq chr ?.) (cond
		     ((eq process-dots 'take-all-dots) (aset fn indx ?_))
		     ((eq process-dots 'leave-dots)) ; noop
		     (t (if dot (aset fn indx ?_) (setq dot t)))))
       ((not (or (and (>= chr ?a) (<= chr ?z)) (and (>= chr ?A) (<= chr ?Z))
		 (and (>= chr ?0) (<= chr ?9))
		 (eq chr ?$) (eq chr ?_) (and (eq chr ?-) (> indx 0))))
	(aset fn indx ?_)))
      (setq indx (1+ indx))))
  fn)

;;; There are no symlinks on VMS. But there are search paths, so
;;; we need another file-truename

(defun file-truename (filename &optional expand-completely)
  "Return the truename of FILENAME, which should be absolute.
The truename of a file name is found by chasing symbolic links
both at the level of the file and at the level of the directories
containing it, until no links are left at any level."
  ;; We get some help from vms-expand-search-paths, which is good for
  ;; finding existing files.
  (let ((handler (find-file-name-handler filename 'file-truename)))
    ;; For file name that has a special handler, call handler.
    ;; This is so that ange-ftp can save time by doing a no-op.
    (if handler
	(funcall handler 'file-truename filename)
      (let ((result (vms-expand-search-paths filename expand-completely)))
	(if (not (file-attributes result))
	    ;; We couldn't find the file... so recurse to find as good a
	    ;; directory specification as possible (as few levels as possible
	    ;; will be bogus).
	    (let ((dir1 (file-name-directory filename)))
	      (if (not (string= dir1 (file-name-directory result)))
		  (setq result (concat (file-name-as-directory
					(file-truename
					 (directory-file-name dir1)
					 expand-completely))
				       (file-name-nondirectory result))))))
	result))))

;;; On VMS, backup files are represented a whole different way than on UNIX,
;;; So let's provide a function that does things right.

(defun file-newest-backup (filename)
  "Return most recent backup file for FILENAME or nil if no backups exist."
  (let* ((filename (expand-file-name filename))
	 (file (file-name-nondirectory filename))
	 (dir  (file-name-directory    filename))
	 (comp (file-name-all-completions-with-versions file dir))
	 newest)
    (while comp
      (setq file (concat dir (car comp))
	    comp (cdr comp))
      (if (and (string= (file-name-sans-versions file) filename)
	       (or (null newest) (file-newer-than-file-p file newest)))
	  (setq newest file)))
    newest))

(defun backup-file-name-p (file)
  "Return non-nil if file is a backup file name.  This is a separate
function so you can redefine it for customization.
You may need to redefine `file-name-sans-versions' as well."
  (string-match "\\(\\.[^.]\\.\\|;\\)\\([0-9]+\\)$" file))

(defun backup-extract-version (fn)
  "Given the name of a numeric backup file, return the backup number."
  (if (backup-file-name-p fn)
      (string-to-int (substring fn (match-beginning 2) (match-end 2)))
    0))

(defun find-backup-file-name (fn)
  "Find a file name for a backup file, and suggestions for deletions.
Value is a list whose car is the name for the backup file
and whose cdr is a list of old versions to consider deleting now."
  (if (eq version-control 'never)
      (list (make-backup-file-name fn))
    (let* ((base-version (file-name-nondirectory (file-name-sans-versions fn)))
	   possibilities number-to-delete)
      (condition-case ()
	  (setq possibilities
		(file-name-all-versions base-version
					(file-name-directory fn))
		number-to-delete
		(- (length possibilities)
		   kept-old-versions (max 1 kept-new-versions)))
	(file-error
	 (setq possibilities nil)))
      (if nil ;(not version-control)
	  (list fn)
	(cons fn
	      (if (and (> number-to-delete 0)
		       (>= (+ (max 1 kept-new-versions) kept-old-versions) 0))
		  (let ((v (nthcdr kept-old-versions possibilities)))
		    (rplacd (nthcdr (1- number-to-delete) v) ())
		    v)))))))

;;; We might get stupid errors from write-region because of some variants
;;; of search-paths... Let's fix that.

(if (not (fboundp 'old-write-region))
    (fset 'old-write-region (symbol-function 'write-region)))

(defun write-region (start end filename &optional app visit)
  (interactive "r\nFWrite region to file: ")
  (condition-case error (old-write-region start end filename app visit)
    (error (old-write-region start end
			     (concat 
			      (file-name-directory
			       (file-truename
				(directory-file-name filename)))
			      (file-name-nondirectory filename))
			     app visit))))

;;; Auto save filesnames start with _$ and end with $.

(defun make-auto-save-file-name ()
  "Return file name to use for auto-saves of current buffer.
Does not consider auto-save-visited-file-name; that is checked
before calling this function.
This is a separate function so your .emacs file or site-init.el can redefine it.
See also auto-save-file-name-p."
  (if buffer-file-name
      (let* ((directory (file-name-directory buffer-file-name))
	     (version (file-name-nondirectory buffer-file-name))
	     (name (file-name-sans-versions version))
	     (version (substring version (length name))))
	(concat directory "_$" name "$" version))
    (expand-file-name (format "_$%s$%s$"
			      (make-legal-file-name (buffer-name))
			      (make-temp-name "")))))

(defun auto-save-file-name-p (filename)
  "Return t if FILENAME can be yielded by make-auto-save-file-name.
FILENAME should lack slashes.
This is a separate function so your .emacs file or site-init.el can redefine it."
  (string-match "^_\\$.*\\$[.]?\\([.;][0-9]+\\|[.;]?\\)$"
		(file-name-nondirectory filename)))

;;;
;;; This goes along with kepteditor.com which defines these logicals
;;; If EMACS_COMMAND_ARGS is defined, it supersedes EMACS_FILE_NAME,
;;;   which is probably set up incorrectly anyway.
;;; The function command-line-again is a kludge, but it does the job.
;;;
(defun vms-suspend-resume-hook ()
  "When resuming suspended Emacs, check for file to be found.
If the logical name `EMACS_FILE_NAME' is defined, `find-file' that file."
  (let ((file (vms-system-info "LOGICAL" "EMACS_FILE_NAME"))
	(args (vms-system-info "LOGICAL" "EMACS_COMMAND_ARGS"))
	(line (vms-system-info "LOGICAL" "EMACS_FILE_LINE")))
    (if (not args)
	(if file
	    (progn (find-file file)
		   (if line (goto-line (string-to-int line)))))
      (let ((dir (and file (file-name-directory file))))
	(if (and file dir) (cd dir))
	(vms-command-line-again args)))))

(add-hook 'suspend-resume-hook 'vms-suspend-resume-hook)

(defun vms-suspend-hook ()
  "Don't allow suspending if logical name `DONT_SUSPEND_EMACS' is defined."
  (if (vms-system-info "LOGICAL" "DONT_SUSPEND_EMACS")
      (error "Can't suspend this emacs"))
  nil)

(add-hook 'suspend-hook 'vms-suspend-hook)

;;;
;;; A kludge that allows reprocessing of the command line.  This is mostly
;;;   to allow a spawned VMS mail process to do something reasonable when
;;;   used in conjunction with the modifications to sysdep.c that allow
;;;   Emacs to attach to a "foster" parent.
;;;
(defun vms-command-line-again (args)
  "Reprocess command line arguments.  VMS specific.
Command line arguments are initialized from the logical EMACS_COMMAND_ARGS
which is defined by kepteditor.com.  On VMS this allows attaching to a
spawned Emacs and doing things like \"emacs -l myfile.el -f doit\""
  (let* ((command-line-args-left '())
	 (beg 0)
	 (end 0)
	 (len (length args))
	 this-char def-dir)
    (if args
	(progn
;;; replace non-printable stuff with spaces
;;; Let's be a little cautious here! 8-bit characters are very much
;;; permitted! --- Richard Levitte
	  (while (< beg (length args))
	    (if (> 33 (logand 127 (aref args beg)))
		(aset args beg 32))
	    (setq beg (1+ beg)))
	  (setq beg (1- (length args)))
	  (while (= 32 (aref args beg)) (setq beg (1- beg)))
	  (setq args (substring args 0 (1+ beg)))
	  (setq beg 0)
;;; now start parsing args
	  (while (< beg (length args))
	    (while (and (< beg (length args))
			(> 33 (logand 127 (aref args beg)))
			(setq beg (1+ beg))))
	    (setq end (1+ beg))
	    (while (and (< end (length args))
			(< 32 (logand 127 (aref args end))))
	      (setq end (1+ end)))
	    (setq command-line-args-left (append 
				     command-line-args-left
				     (list (substring args beg end))))
	    (setq beg (1+ end)))
	  (setq def-dir (car command-line-args-left))
	  (setq command-line-args-left (cdr command-line-args-left))
	  ;; Now, let's skip through the options which are normally parsed
	  ;; by emacs.c.
	  (if (string= (car command-line-args-left) "-MAP")
	      (setq command-line-args-left (cdr (cdr command-line-args-left))))
	  (if (string= (car command-line-args-left) "-T")
	      (setq command-line-args-left (cdr (cdr command-line-args-left))))
	  (if (string= (car command-line-args-left) "-NW")
	      (setq command-line-args-left (cdr command-line-args-left)))
	  (if (string= (car command-line-args-left) "-BATCH")
	      (setq command-line-args-left (cdr command-line-args-left)))
	  (if command-line-args-left
	      (progn
		(let* ((noninteractive t) ; Otherwise, command-line-1 will
					; redo the initial display, and
					; other things that should not be
					; done now.
		       (default-directory def-dir) ; Set up the default
					; directory from which possible
					; files will be loaded.
		       )
		  (command-line-1 command-line-args-left)
		  (setq def-dir default-directory))
		;; This is necessary, because the default-directory in
		;; the let* will be restored to its original value,
		;; which is very confusing when it happens to the last
		;; buffer just loaded...
		(setq default-directory def-dir)))))))

(setq dired-listing-switches
      ;; "/SIZE/DATE/OWNER/WIDTH=(FILENAME=32,SIZE=5)"
      "/SIZE/DATE/OWNER")

;;;
;;; Fuctions for using Emacs as a VMS Mail editor
;;;
(autoload 'vms-pmail-setup "vms-pmail"
  "Set up file assuming use by VMS Mail utility.
The buffer is put into text-mode, auto-save is turned off and the
following bindings are established.

\\[vms-pmail-save-and-exit]	vms-pmail-save-and-exit
\\[vms-pmail-abort]	vms-pmail-abort

All other Emacs commands are still available."
  t)

;;;
;;; Replacement to sendmail-send-it in sendmail.el
;;;
(autoload 'vms-sendmail-send-it "vms-sndmai" "sendmail function for VMS."
  nil)
(defun vms-setup-mailer ()
  (setq send-mail-function 'vms-sendmail-send-it))

(add-hook 'mail-setup-hook 'vms-setup-mailer)

;;; vms-patch.el ends here
