submit_mail
===========

A command line program for sending e-mail.


Building submit_mail
--------------------
Clone the repository then generate the `configure` script, configure and build.

    git clone https://github.com/donmccaughey/submit_mail.git
    cd submit_mail
    autoreconf -i
    mkdir tmp && cd tmp
    ../configure && make && sudo make install
    ./submit_mail -?


License
-------
`submit_mail` is made available under a BSD-style license; see the LICENSE
file for details.

