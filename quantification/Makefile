#
# Makefile pour la bibliotheque graphique
#

SOURCES = $(wildcard *.c)

CFLAGS += -std=c99 -D_DEFAULT_SOURCE -I ../exercice1
OBJETS = $(SOURCES:.c=.o)
CIBLE = quantification.a

#
# Nom de la cible principale
#

all: $(CIBLE)

#
# Cible de nettoyage
#

clean: 
	rm -f core *.o $(CIBLE)

#
# Dependances pour la bibliotheque
#


$(CIBLE): $(OBJETS)
	$(AR) rs $@ $?

$(CIBLE:.a=).o: $(CIBLE:.a=).c $(CIBLE:.a=).h