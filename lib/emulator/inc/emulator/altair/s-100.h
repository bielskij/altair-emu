/*******************************************************************************
 * Copyright (C) 2018 Jaroslaw Bielski (bielski.j@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef EMULATOR_S_100_H_
#define EMULATOR_S_100_H_

typedef struct _S100Bus {
	// Processor command/control output signal which acknowledges that the processor
	// is in a WAIT state
	_U8 PWAIT:1;

	// Processor command/control output signal indicating interrupts are enabled:
	// indicates the content of the CPU internal flip-flop; F-F may be set or reset by
	// EI and DI instruction and inhibits interrupts from being accepted by the CPU
	// if it is reset
	_U8 PINTE:1;

	// Address
	_U16 A;

	// Data in; (EXT -> CPU)
	_U8 DI;

	// Data out (CPU -> EXT)
	_U8 DO;

	// Inverted output of the 2MHz oscillator that generates the 2 phase clock (Inverted state)
	_U8 _CLOCK:1;

	// Disables the data input buffers so the input from the sense switches may
	// strobed onto the bidirectional data bus right at the processor
	_U8 _SSW_DSB:1;

	// Clear signal for I/O devices (front panel switch closure to ground)
	_U8 _EXT_CLR:1;

	// Indicates that the current data on the Data Bus Out is to be written
	// into the memory location currently on the address bus.
	_U8 MWRT:1;

	// Indicates the status of the memory protect flip-flop on the memory board
	// currently addressed
	_U8 _PS:1;

	// Input to the memory protect flip-flop on the memory board  currently addressed
	_U8 PROT:1;

	// Indicates that the RUN/STOP flip-flop is reset
	_U8 RUN:1;

	// Processor command/control input that controls the run state of the
	// processor; if the line is pulled low the processor will enter a wait
	// state until the line is released
	_U8 PRDY:1;

	// The processor recognizes an interrupt request on this line at the end
	// of the current instruction or while halted. If the processor is in the HOLD
	// state or the interrupt enable flip-flop is reset, it will not honor the
	// request.
	_U8 _PINT:1;

	// Processor command/control input signal which requests the processor
	// to enter the HOLD state; allows an external device to gain control of
	// address and data buses as soon as the processor has completed its use
	// of these buses for the current machine cycle.
	_U8 _PHOLD:1;

	// Processor command/control input; while activated the content of the
	// program counter is cleared and the instruction register is set to 0
	_U8 _PRESET:1;

	// Processor command/control output provides a signal to indicate the beginning
	// of each machine cycle
	_U8 PSYNC:1;

	// Processor command/control output used for memory write or I/O output control:
	// data on the data bus is stable while PWR is active
	_U8 _PWR:1;

	// Processor command/control output signal indicates to external circuits
	// that the data bus is in the input mode
	_U8 PDBIN:1;

	// Status output signal that indicates that the porcessor is in the fetch
	// cycle for the first byte of an instruction
	_U8 SM1:1;

	// Status output signal which indicates that the address bus contains the
	// address of an output device and data bus will contain the output data
	// when PWR is active
	_U8 SOUT:1;

	// Status output signal which indicates that the address bus contains the
	// address of an input device and the input data should be placed on the
	// data bus when PDBIN is active
	_U8 SINP:1;

	// Status output signal to acknowledge signal for INTERRUPT request
	_U8 SINTA:1;

	// Status output signal indicates that the operation int the current machine
	// cycle will be a WRITE memory or output function
	_U8 SWO:1;

	// Status output signal indicates that the address bus holds the pushdown
	// stack address form the stack pointer.
	_U8 SSTACK:1;

	// Status output signal which indicates that the data bus will be used
	// for memory read data
	_U8 SMEMR:1;

	// Status output signal which acknowledges a HALT instruction
	_U8 SHLTA:1;

	// Power-On Clear
	_U8 _POC:1;
} S100Bus;

#endif /* EMULATOR_S_100_H_ */
