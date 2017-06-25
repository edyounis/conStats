CC:=gcc
CFLAGS:=-Wall -Werror -fPIC -lm
INCLUDE_DIRS:=-Iinclude
BUILD_DIR:=build
TARGET:=libstats
DEP:=src/constats.c

compile: $(DEP)
	mkdir -p build
	$(CC) $(INCLUDE_DIRS) -c $(DEP) -o $(BUILD_DIR)/$(TARGET).o $(CFLAGS)
	$(CC) -shared -o $(TARGET).so $(BUILD_DIR)/$(TARGET).o

clean:
	rm -rf $(BUILD_DIR)
	rm *.so