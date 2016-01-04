ipf_unpack:
	gcc -D__USE_MINGW_ANSI_STDIO -std=c99 -Wall -pedantic -O2 ipf_unpack.c -o ipf_unpack
clean:
	rm -f *.exe