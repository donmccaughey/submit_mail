submit_mail
===========

A command line program for sending e-mail.


Dependencies
------------
Building `submit_mail` requires `libcurl`.


Building submit_mail
--------------------
Clone the repository then generate the `configure` script, configure and build.

    git clone https://github.com/donmccaughey/submit_mail.git
    cd submit_mail
    autoreconf -i
    mkdir tmp && cd tmp
    ../configure && make && sudo make install


Using submit_mail
-----------------
Call `submit_mail` from a bash script like this:

    submit_mail \
        --host=smtp.example.com \
        --port=587 \
        --username=user@example.com \
        --password=very-secret \
        --from=alice@example.com \
        --subject="Testing submit_mail" \
        --to=bob@example.com \
    <<EOF
    This is a test e-mail sent with the submit_mail command.
    EOF

Call `submit_mail` with the `--help` option to see the full list of options.


License
-------
`submit_mail` is made available under a BSD-style license; see the LICENSE
file for details.

