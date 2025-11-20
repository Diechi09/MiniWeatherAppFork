CC=mpicc
CFLAGS=-O3 -Wall -Wextra -std=c11 -fopenmp
TARGET=miniweather_hpc
SRC=hpc_stencil.c

.PHONY: all clean format

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

format:
	clang-format -i $(SRC) || true

clean:
	rm -f $(TARGET) *.o
