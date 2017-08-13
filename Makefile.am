submit_maildir = $(includedir)/submit_mail
submit_mail_HEADERS = lib/submit_mail/message_format.h \
					  lib/submit_mail/message.h \
					  lib/submit_mail/server_error.h \
					  lib/submit_mail/server.h \
					  lib/submit_mail/str.h \
					  lib/submit_mail/submit_mail.h

AM_CPPFLAGS = -I$(srcdir)/lib
AM_CFLAGS = -std=c99
lib_LIBRARIES = libsubmit_mail.a
libsubmit_mail_a_SOURCES = $(submit_mail_HEADERS) \
						   lib/submit_mail/message_format.c \
						   lib/submit_mail/message.c \
						   lib/submit_mail/server_error.c \
						   lib/submit_mail/server.c \
						   lib/submit_mail/str.c

bin_PROGRAMS = submit_mail
submit_mail_SOURCES = src/main.c \
					  src/options.h src/options.c
submit_mail_LDADD = $(lib_LIBRARIES)

EXTRA_DIST = LICENSE README.md

