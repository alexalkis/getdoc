A quick hack to display amiga's autodocs documentation.

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

######Emacs + getdoc
Put something like that in your ~/.emacs
```
(global-set-key [f12] 'lookup-word-at-point)

(require 'thingatpt)

(defun lookup-word-at-point ()
  (interactive)
  (shell-command (format "getdoc %s" (shell-quote-argument (word-at-point)))))
```
And now with F12 you'll get the autodoc.
