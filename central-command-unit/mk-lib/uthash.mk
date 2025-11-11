# Nom de la lib
UTHASH_NAME := uthash
UTHASH_PATH := $(EXTERNAL_DIR)/$(UTHASH_NAME)

# La lib uthash est une lib header-only, pas de cible de build nécessaire ici

EXT_CFLAGS += -I$(UTHASH_PATH)/src