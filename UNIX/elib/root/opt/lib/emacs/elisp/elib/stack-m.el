;;;; $Id: stack-m.el,v 0.5 1992/08/19 01:58:02 ceder Exp $
;;;; This file implements a simple LIFO stack using macros.
;;;;
;;;; Copyright (C) 1991, 1992 Free Software Foundation
;;;;
;;;; This file is part of the GNU Emacs lisp library, Elib.
;;;;
;;;; GNU Elib is free software; you can redistribute it and/or modify
;;;; it under the terms of the GNU General Public License as published by
;;;; the Free Software Foundation; either version 1, or (at your option)
;;;; any later version.
;;;;
;;;; GNU Elib is distributed in the hope that it will be useful,
;;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;;; GNU General Public License for more details.
;;;;
;;;; You should have received a copy of the GNU General Public License
;;;; along with GNU Emacs; see the file COPYING.  If not, write to
;;;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
;;;; 
;;;; Author: Inge Wallin
;;;; 

;;;
;;; The stack is implemented as a linked list with a tag 'STACK
;;; as the first element.  All entries and removals are done using
;;; destructive functions.
;;;
;;; This file implements the functions as macros for speed in compiled 
;;; code.
;;;


;; Provide the function version and remove the macro version
(provide 'stack-m)
(setq features (delq 'stack-f features))


;;; ================================================================


(defmacro stack-create ()
  "Create an empty lifo stack."
  (` (cons 'STACK nil)))


(defmacro stack-p (stack)
  "Return t if STACK is a stack, otherwise return nil."
  (` (eq (car-safe (, stack)) 'STACK)))


(defmacro stack-push (stack element)
  "Push an element onto the stack.
Args: STACK ELEMENT"
  (` (setcdr (, stack) (cons (, element) (cdr (, stack))))))


(defmacro stack-pop (stack)
  "Remove the topmost element from STACK and return it. 
If the stack is empty, return nil."
  (` (prog1
	 (car-safe (cdr (, stack)))
       (setcdr (, stack) (cdr-safe (cdr (, stack)))))))


(defmacro stack-empty (stack)
  "Return t if STACK is empty, otherwise return nil."
  (` (null (cdr (, stack)))))


(defmacro stack-top (stack)
  "Return the topmost element of STACK or nil if it is empty."
  (` (car-safe (cdr (, stack)))))


(defmacro stack-nth (stack n)
  "Return nth element of a stack, but don't remove it.
Args: STACK N
If the length of the stack is less than N, return nil.

The top stack element has number 0."
  (` (nth (, n) (cdr (, stack)))))


(defmacro stack-all (stack)
  "Return a list of all entries in STACK.
The element last pushed is first in the list."
  (` (cdr (, stack))))


(defmacro stack-copy (stack)
  "Return a copy of STACK.
All entries in STACK are also copied."
  (` (cons 'STACK (copy-sequence (cdr (, stack))))))


(defmacro stack-length (stack)
  "Return the number of elements on STACK."
  (` (length (cdr (, stack)))))


(defmacro stack-clear (stack)
  "Remove all elements from STACK."
  (` (setcdr (, stack) nil)))
