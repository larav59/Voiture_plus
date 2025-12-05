# Configuration du Makefile pour intégrer une librairie externe :

# Nom court de la bibliothèque, utilisé pour construire les autres variables
INIH_NAME := inih

# Chemin vers le code source de la bibliothèque
INIH_PATH := $(EXTERNAL_DIR)/$(INIH_NAME)

# 1. On définit le nom de notre cible .so (ex: lib/libinih.so)
INIH_SO_TARGET := $(LIB_DIR)/lib$(INIH_NAME).so

# 2. On ajoute notre cible à la liste globale des bibliothèques à construire.
#    Le Makefile principal utilisera cette liste pour savoir quoi compiler.
EXT_LIB_TARGETS += $(INIH_SO_TARGET)

# 3. On ajoute nos flags de linkage à la liste globale des bibliothèques à lier.
#    Le Makefile principal ajoutera "-linih" à la commande de linkage finale.
EXT_LIBS += -l$(INIH_NAME)

# 4. On ajoute le dossier contenant les headers de la bibliothèque aux flags de compilation.
EXT_CFLAGS += -I$(INIH_PATH)

# 5. On fournit la règle pour construire notre .so.
#    Cette règle sera découverte et utilisée par le Makefile principal.
$(INIH_SO_TARGET): $(INIH_PATH)/ini.c
	@mkdir -p $(LIB_DIR)
	@echo "SO EXT $@"
	@$(CC) $(CFLAGS) -fPIC -shared $< -o $@

.PHONY: clean-$(INIH_NAME)

clean-$(INIH_NAME):
	@echo "CLEAN $(INIH_NAME)..."
	@rm -f $(INIH_SO_TARGET)
