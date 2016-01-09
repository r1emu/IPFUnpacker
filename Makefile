TARGET=ipf_unpack

$(TARGET):
	gcc -Wall -std=gnu99 -pedantic -O2 ipf_unpack.c -o $(TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe