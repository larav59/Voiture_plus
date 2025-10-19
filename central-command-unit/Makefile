SRC_DIR        := src
INC_DIR        := includes
OBJ_DIR        := obj
BIN_DIR        := bin
LIB_DIR        := lib
TEST_DIR       := tests
MK_LIB_DIR     := mk-lib
EXTERNAL_DIR   := external

SERVICES       := #broker conflict-manager heartbeat route-planner railway-synchronizer

TARGET_LIBA    := $(LIB_DIR)/libcore.a
TARGET_LIBSO   := $(LIB_DIR)/libcore.so
TARGET_TESTS   := $(BIN_DIR)/unit_tests

CC             ?= gcc
AR             ?= ar
LDFLAGS        ?= -L$(LIB_DIR)
CFLAGS         ?= -Wall -Wextra -g
LIBS           ?= 

EXT_LIB_TARGETS :=
EXT_LIBS        :=
EXT_CFLAGS	    :=

EXT_LIBS_MK    := $(wildcard $(MK_LIB_DIR)/*.mk)


# Règles principales
.PHONY: all debug release external-libs core services tests test-run clean distclean docs
all: core tests docs


-include $(EXT_LIBS_MK)

$(info External Libraries to build: $(EXT_LIB_TARGETS))

PROJECT_LIBS   := -lpthread -lcore $(EXT_LIBS)
PROJECT_CFLAGS := -I$(INC_DIR) $(EXT_CFLAGS) -MMD -MP -fPIC


SRC_CORE       := $(shell find $(SRC_DIR)/core -name "*.c")
SRC_SERVICES   := $(foreach s,$(SERVICES),$(shell find $(SRC_DIR)/$(s) -name "*.c"))
SRC_SERVICES   := $(filter-out $(SRC_CORE),$(SRC_SERVICES)) # sécurité
SRC_TESTS      := $(shell find $(TEST_DIR) -name "*.c")

OBJ_CORE       := $(SRC_CORE:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_SERVICES   := $(SRC_SERVICES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_TESTS      := $(SRC_TESTS:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)

ALL_SRC        := $(SRC_CORE) $(SRC_SERVICES) $(SRC_TESTS)

# src/<module>/<file>.c -> obj/<module>/<file>.o
OBJ_ALL        := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%,$(ALL_SRC))) \
                $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter $(TEST_DIR)/%,$(ALL_SRC)))

OBJ_CORE       := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_CORE))
OBJ_SERVICES   := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_SERVICES))
OBJ_TESTS      := $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_TESTS))

# Inclusion des dépendances automatiques
# Quand on génère un fichier .o avec des règles joker pour gérer la compilation lors du changement d'header c'est assez compliqué
# Pour ça on utilise l'option -MMD -MP de gcc qui génère des fichiers .d contenant des regles de dépendances
# On les inclut ici pour que make puisse les utiliser et recompiler les fichiers .o concernés quand un header change
# Note: le - devant include sert à ignorer les erreurs si le fichier .d n'existe pas encore (par exemple lors du premier build)

-include $(shell find $(OBJ_DIR) -name "*.d") 

# Libcore (static et shared)
core: external-libs $(TARGET_LIBA) $(TARGET_LIBSO)

$(TARGET_LIBA): $(OBJ_CORE)
	@mkdir -p $(LIB_DIR)
	@$(AR) rcs $@ $^
	@echo "AR $@"

# Link shared library (obj must be compiled with -fPIC)
$(TARGET_LIBSO): $(OBJ_CORE)
	@mkdir -p $(LIB_DIR)
	@$(CC) -shared -o $@ $^
	@echo "SO $@"


# Services
services: external-libs $(SERVICES:%=$(BIN_DIR)/%)

# Edition de liens des services avec les objets correspondants et la libcore.a
$(BIN_DIR)/%: $(wildcard $(OBJ_DIR)/%/*.o) $(TARGET_LIBA)
	@mkdir -p $(BIN_DIR)
	@$(CC) $^ -o $@ $(LDFLAGS) $(LIBS) $(PROJECT_LIBS)
	@echo "LD Service $@"

# Lanceur de test unitaires
tests: external-libs $(TARGET_TESTS)

$(TARGET_TESTS): $(OBJ_CORE) $(OBJ_TESTS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $^ -o $@ $(LDFLAGS) $(LIBS) $(PROJECT_LIBS)
	@echo "UT $@"

# Run tests
test-run: tests
	@echo "=== Running unit tests ==="
	@./$(TARGET_TESTS)

# Compilation de src/<module>/<file>.c  -> obj/<module>/<file>.o
# Compilation de tests/<module>/<file>.c -> obj/<module>/<file>.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) $(PROJECT_CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) $(PROJECT_CFLAGS) -c $< -o $@


# Construction des bibliothèques externes
external-libs: $(EXT_LIB_TARGETS)

# Documentation
docs:
	@mkdir -p docs
	@doxygen Doxyfile
	@echo "DOC Documentation générée"


# Nettoyage
clean:
	rm -rf $(BIN_DIR) $(LIB_DIR) $(OBJ_DIR)

# Pour le debug des variables Makefile
print-%:
	@echo '$*=$($*)'