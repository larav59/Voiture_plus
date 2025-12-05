CJSON_NAME := cjson
CJSON_PATH := $(EXTERNAL_DIR)/cJSON


CJSON_SO_TARGET := $(LIB_DIR)/lib$(CJSON_NAME).so

_CJSON_MAKE_CMD = $(MAKE) --no-print-directory -s -C $(CJSON_PATH)


EXT_LIB_TARGETS += $(CJSON_SO_TARGET)

EXT_LIBS += -l$(CJSON_NAME)

EXT_CFLAGS += -I$(CJSON_PATH)


$(CJSON_SO_TARGET): $(CJSON_PATH)/cJSON.c $(CJSON_PATH)/Makefile
	@echo "SO EXT $@"
	@$(_CJSON_MAKE_CMD) shared
	@mkdir -p $(LIB_DIR)
	cp $(CJSON_PATH)/libcjson.so.1 $(LIB_DIR)/ 2>/dev/null || true
	mv $(LIB_DIR)/libcjson.so.1 $(LIB_DIR)/libcjson.so

.PHONY: clean-$(CJSON_NAME)

clean-$(CJSON_NAME):
	@echo "CLEAN $(CJSON_NAME)..."
	@$(_CJSON_MAKE_CMD) clean
	@rm -f $(CJSON_SO_TARGET)