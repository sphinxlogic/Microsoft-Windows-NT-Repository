;; bzredit.el: a simple-minded editor for outline fonts.
;; 
;; Copyright (C) 1992 Free Software Foundation, Inc.
;; 
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.
;; 
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;; 
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;;

(defvar bzr-gs-proc nil "The Ghostscript process used for display.
Initialized by bzredit.")

(defun bzredit (fontname)
  "Edit the BZR font FONTNAME."
  (interactive "FBZR font: ")
  
  ;; Edit the file in the buffer `FONTNAME', tossing the old contents.
  (let ((bzrbufname fontname))
    (if (not (get-buffer bzrbufname)) (get-buffer-create bzrbufname))
    (set-buffer bzrbufname)
    (erase-buffer)
    
    ;; Get the human- and Emacs-readable text.
    (call-process "bzrto" nil (current-buffer) nil "-text" fontname)
    
    ;; Set up the editing features.
    (bpl-mode)
    
    ;; Make this buffer visible.
    (if (get-buffer-window bzrbufname)
      (select-window (get-buffer-window bzrbufname))
      (switch-to-buffer bzrbufname)
    )
  )
)

(defvar bzr-gs-width 300
  "Width of the X window started by Ghostscript in bzredit.")
(defvar bzr-gs-height 300
  "Width of the X window started by Ghostscript in bzredit.")
(defvar bzr-gs-dpi 300
  "Resolution in pixels per inch at which Ghostscript renders images.")

;; If we wanted to edit more than one font simultaneously,
;; we'd have to change this, to start up a new GS process for each.
(defun bzr-start-ghostscript ()
  "Start a Ghostscript process to display the modified characters.  Set
bzr-gs-proc to this process."

  (let ((process-connection-type nil))  ; Use a pipe.
    (setq bzr-gs-proc 
      (start-process "bzredit-gs" "*bzredit output: gs*"
                     "gs" "-q" "-dNOPAUSE" 
                     (concat "-g" bzr-gs-width "x" bzr-gs-height)
                     "-")
    )
    ;; Have to have GS do the division, since Elisp doesn't have
    ;; floating-point.
    (bzr-gs-send (concat "["
                         bzr-gs-dpi "72.27 div"
                         " 0 0 "
                         bzr-gs-dpi "72.27 div neg"
                         " 0 "
                         bzr-gs-height 
                         "] setmatrix\n")
    
    ;; Establish a current point.
    (bzr-gs-send "10 10 moveto\n")
  )
)

(defun bzr-gs-send (str)
  "Send the string STR to bzr-gs-proc."
  
  (if (eq (process-status bzr-gs-proc) 'exit)
    (bzr-start-ghostscript)
  )
  (process-send-string bzr-gs-proc str)
)

(defun bpl-mode ()
  "Major mode for editing outline fonts in BZR property list (BPL) format.

It has these extra commands:
\\{bpl-mode-map}

Entering bpl mode calls the value of bpl-mode-hook."
  (interactive)
  
  (setq major-mode 'bpl-mode)
  (setq mode-name "BPL")
  (use-local-map bpl-mode-map)

  (bzr-start-ghostscript)

  (run-hooks 'bpl-mode-hook)
)

(defvar bpl-mode-map nil)
(if bpl-mode-map
  nil
  (setq bpl-mode-map (make-sparse-keymap))
  (define-key bpl-mode-map "\C-cc" 'bpl-show-char)
  (define-key bpl-mode-map "\C-c\C-c" 'bpl-show-char)
  (define-key bpl-mode-map "\C-ce" 'bpl-erasepage)
  (define-key bpl-mode-map "\C-c\C-e" 'bpl-erasepage)
  (define-key bpl-mode-map "\C-cq" 'bpl-quit)
  (define-key bpl-mode-map "\C-c\C-q" 'bpl-quit)
)

(defun bpl-show-char ()
  "Pass the current character to Ghostscript for display."
  (interactive)
  
  ;; Send Ghostscript the commands for this character.
  (eval-defun nil)
)

(defun bpl-quit ()
  "Kill the Ghostscript process and call bury-buffer."
  (interactive)
  
  ;; Stop Ghostscript.
  (process-send-eof bzr-gs-proc)
  
  ;; Should convert back to BZR?
)

(defun bpl-erasepage ()
  "Erase the Ghostscript window."
  (interactive)
  
  (bzr-gs-send "erasepage showpage\n")
)

;;; If we had packages, we could make each of these functions local.  But
;;; I don't know how to do that as things stand.

;;; When we do something about hints we will have to construct a font
;;; and show characters from it.

;; This must be a macro so we do not evaluate CHARDEF first.
(defmacro char (charcode &rest chardef)
  "Ignore the CHARCODE argument.  Evaluate all additional arguments (the
  CHARDEF)."
  
  (mapcar 'eval chardef)
  (bzr-gs-send "fill\n")

  ;; Be sure it is displayed.
  (bzr-gs-send "copypage\n")
)

(defmacro comment (&rest args)
  "Ignore all arguments."
)

;; Nothing to do for the width or bounding box.
(defun width (n)
  "The set width is realstr N."
)

(defun bb (llx lly urx ury)
  "The bounding box is a four-element vector of realstrs: LLX LLY URX URY."
)

;; This must be a macro so we do not evaluate CMDS first.
(defmacro outline (sx sy &rest cmds)
  "A single outline is the starting point (SX,SY) plus an arbitrary
number of additional commands."

  (bzr-gs-send (format "%s %s moveto\n" sx sy))
  (mapcar 'eval cmds)
  (bzr-gs-send "closepath\n")
)

(defun line (px py)
  "Draw a line from the current point to (PX,PY)."

  (bzr-gs-send (format "%s %s lineto\n" px py))
)

(defun spline (c1x c1y c2x c2y ex ey)
  "Draw a spline from the current point with controls (C1X,C1Y) and (C2X,C2Y)
to (EX,EY)."
  
  (bzr-gs-send (format "%s %s %s %s %s %s curveto\n" c1x c1y c2x c2y ex ey))
)
