A quick hack to display amiga's autodocs documentation.

###Configuration
For now edit getdoc.c (line 41) and change the path to where you keep your autodocs.  Alternatively, export a variable "autodoc" holding the path.  The first needs recompile, while the later won't need that.

###Usage
E.g. getdoc Open

and you'll get the documentation for dos.library/Open

It will try to find (and suggest) function when you mispell things.

E.g.
```
getdoc waitbit
dos.library/WaitPkt
graphics.library/WaitBlit
exec.library/Wait
exec.library/WaitIO


4 fuzzy matches.
```

###Emacs + getdoc
Put something like that in your ~/.emacs
```
(global-set-key [f12] 'lookup-word-at-point)

(require 'thingatpt)

(defun lookup-word-at-point ()
  (interactive)
  (shell-command (format "getdoc %s" (shell-quote-argument (word-at-point)))))
```
And now with F12 you'll get the autodoc for the word your cursor is at emacs.
