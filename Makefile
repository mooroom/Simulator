CC = gcc
TARGET1 = project3
MAIN_SOURCE1 = project3.cpp

all: $(TARGET1)

$(TARGET1):
	$(CC) -o $(TARGET1) $(MAIN_SOURCE1) -lstdc++

clean:
	rm -f $(TARGET1)