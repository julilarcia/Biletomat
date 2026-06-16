CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Ilibticket -Ilibtariff

BUILD = build

TARGET = $(BUILD)/biletomat
STATIC_LIB = $(BUILD)/libticket.a
DYNAMIC_LIB = $(BUILD)/libtariff.so

SRC = src/main.c src/menu.c src/payment.c src/printer.c src/shared_state.c src/logger.c
OBJ = $(SRC:src/%.c=$(BUILD)/%.o)

TICKET_OBJ = $(BUILD)/ticket.o
TARIFF_OBJ = $(BUILD)/tariff.o

.PHONY: all clean run

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/ticket.o: libticket/ticket.c libticket/ticket.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(TICKET_OBJ)
	ar rcs $@ $^

$(BUILD)/tariff.o: libtariff/tariff.c libtariff/tariff.h | $(BUILD)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(DYNAMIC_LIB): $(BUILD)/tariff.o
	$(CC) -shared -o $@ $^

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ) $(STATIC_LIB) $(DYNAMIC_LIB)
	$(CC) $(CFLAGS) $(OBJ) $(STATIC_LIB) -ldl -pthread -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD)