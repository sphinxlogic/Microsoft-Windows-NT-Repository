;;; para-mode

;;; Copyright (C) 1989, 1990, 1991 Free Software Foundation
;;; ALPHA test version; not for widespead distribution.
;;; Robert J. Chassell
;;; bugs to bob@ai.mit.edu
;;;
;;; Known bug: para-add-node deletes the detailed part of a master
;;;            menu if the added node is a chapter.  Will need to
;;;            modify para-insert-update-pointers-&-menu, using ideas
;;;            from para-make-master-menu.
;;;
;;; Change Log ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Version 0.27
;;; 17 apr 91 - Defined `para-main-menu-buffer-name' global variable
;;;             with default value of empty string.  `para-xref' now
;;;             works even without listing the main menu.
;;;
;;; Version 0.26
;;;  5 Apr 91 - Added `para-scroll-down' (bound to DEL); works
;;;             parallel to `para-scroll-up' (bound to SPC).
;;;
;;; Version 0.25
;;;  3 Apr 91 - `para-make-file' rewritten.
;;;
;;; Version 0.24
;;; 27 Mar 91 - Indexing commands defined.
;;;
;;; Version 0.23
;;; 25 Mar 91 - `para-menu-explode' defined; forgot to include it before.
;;;              `para-preceding-node-type' returns "top" if in top node
;;;              and current node if on node line.  `para-add-node'
;;;              changed to work with new `para-preceding-node-type'.
;;;              `para-search' defined; searches forward for regexp.
;;;
;;; Version 0.22
;;; 19 Mar 91 - Rewrote `para-extract-menu-name' to use
;;;             `para-extract-menu-node-name' which returns nodename.
;;;             This makes mouse menu selection possible in X.
;;;
;;; Version 0.21
;;;  4 Mar 91 - Added `para-history' to `para-goto' when using tag table.
;;;             Added `para-tagify' and its utility function,
;;;             `para-clean-name' so tag tables can be made.
;;;             Removed infinite loop from `para-last'.
;;;             Added new movement commands: para-scroll-up,
;;;             para-top-node, para-final,
;;;             para-goto-previous-node-in-sequence,
;;;             para-goto-next-node-in-sequence
;;;
;;; Version 0.20
;;; 26 Feb 91 - Rewrote much of code.  No longer dependent on `infosim.el'
;;;
;;; Version 0.10
;;; 12 Feb 91 - Revised all keybindings to be consistent with Texinfo mode.
;;;             Merged bug fixes and new menu and file finding commands.
;;;             Revised `para.texinfo' doc file.
;;;
;;; <other change logs removed>
;;;
;;; Version 0.01
;;; 17 Dec 1989 - 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;; What is Para mode?

;;; Para mode is an extension of Texinfo and Infosim mode that
;;; includes some hypertext-like features.  I chose the name `Para
;;; mode' (from the Greek word for `beyond') to suggest that it is
;;; `beyond' Texinfo mode or Infosim mode, but not to claim that it is
;;; a full-fledged hypertext system.  Eventually, Para mode should be
;;; merged into Texinfo mode.

;;; Para mode was inspired by Infosim mode, which was written by
;;; Matthew P Wiener.

;;; This code is in `alpha-test'.  Please do not distribute it widely.


;;; Summary list of Para mode commands
; 
;     C-c C-p f       para-make-file
;     C-c C-p C-q     para-quit
; 
;     C-c C-p a       para-add-node
;     C-c C-p i       para-insert-menu
;     C-c C-p e       para-menu-explode
;     C-c C-p n       para-narrow-to-node
; 
;     C-c C-p m       para-make-master-menu           (primary updating 
;                                                      command)
; 
;     C-c C-p t       para-tagify                     (for faster movement
;                                                      in large files)
;
; Directory listing and related movement commands
; 
;     C-c C-p d       para-directory
;     C-c C-p l       para-list-visited-nodes
; 
;     C-c C-p C-m     para-main-menu-goto-node        (these commands parallel
;     C-c C-p C-m     para-goto-visited-node           each other in 
;     C-c C-p C-m     para-menu                        different buffers)
; 
; Other movement commands
;
;     C-c C-p C-b     para-beginning                   (beginning of file)
;     C-c C-p C-<     para-top-node
;     C-c C-p C->     para-final                       (last node in file)
;
;     C-c C-p C-u     para-up
;     C-c C-p C-p     para-prev
;     C-c C-p C-[     para-goto-previous-node-in-sequence
;     C-c C-p C-n     para-next
;     C-c C-p C-]     para-goto-next-node-in-sequence
;     C-c C-p RET     para-menu        (i.e., key chord is `C-p C-c C-m')
;     C-c C-p C-l     para-last
;     C-c C-p C-g     para-goto-node
;     C-c C-p C-f     para-follow
; 
;     C-c C-p 1       para-menu-1      go to the node of the first menu 
;                                         item, etc., up to `9'
; 
;     C-c C-p C-s     para-search
; 
; Cross reference creation commands
;
;     C-c C-p r       para-main-menu-buffer-xref      (these commands parallel
;     C-c C-p r       para-visited-xref                each other in 
;     C-c C-p r       para-xref                        different buffers)
;
; Indexing commands
;
;     C-c C-x c       para-index-concept             (these commands insert
;     C-c C-x f       para-index-function             index entries)
;     C-c C-x k       para-index-keystroke
;     C-c C-x p       para-index-program
;     C-c C-x t       para-index-datatype
;     C-c C-x v       para-index-variable
;
;     C-c C-x x       para-insert-index              (cause @printindex
;                                                     to insert indices)
; 
; Texinfo updating commands  (other Texinfo commands work also) 
;
;     C-c C-u C-e     texinfo-every-node-update
;     C-c C-u C-a     texinfo-all-menus-update
;

;;; Load files

;;; The standard Emacs 18 distribution loads version 1 Texinfo files.
;;; Para mode requires version 2 Texinfo files.

;;; To ensure that you load version 2 Texinfo files, you must list the
;;; directory containing the version 2 Texinfo files in your load path
;;; before you list the directory containing the version 1 files.  You
;;; could do this by setting your load path in your `.emacs' file in a
;;; manner somewhat similar to the following: (If the first element of
;;; the load path is nil, it signifies the current directory.)

;;; (setq load-path '(nil "/u/texinfo2" "/u/para" "/usr/local/lib/emacs/lisp")

;;; After setting the load path, you may load the necessary Texinfo
;;; files the following commands:

;(load "texinfo")
;(load "texnfo-upd")

;;; Or you may use `require' commands in this `para.el' file, which is
;;; more elegant:

(require 'texinfo)
(require 'texnfo-upd)

(message "Loading Para mode ... ")
;;; (Place `provide' at end of file.)


;;; Syntax table

(defvar para-mode-syntax-table nil)

(if para-mode-syntax-table nil          ; based in Infosim mode
  (setq para-mode-syntax-table (make-syntax-table))
  (modify-syntax-entry ?\" " " para-mode-syntax-table)
  (modify-syntax-entry ?\\ " " para-mode-syntax-table)
  (modify-syntax-entry ?@ "\\" para-mode-syntax-table)
  (modify-syntax-entry ?\^q "\\" para-mode-syntax-table)
  (modify-syntax-entry ?\[ "(]" para-mode-syntax-table)
  (modify-syntax-entry ?\] ")[" para-mode-syntax-table)
  (modify-syntax-entry ?{ "(}" para-mode-syntax-table)
  (modify-syntax-entry ?} "){" para-mode-syntax-table)
  (modify-syntax-entry ?\' "w" para-mode-syntax-table))


;;; Keybindings

;;; Keybindings are compatible with Texinfo mode,
;;; which are defined in `texinfo.el'.

    ; TeX commands              prefix key: `C-c C-t'
    ; Elisp formatting commands prefix key: `C-c C-e'
    ; C code commands           prefix key: `C-c C-m'
    ; Updating commands         prefix key: `C-c C-u'
    ; String insertion commands prefix key: `C-c C-c'

    ; Para indexing commands    prefix key: `C-c C-x'
    ; Para mode general         prefix key: `C-c C-p'

;;; Para mode keys should use a `C-c C-p' prefix, except for
;;; indexing commands and those keys that have special
;;; meaning only in the upper left-hand corner of the window, or in
;;; read-only buffers.

;;; The Para mode keybinding convention is that movement commands use
;;; only control keys and insertion commands use prefix plus regular
;;; key, like the Texinfo string insertion commands.

(defvar para-mode-map nil "Keymap for Para mode.")

;;; !!! remove this setq after finishing testing!
(setq para-mode-map nil)

(if para-mode-map nil

;;; Provide Texinfo mode keybindings;
;;; must be from Texinfo version 2.
  (setq para-mode-map (copy-keymap texinfo-mode-map))

  (define-key para-mode-map "\t" 'tab-to-tab-stop)

;;; The following keybindings apply only when the cursor is in the
;;; upper left hand corner of the window.
  (define-key para-mode-map " " 'para-@)
  (define-key para-mode-map "0" 'para-@)
  (define-key para-mode-map "1" 'para-@)
  (define-key para-mode-map "2" 'para-@)
  (define-key para-mode-map "3" 'para-@)
  (define-key para-mode-map "4" 'para-@)
  (define-key para-mode-map "5" 'para-@)
  (define-key para-mode-map "6" 'para-@)
  (define-key para-mode-map "7" 'para-@)
  (define-key para-mode-map "8" 'para-@)
  (define-key para-mode-map "9" 'para-@)
  (define-key para-mode-map "b" 'para-@)
  (define-key para-mode-map "d" 'para-@)
  (define-key para-mode-map "f" 'para-@)
  (define-key para-mode-map "g" 'para-@)
  (define-key para-mode-map "l" 'para-@)
  (define-key para-mode-map "m" 'para-@)
  (define-key para-mode-map "n" 'para-@)
  (define-key para-mode-map "p" 'para-@)
  (define-key para-mode-map "q" 'para-@)
  (define-key para-mode-map "s" 'para-@)
  (define-key para-mode-map "u" 'para-@)
  (define-key para-mode-map "<" 'para-@)
  (define-key para-mode-map ">" 'para-@)
  (define-key para-mode-map "[" 'para-@)
  (define-key para-mode-map "]" 'para-@)
  (define-key para-mode-map "\C-?" 'para-@)

;;; `C-c C-p' is the general prefix key for Para mode commands.

;;; Go to first, second, third, etc. node in menu
  (define-key para-mode-map "\C-c\C-p1" 'para-menu-1)
  (define-key para-mode-map "\C-c\C-p2" 'para-menu-2)
  (define-key para-mode-map "\C-c\C-p3" 'para-menu-3)
  (define-key para-mode-map "\C-c\C-p4" 'para-menu-4)
  (define-key para-mode-map "\C-c\C-p5" 'para-menu-5)
  (define-key para-mode-map "\C-c\C-p6" 'para-menu-6)
  (define-key para-mode-map "\C-c\C-p7" 'para-menu-7)
  (define-key para-mode-map "\C-c\C-p8" 'para-menu-8)
  (define-key para-mode-map "\C-c\C-p9" 'para-menu-9)
  (define-key para-mode-map "\C-c\C-p0" 'para-menu-1x)

;;; Para mode movement keybindings -- use only control characters:
  (define-key para-mode-map "\C-c\C-p\C-b" 'para-beginning)
  (define-key para-mode-map "\C-c\C-p\C-f" 'para-follow)
  (define-key para-mode-map "\C-c\C-p\C-g" 'para-goto-node)
  (define-key para-mode-map "\C-c\C-p\C-l" 'para-last)
  (define-key para-mode-map "\C-c\C-p\C-m" 'para-menu)
  (define-key para-mode-map "\C-c\C-p\C-n" 'para-next)
  (define-key para-mode-map "\C-c\C-p\C-p" 'para-prev)
  (define-key para-mode-map "\C-c\C-p\C-q" 'para-quit)
  (define-key para-mode-map "\C-c\C-p\C-s" 'para-search)
  (define-key para-mode-map "\C-c\C-p\C-u" 'para-up)
  (define-key para-mode-map "\C-c\C-p\C-<" 'para-top-node)
  (define-key para-mode-map "\C-c\C-p\C->" 'para-final)
  (define-key para-mode-map "\C-c\C-p\C-]"
    'para-goto-next-node-in-sequence)
  (define-key para-mode-map "\C-c\C-p\C-["
    'para-goto-previous-node-in-sequence)

;;; Keybindings that create or change a buffer use prefix plus key.

;;; Para mode keybindings for commands to create or change the Para
;;; mode file:

  (define-key para-mode-map "\C-c\C-pa" 'para-add-node)
  (define-key para-mode-map "\C-c\C-pe" 'para-menu-explode)
  (define-key para-mode-map "\C-c\C-pf" 'para-make-file)
  (define-key para-mode-map "\C-c\C-pi" 'para-insert-menu)
  (define-key para-mode-map "\C-c\C-pm" 'para-make-master-menu)
  (define-key para-mode-map "\C-c\C-pr" 'para-xref)
  (define-key para-mode-map "\C-c\C-pt" 'para-tagify)

;;; Para mode keybindings for commands to create a temporary buffer
;;; listing all the nodes:

  (define-key para-mode-map "\C-c\C-pd" 'para-directory)
  (define-key para-mode-map "\C-c\C-pl" 'para-list-visited-nodes)

;;; Exception to C-c C-p keybinding conventions:
  (define-key para-mode-map "\C-c\C-pn" 'para-narrow-to-node)

;;; `C-c C-x' is the prefix key for Para indexing.

  (define-key para-mode-map "\C-c\C-xc" 'para-index-concept)
  (define-key para-mode-map "\C-c\C-xf" 'para-index-function)
  (define-key para-mode-map "\C-c\C-xk" 'para-index-keystroke)
  (define-key para-mode-map "\C-c\C-xp" 'para-index-program)
  (define-key para-mode-map "\C-c\C-xt" 'para-index-datatype)
  (define-key para-mode-map "\C-c\C-xv" 'para-index-variable)

  (define-key para-mode-map "\C-c\C-xx" 'para-insert-index))

;;; For bindings inside the temporary buffer created by
;;; `para-list-visited-nodes', use:
;;;
;;; C-c C-p r         para-visited-xref       (based on `para-xref')
;;; C-c C-p C-m       para-goto-visited-node  (parallel to `para-menu')
;;;
;;;  (Note that the keybinding for `para-visited-xref' and
;;;  `para-goto-visited-node' are defined locally in the mode map for
;;;  the temporary buffer, not in the para-mode-map.)

;;; For bindings inside the temporary buffer created by
;;; `para-directory', use:
;;;
;;; C-c C-p r       para-main-menu-buffer-xref (based on `para-xref')
;;; C-c C-p C-m     para-main-menu-goto-node   (parallel to `para-menu')
;;;
;;; (Keybinding defined in local keymap.)


;;; Mode line for Para mode

(defvar para-ignore-extension nil
  "*If non-nil, visited filename extension not shown in mode-line.")

(defun para-set-mode-line ()
  (let ((filename (file-name-nondirectory (buffer-file-name))))
    (if para-ignore-extension
        (setq filename
              (substring filename 0         ; name sans extension
                         (or (string-match "\\.tex" filename)
                             (length filename)))))
    (setq mode-line-modified '("--%1*%1*-")
	  mode-line-buffer-identification
	  (concat "Para mode: ("
                  filename
                  ")"
                  (or para-current-node "")
                  " "))))


;;; Para mode

(defun para-mode ()
  "Hypertext-like extension of Texinfo.
Provides editing and Info-like movement commands for Texinfo files.

When point is at the upper-left corner, or if the buffer is read-only,
Emacs interprets key strokes as Info-like commands.  Otherwise,
keystrokes self-insert.

The default keybindings are compatible with
Texinfo mode.  The prefix keys for the different groups of commands
are:

      TeX commands              prefix key: `C-c C-t'
      Elisp formatting commands prefix key: `C-c C-e'
      Makeinfo commands         prefix key: `C-c C-m'
      Updating commands         prefix key: `C-c C-u'
      String insertion commands prefix key: `C-c C-c'

      Para indexing commands    prefix key: `C-c C-x'
      Para mode general         prefix key: `C-c C-p'

Primary movement commands are similar to those in Info:
\\<para-mode-map>
    Display the first menu:             \\[para-directory]
    Go to the `Next' node:              \\[para-next]
    Go to the `Previous' node:          \\[para-prev]
    Go to the `Up' node:                \\[para-up]
    Go to the menu item named:          \\[para-menu]

Primary editing features are:

    Create a file in Para mode 
        and insert its header:          \\[para-make-file]
    Create a new node
        and section header:             \\[para-add-node]
    Insert a Texinfo menu template:     \\[para-insert-menu]
    Create new node and section 
        headers at the end of the 
        current node, based on menu:    \\[para-menu-explode]
    Insert Texinfo cross reference:     \\[para-xref]
    Make a master menu:                 \\[para-make-master-menu]

The commands are:

\\{para-mode-map}"
  (interactive)
  (text-mode)
  (kill-all-local-variables)
  (use-local-map para-mode-map)
  (set-syntax-table para-mode-syntax-table)
  (setq mode-name "Para"
        major-mode 'para-mode)
  (make-variable-buffer-local 'para-main-menu-completions-list)
  (make-local-variable 'require-final-newline)
  (make-local-variable 'paragraph-separate)
  (make-local-variable 'paragraph-start)
  (make-local-variable 'fill-column)
  (make-local-variable 'comment-start)
  (make-local-variable 'comment-start-skip)
  (make-local-variable 'para-history)
  (make-local-variable 'para-current-node)
  (make-local-variable 'para-ignore-extension)
  (make-local-variable 'para-last-search)
  (setq require-final-newline t
        paragraph-separate (concat "^\b\\|^@[a-zA-Z]*[ \n]\\|"
                                   paragraph-separate)
        paragraph-start (concat "^\b\\|^@[a-zA-Z]*[ \n]\\|" paragraph-start)
        fill-column 72
        comment-start "@c \|@comment "
        comment-start-skip "@c +\|@comment +")
  (para-narrow-to-node)
  (run-hooks 'text-mode-hook 'texinfo-mode-hook 'para-mode-hook))


;;; Make a Para mode file; or quit.

;;;     C-c C-p f       para-make-file       Create a file in Para mode 
;;;     C-c C-p C-q     para-quit            Change to Texinfo mode

(defun para-make-file (filename title node-name chapter-name)
  "Create a file in Para mode and insert its header.
Filename should end in `.tex', `.texi', or `.texinfo'. extension.

This command inserts the `setfilename' and `settitle' commands used by
Texinfo.  Prompts user for FILENAME, TITLE used by TeX, first
NODE-NAME, and first CHAPTER-NAME.

The Info file name is generated from the FILENMAE by removing the
extension and replacing it with `.info'.  If the file already exists
an error is signaled."
  
  (interactive
   (list
    (read-from-minibuffer "File name: ")
    (read-from-minibuffer "Title: ")
    (read-from-minibuffer "Name for first node: ")
    (read-from-minibuffer "Name for first chapter: ")))
  
  (if (file-exists-p filename)
      (error "File `%s' already exists!" filename))
  
  (find-file filename)
  (set-syntax-table para-mode-syntax-table)
  (goto-char (point-min))
  
  (let ((info-filename
         (concat
          (file-name-nondirectory
           (substring 
            filename 
            0                         
            (or (string-match "\\.tex" filename) (length filename))))
          ".info")))
    
    ;; Format:
    
    ;; \input texinfo    @c -*-para-*-
    ;; @setfilename INFO-FILENAME
    ;; @settitle TITLE
    ;; 
    ;; @node top, 
    ;; 
    ;; @node NODE-NAME
    ;; @chapter CHAPTER-NAME
    ;; ...
    ;; @bye

    (insert "\\input texinfo    @c -*-para-*-\n")
    (insert (format "@setfilename %s\n" info-filename))
    (insert (format "@settitle %s\n\n"  title))
    (insert "@node top, \n\n")
    (insert (format "@node %s,\n" node-name))
    (insert (format "@chapter %s\n\n\n\n" chapter-name))
    (insert "@bye\n")
    (para-mode)
    (forward-line -3)
    (widen)))

(defun para-quit ()
  "Exit para mode"
  (interactive)
  (widen)
  (texinfo-mode))


;;; Add a new node
;;;    C-c C-p a       para-add-node

(defun para-add-node (new-node-type name title)
  "Create new node and section headers.

In interactive use, 

    1. No prefix argument: section type defaults to the same type as
       the preceding node.

    2. With prefix arg, obtain a prompt for the section type.  A
       default is offered, which one level lower than the preceding
       node (unless the previous node is already at the lowest level).

    You are prompted for the node name and section title.

If the new node is 

    * at the same or lower hierarchical level than the preceding node,
      the function inserts new headers at point;

    * at a higher hierarchical level than the preceding node,
      the function inserts new headers just before next node of 
      the new level, or at the end of the buffer.

The function updates the appropriate node lines and the associated
menu, but not the main menu (unless the new node is a chapter).

In non-interactive use, the first argument is the TYPE or level of the
new node; the second argument is the the node NAME and the and third
argument is the section TITLE."
  (interactive
     (if (not current-prefix-arg) 
         (let ((preceding-node-type (para-preceding-node-type)))
           ;; top node case
           (if (string= preceding-node-type "top")
               (setq preceding-node-type "chapter"))
           (list preceding-node-type
                 (read-string
                  (format "Node name for `%s' (end with RET): "
                              (para-preceding-node-type)))
                 (read-string "Title (end with RET): ")))
       ;; Ask what new node level should be; this becomes the 
       ;; value of new-node-type even though it starts out as
       ;; the prefix arg
       (list
        (read-no-blanks-input
         "New node level: "
         (cdr (assoc (para-preceding-node-type) para-subtype-alist)))
        (read-string "Node name (end with RET): ")
        (read-string "Title (end with RET): "))))

  (let* ((preceding-node-type (para-preceding-node-type))
         (new-node-general-type
          (cdr (assoc preceding-node-type texinfo-section-to-generic-alist))))
    ;; top node case
    (if (string= preceding-node-type "top")
        (setq preceding-node-type "chapter"
              new-node-general-type "chapter"))

    ;; Adding new nodes has three cases:
    ;;
    ;;     1. New node is at same level as preceding node:
    ;;        insert it at point.
    ;;
    ;;     2. New node is at lower level than preceding node:
    ;;        also, insert it at point.
    ;;
    ;;     3. New node is at higher level than preceding node:
    ;;        insert it at end of the preceding node of the same level
    ;;        after any intervening nodes; i.e. put it just before the
    ;;        next node of that level.

    (if (not new-node-type)
        ;; 1. most common case; insert same level node at point
        (insert
         (format "\n@node %s,\n@%s %s\n" name preceding-node-type title))

      ;; Else insert a node at a different level:

      (setq new-node-general-type      
            (cdr
             (assoc
              new-node-type texinfo-section-to-generic-alist)))

      ;; 2. If the node is at a same or lower level, insert at point;
      ;; but if the node is at a higher level, insert it just before
      ;; next node of that level.

      (if (node-same-or-lower-level-p new-node-type preceding-node-type)
          (insert (format "\n@node %s,\n@%s %s\n" name new-node-type title))

        ;; 3. Else the new node level is higher then the preceding node.
        ;; Find next node of that level:
        (if (re-search-forward
             (concat
              "\\(^@node\\).*\n"        ; match node line
              "\\(\\(^@c\\|^@if\\).*\n\\)?"    ; match comment or
                                               ; ifinfo line, if any
              (eval
               (cdr                     ; return regexp for search
                (assoc
                  new-node-general-type
                 texinfo-update-menu-same-level-regexps))))
             nil
             'goto-end)
            (goto-char (match-beginning 0))
          (point-max))
        (insert
         (format "\n@node %s,\n@%s %s\n\n" name new-node-type title))
        (forward-line -1)))

    ;; Finally, update relevant pointers and menu
    (para-insert-update-pointers-&-menu new-node-general-type)
    (message "Added new node: name, `%s'; title, `%s' ... Done."
             name title)))


;;; Utility functions for `para-add-node'

(defun para-preceding-node-type ()
  "Return type of preceding node as string, eg \"section\" or \"chapter\"."
    (save-excursion
      (end-of-line)
      (if (re-search-backward "^@node" nil t)
          (if (looking-at "^@node [ \t]*top")
              ;; top node case: return top
              "top"
            ;;else
            (if (re-search-forward
                 (concat
                  "\\(^@node\\).*\n"            ; match node line
              "\\(\\(^@c\\|^@if\\).*\n\\)?"     ; match comment or
                                                ; ifinfo line, if any
                  "\\(^@\\).*")                 ; match title line 
                 nil 
                 t)
                (buffer-substring
                 (progn (beginning-of-line) ; copy its name
                        (1+ (point)))
                 (progn (forward-word 1)
                        (point)))))
        ;; else no node line found
        (error "No preceding node!"))))

(defun node-same-or-lower-level-p (new-node-type preceding-node-type)
  "t if NEW-NODE-TYPE at same or lower level as PRECEDING-NODE-TYPE."
  (let ((new-node-general-type
         (cdr (assoc new-node-type texinfo-section-to-generic-alist)))
        (node-sequence
         (assoc        ; returns list of same and lower types
          (cdr (assoc         
                preceding-node-type
                texinfo-section-to-generic-alist))
          para-matching-subtypes-alist)))
    (node-in-sequence-p new-node-general-type node-sequence)))

(defun node-in-sequence-p (node node-sequence)
  "Return t if NODE is at same or lower level in NODE-SEQUENCE.
The node sequence is a list of strings of the general type of node,
such as `subsection' or `unnumbered'."
  (if node-sequence
      (if (string= node
                   (car node-sequence))
          t
        (node-in-sequence-p node (cdr node-sequence)))))


;;; `para-add-node' utility; goto node of specified general type

(defun para-goto-node-of-type (type direction)
  "Move point to node of specified general TYPE in specified DIRECTION.
Return nil if specified node not found, otherwise, move point."
  (widen)
  (let ((search-function
         (cond ((eq direction 'next) 're-search-forward)
               ((eq direction 'previous) 're-search-backward))))
    (if (string= "chapter" type)
        (if
            (funcall search-function
                     (concat
                      "\\("
                      "\\(^@node [ \t]*top\\)"    ; top node case
                      "\\|"                       ; or
                      "\\(^@node\\).*\n"          ; match node line
                      "\\(\\(^@c\\|^@if\\).*\n\\)?"      ; match comment
                                                  ; or ifinfo line, if any
                      "\\(^@"                     ;match chapter line
                      texinfo-chapter-level-regexp 
                      "\\)"                 
                      "\\)")
                     nil
                     t)
            (goto-char (match-beginning 0))
          ;; else node not found
          nil)
      ;;else not a chapter
      (if (funcall search-function
                   (concat
                    "\\(^@node\\).*\n"     ; match node line
                    "\\(\\(^@c\\|^@if\\).*\n\\)?" ; match comment 
                                           ; or ifinfo line, if any
                    (eval
                     (cdr
                      (assoc type texinfo-update-menu-same-level-regexps))))
                   nil
                   t)
          (goto-char (match-beginning 0))
        ;; else node not found
        nil))))


;;; `para-add-node' utility; insert and update pointers and menu 

(defun para-insert-update-pointers-&-menu (new-node-general-type)
  "Insert pointers of an added node, update other pointers and menu.
Specifically, insert the Next, Previous, and Up pointers of the current
node, update the pointers of the Next and Previous nodes, and update
the menu for the added node."
  (let ((auto-fill-hook nil))
    (save-excursion
    ;; First, update current node:
    (if (not (re-search-backward "^@node" (point-min) t))
        (error "Node line not found before this position."))
    (texinfo-update-the-node)
    ;; Second, update previous node:
    (save-excursion
      (if (para-goto-node-of-type new-node-general-type 'previous)
          (texinfo-update-the-node)))
    ;; Third, update next node:
    (save-excursion
      (if (para-goto-node-of-type new-node-general-type 'next)
          (texinfo-update-the-node)))
    ;; Finally, update the menu for this node
    (save-excursion
      (texinfo-make-one-menu new-node-general-type)))))


;;; Add new nodes based on menu:  `para-menu-explode'

(defun para-menu-explode ()
  "Based on menu, create new node and section headers at end of current node.
Each node name is extracted from a menu entry, and the corresponding
section title is the same as the descriptive string in that menu
entry."
  (interactive)
  (if (re-search-backward "^@node" (point-min) t)
      (let* ((up-node (texinfo-copy-node-name))
             (new-node-type
              (cdr (assoc (para-preceding-node-type) para-subtype-alist)))
             (new-node-general-type
              (cdr (assoc new-node-type
                          texinfo-section-to-generic-alist)))

             menu-end menu next-node previous-node)
        
        (if (string= new-node-general-type "subsubsection")
            (error "This node is too deep to create subnodes."))
        
        (if (search-forward "\n@menu" nil t)
            (save-excursion
              (if (search-forward "\n@end menu" nil t)
                  (setq menu-end (point))
                (error "No \"@end menu\" found!")))
          (error "No menu found!"))
        ;; create list of node names and titles
        (while (not (looking-at "^@end menu"))
          (setq menu
                (cons
                 (cons
                  (para-extract-menu-node-name)
                  (para-main-menu-buffer-copy-description menu-end))
                 menu))
          (forward-line 1))
        
        (setq menu (reverse menu)
              next-node (append (cdr menu) (list (cons "" "")))
              previous-node (append (list (cons "" "")) menu))
        
        (if (re-search-forward "^@node\\|^@bye" (point-max) 'move)
            (forward-line -1))
        
        (while menu
          
          (insert
           (format
            "\n@node %s, %s, %s, %s\n@%s %s\n"
            (car (car menu)) (car (car next-node))
            (car (car previous-node)) up-node
            new-node-type (cdr (car menu))))
          
          (setq menu (cdr menu) 
                next-node (cdr next-node)
                previous-node (cdr previous-node)))
        
        (recenter -1))
    (error "Node line not found before this position.")))


;;; Insert a Texinfo cross reference
;;; C-c C-p r       para-xref

(defun para-xref (insert-topic-p nodename description)
  "Insert a Texinfo cross reference.

The first argument, if non-nil, means incorporate topic description in
reference.  In interactive use, this argument is optional prefix
argument.  The second argument is the NODENAME for the reference, the
third argument is the topic DESCRIPTION.  

If `para-main-menu-completions-list' is non-nil, completion for node
names in the list. \(No default offered---you have to type the
beginning of the node name.  Type \\[para-directory] to generate a
`para-main-menu-completions-list' if it does not exist.\)"
  
  (interactive
   (let* ((completion-ignore-case t)
          (insert-topic-p  current-prefix-arg)
          (node-name
           (if (get-buffer para-main-menu-buffer-name)
               (save-excursion
                 (set-buffer para-main-menu-buffer-name)
                 (completing-read
                  "Node name (with completion): "
                  para-main-menu-completions-list))
             (read-string "Node name: ")))

          (description
           (if insert-topic-p  
               (read-string "Topic description (end with RET): "
                        (if (get-buffer para-main-menu-buffer-name)
                            (save-excursion
                              (set-buffer para-main-menu-buffer-name)
                              (goto-char (point-min))
                              (if (re-search-forward
                                   (regexp-quote node-name)
                                   nil t)
                                  (progn
                                    (beginning-of-line)
                                    (search-forward ":")
                                    (para-main-menu-buffer-copy-description
                                     (point-max)))
                                ""))
                          ""))
             "")))
     (list  insert-topic-p node-name description)))
  (if (string= "" description)
      (insert (format "@xref{%s}." nodename))
    (insert (format "@xref{%s, , %s}." nodename  description))))


;;; Chapter, section, etc. types alists
(defvar para-matching-subtypes-alist
  '(("top" . ("chapter" "section" "subsection" "subsubsection"))

    ("chapter" . ("section" "subsection" "subsubsection"))
    
    ("section" . ("subsection" "subsubsection"))
    
    ("subsection" . ("subsubsection"))
    
    ("subsubsection" . nil))
  "*Alist matchining each general section type with a list of its same
  and sub types.")

(defvar para-subtype-alist
  '(("top" . "chapter")

    ("chapter" . "section")
    ("unnumbered" . "unnumberedsec")
    ("majorheading" . "heading")
    ("chapheading" . "heading")
    ("appendix" . "appendixsec")
    
    ("section" . "subsection")
    ("unnumberedsec" . "unnumberedsubsec")
    ("heading" . "subheading")
    ("appendixsec" . "appendixsubsec")
    
    ("subsection" . "subsubsection")
    ("unnumberedsubsec" . "unnumberedsubsubsec")
    ("subheading" . "subsubheading")
    ("appendixsubsec" . "appendixsubsubsec")
    
    ("subsubsection" . "subsubsection")
    ("unnumberedsubsubsec" . "unnumberedsubsubsec")
    ("subsubheading" . "subsubheading")
    ("appendixsubsubsec" . "appendixsubsubsec"))
  "*Alist of section types and their subtypes.")


;;; Narrow to node

;;;    C-c C-p n       para-narrow-to-node

(defun para-narrow-to-node ()
  "Narrow to current node."
  (interactive)
  (let ((current-position (point))
        end-of-node-name
        beginning-of-node)
    (widen)
    (beginning-of-line)
    (if (or (looking-at "^@node")
            (re-search-backward "^@node"  nil 'move-to-limit))
        ;; point is within a node
        (progn
          (re-search-forward "@node[ \t]*\\<\\([^,]*\\),")
          (setq end-of-node-name
                (point)
                para-current-node
                (buffer-substring (match-beginning 1) (match-end 1)))
          (beginning-of-line)
          (while (looking-at "^@")
            (forward-line -1))
          (forward-line 1)
          (setq beginning-of-node (point))
          ;; skip past beginning node name
          (goto-char end-of-node-name))
      ;; else point is before first node
      (setq beginning-of-node 1
            para-current-node  "Beginning of Buffer"))
    (para-set-mode-line)
    ;; search for next node
    (if (search-forward "\n@node" nil 'move-to-limit)
        (progn
          (beginning-of-line)
          (while (looking-at "^@")
            (forward-line -1))))
    (narrow-to-region beginning-of-node (point))
    (goto-char current-position)))


;;; Goto a node.

(defun para-goto-node (name)
  "Go to NAME, which may be `nodename' or `(filename)nodename'.
This is an interface to the function `para-goto'."
  (interactive "sGoto: ")
  (if (not (para-goto name)
           (error "Node not found: %s " name))))

(defun para-goto (name)
  "Go to NAME, which may be `nodename' or `(filename)nodename'.
Use `para-goto-node' interactively."
  
  ;; If the string passed as `name' starts with a left parenthesis,
  ;; then the text within parentheses is the name of a file
  
  (let (filename nodename)
    (if (string-equal (substring name 0 1) (char-to-string ?\())
        ;; then name starts with filename
        (setq filename
              (substring name 1 (string-match (char-to-string ?\)) name))
              nodename
              (substring name (1+ (string-match (char-to-string ?\))  name))))
      ;; else name is nodename only
      (setq filename nil
            nodename name))
    
    (cond 
           ;; if file name is `dir', go to Info directory
           ((and filename
                (string= (downcase filename) "dir")) (Info-directory))

           (filename                       ; i.e., if there is a filename
            (if (string-equal nodename "") ; go to file, which will
                                           ; enter para mode if it is
                                           ; that type
                (switch-to-buffer (find-file-noselect filename))
              ;; else go to a  node in the file \(must be a Para mode file\)
              (find-file filename)
              (widen) 
              (goto-char (point-min))
              (para-goto nodename)))
           
           ((string-equal nodename "*")
            (para-beginning)
            (setq para-current-node nodename)
            (setq para-history (cons para-current-node para-history))
            (para-set-mode-line))
          
           ;; else must be a node to go to
           (t
            (let ((current-position (point)))
              (widen)
              (goto-char (point-max))
              ;; if both a tag table and find node, goto it
              (if (and
                   (search-backward "End tag table\n@end ignore" nil t)
                   (para-tag-table-find))
                  ;; then
                  (progn
                    (para-narrow-to-node)
                    ;; para-narrow-to-node specifies value of para-current-node
                    (setq para-history (cons para-current-node para-history))
                    (para-set-mode-line)
                    t)
                ;; else either no tag table or node not in tag table
                (if (re-search-backward
                     (concat "^@node [ \t]*" nodename) nil t)
                    ;; then
                    (progn
                      (para-narrow-to-node)
                      (setq para-history (cons para-current-node para-history))
                      (para-set-mode-line)
                      t)
                  ;; else could not find new node, return to current position
                  (goto-char current-position)
                  (para-narrow-to-node)
                  nil)))))))

(defun para-tag-table-find ()
  "Use a tag table to assist in going to a node quickly.
Tag table must be at the end of a Para mode file in an @ignore.
Point must be closer to the end than the beginning of the @ignore."
  (and (search-backward "@ignore\nTag table:" nil t)
       (search-forward (concat "Node: " nodename "\177") (point-max) t)
       (para-tag-search nodename (read (current-buffer)) 500)))

(defun para-tag-search (node point chunk)
  "Search for NODENAME from POINT, forward and back in CHUNK-sized spans."
  (let ((forward-search-position point)
	(backward-search-position point)
	(target-node (concat "^@node[ \t]+" (regexp-quote node) "[ \t]*,"))
        (max-limit (point-max))
	forward-limit backward-limit found)
    
    (while (not found)
      (setq forward-limit
            (min max-limit (+ forward-search-position chunk))
	    backward-limit
            (max 1 (- backward-search-position chunk)))
      (goto-char forward-search-position)
      
      (if (and (< forward-search-position max-limit)
               (re-search-forward target-node forward-limit t))
          (progn
            (beginning-of-line)
            (setq found (point))) ; and exit while loop
        
	;;else
        (goto-char backward-search-position)
	(if (and (< 1 backward-search-position)
                 (re-search-backward target-node backward-limit t))
            (setq found (point)) ; and exit while loop
          
          ;; else
	  (if (and (= forward-search-position max-limit)
                   (= backward-search-position (point-min)))
	      (setq found 'at-limits) ; and exit while loop
            ;; else repeat while loop 
	    (setq forward-search-position forward-limit
                  backward-search-position backward-limit)))))
    
    (if (numberp found)
	(goto-char found)
      nil)))


;;; Tagify a Para mode file

(defun para-tagify ()
  "Create or update a Para mode tag table in the current buffer."
  (interactive)
  ;; Save and restore point and restrictions.
  ;; `save-restriction' would not work
  ;; because it records the old max relative to the end.
  ;; Record it relative to the beginning.
  (let ((old-minimum (point-min))
	(old-maximum (point-max))
	(no-max (= (point-max) (1+ (buffer-size))))
	(old-point (point)))
    (unwind-protect
	(progn
	  (widen)
	  (goto-char 1)
	  (let ((case-fold-search t)
		list)
	    (while (search-forward "\n@node" nil t)
	      (forward-line -1)
	      (let ((beg (point)))
		(forward-line 2)
		(if (re-search-backward "^@node[ \t]+\\([^,]+\\)," beg t)
                    (setq list
			  (cons (list (para-clean-name
				       (buffer-substring
					(match-beginning 1)
					(match-end 1))
				       " \t")
				      beg)
				list)))))
	    (goto-char (point-max))
	    (let ((buffer-read-only nil))
	      (if (not (search-backward "\n@bye" nil t))
                  (error "This file requires an `@bye'!")
                ;; else removing existing tag table if there is one
                (if (search-forward "\nEnd tag table\n@end ignore\n" nil t)
		  (let ((end (point)))
		    (search-backward "\nTag table:\n")
		    (beginning-of-line)
		    (delete-region (point) end))))
	      (goto-char (point-max))
	      (insert "\n\n@ignore\nTag table:\n")
	      (setq list (nreverse list))
	      (while list
		(insert "Node: " (car (car list)) ?\177)
		(princ (car (cdr (car list))) (current-buffer))
		(insert ?\n)
		(setq list (cdr list)))
		(insert "\nEnd tag table\n@end ignore\n")))))
      (goto-char old-point)
      (narrow-to-region old-minimum (if no-max (1+ (buffer-size))
			     (min old-maximum (point-max))))))

(defun para-clean-name (string chars-to-remove)
  "Remove leading/trailing characters from STRING that match CHARS-TO-REMOVE."
  (if (string= chars-to-remove "") string
    ;; else
    (let* ((i 0)
	   (regex-to-remove (concat "[" chars-to-remove "]"))
	   (dont-remove (concat "[^" chars-to-remove "]"))
           ;; delete leading chars-to-remove
	   (string (substring string (or (string-match dont-remove string) 0)))
	   (l (length string)))
      (while (and (< i l) (string-match regex-to-remove (substring string -1)))
	(setq i (1+ i) string (substring string 0 -1)))
      string)))


;;; Insert a menu.
;;;    C-c C-p i       para-insert-menu

(defun para-insert-menu () 
  "Insert a Texinfo menu template and position point within it."
  (interactive)
  (insert "@menu\n* ::\t\t\n@end menu\n")
  (forward-line -2)
  (forward-char 2))


;;; Make a Master Menu

;;; C-c C-p m       para-make-master-menu

;;; `para-master-menu' is a rewrite of texinfo-master-menu and,
;;; perhaps, should replace that function.  What is new is that
;;; para-master-menu removes a pre-existing detailed part of the
;;; master menu---but if the pre-existing master menu uses a
;;; non-standard `texinfo-master-menu-header', this removal fails.
;;; Also, para-master-menu removes extra blank lines that the menu
;;; insert functions insert (and should insert, lest there be no blank
;;; line separates when you make the first master menu).

;;; Replace following with texinfo-master-menu?  Yes, but also provide
;;; an indented master menu option.

;;; Currently, `texinfo-master-menu' works like this:
;;;  "Make a master menu for a whole Texinfo file.
;;;Non-nil argument (prefix, if interactive) means first update all
;;;existing nodes and menus.  Remove pre-existing master menu, if there is one.

(defun para-make-master-menu
  (&optional update-all-nodes-menus-p para-indented-master-menu-p)
  "Make a master menu for a whole Para mode file.

In interactive use:

    1. With no prefix arg, make a regular master menu, first removing
       detailed part of pre-existing master menu, if there is one.

    2. With prefix arg, first update all existing nodes and menus
       \(incorporating descriptions from pre-existing menus\), first
       removing detailed part of pre-existing master menu, if there is
       one.

    3. With numeric prefix arg, insert an indented master menu.  All
       the nodes are in sequence.

    4. With negative numeric prefix arg, first update all existing
       nodes and menus \(incorporating descriptions from pre-existing
       menus\).  First remove detailed part of pre-existing master
       menu, if there is one, then insert an indented master menu.

When called from a program, non-nil optional first arg means first
update all existing nodes and menus \(incorporating descriptions from
pre-existing menus\); non-nil optional second arg means insert an
indented master menu in which all the nodes are in sequence.

This function creates a menu that follows the top node.  It replaces
any existing menu there, first removing the detailed part of an
already existing master.  \(This action depends on the pre-exisitng
master menu using the standard `texinfo-master-menu-header'.\)

A regular master menu has the following format, which is adapted from the
recommendation in the Texinfo Manual:

   * The first part contains the major nodes in the Texinfo file: the
     nodes for the chapters, chapter-like sections, and the major
     appendices.  This includes the indices, so long as they are in
     chapter-like sections, such as unnumbered sections.

   * The second and subsequent parts contain a listing of the other,
     lower level menus, in order.  This way, an inquirer can go
     directly to a particular node if he or she is searching for
     specific information.

Each of the menus in the detailed node listing is introduced by the
title of the section containing the menu.

An indented master menu consists of all the menu entries in the
buffer, in sequence, with lower level menus indented according to
their hierarchical level."
  (interactive
   (cond ((not current-prefix-arg) (list nil nil))
         ((listp current-prefix-arg) (list t nil))
         ((> (prefix-numeric-value current-prefix-arg) 0) (list nil t))
         ((< (prefix-numeric-value current-prefix-arg) 0) (list t t))))
  (widen)
  (goto-char (point-min))
  
  ;; Move point to location after `top'.
  (if (not (re-search-forward "^@node [ \t]*top[ \t]*\\(,\\|$\\)" nil t))
      (error "This buffer needs a Top node!"))
  
  (let ((first-chapter                  
         (save-excursion (re-search-forward "^@node") (point))))
    (if (re-search-forward texinfo-master-menu-header first-chapter t)
        ;; Remove detailed master menu listing
        (progn
          (goto-char (match-beginning 0))
          (let ((end-of-detailed-menu-descriptions
                 (save-excursion     ; beginning of end menu line
                   (goto-char (texinfo-menu-end))
                   (beginning-of-line) (forward-char -1)
                   (point))))
            (delete-region (point) end-of-detailed-menu-descriptions)))))
  
  (if update-all-nodes-menus-p
      (progn
        (message "Making a master menu...first updating all nodes... ")
        (sleep-for 2)
        (mark-whole-buffer)
        (texinfo-update-node t)
        
        (message "Updating all menus... ")        
        (sleep-for 2)
        (mark-whole-buffer)
        (texinfo-make-menu t)))
  
  (message "Now making the master menu... ")
  (sleep-for 2)
  (goto-char (point-min))
  
  (if para-indented-master-menu-p
      (progn
        (para-insert-indented-master-menu)
        (message
         "Done...completed making indented master menu.  You may save the buffer."))
    ;; else
    (texinfo-insert-master-menu-list
     (texinfo-master-menu-list))
    
    ;; Remove extra newlines that texinfo-insert-master-menu-list
    ;; may have inserted.
    
    (save-excursion
      (goto-char (point-min))
      
      (re-search-forward texinfo-master-menu-header)
      (goto-char (match-beginning 0))
      (insert "\n")
      (delete-blank-lines)
      
      (re-search-backward "^@menu")
      (forward-line -1)
      (delete-blank-lines)
      
      (re-search-forward "^@end menu")
      (forward-line 1)
      (delete-blank-lines)
      
      (message 
       "Done...completed making master menu.  You may save the buffer."))))


;;; Find node in current buffer or other file with `para-menu'.
;;; C-c C-p C-m     para-menu
 
; The format must be one of following:

    ; * nodename::                   description
    ; * item:nodename.               description
    ; * item:(filename).             description
    ; * item:(filename)nodename.     description

; (The `description' is optional.)
; 
; The file name must either be to a file in the current directory or a
; full, absolute pathname.
; 
; To go to the item, position the cursor at the beginning of the line
; for the item and type `C-c C-p C-m' and then type a RET to confirm
; the default item.

(defun para-menu (menu-item)
  "Go to node for menu item named (or abbreviated) NAME.
In interactive use, default is node under cursor. 
Completion for any name in this menu."
  (interactive
   (list
    (let (completions
	  (p (point))
          (default
            (save-excursion
              (beginning-of-line)
              (if (re-search-forward "\\* \\([^:\t\n]*\\):" nil t)
                  (buffer-substring (match-beginning 1) (match-end 1))
                (prog1 nil (message "No default menu item found."))))))
      (save-excursion
	(goto-char (point-min))
	(if (not (search-forward "\n@menu" nil t))
	    (error "No menu in this node.")
          ;; create alist for completing-read
          (while (re-search-forward
                  "\\* \\([^:\t\n]*\\):" nil t)
            ;; make a list,
            ;; each of element of which is \("name" . position\)
            (setq completions (cons (cons (buffer-substring
                                           (match-beginning 1)
                                           (match-end 1))
                                          (match-beginning 1))
                                    completions)))
          (let (item)
            (setq item (let ((completion-ignore-case t))
                         (completing-read
                          (if default
                              (format "Menu item (default %s): " default)
                            "Menu item: ")
                          completions nil t)))
            (if (string= item "") default item)))))))
  (save-excursion
    (goto-char (point-min))
    (or (search-forward "\n@menu" nil t)
	(error "No menu in this node"))
    (para-goto (para-extract-menu-name menu-item))))

(defun para-extract-menu-name (menu-item)
  "Return `nodename' or `(filename)nodename' for MENU-ITEM.
Point must be on or before named menu-item."
  (beginning-of-line)
  (or (re-search-forward (concat "* " "\\(" menu-item "\\)") nil t)
      (error "No such item in menu"))
  (para-extract-menu-node-name))

(defun para-extract-menu-node-name ()
  "Return `nodename' or `(filename)nodename'
Point must be on or before menu line."

;;; Four circumstances:

;;; * nodename::                   description
;;; * item:nodename.               description
;;; * item:(filename).             description
;;; * item:(filename)nodename.     description

;;; If a nodename is the second part of a menu item, you may follow it
;;; with a period, comma, tab, or newline.

  (beginning-of-line)
  (or (re-search-forward (concat "* \\(\\w+\\)") nil t)
      (error "Cannot find a menu line."))
  (let ((start-of-name (match-beginning 1))
        node)
    (if (and
         (search-forward ":" nil t)
         (looking-at ":"))                            ; look at `::'
        (setq node
              (buffer-substring start-of-name (1- (point)))) ; return nodename
      ;;else
      (skip-chars-forward ": \t")
      (setq start-of-name (point))
      (if (search-forward "(" nil t)    ; skip over node name, if any
          (progn (goto-char (1- (point)))
                 (forward-sexp)))
      (re-search-forward "[.,\t\n]" nil t)
      ;; return either `nodename',`(filename)', or `(filename)nodename'
      (setq node (buffer-substring start-of-name (1- (point)))))
    node))


;;; Goto the first, second, third node in a menu.
;;;    C-c C-p 1       para-menu-1      go to the node of the first menu 
;;;                                        item, etc., up to `9'

;;; Go to first, second, third etc. menu item

(defun para-menu-1 ()
  "Go to the node of the first menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 1)))
      (error "Node not found.")))

(defun para-menu-2 ()
  "Go to the node of the second menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 2)))
      (error "Node not found.")))

(defun para-menu-3 ()
  "Go to the node of the third menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 3)))
      (error "Node not found.")))

(defun para-menu-4 ()
  "Go to the node of the fourth menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 4)))
      (error "Node not found.")))

(defun para-menu-5 ()
  "Go to the node of the fifth menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 5)))
      (error "Node not found.")))

(defun para-menu-6 ()
  "Go to the node of the sixth menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 6)))
      (error "Node not found.")))

(defun para-menu-7 ()
  "Go to the node of the seventh menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 7)))
      (error "Node not found.")))

(defun para-menu-8 ()
  "Go to the node of the eighth menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 8)))
      (error "Node not found.")))

(defun para-menu-9 ()
  "Go to the node of the ninth menu item."
  (interactive)
  (if (not (para-goto (para-menu-count 9)))
      (error "Node not found.")))
 
(defun para-menu-1x (count)
  "Go to the prompted for menu item number."
  (interactive "nMenu item #: ")
  (if (not (para-goto (para-menu-count count)))
      (error "Node not found.")))

(defun para-menu-count (count)
  (let (name)
    (save-excursion
      (goto-char (point-min))
      (or (search-forward "\n@menu" nil t)
          (error "No menu in this node"))
      (or  (re-search-forward "\n[ ]*\\* \\([^:\t\n]*\\):" nil t count)
           (error "Too few items in menu"))
      (beginning-of-line)
      (if (setq name (para-extract-menu-name 
                      (buffer-substring (match-beginning 1) (match-end 1))))
          name
        (error "Node not found.")))))


;;; Display the first menu in a temporary buffer using `para-directory'
;;; C-c C-p d     para-directory


(defvar para-main-menu-buffer-name ""
  "Temporary buffer used by `para-directory' for main menu.
Also, used by `para-xref' and `para-visited-xref' for cross reference
completion.")

(defun para-directory ()
  "Display the first menu in a temporary buffer.
Usually, the first menu is the master menu.

In this buffer,
\\<para-main-menu-buffer-mode-map>

`para-main-menu-goto-node' is bound to \\[para-main-menu-goto-node]

which will jump you to the prompted for node in the Para Mode file.
Default is the name of the node under the cursor.  Completion offered.

Also,

`para-main-menu-buffer-xref'  is bound to \\[para-main-menu-buffer-xref]

which inserts a Texinfo cross reference in the associated Para mode
file at the current position of point in the Para mode file.  Default
is the name of the node under the cursor.  With optional argument
\(prefix if interactive\), Emacs prompts you for a topic description.
Completion is offered, but only for node names listed in this
temporary buffer."
  
  (interactive)
  (setq para-current-buffer (current-buffer))
  (setq para-main-menu-buffer-name
        (concat para-main-menu-prefix (buffer-name) " "))
  (let (this-node
        (this-line 0))
    (save-excursion
      (save-restriction
        (widen)
        ;; Record current node name for later positioning in buffer.
        (end-of-line)
        (if (re-search-backward "^@node" nil t)
            (setq this-node (texinfo-copy-node-name))
          (setq this-node nil))
        (goto-char (point-min))
        (if (not (re-search-forward "^@menu"))
            (error "Menu not found!"))
        (forward-line 1)
        (let ((menu
               (buffer-substring
                (point)
                (save-excursion 
                  (if (not (re-search-forward "^@end menu" nil t))
                      (error "End of menu not found!"))
                  (beginning-of-line)
                  (point)))))
          
          (with-output-to-temp-buffer para-main-menu-buffer-name
            (set-buffer para-main-menu-buffer-name)
            (kill-all-local-variables)
            (set-syntax-table para-mode-syntax-table)
            (use-local-map para-main-menu-buffer-mode-map)
            
            (setq mode-name "Para Menu"
                  major-mode 'para-main-menu-buffer-mode)
            (insert menu)
            (setq buffer-read-only t)
            ;; create alist for completing-read
            (goto-char (point-min))
            (while (re-search-forward
                    "\n[ ]*\\* \\([^:\t\n]*\\):" nil t)
              ;; make a list,
              ;; each of element of which is \("name" . position\)
              (setq para-main-menu-completions-list
                    (cons (cons (buffer-substring
                                 (match-beginning 1)
                                 (match-end 1))
                                (match-beginning 1))
                          para-main-menu-completions-list)))
            (goto-char (point-min))
            ))))
    (pop-to-buffer para-main-menu-buffer-name)
    (and this-node
         (re-search-forward this-node nil t) (beginning-of-line))))


;;; Utility functions for displaying first menu

;;; Includes commands to go to the node under the cursor and to insert
;;; a cross reference in the source file refering to the node under
;;; the cursor

(defvar para-main-menu-prefix "*Menu for: "
  "Prefix of name of temporary buffer displaying the main menu.")

(defvar para-main-menu-buffer-mode-map nil
  "Keymap used in temporary buffer displaying the main menu.")

;;; !!! remove this setq after testing
(setq para-main-menu-buffer-mode-map nil)

(if para-main-menu-buffer-mode-map
    nil
  (setq para-main-menu-buffer-mode-map (copy-keymap text-mode-map))
  (define-key para-main-menu-buffer-mode-map "n"     'next-line)
  (define-key para-main-menu-buffer-mode-map " "     'next-line)
  (define-key para-main-menu-buffer-mode-map "p"     'previous-line)
  (define-key para-main-menu-buffer-mode-map "\177"  'previous-line)
  (define-key para-main-menu-buffer-mode-map "q"     
    '(lambda () "Bury current buffer; switch to current Para mode buffer."
       (interactive)
       (bury-buffer (current-buffer))
       (switch-to-buffer para-current-buffer)))
  (define-key para-main-menu-buffer-mode-map "\C-c\C-p\C-m"
    'para-main-menu-goto-node)
  (define-key para-main-menu-buffer-mode-map "m" 
    'para-main-menu-goto-node)
  (define-key para-main-menu-buffer-mode-map "\C-c\C-pr"
    'para-main-menu-buffer-xref)
  (define-key para-main-menu-buffer-mode-map "r"
    'para-main-menu-buffer-xref))

(defvar para-main-menu-completions-list nil
"List used by `para-main-menu-goto-node' for completions.")


;;; Menu goto function for `para-directory'

;;; !!! check whether this handles menus of form:
;;;     * item:(filename)nodename.     description

(defun para-main-menu-goto-node (menu-item)
  "Go to the Para mode node prompted for; default is node under cursor.
Completion."
  (interactive
   (list
    (let ((default
            (save-excursion
              (beginning-of-line)
              (re-search-forward "\\* \\([^:\t\n]*\\):" nil t)
              (buffer-substring (match-beginning 1) (match-end 1))))
          item)
      (setq item (let ((completion-ignore-case t))
                   (completing-read (if default
                        (format "Node name (default %s): " default)
                      "Node name: ")
                    para-main-menu-completions-list nil t)))
      (if (string= item "") default item))))
  (let ((file-name (para-extract-menu-file-name))
        (node-name (para-extract-menu-name menu-item)))
    (pop-to-buffer (get-file-buffer file-name))
    (if (not (para-goto node-name))
        (error "Node not found: %s " node-name))))

(defun para-extract-menu-file-name ()
  "Return name of Para mode file for which this is the menu buffer."
  (let ((menu-file  (buffer-name)))
    (substring menu-file
             (progn (string-match para-main-menu-prefix menu-file)
                    (match-end 0)) -1)))


;;; `para-directory'  cross reference insertion

(defun para-main-menu-buffer-xref
  (reference &optional topic-description)
  "Insert cross reference in Texinfo file to name of node under cursor.

The function inserts a Texinfo cross reference in the associated Para
mode file at the current position of point in the Para mode file.
Default is the name of the node under the cursor.  Completion.

Optional second arg is topic description; with prefix arg if
interactive, prompt for topic description."
  
  (interactive
    (let ((default
            (save-excursion
             (para-extract-menu-name 
              (progn
               (beginning-of-line)
               (re-search-forward "\\* \\([^:\t\n]*\\):" nil t)
               (buffer-substring (match-beginning 1) (match-end 1))))))
          (description "")
          node-name)
      (setq node-name 
            (let ((completion-ignore-case t))
              (completing-read
               (if default
                   (format "Node name (default %s): " default)
                 "Node name: ")
               para-main-menu-completions-list nil t)))
      (if (string= node-name "") (setq node-name default))
      (if current-prefix-arg
          (setq description
                (read-string
                 "Topic description (end with RET): " 
                 (save-excursion
                   (goto-char (point-min))
                   (if (search-forward node-name nil t)
                       (para-main-menu-buffer-copy-description
                        (point-max)))))))
      (list node-name description)))
  (pop-to-buffer (get-file-buffer (para-extract-menu-file-name)))
  (if (string= "" topic-description)
      (insert (format "@xref{%s}." reference))
    (insert (format "@xref{%s, , %s}." reference topic-description))))

;;; merge with texinfo-menu-copy-old-description 
;;; this function handles the texinfo-master-menu-header
(defun para-main-menu-buffer-copy-description (end-of-menu)

  "Return description field of menu line as a string.
Point must be located on the same line before the description.  Point
left before description.  Single argument, END-OF-MENU, is position
limiting search."
  (skip-chars-forward "[:.,\t\n ]+")
;  (re-search-forward ":[^:.,\t\n]*[:.,\t\n]+")
  ;; don't copy a carriage return at line beginning with asterisk!
  (if (and (looking-at "\\w+")    
           (not (looking-at
                 (concat 
                  "\\("
                  "^[ \t]\\* "          ; beginning of menu line
                  "\\|"
                  "^@end menu"          ; end of menu
                  "\\|"
                  texinfo-master-menu-header ; header line
                  "\\)"))))
      (buffer-substring
       (point)
       (save-excursion
         (if (not (if (re-search-forward 
                       (concat 
                        "\\("                ; beginning of menu line
                        "^[ \t]*\\* "        ; works with master directory
                        "\\|"
                        "^@end menu"         ; end of menu
                        "\\|"                ; text at beginning of line
                        "^\\w+"              ;  is usually not part of desc.
                        "\\|"
                        texinfo-master-menu-header ; header line
                        "\\)")
                       end-of-menu t)
                      (goto-char (match-beginning 1))))
             (goto-char end-of-menu))
         (forward-line -1)
         (while (looking-at "[ \t]*$")
           (forward-line -1))
         (end-of-line)                  ; go to end of last description line
         (point)))
    ""))


;;; Utility functions to create indented menus

;;; used by `para-make-master-menu'

(defun para-insert-indented-master-menu ()
  "Insert an indented master menu in the current buffer."
  (re-search-forward "^@menu")
  (beginning-of-line)
  (let* ((buffer-original-position (point))
         (master-menu (para-list-directory-list-menus-indentedly)))
    (goto-char buffer-original-position)
    ;; Delete existing menu; buffer must have ordinary top menu
    (delete-region (point)  
                   (save-excursion (re-search-forward "^@end menu") (point)))
    ;; Handle top of menu
    (insert "@menu\n")
    (while  (car master-menu)
      (insert (format "%s" (car master-menu)))
      (setq master-menu (cdr master-menu)))
    ;; Finish menu
    (insert "@end menu")))

(defun para-list-directory-list-menus-indentedly ()
  "List node names and descriptions as indented menu entries.
Must start after first menu.  Returns list of menu entries."
  (let ((master-directory-list))
    (while (re-search-forward "^@node" nil t)
      (beginning-of-line)

      (let ((spaces (para-list-directory-indent-spaces))
            (this-node (texinfo-copy-node-name)))
        (message "Getting menu entry for `%s' ... " this-node)
        (save-excursion
          (if (re-search-backward (concat "\\* " this-node ":") nil t)
              (setq master-directory-list
                    (cons
                     (concat
                      (para-list-directory-indent-spaces)
                      (para-list-directory-menu-entry))
                     master-directory-list))
            ;; else no menu entry
            )))
      (end-of-line))
    (nreverse master-directory-list)))

(defun para-list-directory-indent-spaces ()
  "Return a string of spaces according to level of menu."
  (cdr (assoc
        (texinfo-hierarchic-level)
       ;; Perhaps this alist should be a variable rather than written in.
       '(("chapter" . "")
         ("section" . "  ")
         ("subsection" . "    ")
         ("subsubsection" . "      ")))))

(defun para-list-directory-menu-entry ()
  "Return this menu entry, including description, as string.
Start at beginning of line."
  (buffer-substring (point)
                    (save-excursion
                      (end-of-line)
                      (re-search-forward
                       (concat
                        "\\(\\* \\([^:\t\n]*\\):"
                        "\\|^@end menu"
                        "\\|"
                        texinfo-master-menu-header
                        "\\)")
                        nil
                        t)
                      (goto-char (match-beginning 0))
                      (beginning-of-line)
                      (point))))


;;; Display recently visited nodes in a temporary buffer

;;; History of visited nodes; easy cross reference making

;;; `para-list-visited-nodes' displays a list of the nodes you have
;;; visited in the Para mode file, most recent first.  The nodes are
;;; displayed in a temporary buffer.
;;;
;;; `para-goto-visited-node' jumps you to the prompted for node in the
;;; Para Mode file.  Default is the name of the node under the cursor.
;;; Completion."
;;;
;;; `para-visited-xref' inserts a cross reference in the current Para
;;; mode buffer at point in that buffer. The default node name is the
;;; name of the node under the cursor in the `*Recently Visited
;;; Nodes*' buffer.  Completion offered for other node names, but only
;;; for node names listed in the `*Recently Visited Nodes*' buffer
;;; This function makes it easier to insert cross references,
;;; expecially when you cannot readily remember the node names.
;;;
;;; With optional argument (prefix if interactive), Emacs prompts you
;;; for a topic description.

;;; Keybindings are:
;;;
;;; C-c C-p l       para-list-visited-nodes
;;;
;;; For bindings inside the temporary buffer created by
;;; `para-list-visited-nodes', use
;;;
;;; C-c C-p r       para-visited-xref       (based on `para-xref')
;;; C-c C-p C-m     para-goto-visited-node  (parallel to `para-menu')
;;;
;;;  (Note that the keybinding for `para-visited-xref' and
;;;  `para-goto-visited-node' are defined locally in the temporary
;;;  buffer, not in the para-mode-map.)

(defvar para-history nil
  "List of names of recently visited nodes.")

(defvar para-current-buffer nil
  "Current buffer in Para Mode.")

(defvar para-current-node nil
  "Current Para Mode node.")

(defvar para-list-visited-nodes-syntax-table nil)

(if para-list-visited-nodes-syntax-table nil
  (setq para-mode-syntax-table (make-syntax-table))
      (modify-syntax-entry ?\" "\"" para-list-visited-nodes-syntax-table))

(defvar para-list-visited-nodes-mode-map nil 
"Keymap used in node listing buffer in Para mode.")

;;; !!! remove this setq after testing
(setq para-list-visited-nodes-mode-map nil)

(if para-list-visited-nodes-mode-map
    nil
  (setq para-list-visited-nodes-mode-map (copy-keymap text-mode-map))
  (define-key para-list-visited-nodes-mode-map "n"     'next-line)
  (define-key para-list-visited-nodes-mode-map " "     'next-line)
  (define-key para-list-visited-nodes-mode-map "p"     'previous-line)
  (define-key para-list-visited-nodes-mode-map "\177"  'previous-line)
  (define-key para-list-visited-nodes-mode-map "q"     
    '(lambda () "Bury current buffer; switch to current Para mode buffer."
       (interactive)
       (bury-buffer (current-buffer))
       (switch-to-buffer para-current-buffer)))
  (define-key para-list-visited-nodes-mode-map "\C-c\C-p\C-m"
    'para-goto-visited-node)
  (define-key para-list-visited-nodes-mode-map "m" 
    'para-goto-visited-node)
  (define-key para-list-visited-nodes-mode-map "\C-c\C-pr"
    'para-visited-xref)
  (define-key para-list-visited-nodes-mode-map "r"
    'para-visited-xref))


;;; List recently visited nodes.
;;; C-c C-p l       para-list-visited-nodes

(defun para-list-visited-nodes ()
  "Display a list of recently visited nodes in a temporary buffer.

In this buffer,
\\<para-list-visited-nodes-mode-map>

`para-visited-xref' is bound to \\[para-visited-xref]

which inserts a Texinfo cross reference in the associated Para mode
file at the current position of point in the Para mode file.  Default
is the name of the node under the cursor.  Completion is offered, but
only for node names listed in this temporary buffer.

With optional argument (prefix if interactive), Emacs prompts you
for a topic description.

`para-goto-visited-node' is bound to \\[para-goto-visited-node] 

which will jump you to the prompted for node in the Para Mode file.
Default is the name of the node under the cursor.  Completion offered."

  (interactive)
  (setq para-current-buffer (current-buffer))

;;; para-history is set in para-last and para-goto

  (let ((visited-list para-history))
    (with-output-to-temp-buffer "*Recently Visited Nodes*"
      (pop-to-buffer "*Recently Visited Nodes*")
      (kill-all-local-variables)
      (use-local-map para-list-visited-nodes-mode-map)
      (setq mode-name ""
            major-mode 'para-list-visited-nodes-mode)
      (set-syntax-table para-mode-syntax-table)
      (while (car visited-list)
        (insert (format "\"%s\"\n"  (car visited-list)))
        (setq visited-list (cdr visited-list))))))

(defun para-goto-visited-node (node-name)
  "Go to the Para mode node prompted for; default is node under cursor.
Completion."
;;; Format of recently visited nodes buffer is the following:
;;; 
;;; "Create Files and Nodes"
;;; "Movement Commands"
;;; "Shortcut Commands"
;;; "Para Mode Commands"
  
  (interactive
   (let ((default 
           (save-excursion
             (beginning-of-line)
             (re-search-forward "^\"\\([^\"]+\\)\"$" nil t)
             (buffer-substring (match-beginning 1) (match-end 1))))
         (description "")
         completions)
     (save-excursion              ; create alist for completing-read
       (goto-char (point-min))
       (while (re-search-forward "^\"\\([^\"]+\\)\"$" nil t)
         ;; make a list each of element of which is \("name" . position\)
         (setq completions (cons (cons (buffer-substring
                                        (match-beginning 1)
                                        (match-end 1))
                                       (match-beginning 1))
                                 completions))))
     (setq node-name
           (let ((completion-ignore-case t))
             (completing-read
              (if default
                  (format "Node name (default %s): " default)
                "Node name: ")
              completions nil t)))
     (if (string= node-name "") (setq node-name default))
     (list node-name)))
  (pop-to-buffer para-current-buffer)
  (if (not (para-goto node-name))
      (error "Node not found: %s " node-name)))

(defun para-visited-xref   (reference &optional topic-description)
  "Insert cross reference to node cursor is on in Texinfo file.

The function inserts a Texinfo cross reference in the associated Para
mode file at the current position of point in the Para mode file.
Default is the name of the node under the cursor.  Completion for
other node names listed in the buffer (not to all node names in source
file).

Optional second arg is topic description; with prefix arg if
interactive, prompt for topic description.

If temporary main menu buffer exists, default topic description is
description line in the menu.  (Type \\[para-directory] to generate a
main menu buffer if it does not exist.\)"
  
;;; Format of recently visited nodes buffer is the following:
;;; 
;;; "Create Files and Nodes"
;;; "Movement Commands"
;;; "Shortcut Commands"
;;; "Para Mode Commands"
  
  (interactive
   (let ((default 
           (save-excursion
             (beginning-of-line)
             (re-search-forward "^\"\\([^\"]+\\)\"$" nil t)
             (buffer-substring (match-beginning 1) (match-end 1))))
         (description "")
         completions
         node-name)
     (save-excursion              ; create alist for completing-read
       (goto-char (point-min))
       (while (re-search-forward "^\"\\([^\"]+\\)\"$" nil t)
         ;; make a list each of element of which is \("name" . position\)
         (setq completions (cons (cons (buffer-substring
                                        (match-beginning 1)
                                        (match-end 1))
                                       (match-beginning 1))
                                 completions))))
     (setq node-name
           (let ((completion-ignore-case t))
             (completing-read
              (if default
                  (format "Node name (default %s): " default)
                "Node name: ")
              completions nil t)))
     (if (string= node-name "") (setq node-name default))
     (if current-prefix-arg
         (setq description
               (read-string
                "Topic description (end with RET): " 
                (if (get-buffer para-main-menu-buffer-name)
                    (save-excursion
                      (set-buffer para-main-menu-buffer-name)
                      (goto-char (point-min))
                      (if (search-forward node-name nil t)
                          (para-main-menu-buffer-copy-description
                           (point-max))))))))
     (list node-name description)))
  (pop-to-buffer para-current-buffer)
  (if (string= "" topic-description)
      (insert (format "@xref{%s}." reference))
    (insert (format "@xref{%s, , %s}." reference topic-description))))


;;; Movement commands with `C-c C-p' prefix: top, next, prev, etc.

;     C-c C-p C-b     para-beginning
;     C-c C-p C-<     para-top-node
;     C-c C-p C->     para-final

;     C-c C-p C-u     para-up
;     C-c C-p C-p     para-prev
;     C-c C-p C-n     para-next

;     C-c C-p C-[     para-goto-previous-node-in-sequence
;     C-c C-p C-]     para-goto-next-node-in-sequence

;     C-c C-p RET     para-menu
;     C-c C-p C-l     para-last
;     C-c C-p C-g     para-goto
;     C-c C-p C-f     para-follow

;;; Basic movement commands: top, next, prev, etc.

(defun para-beginning ()
  "Go to the very beginning of the file (before a `Top' node)."
  ;;; Does not handle include files.
  (interactive)
  (widen)
  (goto-char (point-min))
  (setq para-history
        (cons (save-excursion (para-extract-pointer 'next)) para-history)))

(defun para-top-node ()
  "Go to the `Top' node."
  (interactive)
  (if (not (para-goto "top"))
      (error "`Top' node not found.")))

(defun para-final ()
  "Go to the last node in the Para file."
  ;;; Does not handle include files.
  (interactive)
  (widen)
  (goto-char (point-max))
  (re-search-backward "^@node")
  (end-of-line)
  (para-narrow-to-node)
  (beginning-of-line)
  (setq para-history
        (cons (texinfo-copy-node-name)
              para-history)))

(defun para-up ()
  "Go to the superior node of this node."
  (interactive)
  (if (not (para-goto (para-extract-pointer 'up)))
      (error "`Up' node not found.")))

(defun para-prev (&optional arg)
  "Go to the previous node of this node.
With optional arg (prefix if interactive) go to the previous node in
sequence, regardless of its hierarcichal level or type."
  (interactive "P")
  (if arg
      (para-goto (para-previous-node-in-sequence))
    (if (not (para-goto (para-extract-pointer 'prev)))
        (error "`Previous' node not found."))))

(defun para-next (&optional arg)
  "Go to the next node of this node.
With optional arg (prefix if interactive) go to the next node in
sequence, regardless of its hierarcichal level or type."
  (interactive "P")
  (if arg
      (para-goto (para-next-node-in-sequence))
    (if (not (para-goto (para-extract-pointer 'next)))
      (error "`Next' node not found."))))

(defun para-last ()
  "Go back to the last node visited."
  (interactive)
  (or para-history
      (error "This is the first para node you looked at"))
  (let ((node-name (car (cdr para-history))))
    (if (para-goto node-name)
        (setq para-history (cdr (cdr para-history)))
      (error "Node not found: %s " node-name))))

(defun para-extract-pointer (next-prev-up)
  "Return name of   `Next', `Previous', or `Up' pointer as a string."
  ;; only argument `next-prev-up' is  'next, 'previous, or 'up
  (or (looking-at "^@node")
      (re-search-backward "^@node"  nil 'move-to-limit))
  (widen)
  (save-excursion
  ;; done this ugly way because of troubles with match-data when
  ;; trying to record all the matches for the next, prev and up
  ;; pointers and then using match-beginning and match-end to extract
  ;; desired name.
  (cond ((eq next-prev-up 'next) 
         (if (not (looking-at "^@node"))            ; not within a node
             (para-next-node-in-sequence)
           ;; else within a node
           ;; search for end of `next' nodename
           (if (re-search-forward (concat 
                                   "^@node"
                                   "[ \t]*[^,]*[ \t]*,"     
                                   "[ \t]*\\([^,]*\\)[ \t]*,") nil t)
               (buffer-substring
                (match-beginning 1)
                (progn (skip-chars-backward ", \t\n") (point)))
             ;; else
             (error "Next node not found!"))))
        
        ((eq next-prev-up 'prev) 
         ;; search for end of `previous' nodename
         (if (re-search-forward (concat 
                                 "^@node"
                                 "[ \t]*[^,]*[ \t]*,"               ; nodename
                                 "[ \t]*[^,]*[ \t]*,"               ; `Next' 
                                 "[ \t]*\\([^,]*\\)[ \t]*,") nil t) ; `Prev'
             (buffer-substring
              (match-beginning 1)
              (progn (skip-chars-backward ", \t\n") (point)))
           ;; else
           (error "Previous node not found!")))
        
        ((eq next-prev-up 'up) 
         ;; search for end of `up' nodename
         (if (re-search-forward (concat 
                                 "^@node"
                                 "[ \t]*[^,]*[ \t]*,"               ; nodename
                                 "[ \t]*[^,]*[ \t]*,"               ; `Next'
                                 "[ \t]*[^,]*[ \t]*,"               ; `Prev'
                                 "[ \t]*\\([^,\n]*\\)"              ; `Up'
                                 "[ \t]*$") nil t)
             (buffer-substring
              (match-beginning 1)
              (progn (skip-chars-backward ", \t\n") (point)))
           ;; else
           (error "Up node not found!"))))))

(defun para-goto-next-node-in-sequence ()
  "Go to the next node in buffer regardless of its hierarchical level.
This is an interface to the function `para-next-node-in-sequence'."
  (interactive)
  (para-goto (para-next-node-in-sequence)))

(defun para-goto-previous-node-in-sequence ()
  "Go to the previous node in buffer regardless of its hierarchical level.
This is an interface to the function `para-previous-node-in-sequence'."
  (interactive)
  (para-goto (para-previous-node-in-sequence)))

(defun para-follow (reference)
  "Follow REFERENCE to the node it refers to.
REFERENCE may include a filename or be an abbreviation of the reference name."
  (interactive
   (list
    (let ((default 
            (save-excursion
              (beginning-of-line)
              (if (re-search-forward  "@p?x?ref{[ \t]*\\([^,}]*\\)" nil t)
                  (buffer-substring (match-beginning 1) (match-end 1))
                (error "No cross-reference in this node"))))
          completions
          nodename
          filename
          current-point
          i)
      (save-excursion
        (goto-char (point-min))
        (while
            ;; search for cross reference
            (re-search-forward "@p?x?ref{[ \t]*\\([^,}]*\\)" nil t)
          (setq current-point (point))
          (setq nodename
                (buffer-substring
                 (match-beginning 1)
                 (progn (skip-chars-backward " \t\n") (point))))
          
          ;; remove extraneous characters from nodename string
          (while (setq i (string-match "[ \n\t]+" nodename i))
            (setq nodename (concat (substring nodename 0 i) " "
                                   (substring nodename (match-end 0))))
            (setq i (1+ i)))
          
          (skip-chars-forward " \t\n")
          (if (looking-at "}")        ; no filename entry
              (setq filename nil)
            ;; else not looking at a closing brace
            
            ;; possibilities:  is a two, three, four, or five entry xref
            ;; set up for search 
            (re-search-backward "@p?x?ref{" nil t)
            
            (cond (;; search for xref ending with second entry
                   (re-search-forward "@p?x?ref{[^,]*,[^,]*}" nil t)
                   (setq filename nil))
                  ;; search for xref ending with third entry
                  ((re-search-forward "@p?x?ref{[^,]*,[^,]*,[^,]*}" nil t)
                   (setq filename nil))
                  ;; search for xref ending with filename \(fourth entry\)
                  ((re-search-forward
                    "@p?x?ref{[^,]*,[^,]*,[^,]*,\\([^,]*\\)}" nil t)
                   (setq filename 
                         (buffer-substring
                          (progn
                            (goto-char (match-beginning 1))
                            (skip-chars-forward " \t\n")
                            (point))
                          (progn
                            (goto-char (match-end 1))
                            (skip-chars-forward " \t\n")
                            (point)))))
                  ;; else look for xref ending in manual name entry
                  ((re-search-forward
                    "@p?x?ref{[^,]*,[^,]*,[^,]*,\\([^,]*\\)\\(,[^,]+\\)}"
                    nil t)
                   (setq filename 
                         (buffer-substring
                          (progn
                            (goto-char (match-beginning 1))
                            (skip-chars-forward " \t\n")
                            (point))
                          (progn
                            (goto-char (match-end 1))
                            (skip-chars-forward " \t\n")
                            (point)))))
                  (t (setq filename nil))))
          (if (assoc nodename completions)
              nil
            (setq completions (cons (cons nodename filename) completions)))
          (goto-char current-point)))
      

      (cond (completions
             (let ((completion-ignore-case t))
               (setq reference
                     (completing-read
                      (if default
                          (format "Follow cross ref to (default %s): " default)
                        "Follow cross ref to: ") 
                      completions nil t)))
             (if (string= reference "") (setq reference default))
             
             (if (cdr (assoc reference completions))
                 (setq filename
                       (read-from-minibuffer
                        "Go to cross reference in file: "
                        (cdr (assoc reference completions))))))

            (t (error "No cross-reference in this node")))
      (and filename (setq filename (concat "(" filename ")")))
      (concat filename reference))))

  (para-goto reference))


;;; Regular Expression Search 

(defvar para-last-search nil
  "Default regexp for para-search command to search for.")

(defun para-search (regexp)
  "Search for REGEXP, starting from point, and select node it's found in."
  (interactive "sSearch (regexp): ")
  (if (equal regexp "")
      (setq regexp para-last-search)
    (setq para-last-search regexp))
  
  (let ((this-node para-current-node)
	found)
    (save-excursion
      (save-restriction
	(widen)
	(re-search-forward regexp)
	(setq found (point))))
    (widen)
    (goto-char found)
    (para-narrow-to-node)
    ;; para-narrow-to-node specifies value of para-current-node
    (or (equal this-node para-current-node)
        (setq para-history (cons para-current-node para-history)))
    (para-set-mode-line)))


;;; Name of next or previous node in strict sequence

(defun para-next-node-in-sequence ()
  "Return name of next node in buffer regardless of its hierarchical level."
  (interactive)
  (widen)
  (end-of-line) ; to skip past `@node' at beginning of node.
  (if (not (re-search-forward "^@node" nil t))
      (progn (beginning-of-line)
             (para-narrow-to-node)
             (error "Next node not found!"))
    (beginning-of-line)
    (let ((nodename (texinfo-copy-node-name)))
      (or nodename
          (error "No node name found for Next node!")))))

(defun para-previous-node-in-sequence ()
  "Go to the previous node in buffer regardless of its hierarchical level."
  (interactive)
  (para-narrow-to-node)  ; so back search is not stuck at current node
  (goto-char (point-min))
  (widen)
  (if (not (re-search-backward "^@node" nil t))
      (progn (beginning-of-line)
             (para-narrow-to-node)
             (error "Previous node not found!"))
    (let ((nodename (texinfo-copy-node-name)))
      (or nodename
          (error "No node name found for Previous node!")))))


;;; Single keystroke Info commands

(defun para-@ (key)
  "Provide Info-like commands when point in upper-left or buffer read-only.
In a writable buffer when point is not in the upper-left corner, keys 
self-insert."
  (interactive "p")
  (if (and (not buffer-read-only)
           (pos-visible-in-window-p (1- (point))))
      (progn
        (self-insert-command key)
        (if (<= fill-column (current-column))
            (if auto-fill-hook
                (funcall auto-fill-hook)
              nil)))
    
    (let* ((this-key (aref (this-command-keys) 0)))
      (call-interactively
       (cond ((= this-key ?g)    'para-goto-node)
             ((= this-key ?<)    'para-top-node)
             ((= this-key ?>)    'para-final)
             ((= this-key ?\[)   'para-goto-previous-node-in-sequence)
             ((= this-key ?\])   'para-goto-next-node-in-sequence)
             ((= this-key ? )    'para-scroll-up)
	     ((= this-key ?\^?)  'para-scroll-down)
             ((and (<= ?0 this-key) (<= this-key ?9))
              (cdr (assoc
                    this-key
                    (cdr (assoc 16  (cdr (assoc 3 para-mode-map)))))))
             (t (cdr (assoc
                      (- this-key ?`)
                      (cdr (assoc 16 (cdr (assoc 3 para-mode-map))))))))))))

(defun para-scroll-up (arg)
  "Scroll text of current window upward ARG lines.
Keep point in same relative position in window."
  (interactive "P")
  (let ((current-relative-position (- (window-point) (window-start))))
    (scroll-up arg)
    (goto-char (+ (window-start) current-relative-position))))

(defun para-scroll-down (arg)
  "Scroll text of current window downward ARG lines.
Keep point in same relative position in window."
  (interactive "P")
  (let ((current-relative-position (- (window-point) (window-start))))
    (scroll-down arg)
    (goto-char (+ (window-start) current-relative-position))))


;;; Index entry commands:  @cindex, @findex, etc.

;; Must redefine each index command to use `para-index' since
;; `texinfo-index' deletes the index line after reading it.
;; Hence the lack of shared code.

(put 'vindex 'para-format 'para-format-vindex)
(defun para-format-vindex () (para-index 'para-vindex))

(put 'cindex 'para-format 'para-format-cindex)
(defun para-format-cindex () (para-index 'para-cindex))

(put 'findex 'para-format 'para-format-findex)
(defun para-format-findex () (para-index 'para-findex))

(put 'pindex 'para-format 'para-format-pindex)
(defun para-format-pindex () (para-index 'para-pindex))

(put 'tindex 'para-format 'para-format-tindex)
(defun para-format-tindex () (para-index 'para-tindex))

(put 'kindex 'para-format 'para-format-kindex)
(defun para-format-kindex () (para-index 'para-kindex))

(defun para-index (indexvar)
  (let ((arg (texinfo-parse-expanded-arg)))
    (set indexvar
	 (cons (list arg (para-current-node))
	       (symbol-value indexvar)))))

(defconst para-indexvar-alist
  '(("cp" . para-cindex)
    ("fn" . para-findex)
    ("vr" . para-vindex)
    ("tp" . para-tindex)
    ("pg" . para-pindex)
    ("ky" . para-kindex)))


;;; Define indices:  @defindex   @defcodeindex

(put 'defindex 'para-format 'para-format-defindex)
(put 'defcodeindex 'para-format 'para-format-defindex)
(defun para-format-defindex ()
  (let* ((index-name (texinfo-parse-line-arg)) ; eg: `aa'
         (indexing-command (intern (concat index-name "index")))
         (index-formatting-command      ; eg: `para-format-aaindex'
          (intern (concat "para-format-" index-name "index")))
         (index-alist-name              ; eg: `para-aaindex'
          (intern (concat "para-" index-name "index"))))

    (set index-alist-name nil)

    (put indexing-command               ; eg, aaindex
         'para-format
         index-formatting-command)      ; eg, para-format-aaindex

    ;; eg: "aa" . para-aaindex
    (or (assoc index-name para-indexvar-alist)
        (setq para-indexvar-alist
              (cons
               (cons index-name
                     index-alist-name)
               para-indexvar-alist)))

    (fset index-formatting-command
          (list 'lambda 'nil
                (list 'para-index 
                      (list 'quote index-alist-name))))))


;;; Merge indices: @synindex   @syncodeindex

(put 'synindex 'para-format 'para-format-synindex)
(put 'syncodeindex 'para-format 'para-format-synindex)

(defun para-format-synindex ()
  (let* ((args (texinfo-parse-line-arg))
         (second (cdr (read-from-string args)))
         (joiner (symbol-name (car (read-from-string args))))
         (joined (symbol-name (car (read-from-string args second)))))

    (if (assoc joiner para-short-index-cmds-alist)
        (put
          (cdr (assoc joiner para-short-index-cmds-alist))
         'para-format
         (or (cdr (assoc joined para-short-index-format-cmds-alist))
             (intern (concat "para-format-" joined "index"))))
      (put
       (intern (concat joiner "index"))
       'para-format
       (or (cdr(assoc joined para-short-index-format-cmds-alist))
           (intern (concat "para-format-" joined "index")))))))

(defconst para-short-index-cmds-alist
  '(("cp" . cindex)
    ("fn" . findex)
    ("vr" . vindex)
    ("tp" . tindex)
    ("pg" . pindex)
    ("ky" . kindex)))

(defconst para-short-index-format-cmds-alist
  '(("cp" . para-format-cindex)
    ("fn" . para-format-findex)
    ("vr" . para-format-vindex)
    ("tp" . para-format-tindex)
    ("pg" . para-format-pindex)
    ("ky" . para-format-kindex)))


;;; Print index: @printindex

(put 'printindex 'para-format 'para-format-printindex)

(defun para-format-printindex ()
  (let ((indexelts (symbol-value
		    (cdr (assoc (texinfo-parse-line-arg)
				para-indexvar-alist))))
	opoint)
    (insert "\n@ignore\n* Menu:\n\n")
    (setq opoint (point))
    (para-print-index nil indexelts)

    (if (eq system-type 'vax-vms)
        (texinfo-sort-region opoint (point))
      (shell-command-on-region opoint (point) "sort -fd" 1))
        (insert "\n@end ignore\n")))

(defun para-print-index (file indexelts)
  (while indexelts
    (if (stringp (car (car indexelts)))
	(insert "* " (car (car indexelts))
		": " (if file (concat "(" file ")") "")
		(nth 1 (car indexelts)) ".\n")
      ;; index entries from @include'd file
      (para-print-index (nth 1 (car indexelts))
			   (nth 2 (car indexelts))))
    (setq indexelts (cdr indexelts))))


;;; Insert index into Para mode file (interactive command)

(defun para-insert-index ()
  "Insert index at @printindex command in Para mode file."
  (interactive)
  (require 'texinfmt)
  (widen)
  (let (para-command-name
        texinfo-command-start
        texinfo-command-end
        texinfo-last-node               ; Texinfo mode variable
        para-vindex
        para-findex
        para-cindex
        para-pindex
        para-tindex
        para-kindex)
    
    (goto-char (point-min))
    (while (re-search-forward "^@\\w+index" nil t)
      (forward-word -1)
      ;; @ is followed by a command-word; find the end of the word.
      (setq texinfo-command-start (point))
      (forward-word 1)
      (setq texinfo-command-end (point))
      ;; Call the handler for this command.
      (setq para-command-name
            (intern (buffer-substring texinfo-command-start
                                      texinfo-command-end)))
      (let ((cmd (get para-command-name 'para-format)))
        (if cmd (funcall cmd)
          (error "Para mode cannot handle \"%s\"."
                 (buffer-substring
                  texinfo-command-start 
                  texinfo-command-end)))))))


;;; Create index entries (interactive commands)

(defun para-index-function (function &optional where)
  "Insert entry for function index at end of paragraph.
Interactively, offer first symbol following point within paragraph
that is enclosed by braces of @code command.  With prefix arg, insert
entry at point \(no default offered\)."
  (interactive (para-index-what-and-where-utility))
  (if (eq where 'end-para)  (forward-paragraph 1))
  (insert "@findex " function "\n"))

(defun para-index-variable (variable &optional where)
  "Insert entry for variable index at end of paragraph.
Interactively, offer first symbol following point within paragraph
that is enclosed by braces of @code command.  With prefix arg, insert
entry at point \(no default offered\)."

  (interactive (para-index-what-and-where-utility))
  (if (eq where 'end-para)  (forward-paragraph 1))
    (insert "@vindex " variable "\n"))

(defun para-index-program (program &optional where)
  "Insert entry for program index at end of paragraph.
Interactively, offer first symbol following point within paragraph
that is enclosed by braces of @code command.  With prefix arg, insert
entry at point \(no default offered\)."
  
  (interactive (para-index-what-and-where-utility))
  (if (eq where 'end-para)  (forward-paragraph 1))
  (insert "@pindex " program "\n"))

(defun para-index-datatype (datatype &optional where)
  "Insert entry for datatype index at end of paragraph.
Interactively, offer first symbol following point within paragraph
that is enclosed by braces of @code command.  With prefix arg, insert
entry at point \(no default offered\)."
  
  (interactive (para-index-what-and-where-utility))
  (if (eq where 'end-para)  (forward-paragraph 1))
  (insert "@tindex " program "\n"))

(defun para-index-keystroke (keystrokes &optional where)
  "Insert entry for keystroke index at end of paragraph.
Interactively, offer default:

    1. If point is at beginning of an @item or @itemx line,
       offer rest of that line.

    2. Else, offer first symbol following point within paragraph
       that is enclosed by braces of @kbd command.

With prefix arg, insert entry at point \(no default offered\)."
  
  (interactive
   (let (keystrokes where)
     (if current-prefix-arg
         (setq keystrokes (read-from-minibuffer "Keystrokes: ")
               where 'here)
       (let((default-keystrokes
              (if (looking-at "@itemx? \\(.*\\)$")
                  (buffer-substring (match-beginning 1) (match-end 1))
                
                (if (re-search-forward "@kbd\\s(\\(\\S)+\\)\\s)")
                    (buffer-substring (match-beginning 1) (match-end 1))
                  ""))))
         
         (setq keystrokes
               (read-from-minibuffer 
                "Keystrokes: "
                (if default-keystrokes default-keystrokes ""))
               where 'end-para)))
     
     (list keystrokes where)))
  (if (eq where 'end-para)  (forward-paragraph 1))
  (insert "@kindex " keystrokes "\n"))

(defun para-index-concept (concept &optional where)
  "Insert entry for concept index at end of paragraph.
Offer word following point as default; with numeric prefix arg, offer
that many words following point as default.  With prefix arg (not a
numeric prefix arg), insert entry at point \(no default offered\).
Non-interactively, first arg CONCEPT is string; optional second arg
WHERE is either symbol 'here or symbol 'end-para."

  (interactive
   (let (concept where how-many)
     (cond ((and (listp current-prefix-arg)
                 (eq 4 (prefix-numeric-value current-prefix-arg)))
            (setq where 'here))
           ((prefix-numeric-value current-prefix-arg) 
            (setq how-many (prefix-numeric-value current-prefix-arg)
                  where 'end-para))
           (t (setq how-many 1 where 'end-para)))
     (if (eq where 'end-para)
         (let((default-concept
                (buffer-substring
                 (save-excursion (skip-chars-forward " \t") (point))
                 (save-excursion (forward-word how-many) (point)))))
           (setq concept (read-from-minibuffer 
                          "Concept: " (if default-concept default-concept
                                        ""))))
       (setq concept (read-from-minibuffer 
                      "Concept: ")))
     (list concept where)))
  
  (if (eq where 'end-para)  (forward-paragraph 1))
  (insert "@cindex " concept "\n"))


;;; Index utility functions
;;; para-current-node, para-index-what-and-where-utility 

(defun para-current-node ()
  "Return name of current node at or preceding point."
  (save-excursion
    (if (or (looking-at "^@node")
            (re-search-backward "^@node"  nil 'move-to-limit))
        ;; point is within a node
        (progn
          (re-search-forward "@node[ \t]*\\<\\([^,]*\\),")
          (buffer-substring (match-beginning 1) (match-end 1)))
      ;; else node not found
      (error "Node not found."))))

(defun para-index-what-and-where-utility ()
  "Utility used by para-index-function and others."
  (let (what where)
    (if current-prefix-arg
        (setq what (read-from-minibuffer "Entry: ")
              where 'here)
      (let((default-what
             (if (re-search-forward
                  "@code\\s(\\(\\S)+\\)\\s)"
                  (save-excursion (forward-paragraph 1) (point))
                  t)
                 (buffer-substring
                  (match-beginning 1) (match-end 1)))))
        (setq what (read-from-minibuffer 
                        "Entry: " (if default-what default-what ""))
               where 'end-para)))
    (list what where)))


(message "Loading Para mode ... done ")
(provide 'para)



;;; Bob only: site specific key rebinding

; `C-c C-x' is the prefix key for Para indexing.
; Redefine para-mode-map to use control keys in order to avoid
; conflict with Bob's keybindings for edebug.

(define-key para-mode-map "\C-c\C-x\C-c" 'para-index-concept)
(define-key para-mode-map "\C-c\C-x\C-f" 'para-index-function)
(define-key para-mode-map "\C-c\C-x\C-k" 'para-index-keystroke)
(define-key para-mode-map "\C-c\C-x\C-p" 'para-index-program)
(define-key para-mode-map "\C-c\C-x\C-t" 'para-index-datatype)
(define-key para-mode-map "\C-c\C-x\C-v" 'para-index-variable)

(define-key para-mode-map "\C-c\C-x\C-x" 'para-insert-index)