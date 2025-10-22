# Nom de la lib
MOSQ_NAME := mosquitto

# Dossier du submodule
MOSQ_PATH := $(EXTERNAL_DIR)/$(MOSQ_NAME)

# Cible (lib partagée)
MOSQ_SO_TARGET := $(LIB_DIR)/lib$(MOSQ_NAME).so

# Variables globales
EXT_LIB_TARGETS += $(MOSQ_SO_TARGET)
EXT_LIBS += -l$(MOSQ_NAME)
EXT_CFLAGS += -I$(MOSQ_PATH)/include -I$(MOSQ_PATH)/lib

# Règle de construction
$(MOSQ_SO_TARGET):
	@echo "Building libmosquitto via submodule..."
	@$(MAKE) -C $(MOSQ_PATH)/lib WITH_SHARED_LIBRARIES=yes WITH_STATIC_LIBRARIES=no
	@mkdir -p $(LIB_DIR)
	@cp $(MOSQ_PATH)/lib/libmosquitto.so.* $(LIB_DIR)/ 2>/dev/null || true
	@mv $(LIB_DIR)/libmosquitto.so.* $(LIB_DIR)/libmosquitto.so