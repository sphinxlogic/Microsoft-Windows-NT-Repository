;;; compile-vms.el --- functions to parse VMS error messages.

;; Copyright (C) 1994 Free Software Foundation, Inc.

;; Author: Richard Levitte <levitte@e.kth.se>
;; Maintainer: Richard Levitte
;; Keywords: VMS

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

;;; Code:

(defvar compilation-parse-error-subfunction-alist
  '(("PASCAL" . compilation-parse-vms-pascal-error)
    ("CC" . compilation-parse-vms-cc-error)
    ("CXX" . compilation-parse-vms-cc-error)
    ("FORT" . compilation-parse-vms-fortran-error)
    ("MACRO" . compilation-parse-vms-macro-error))
  "Alist specifying how to match compilers with error processing functions.
Each element has the form (NAME . FUNCTION).  NAME matches the facility
name in the VMS status message `\%facility-severity-identity, text'.")

(defun compilation-parse-vms-error (match-index)
  (let* ((case-fold-search nil)
	 (here (cons (match-beginning match-index)
		     (match-end match-index)))
	 (facility (buffer-substring (match-beginning (1+ match-index))
				     (match-end (1+ match-index))))
	 (identity (cons (match-beginning (+ 2 match-index))
			 (match-end (+ 2 match-index))))
	 (errtext (cons (match-beginning (+ 3 match-index))
			(match-end (+ 3 match-index))))
	 (parse-error-function (assoc
				facility
				compilation-parse-error-subfunction-alist))
	 result)
    (if parse-error-function
	(setq result (funcall (cdr parse-error-function)
			      here identity errtext)))
    (if (not result)
	(save-excursion
	  (save-restriction
	    (goto-char (car errtext))
	    (and (re-search-forward
		  "at line number \\([0-9]+\\) in file \\([^;\n]+\\)" nil t)
		 (list (buffer-substring (match-beginning 2)
					 (match-end 2))
		       (progn
			 (goto-char (match-beginning 1))
			 (read (current-buffer)))
		       (progn
			 (goto-char (car here))
			 (forward-line -1)
			 (beginning-of-line)
			 (if (re-search-forward "^\t?\\(\\.*\\)\\^$"
						(car here) t)
			     (- (match-end 1) (match-beginning 1))
			   0))))))
      result)))

(defun compilation-parse-vms-pascal-error (here ident errtext)
  (save-excursion
    (save-restriction
      (goto-char (car errtext))
      (if (re-search-forward "\\((\\([0-9]\\))\\|Compiling file \\([^;]*\\)\\)"
			     (cdr errtext) t)
	  (if (match-beginning 2)
	      (let ((col (progn
			   (goto-char (match-beginning 1))
			   (read (current-buffer))))
		    (row (progn
			   (re-search-backward "^[0-9][0-9][0-9][0-9][0-9]"
					       nil t)
			   (read (current-buffer)))))
		(list compilation-last-vms-file-name
		      row
		      (progn
			(forward-line 1)
			(beginning-of-line)
			(search-forward (int-to-string num))
			(- (point)
			   1 ; Those index numbers are one character long
			   (progn (beginning-of-line)
				  (forward-char 2) ; skip the two first tabs
				  (point))))))
	    (setq compilation-last-vms-file-name
		  (buffer-substring (match-beginning 3)
				    (match-end 3)))
	    nil)))))

(defun compilation-parse-vms-fortran-error (here ident errtext)
  (save-excursion
    (save-restriction
      (goto-char (car errtext))
      (if (re-search-forward "at line \\([0-9]+\\)" (cdr errtext) t)
	  (progn
	    (goto-char (match-beginning 1))
	    (if (< compilation-last-vms-file-name-point
		   (point))
		(save-excursion
		  (setq compilation-last-vms-file-name nil)
		  (if (re-search-forward
		       "^%FORT-W-END\\(DIAGS\\|NOOBJ\\), \\([^ ]*\\)$")
		      (setq compilation-last-vms-file-name-point
			    (match-beginning 2)
			    compilation-last-vms-file-name
			    (buffer-substring (match-beginning 2)
					      (match-end 2))))))
                   (if compilation-last-vms-file-name
                       (list compilation-last-vms-file-name
                             (read (current-buffer))
                             0)))))))

(defun compilation-parse-vms-cc-error (here identity errtext)
  (save-excursion
    (save-restriction
      (goto-char (car errtext))
      (and (re-search-forward
	    "At line number \\([0-9]+\\) in \\([^;\n]+\\)" nil t)
	   (list (buffer-substring (match-beginning 2)
				   (match-end 2))
		 (progn
		   (goto-char (match-beginning 1))
		   (read (current-buffer)))
		 (progn
		   (goto-char (car here))
		   (forward-line -1)
		   (beginning-of-line)
		   (if (re-search-forward "^\t\\(\\.*\\)\\^$"
					  (car here) t)
		       (- (match-end 1) (match-beginning 1))
		     0)))))))

(add-hook 'compilation-mode-hook
	  '(lambda ()
	     (setq compilation-last-vms-file-name nil
		   compilation-last-vms-file-name-point nil)))

(provide 'compile-vms)
