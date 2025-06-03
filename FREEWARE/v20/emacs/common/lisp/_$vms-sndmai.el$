;;; vms-sndmai.el --- replacement of the sendmail-send-it function on VMS.
;;; Copyright (C) 1993 Free Software Foundation, Inc.

;; Author: Richard Levitte <levitte@e.kth.se>.
;; Keywords: vms, mail

;;; This file is part of GNU Emacs.

;;; GNU Emacs is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2, or (at your option)
;;; any later version.

;;; GNU Emacs is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.

;;; You should have received a copy of the GNU General Public License
;;; along with GNU Emacs; see the file COPYING.  If not, write to
;;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

;;; Commentary:

;;; The changelog is at the end of file.

;;; Please send me bug reports, bug fixes, and extensions, so that I can
;;; merge them into the master source.
;;;     - Richard Levitte <levitte@e.kth.se>

;;; This file contains (almost) what is needed to send mail from inside
;;; GNU Emacs on VMS. No installation is needed in your .emacs file.
;;; It's all done in vms-patch.el

;;; Code:

(require 'mail-utils)

(defvar vms-inet-prefix "MX%"
  "This is the prefix in VMS for Internet addresses.
This may be overriden if you define a variable vms-inet-prefix
with a string containing the name of your local prefix, like
\"INET%\" for example.")

(defvar vms-sendmail-temp-dir "SYS$SCRATCH:"
  "This is the directory where the temporary files should be put.
This may be changed for better control.")

(defvar vms-sendmail-function 'vms-default-sendmail-method
  "This variable contains the name of the function which send the mail.
You might want to define your own function.  The function should take
three arguments, like this:

  (mailfile maildescfile &optional errbuf)

MAILFILE and MAILDESCFILE will contain the names of two files.  The
first file (MAILFILE) contains the  body of the mail.  The second file
(MAILDESCFILE) contains the headers of the mail, including a `File:'
header which points at the first file.

Your function has the responsability to remove the files after they
are used.")

;;; End of customization part

(defun vms-default-sendmail-method (mailfile maildescfile &optional outbuf)
  "Default way to send mail."
  (if (fboundp 'call-process)
      (call-process "vmssendmail" nil outbuf nil maildescfile)
    (save-excursion
      (set-buffer outbuf)
      (insert "%FOO, there is no way I can call another process.")))
  (delete-file mailfile)
  (delete-file maildescfile))

(defun vms-build-mail-desc (to cc bcc subject filename)
  (erase-buffer)
  (vms-insert-mail-desc-list "To" (mapcar 'vms-process-one-address to))
  (vms-insert-mail-desc-list "CC" (mapcar 'vms-process-one-address cc))
  (vms-insert-mail-desc-list "BCC" (mapcar 'vms-process-one-address bcc))
  (and subject (insert "Subject: " subject "\n"))
  (and filename (insert "File: " filename "\n")))

(defun vms-insert-mail-desc-list (cmnd desc-list)
  (while desc-list
    (insert cmnd ": " (car desc-list) "\n")
    (setq desc-list (cdr desc-list))))

(defun vms-mail-desc-list (string)
  (let (tem)
    (while (setq tem (string-match "[ \t\n]+" string tem))
      (if (= (match-end 0) (length string))
	  (setq string (substring string 0 tem))
	(setq string (concat (substring string 0 tem)
			     " "
			     (substring string (match-end 0))))
	(setq tem (1+ tem))))
    (erase-buffer)
    (insert string)
    (goto-char (point-min))
    (nreverse (mail-parse-comma-list))))

(defun vms-process-quotes (line)
  (let ((result "")
	(tempend 0)
	tempstr)
    (while (string-match "\"" line tempend)
      (progn
	(setq tempstr (substring line tempend (match-beginning 0)))
	(setq tempend (match-end 0))
	(setq result (concat result tempstr "\"\""))))
    (concat result (substring line tempend))))
	
(defun vms-process-one-address (addr)
  "Takes one address, and munges it until it is usable."
  (let (tempaddr)
    (cond ((string-match "^[ \t]*_?[A-Za-z]*%\".*\"[ \t]*$" addr)
	   ;; This is an address of the form foo%"bar@host" (whatever foo is)
	   addr)
	  ((string-match "^[ \t]*_?[A-Za-z]*%[^@ \t]*[ \t]*$" addr)
	   ;; This is an address starting with foo% (whatever foo is)
	   ;; and not containing a @ (thus, not being an internet addr)
	   addr)
	  ((string-match "^[ \t]*_?[A-Za-z0-9]*::" addr)
	   ;; This is mail to another DECnet node. Do not munge!
	   addr)
	  ((string-match "^[ \t]*@" addr)
	   ;; This most likely is a distribution file, so DO NOT MUNGE!
	   addr)
	  (t
	   ;; Now, we have probably found al the VMS specific cases, so now,
	   ;; we only have the Internet addresses to take care of
	   (if (string-match "@" addr)
	       ;;(progn
	       ;;  (if vms-mail-fix-MX-bug (setq addr (vms-fix-MX-bug addr)))
	       ;;  (setq addr (mail-strip-quoted-names addr))
	       ;;  (concat vms-inet-prefix "\"" (vms-process-quotes addr) "\""))
	       (concat vms-inet-prefix
		       "\""
		       (vms-process-quotes (mail-strip-quoted-names addr))
		       "\"")
	     addr)))))

(defun vms-sendmail-send-it (&optional process)
  (let ((errbuf (get-buffer-create " sendmail errors"))
	(tembuf (generate-new-buffer " sendmail temp"))
	(case-fold-search nil)
	delimline
	(there-were-errors nil)
	(mailbuf (current-buffer)))
    (unwind-protect
	(save-excursion
	  (set-buffer errbuf)
	  (erase-buffer)
	  (set-buffer tembuf)
	  (erase-buffer)
	  (insert-buffer-substring mailbuf)
	  (goto-char (point-max))
	  ;; require one newline at the end.
	  (or (= (preceding-char) ?\n)
	      (insert ?\n))
	  ;; Change header-delimiter to be what sendmail expects.
	  (goto-char (point-min))
	  (re-search-forward
	    (concat "^" (regexp-quote mail-header-separator) "\n"))
	  (replace-match "\n")
	  (backward-char 1)
	  (setq delimline (point-marker))
	  (if mail-aliases
	      (expand-mail-aliases (point-min) delimline))
	  (goto-char (point-min))
	  ;; ignore any blank lines in the header
	  (while (and (re-search-forward "\n\n\n*" delimline t)
		      (< (point) delimline))
	    (replace-match "\n"))
	  (let ((case-fold-search t))
	    (goto-char (point-min))
	    (if (re-search-forward "^Sender:" delimline t)
		(error "Sender may not be specified."))
	    ;; Find and handle any FCC fields.
	    (goto-char (point-min))
	    (if (re-search-forward "^FCC:" delimline t)
		(mail-do-fcc delimline))
;;; Apparently this causes a duplicate Sender.
;;;	    ;; If the From is different than current user, insert Sender.
;;;	    (goto-char (point-min))
;;;	    (and (re-search-forward "^From:"  delimline t)
;;;		 (progn
;;;		   (require 'mail-utils)
;;;		   (not (string-equal
;;;			 (mail-strip-quoted-names
;;;			  (save-restriction
;;;			    (narrow-to-region (point-min) delimline)
;;;			    (mail-fetch-field "From")))
;;;			 (user-login-name))))
;;;		 (progn
;;;		   (forward-line 1)
;;;		   (insert "Sender: " (user-login-name) "\n")))
	    ;; "S:" is an abbreviation for "Subject:".
	    (goto-char (point-min))
	    (if (re-search-forward "^S:" delimline t)
		(replace-match "Subject:"))
	    ;; Don't send out a blank subject line
	    (goto-char (point-min))
	    (if (re-search-forward "^Subject:[ \t]*\n" delimline t)
		(replace-match ""))
	    (if mail-interactive
		(save-excursion
		  (set-buffer errbuf)
		  (erase-buffer))))
	  (let* ((random-string (int-to-string (random t)))
		 (emacsmail-file
		  (concat vms-sendmail-temp-dir
			  "EMACSMAIL"  random-string ".tmp;1"))
		 (emacsmaildesc-file
		  (concat vms-sendmail-temp-dir
			  "EMACSMAILDESC" random-string ".tmp;1"))
		 (maildesc-buf
		  (generate-new-buffer " vms sendmail temp"))
		 to-string cc-string bcc-string subject to cc bcc)
	    (append-to-file (+ delimline 1) (point-max) emacsmail-file)
	    (save-excursion
	      (save-restriction
		(narrow-to-region (point-min) delimline)
		(setq to-string (mail-fetch-field "To" nil t))
		(setq cc-string (mail-fetch-field "CC" nil t))
		(setq bcc-string (mail-fetch-field "BCC" nil t))
		(setq subject (mail-fetch-field "Subject")))
	      (set-buffer maildesc-buf)
	      ;; Make descriptor lists
	      (if to-string
		  (setq to (vms-mail-desc-list to-string)))
	      (if cc-string
		  (setq cc (vms-mail-desc-list cc-string)))
	      (if bcc-string
		  (setq bcc (vms-mail-desc-list bcc-string)))
	      ;; Build the VMS mail description file
	      (vms-build-mail-desc to cc bcc subject emacsmail-file)
	      (append-to-file (point-min) (point-max) emacsmaildesc-file)
	      (kill-buffer maildesc-buf))
	    (apply vms-sendmail-function
		   emacsmail-file emacsmaildesc-file
		   (if (bufferp errbuf) errbuf) '()))
	  (let ((cur-buf (current-buffer)))
	    (set-buffer errbuf)
	    (goto-char (point-min))
	    (while (search-forward "\n\n" nil t)
	      (replace-match "\n"))
	    (goto-char (point-min))
	    (if (re-search-forward "^%" nil t)
		(let ((win (get-buffer-window errbuf)))
		  (if win
		      (delete-window win))
		  (goto-char (1- (point-max)))
		  (delete-char 1)
		  (goto-char (point-min))
		  (setq mode-line-format
			"-------------------------- Mail Errors %-")
		  (require 'electric)
		  (let* ((window-min-height 2)
			 (new-win (split-window))
			 new-height
			 (orig-height (window-height new-win)))
		    (set-window-buffer new-win errbuf)
		    (save-window-excursion
		      (shrink-window-if-larger-than-buffer new-win)
		      (setq new-height (window-height new-win)))
		    (enlarge-window (- orig-height new-height)))
		  (setq there-were-errors t)
		  (error "Errors while sending mail")))))
      (kill-buffer tembuf)
      (if (not there-were-errors)
	  (let ((win (get-buffer-window errbuf)))
	    (if win
		(delete-window win))
	    (kill-buffer errbuf))))))

;;; Changelog:
;;;
;;; Richard Levitte, 1993-05-01
;;;	- Created

;;; vms-sndmai.el ends here
