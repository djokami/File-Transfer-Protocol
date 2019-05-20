.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

CC = gcc
CFLAGS = -g -Wall -Wextra -Wfatal-errors -fdiagnostics-color=auto
LDFLAGS =

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions
#LIBS += -lsocket -lnsl -lrt
LIBS += -lpthread

INCLUDE = Dependance/csapp.h Dependance/lireCommande.h
OBJS = Dependance/csapp.o Dependance/lireCommande.o
INCLDIR = -I.

PROGS= Client/clientFTP Serveur/serveurFTP

all: $(PROGS)

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) $(INCLDIR) -c -o $@ $<

%: %.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

clean:
	rm -f $(PROGS) Dependance/*.o  Client/*.o  Serveur/*.o
