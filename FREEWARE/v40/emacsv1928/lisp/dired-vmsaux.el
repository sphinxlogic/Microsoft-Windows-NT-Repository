;;; dired-vmsaux.el --- all of dired except what people usually use

;; Copyright (C) 1985, 1986, 1992 Free Software Foundation, Inc.

;; Author: Richard Levitte <levitte@e.kth.se>

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

;; Some of the functions in dired-aux.el need to be changed for VMS.
;; That's what is done in this file.

;;; Code:


;;; Cleaning a directory: flagging some backups for deletion.

;; The only thing which is needed to change for VMS are the subroutines
;; to dired-clean-directory which directly handle version numbers.

;;; Subroutines of dired-clean-directory.

(defun dired-collect-file-versions (fn)
  ;;  "If it looks like file FN has versions, return a list of the versions.
  ;;That is a list of strings which are file names.
  ;;The caller may want to flag some of these files for deletion."
  (let ((fn (file-name-sans-versions fn)))
    ;; We don't want to redo the whole operation if it has already been done...
    (if (not (assoc fn dired-file-version-alist))
	(let* ((base-file-name (file-name-nondirectory fn))
	       (possibilities (file-name-all-versions
			       base-file-name
			       (file-name-directory fn)))
	       (versions (mapcar 'backup-extract-version possibilities)))
	  (if versions
	      (setq dired-file-version-alist (cons
					      (cons fn versions)
					      dired-file-version-alist)))))))

(defun dired-trample-file-versions (fn)
  (let* ((base-file-name (file-name-sans-versions fn))
	 base-version-list)
    (and (setq base-version-list	; there was a base version to which
	       (assoc base-file-name	; this looks like a
		      dired-file-version-alist)) ; subversion
	 (not (memq (backup-extract-version fn)
		    base-version-list))	; this one doesn't make the cut
	 (progn (beginning-of-line)
		(delete-char 1)
		(insert dired-del-marker)))))


(defun dired-create-directory (directory)
  "Create a directory called DIRECTORY."
  (interactive
   (list (read-file-name "Create directory: " (dired-current-directory))))
  (let ((expanded (file-name-as-directory (expand-file-name directory))))
    (make-directory expanded)
    (dired-add-file (directory-file-name expanded))
    (dired-move-to-filename)))


(defun dired-remove-entry (file)
  (save-excursion
    (and (dired-goto-file file)
	 (let (buffer-read-only)
	   (delete-region (progn (beginning-of-line) (point))
			  (save-excursion
			    (forward-line 1)
			    (while (looking-at "	")
			      (forward-line 1))
			    (point)))))))
