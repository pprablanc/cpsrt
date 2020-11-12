EXEC=pitch_shifting_bin
LIBS=lib/libportaudio.a -lrt -lasound -lpthread -lm -ljack

all: ${EXEC}

${EXEC}: main.o init.o pitch_shifting.o resample.o
	gcc -no-pie -o $@ $^ ${LIBS}

%.o: %.c
	gcc -c -I ./lib $<

clean:
	rm -rf *.o ${EXEC}
	rm check_output
	rm check_input
