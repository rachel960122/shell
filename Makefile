CC = g++
CFLAGS = -std=c++11 -g -Wall
TARGET1 = shell
TARGET2 = tester

all: $(TARGET1) sleeper

$(TARGET1) : $(TARGET1).cpp  ziting.o rachel.o joblist.o
	$(CC) $(CFLAGS) -lreadline -o $(TARGET1) $(TARGET1).cpp ziting.o rachel.o joblist.o

$(TARGET2) : $(TARGET2).cpp  ziting.o
	$(CC) $(CFLAGS) -lreadline -o $(TARGET2) $(TARGET2).cpp ziting.o

sleeper: sleeper.cpp
	$(CC) $(CFLAGS) sleeper.cpp -o sleeper

joblist.o: joblist.cpp
	$(CC) $(CFLAGS) -c joblist.cpp

ziting.o: ziting.cpp
	$(CC) $(CFLAGS) -c ziting.cpp

rachel.o: rachel.cpp
	$(CC) $(CFLAGS) -c rachel.cpp

clean:
	$(RM) $(TARGET1) sleeper $(TARGET2) ziting.o rachel.o joblist.o
