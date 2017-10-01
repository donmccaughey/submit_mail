# TODO

## Command Line Options

    - support multiple -t, --to options

    - support -w, --password-file option

    - -F, --body-file option,
        reads body from a file

    - -B, --body option,
        set body on the command line

    - -c, --cc command line option,
        may be specified multiple times

    - -b, --bcc command line option,
        may be specified multiple times

    - --from-display-name option

    - --to-display-name option

    - --cc-display-name option

    - --bcc-display-name option

    - -n, --no-ssl command line options,
        turns off SSL

    - -S, --server-config option
        the server config file looks like:
            host: mail.example.com
            port: 25
            username: user@example.com
            password: donttell
            ssl: off

# Validation

    - validate from/to/cc/bcc addresses

# Encoding

    - correctly encode display names

    - correctly encode subject

    - correctly encode body

# Other Changes

    - logic for substituting username and `from' field:
        * if username is given and looks like an e-mail address, it becomes
        the default for the `from' field
        * if password and `from' are given, `from' becomes the default for
        the username

    - only serialize headers into memory, stream body from
        source file

