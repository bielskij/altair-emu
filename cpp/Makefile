all:
	g++ -o app.elf \
		-I lib/include \
		-I lib/src \
		\
		lib/src/Core.cpp \
		lib/src/Core/MachineCycle.cpp \
		lib/src/Core/Instruction.cpp \
		lib/src/Core/InstructionDecoder.cpp \
		app/src/main.cpp 

clean:
	rm -f app.elf