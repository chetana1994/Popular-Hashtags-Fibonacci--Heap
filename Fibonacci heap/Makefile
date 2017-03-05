# the compiler: g++ for C++
  CC = g++

  # compiler flags:
  #  -g    adds debugging information to the executable file
  CFLAGS  = -g -fpermissive

  # the build target executable:
  TARGET = hashtagcounter

  all: $(TARGET)

  $(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

  clean:
	$(RM) $(TARGET)
