# Example config.make.

# Install files under /usr/local instead of under $HOME.
prefix=/usr/local

# Use ncursesw.
LDLIBS = -lncursesw
CPPFLAGS = -DHAVE_NCURSESW_CURSES_H

# Use readline.
#LDLIBS += -lreadline
#CPPFLAGS += -DHAVE_READLINE

# Uncomment to enable work-around for missing setenv().
#NO_SETENV=y

# Uncomment to enable work-around for missing mkstemps().
#NO_MKSTEMPS=y

# Uncomment to enable work-around for missing wordexp().
#NO_WORDEXP=y

# Uncomment to not include built-in tigrc inside the binary.
#NO_BUILTIN_TIGRC=y

# vim: ft=make:
