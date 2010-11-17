Launching the script without any parameters will show a short help. It
should be pretty self-explanatory.

Here are some examples on how the script could be invoked:

- this will read the file list.txt, and create the mailboxes listed in it
  with a 50MB quota:

  cat list.txt | ./imapcreate.pl -u cyradm -p 'cyrpass' -q 50M mail.example.com

- this will create a mailbox for john, using the Unix Hierarchy separator,
  no quota, in verbose mode:

  ./imapcreate.pl -u cyradm -m john -s -v mail.example.com

  the output would look like this:

  Creating user.john on default

This script is far from being perfect, but it works great for me. feel free
to e-mail me about it, to report bugs, to send patches etc:

clement.hermann@free.fr
