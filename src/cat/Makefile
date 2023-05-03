TARGET = s21_cat

CC = gcc -Wall -Werror -Wextra

all : $(TARGET) clean

$(TARGET).o : $(TARGET).c $(TARGET).h
	$(CC) -c $(TARGET).c

$(TARGET) : $(TARGET).o
	$(CC) -o $(TARGET) $(TARGET).o 

testfunc :
	sh test_func_grep.sh

testleak :
	sh test_leak_grep.sh

clangtest :
	clang-format -n $(TARGET).*

rebuild : clean all

clean :
	rm -rf $(TARGET).o