# vim: set noexpandtab tabstop=2 shiftwidth=2 softtabstop=2:
###################################################
# Makefile for elven project
###################################################

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -Iprotocols `pkg-config cairo --cflags`
LDFLAGS := -lwayland-client -lrt `pkg-config cairo --libs`

# Debugging
ifeq ($(DEBUG), 1)
	$(info DEBUG=1: adding -g) CFLAGS += -g
endif

# Directories
SRC_DIR := src
PROTOCOLS_DIR := protocols
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# Source files
WAYLAND_XMLS := \
								/usr/share/wlr-protocols/unstable/wlr-layer-shell-unstable-v1.xml \
								/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
WAYLAND_XMLS_NAMES := $(basename $(notdir $(WAYLAND_XMLS)))
WAYLAND_SRCS := $(patsubst %, $(PROTOCOLS_DIR)/%.c, $(subst -,_,$(WAYLAND_XMLS_NAMES)))

LIB_SRCS := $(filter-out src/main.c, $(wildcard $(SRC_DIR)/*.c))
APP_SRCS := src/main.c
SRC_SRCS := $(APP_SRCS) $(LIB_SRCS) $(WAYLAND_SRCS)

OBJ_DIR_SRC := $(OBJ_DIR)/src
OBJ_DIR_PROTOCOLS := $(OBJ_DIR)/protocols

# Objects
OBJS := $(patsubst $(PROTOCOLS_DIR)/%.c, $(OBJ_DIR_PROTOCOLS)/%.o, $(WAYLAND_SRCS)) \
				$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR_SRC)/%.o, $(LIB_SRCS) $(APP_SRCS))

# Phony targets
.PHONY: all default build-gen build-headers build-soruces clean

# Default target
default: all

all: build/elven

# Final link
$(BUILD_DIR)/elven: $(OBJS) | $(BUILD_DIR)
	# $(CC) $(CFLAGS) -o $@ $(addprefix $(OBJ_DIR)/, $(notdir $(OBJS))) $(LDFLAGS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Object Creation
$(OBJ_DIR_SRC)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(OBJ_DIR_SRC)
	$(CC) $(CFLAGS) -c $< -o $@ 

# Compile generated protocol sources
$(OBJ_DIR_PROTOCOLS)/%.o: $(PROTOCOLS_DIR)/%.c | $(OBJ_DIR) $(OBJ_DIR_PROTOCOLS)
	$(CC) $(CFLAGS) -c $< -o $@

# Generated sources from Wayland
build-gen: build-sources build-headers

build-sources: | $(PROTOCOLS_DIR)
	wayland-scanner private-code /usr/share/wlr-protocols/unstable/wlr-layer-shell-unstable-v1.xml \
		$(PROTOCOLS_DIR)/wlr_layer_shell_unstable_v1.c
	wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
		$(PROTOCOLS_DIR)/xdg_shell.c

build-headers: 
	wayland-scanner client-header /usr/share/wlr-protocols/unstable/wlr-layer-shell-unstable-v1.xml \
		$(PROTOCOLS_DIR)/wlr_layer_shell_unstable_v1.h

# Create necessary directories
$(BUILD_DIR) $(OBJ_DIR) $(PROTOCOLS_DIR):
	@mkdir -p $@

$(OBJ_DIR_SRC) $(OBJ_DIR_PROTOCOLS):
	@mkdir -p $@

# Clean up
clean: 
	rm -rf $(BUILD_DIR) $(PROTOCOLS_DIR)/*
