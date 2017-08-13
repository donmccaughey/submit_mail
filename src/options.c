#include "options.h"

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static struct option long_options[] = {
    {
        .name="help",
        .has_arg=no_argument,
        .flag=NULL,
        .val='?'
    },
    {
        .name="debug",
        .has_arg=no_argument,
        .flag=NULL,
        .val='d'
    },
    {
        .name="from",
        .has_arg=required_argument,
        .flag=NULL,
        .val='f'
    },
    {
        .name="host",
        .has_arg=required_argument,
        .flag=NULL,
        .val='h'
    },
    {
        .name="port",
        .has_arg=required_argument,
        .flag=NULL,
        .val='P'
    },
    {
        .name="password",
        .has_arg=required_argument,
        .flag=NULL,
        .val='p'
    },
    {
        .name="subject",
        .has_arg=required_argument,
        .flag=NULL,
        .val='s'
    },
    {
        .name="to",
        .has_arg=required_argument,
        .flag=NULL,
        .val='t'
    },
    {
        .name="username",
        .has_arg=required_argument,
        .flag=NULL,
        .val='u'
    },
    {
        .name="verbose",
        .has_arg=no_argument,
        .flag=NULL,
        .val='v',
    },
    {
        .name=NULL,
        .has_arg=no_argument,
        .flag=NULL,
        .val=0,
    },
};

static char const short_options[] = "?df:h:P:p:s:t:u:v";


void
options_usage_and_exit(struct options const *options)
{
    assert(options);
    
    size_t spacer_length = strlen(options->command_name);
    char spacer[spacer_length + 1];
    memset(spacer, ' ', spacer_length);
    spacer[spacer_length] = '\0';
    
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "  %s (-h|--host) SMTPSERVER (-f|--from) USER@DOMAIN \\\n", options->command_name);
    fprintf(stdout, "  %s (-t|--to) USER@DOMAIN [options...]\n", spacer);
    fprintf(stdout, "\n");
    fprintf(stdout, "General Options:\n");
    fprintf(stdout, "  -?, --help                Show this help message\n");
    fprintf(stdout, "  -d, --debug               Print internal debugging info\n");
    fprintf(stdout, "  -v, --verbose             Print progress messages\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Server Options:\n");
    fprintf(stdout, "  -h, --host=SMTPSERVER     SMTP server hostname (required)\n");
    fprintf(stdout, "  -P, --port=PORT           SMTP server port (default: 587)\n");
    fprintf(stdout, "  -p, --password=PASSWORD   The sender's SMTP server password\n");
    fprintf(stdout, "  -u, --username=USERNAME   The sender's SMTP server username\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Mail Options:\n");
    fprintf(stdout, "  -f, --from=USER@DOMAIN    The sender's e-mail address (required)\n");
    fprintf(stdout, "  -s, --subject=SUBJECT     The subject of the e-mail message\n");
    fprintf(stdout, "  -t, --to=USER@DOMAIN      The recipient of the e-mail (required)\n");
    fprintf(stdout, "\n");
    
    exit(EXIT_FAILURE);
}


static int
str_to_port(char const *str, in_port_t *port)
{
    errno = 0;
    long long_value = strtol(str, NULL, 10);
    if ((!long_value && errno) || long_value < 0 || long_value > UINT16_MAX) {
        return -1;
    } else {
        *port = (uint16_t)long_value;
        return 0;
    }
}


static int
get_options(struct options *options, int argc, char **argv)
{
    int long_option_index;
    int ch;
    while (-1 != (ch = getopt_long(argc, argv, short_options, long_options, &long_option_index))) {
        switch (ch) {
            case 'd':
                options->is_debug = true;
                break;
            case 'f':
                options->from = strdup(optarg);
                if (!options->from) return -1;
                break;
            case 'h':
                options->host = strdup(optarg);
                if (!options->host) return -1;
                break;
            case 'P':
                if (-1 == str_to_port(optarg, &options->port)) {
                    fprintf(stderr, "Invalid port \"%s\"\n", optarg);
                    options->is_usage = true;
                    continue;
                }
                break;
            case 'p':
                options->password = strdup(optarg);
                if (!options->password) return -1;
                break;
            case 's':
                options->subject = strdup(optarg);
                if (!options->subject) return -1;
                break;
            case 't':
                options->to = strdup(optarg);
                if (!options->to) return -1;
                break;
            case 'u':
                options->username = strdup(optarg);
                if (!options->username) return -1;
                break;
            case 'v':
                options->is_verbose = true;
                break;
            default:
                options->is_usage = true;
                break;
        }
    }
    
    if (!options->host) {
        fprintf(stderr, "%s: option -h SMTPSERVER or --host SMTPSERVER is required\n",
                options->command_name);
        options->is_usage = true;
    }
    
    if (!options->from) {
        fprintf(stderr, "%s: option -f USER@DOMAIN or --from USER@DOMAIN is required\n",
                options->command_name);
        options->is_usage = true;
    }
    
    if (!options->to) {
        fprintf(stderr, "%s: option -t USER@DOMAIN or --to USER@DOMAIN is required\n",
                options->command_name);
        options->is_usage = true;
    }
    
    return 0;
}


struct options *
options_alloc(int argc, char *argv[])
{
    if (!argc || !argv) {
        errno = EINVAL;
        return NULL;
    }
    
    struct options *options = malloc(sizeof(struct options));
    if (!options) return NULL;
    
    options->body_file = stdin;
    options->port = 587;
    
    char *command_name = basename(argv[0]);
    if (!command_name) {
        options_free(options);
        return NULL;
    }
    
    options->command_name = strdup(command_name);
    if (!options->command_name) {
        options_free(options);
        return NULL;
    }
    
    int result = get_options(options, argc, argv);
    if (-1 == result) {
        options_free(options);
        return NULL;
    }
    
    return options;
}


void
options_free(struct options *options)
{
    if (!options) return;
    free(options->command_name);
    free(options->from);
    free(options->host);
    free(options->password);
    free(options->subject);
    free(options->to);
    free(options->username);
    free(options);
}
