TARGET=ipf_unpack

$(TARGET):
	gcc -D__USE_MINGW_ANSI_STDIO -Wall -std=gnu99 -pedantic -O2 ipf_unpack.c -o $(TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe