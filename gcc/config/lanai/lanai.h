#ifndef _lanai_h_
#define _lanai_h_

/*************************************************************************
 *                                                                       *
 * lanai.h: header file for LANai implementation of gcc                  *
 *                                                                       *
 * Copyright 1994,95,96,97,98,99,2000 by Myricom, Inc.               	 *
 * All rights reserved.                                                  *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of version 2 of the GNU General Public License     *
 * as published by the Free Software Foundation.  Myricom requests that  *
 * all modifications of this software be returned to Myricom, Inc. for   *
 * redistribution.  The name of Myricom, Inc. may not be used to endorse *
 * or promote products derived from this software without specific prior *
 * written permission.                                                   *
 *                                                                       *
 * Myricom, Inc. makes no representations about the suitability of this  *
 * software for any purpose.                                             *
 *                                                                       *
 * THIS FILE IS PROVIDED "AS-IS" WITHOUT WARRANTY OF ANY KIND, WHETHER   *
 * EXPRESSED OR IMPLIED, INCLUDING THE WARRANTY OF MERCHANTABILITY OR    *
 * FITNESS FOR A PARTICULAR PURPOSE.  MYRICOM, INC. SHALL HAVE NO        *
 * LIABILITY WITH RESPECT TO THE INFRINGEMENT OF COPYRIGHTS, TRADE       *
 * SECRETS OR ANY PATENTS BY THIS FILE OR ANY PART THEREOF.              *
 *                                                                       *
 * In no event will Myricom, Inc. be liable for any lost revenue         *
 * or profits or other special, indirect and consequential damages, even *
 * if Myricom has been advised of the possibility of such damages.       *
 *                                                                       *
 * Other copyrights might apply to parts of this software and are so     *
 * noted when applicable.                                                *
 *                                                                       *
 * Myricom, Inc.                    Email: info@myri.com                 *
 * 325 N. Santa Anita Ave.          World Wide Web: http://www.myri.com/ *
 * Arcadia, CA 91024                                                     *
 *************************************************************************/
 /* file first release 5/95 */

/* This file is an adaptation (completed in May, 1995) by Glenn Brown
   (glenn@myri.com) of i860.h, which is Copyright (C) 1993 Free
   Software Foundation, Inc.  THAT file was written by Richard
   Stallman (rms@ai.mit.edu) and hacked substantially by Ron Guilmette
   (rfg@netcom.com) to cater to the whims of the System V Release
   4 assembler.
*/

/* Names to predefine in the preprocessor for this target machine.  */

#define CPP_PREDEFINES "-Dlanai"

/* Print subsidiary information on the compiler version in use.  */
#define TARGET_VERSION fprintf (stderr, " (lanai)")

/* Run time compilation flags. */

/*
  ONE_ALU		    processor has one ALU (else 2)
  NO_PC_LOAD		    loads into PC are forbidden
  NOPS_AFTER_BRANCHES	    emit NOP insns after brances for legibility
  SINGLE_CONTEXT	    Use all L[3456] regs for a single context.
  ALLOW_ISR_LOADS	    Allow loads directly into ISR.
  NO_PARTWORD_LOAD	    Use word read & extract instead
  NO_PARTWORD_LOAD_WARNING  Don't issue compatibility warning.
  BIG_CONTEXTS
  NO_PARTWORD_STORE	    Use read-modify write instead.
  MIN_BRANCH_DELAY_1	    minimum number of ...
  MIN_BRANCH_DELAY_2	    ... branch delay slots
  MAX_INDIRECT_JUMP_DELAY_1 max number of delay slots for and indirect
  MAX_INDIRECT_JUMP_DELAY_2 ... jump ...
  MAX_INDIRECT_JUMP_DELAY_4 ...
  LD_DELAY_1		    Delay slots for LD insns ...
  LD_DELAY_2		    ...
  HAS_PS		    CPU has a PS register with visible flags
  HAS_ISR_IMR_IN_REG	    CPU has ISR and IMR in hard registers
  HAS_R28_TO_R31	    CPU has general purpose registers at R28-R31
  MAX_BRANCH_DELAY_1	    maximum number of branch delay slots
  MAX_BRANCH_DELAY_2	    ....
  HAS_SCC		    CPU has sCC set of instructions.
  WORKING_SUBB_F            SUBB.F insn properly sets Z bit
*/

#define _TARGET_ALLOW_ISR_LOADS			0x00000001
#define _TARGET_BIG_CONTEXTS			0x00000002
#define _TARGET_EXPLICIT_LD_STALLS_REQUIRED	0x00000004
#define _TARGET_HAS_ISR_IMR_IN_REG		0x00000008
#define _TARGET_HAS_PS				0x00000010
#define _TARGET_HAS_R28_TO_R31			0x00000020
#define _TARGET_HAS_SCC				0x00000040
#define _TARGET_LD_DELAY_1			0x00000080
#define _TARGET_LD_DELAY_2			0x00000100
#define _TARGET_MAX_BRANCH_DELAY_1		0x00000200
#define _TARGET_MAX_BRANCH_DELAY_2		0x00000400
#define _TARGET_MAX_INDIRECT_JUMP_DELAY_1	0x00000800
#define _TARGET_MAX_INDIRECT_JUMP_DELAY_2	0x00001000
#define _TARGET_MAX_INDIRECT_JUMP_DELAY_4	0x00002000
#define _TARGET_MIN_BRANCH_DELAY_1		0x00004000
#define _TARGET_MIN_BRANCH_DELAY_2		0x00008000
#define _TARGET_NOPS_AFTER_BRANCHES		0x00010000
#define _TARGET_NO_PARTWORD_LOAD		0x00020000
#define _TARGET_NO_PARTWORD_LOAD_WARNING	0x00040000
#define _TARGET_NO_PARTWORD_STORE		0x00080000
#define _TARGET_NO_PC_LOAD			0x00100000
#define _TARGET_OLD_VALUE_USABLE_IN_LD_DELAY	0x00200000
#define _TARGET_ONE_ALU				0x00400000
#define _TARGET_SINGLE_CONTEXT			0x00800000
#define _TARGET_WORKING_SUBB_F			0x01000000
#define _TARGET_BROKEN_SPLS_Q_NEG		0x02000000

/* The bits that should be set by a -m<version.number> option */

#define _TARGET_VERSION_SPECIFIC_BITS					\
(0									\
 | _TARGET_ALLOW_ISR_LOADS						\
 | _TARGET_BROKEN_SPLS_Q_NEG						\
 | _TARGET_EXPLICIT_LD_STALLS_REQUIRED					\
 | _TARGET_HAS_ISR_IMR_IN_REG						\
 | _TARGET_HAS_PS							\
 | _TARGET_HAS_R28_TO_R31						\
 | _TARGET_HAS_SCC							\
 | _TARGET_LD_DELAY_1							\
 | _TARGET_LD_DELAY_2							\
 | _TARGET_MAX_BRANCH_DELAY_1						\
 | _TARGET_MAX_BRANCH_DELAY_2						\
 | _TARGET_MAX_INDIRECT_JUMP_DELAY_1					\
 | _TARGET_MAX_INDIRECT_JUMP_DELAY_2					\
 | _TARGET_MAX_INDIRECT_JUMP_DELAY_4					\
 | _TARGET_MIN_BRANCH_DELAY_1						\
 | _TARGET_MIN_BRANCH_DELAY_2						\
 | _TARGET_NOPS_AFTER_BRANCHES						\
 | _TARGET_NO_PARTWORD_LOAD						\
 | _TARGET_NO_PARTWORD_STORE						\
 | _TARGET_NO_PC_LOAD							\
 | _TARGET_OLD_VALUE_USABLE_IN_LD_DELAY					\
 | _TARGET_ONE_ALU							\
 | _TARGET_WORKING_SUBB_F						\
 )

#define CLEAR_FOR_VERSION(v) (_TARGET_VERSION_SPECIFIC_BITS & ~(v))

/* Basic flavors of LANai processor output code */

#define __TARGET_3_0	(0						\
			 | _TARGET_EXPLICIT_LD_STALLS_REQUIRED		\
			 | _TARGET_HAS_ISR_IMR_IN_REG			\
			 | _TARGET_HAS_PS				\
			 | _TARGET_LD_DELAY_1				\
			 | _TARGET_MAX_BRANCH_DELAY_1			\
			 | _TARGET_MAX_INDIRECT_JUMP_DELAY_2		\
			 | _TARGET_MIN_BRANCH_DELAY_1			\
			 | _TARGET_NO_PARTWORD_LOAD			\
			 | _TARGET_OLD_VALUE_USABLE_IN_LD_DELAY		\
		         | _TARGET_NO_PC_LOAD				\
			 )
#define __TARGET_4_1	(0						\
			 | _TARGET_EXPLICIT_LD_STALLS_REQUIRED		\
			 | _TARGET_HAS_ISR_IMR_IN_REG			\
			 | _TARGET_HAS_PS				\
			 | _TARGET_LD_DELAY_1				\
			 | _TARGET_MAX_BRANCH_DELAY_1			\
			 | _TARGET_MAX_INDIRECT_JUMP_DELAY_2		\
			 | _TARGET_MIN_BRANCH_DELAY_1			\
			 | _TARGET_OLD_VALUE_USABLE_IN_LD_DELAY		\
		         | _TARGET_NO_PC_LOAD				\
			 )
#define __TARGET_7_0	(0						\
			 |_TARGET_ONE_ALU				\
			 | _TARGET_SINGLE_CONTEXT			\
			 | _TARGET_NO_PARTWORD_LOAD_WARNING		\
			 | _TARGET_LD_DELAY_2				\
			 | _TARGET_MIN_BRANCH_DELAY_2			\
			 | _TARGET_MAX_INDIRECT_JUMP_DELAY_4		\
			 | _TARGET_HAS_PS				\
			 | _TARGET_MAX_BRANCH_DELAY_2			\
			 | _TARGET_OLD_VALUE_USABLE_IN_LD_DELAY		\
			 | _TARGET_EXPLICIT_LD_STALLS_REQUIRED		\
			 )
#define __TARGET_8_0	(0						\
			 | _TARGET_EXPLICIT_LD_STALLS_REQUIRED		\
			 | _TARGET_HAS_PS				\
			 | _TARGET_HAS_R28_TO_R31			\
			 | _TARGET_LD_DELAY_2				\
			 | _TARGET_MAX_BRANCH_DELAY_2			\
			 | _TARGET_MAX_INDIRECT_JUMP_DELAY_4		\
			 | _TARGET_MIN_BRANCH_DELAY_2			\
			 | _TARGET_NO_PARTWORD_LOAD_WARNING		\
			 | _TARGET_OLD_VALUE_USABLE_IN_LD_DELAY		\
			 | _TARGET_ONE_ALU				\
			 | _TARGET_SINGLE_CONTEXT			\
			 | _TARGET_WORKING_SUBB_F			\
			 )
#define __TARGET_9_0	(0						\
			 | _TARGET_BROKEN_SPLS_Q_NEG			\
			 | _TARGET_HAS_R28_TO_R31			\
			 | _TARGET_HAS_SCC				\
			 | _TARGET_LD_DELAY_2				\
			 | _TARGET_MAX_BRANCH_DELAY_1			\
			 | _TARGET_MAX_INDIRECT_JUMP_DELAY_2		\
			 | _TARGET_MIN_BRANCH_DELAY_1			\
			 | _TARGET_NO_PARTWORD_LOAD_WARNING		\
			 | _TARGET_ONE_ALU				\
			 | _TARGET_SINGLE_CONTEXT			\
			 | _TARGET_WORKING_SUBB_F			\
			 )
#define __TARGET_9_1	(0						\
			 | _TARGET_HAS_R28_TO_R31			\
			 | _TARGET_HAS_SCC				\
			 | _TARGET_LD_DELAY_2				\
			 | _TARGET_MAX_BRANCH_DELAY_1			\
			 | _TARGET_MAX_INDIRECT_JUMP_DELAY_2		\
			 | _TARGET_MIN_BRANCH_DELAY_1			\
			 | _TARGET_NO_PARTWORD_LOAD_WARNING		\
			 | _TARGET_ONE_ALU				\
			 | _TARGET_SINGLE_CONTEXT			\
			 | _TARGET_WORKING_SUBB_F			\
			 )
#define __TARGET_ANY    (0						\
			 | _TARGET_BROKEN_SPLS_Q_NEG			\
			 | _TARGET_EXPLICIT_LD_STALLS_REQUIRED		\
			 | _TARGET_LD_DELAY_2				\
			 | _TARGET_MAX_BRANCH_DELAY_2			\
			 | _TARGET_MAX_INDIRECT_JUMP_DELAY_4		\
			 | _TARGET_MIN_BRANCH_DELAY_1			\
			 | _TARGET_NOPS_AFTER_BRANCHES			\
			 | _TARGET_NO_PARTWORD_LOAD			\
			 | _TARGET_NO_PARTWORD_LOAD_WARNING		\
			 | _TARGET_NO_PARTWORD_STORE			\
			 | _TARGET_NO_PC_LOAD				\
			 )

/* Mapping from specific type to basic type. */

#define _TARGET_3_0	__TARGET_3_0
#define _TARGET_3_1	__TARGET_3_0
#define _TARGET_3_2	__TARGET_3_0
#define _TARGET_4_0	__TARGET_3_0
#define _TARGET_4_1	__TARGET_4_1
#define _TARGET_5_0	__TARGET_4_1
#define _TARGET_6_0	__TARGET_4_1
#define _TARGET_7_0	__TARGET_7_0
#define _TARGET_8_0	__TARGET_8_0
#define _TARGET_9_0	__TARGET_9_0
#define _TARGET_9_1	__TARGET_9_1
#define _TARGET_ANY	__TARGET_ANY

#define TARGET_DEFAULT _TARGET_ANY

/*
`TARGET_...'
     This series of macros is to allow compiler command arguments to
     enable or disable the use of optional features of the target
     machine.  For example, one machine description serves both the
     68000 and the 68020; a command argument tells the compiler whether
     it should use 68020-only instructions or not.  This command
     argument works by means of a macro `TARGET_68020' that tests a bit
     in `target_flags'.

     Define a macro `TARGET_FEATURENAME' for each such option.  Its
     definition should test a bit in `target_flags'; for example:

          #define TARGET_68020 (target_flags & 1)

     One place where these macros are used is in the
     condition-expressions of instruction patterns.  Note how
     `TARGET_68020' appears frequently in the 68000 machine description
     file, `m68k.md'.  Another place they are used is in the
     definitions of the other macros in the `MACHINE.h' file.
*/
/* This declaration should be present. */
extern int target_flags;

#define TFS(foo) (target_flags & (foo))	/* target flag set */

#define TARGET_allow_isr_loads		TFS (_TARGET_ALLOW_ISR_LOADS)
#define TARGET_big_contexts		TFS (_TARGET_BIG_CONTEXTS)
#define TARGET_explicit_ld_stalls_required				\
(TFS (_TARGET_EXPLICIT_LD_STALLS_REQUIRED))
#define TARGET_has_isr_imr_in_reg	TFS (_TARGET_HAS_ISR_IMR_IN_REG)
#define TARGET_has_ps			TFS (_TARGET_HAS_PS)
#define TARGET_has_r28_to_r31		TFS (_TARGET_HAS_R28_TO_R31)
#define TARGET_has_scc			TFS (_TARGET_HAS_SCC)
#define TARGET_no_partword_load 	TFS (_TARGET_NO_PARTWORD_LOAD)
#define TARGET_no_partword_load_warning	TFS (_TARGET_NO_PARTWORD_LOAD_WARNING)
#define TARGET_no_partword_store 	TFS (_TARGET_NO_PARTWORD_STORE)
#define TARGET_no_pc_load  		TFS (_TARGET_NO_PC_LOAD)
#define TARGET_nops_after_branches	TFS (_TARGET_NOPS_AFTER_BRANCHES)
#define TARGET_old_value_usable_in_ld_delay				\
(TFS (_TARGET_OLD_VALUE_USABLE_IN_LD_DELAY))
#define TARGET_one_alu     		TFS (_TARGET_ONE_ALU)
#define TARGET_single_context		TFS (_TARGET_SINGLE_CONTEXT)
#define TARGET_working_subb_f		TFS (_TARGET_WORKING_SUBB_F)
#define TARGET_broken_spls_q_neg	TFS (_TARGET_BROKEN_SPLS_Q_NEG)

/* bitfield extraction */

#define TARGET_ld_delay							\
((target_flags & (_TARGET_LD_DELAY_1 | _TARGET_LD_DELAY_2))		\
 / _TARGET_LD_DELAY_1)

#define TARGET_min_branch_delay						\
((target_flags & (_TARGET_MIN_BRANCH_DELAY_1				\
		  | _TARGET_MIN_BRANCH_DELAY_2))			\
 / _TARGET_MIN_BRANCH_DELAY_1)

#define TARGET_max_branch_delay						\
((target_flags & (_TARGET_MAX_BRANCH_DELAY_1				\
		  | _TARGET_MAX_BRANCH_DELAY_2))			\
 / _TARGET_MAX_BRANCH_DELAY_1)

#define TARGET_max_indirect_jump_delay					\
((unsigned) (target_flags & (_TARGET_MAX_INDIRECT_JUMP_DELAY_1		\
			     | _TARGET_MAX_INDIRECT_JUMP_DELAY_2	\
			     | _TARGET_MAX_INDIRECT_JUMP_DELAY_4	\
			     ))						\
 / _TARGET_MAX_INDIRECT_JUMP_DELAY_1)

/* Macro to define tables used to set the flags.
   This is a list in braces of pairs in braces,
   each pair being { "NAME", VALUE }
   where VALUE is the bits to set or minus the bits to clear.
   An empty string NAME is used to identify the default VALUE.

   Note that multiple matches are allowed per name, allowing some bits
   to be set and others cleared using two machines. */
#define TARGET_SWITCHES							\
{									\
  {"one-alu", _TARGET_ONE_ALU, ""},					\
  {"two-alus", -_TARGET_ONE_ALU, ""},					\
									\
  {"no-pc-load", _TARGET_NO_PC_LOAD, ""},				\
  {"pc-load", -_TARGET_NO_PC_LOAD, ""},					\
  {"nops-after-branches", _TARGET_NOPS_AFTER_BRANCHES, ""},		\
  {"no-extra-nops", -_TARGET_NOPS_AFTER_BRANCHES, ""},			\
									\
  {"single-context", _TARGET_SINGLE_CONTEXT, ""},			\
  {"single-context", -_TARGET_BIG_CONTEXTS, ""},			\
  {"dual-context", -_TARGET_SINGLE_CONTEXT, ""},			\
  {"big-contexts", _TARGET_BIG_CONTEXTS, ""},				\
  {"big-contexts", -_TARGET_SINGLE_CONTEXT, ""},			\
									\
  {"allow-isr-loads", _TARGET_ALLOW_ISR_LOADS, ""},			\
  {"isr-load", _TARGET_ALLOW_ISR_LOADS, ""},				\
  {"no-isr-load", -_TARGET_ALLOW_ISR_LOADS, ""},			\
									\
  {"no-partword-load", _TARGET_NO_PARTWORD_LOAD, ""},			\
  {"partword-load", -_TARGET_NO_PARTWORD_LOAD, ""},			\
									\
  {"no-partword-store", _TARGET_NO_PARTWORD_STORE, ""},			\
  {"partword-store", -_TARGET_NO_PARTWORD_STORE, ""},			\
									\
  {"partword-load-warning", -_TARGET_NO_PARTWORD_LOAD_WARNING, ""},	\
  {"no-partword-load-warning", _TARGET_NO_PARTWORD_LOAD_WARNING, ""},	\
									\
  {"3.0", _TARGET_3_0, ""},						\
  {"3.0", -CLEAR_FOR_VERSION (_TARGET_3_0), ""},			\
									\
  {"3.1", _TARGET_3_1, ""},						\
  {"3.1", -CLEAR_FOR_VERSION (_TARGET_3_1), ""},			\
									\
  {"3.2", _TARGET_3_2, ""},						\
  {"3.2", -CLEAR_FOR_VERSION (_TARGET_3_2), ""},			\
									\
  {"4.0", _TARGET_4_0, ""},						\
  {"4.0", -CLEAR_FOR_VERSION (_TARGET_4_0), ""},			\
									\
  {"4.1", _TARGET_4_1, ""},						\
  {"4.1", -CLEAR_FOR_VERSION (_TARGET_4_1), ""},			\
									\
  {"5.0", _TARGET_5_0, ""},						\
  {"5.0", -CLEAR_FOR_VERSION (_TARGET_5_0), ""},			\
									\
  {"6.0", _TARGET_6_0, ""},						\
  {"6.0", -CLEAR_FOR_VERSION (_TARGET_6_0), ""},			\
									\
  {"7.0", _TARGET_7_0, ""},						\
  {"7.0", -CLEAR_FOR_VERSION (_TARGET_7_0), ""},			\
									\
  {"8.0", _TARGET_8_0, ""},						\
  {"8.0", -CLEAR_FOR_VERSION (_TARGET_8_0), ""},			\
									\
  {"9.0", _TARGET_9_0, ""},						\
  {"9.0", -CLEAR_FOR_VERSION (_TARGET_9_0), ""},			\
									\
  {"9.1", _TARGET_9_1, ""},						\
  {"9.1", -CLEAR_FOR_VERSION (_TARGET_9_1), ""},			\
									\
  /* "-mb" alias for "-m9.0" in honor of Martin Benes, the designer */	\
  /* of the LANai9 prefetch circuitry. */				\
  {"b", _TARGET_9_0, ""},						\
  {"b", -CLEAR_FOR_VERSION (_TARGET_9_0), ""},				\
									\
  {"any", _TARGET_ANY, ""},						\
  {"any", -(_TARGET_SINGLE_CONTEXT | _TARGET_BIG_CONTEXTS), ""},	\
  {"any", -CLEAR_FOR_VERSION (_TARGET_ANY), ""},			\
									\
  {"", TARGET_DEFAULT, ""}						\
}

/* This macro is similar to `TARGET_SWITCHES' but defines names of
   command options that have values.  Its definition is an
   initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   fixed part of the option name, and the address of a variable.  The
   variable, type `char *', is set to the variable part of the given
   option if the fixed part matches.  The actual option name is made
   by appending `-m' to the specified name.  */
#define TARGET_OPTIONS							\
{ { "regparm=",		&lanai_regparm_string,				\
      N_ ("Number of registers used to pass integer arguments")},	\
}

/* Sometimes certain combinations of command options do not make
   sense on a particular target machine.  You can define a macro
   `OVERRIDE_OPTIONS' to take account of this.  This macro, if
   defined, is executed once just after all the command options have
   been parsed.

   Don't use this macro to turn on various extra optimizations for
   `-O'.  That is what `OPTIMIZATION_OPTIONS' is for.  */

#define OVERRIDE_OPTIONS override_options ()

/*   Define this macro if GNU CC should generate calls to the System V
     (and ANSI C) library functions `memcpy' and `memset' rather than
     the BSD functions `bcopy' and `bzero'. */

#define TARGET_MEM_FUNCTIONS 1

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers. */

#define FIRST_PSEUDO_REGISTER 32

/* Update FIXED_REGS to reflect any options specified by the user. */

#define CONDITIONAL_REGISTER_USAGE do {					\
/*  fixed_regs[0] = 1; */						\
/*  fixed_regs[1] = 1; */						\
/*  fixed_regs[2] = 1; */	/* pc */				\
/*  fixed_regs[3] = 1; */       /* ps --- yes, fixed even on LANai9 */	\
/*  fixed_regs[4] = 1; */	/* sp */				\
/*  fixed_regs[5] = 1; */	/* fp */				\
/*  fixed_regs[6] = 0; */						\
/*  fixed_regs[7] = 0; */						\
/*  fixed_regs[8] = 0; */						\
/*  fixed_regs[9] = 0; */						\
/*  fixed_regs[10] = 0; */						\
/*  fixed_regs[11] = 0; */						\
/*  fixed_regs[12] = 0; */						\
/*  fixed_regs[13] = 0; */						\
/*  fixed_regs[14] = 0; */						\
/*  fixed_regs[15] = 0; */						\
fixed_regs[16] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[17] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[18] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[19] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[20] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[21] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[22] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[23] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[24] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[25] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[26] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[27] = !TARGET_single_context	&& !TARGET_big_contexts;	\
fixed_regs[28] = !TARGET_has_r28_to_r31;				\
fixed_regs[29] = !TARGET_has_r28_to_r31;				\
fixed_regs[30] = !TARGET_has_r28_to_r31;				\
fixed_regs[31] = !TARGET_has_r28_to_r31;				\
} while (0)


/* target machine storage layout */

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields. */
#define BITS_BIG_ENDIAN 0

/* Define this if most significant byte of a word is the lowest numbered.  */
#define BYTES_BIG_ENDIAN 1

/* Define this if most significant word of a multiword number is the lowest
   numbered.  */
/* NOTE: GCC probably cannot support a big-endian i860
   because GCC fundamentally assumes that the order of words
   in memory as the same as the order in registers.
   That's not true for the big-endian i860.
   The big-endian i860 isn't important enough to
   justify the trouble of changing this assumption.  */
#define WORDS_BIG_ENDIAN 1

#define FLOAT_WORDS_BIG_ENDIAN 1

/* #define BITS_PER_UNIT default */
/* #define BITS_PER_WORD default */

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD 4

#if 0
/* Width in bits of a pointer.
   See also the macro `Pmode' defined below.  */
#define POINTER_SIZE 32
#endif

/*   A macro to update M and UNSIGNEDP when an object whose type is
     TYPE and which has the specified mode and signedness is to be
     stored in a register.  This macro is only called when TYPE is a
     scalar type.

     On most RISC machines, which only have operations that operate on
     a full register, define this macro to set M to `word_mode' if M is
     an integer mode narrower than `BITS_PER_WORD'.  In most cases,
     only integer modes should be widened because wider-precision
     floating-point operations are usually more expensive than their
     narrower counterparts.

     For most machines, the macro definition does not change UNSIGNEDP.  */
#define PROMOTE_MODE(M, UNSIGNEDP, TYPE) \
	(M=(GET_MODE_SIZE(M) < UNITS_PER_WORD \
	    && GET_MODE_CLASS(M) == MODE_INT ? word_mode : (M)))

/*   Define this macro if the promotion described by `PROMOTE_MODE'
     should also be done for outgoing function arguments. */
#define PROMOTE_FUNCTION_ARGS 1

/*   Define this macro if the promotion described by `PROMOTE_MODE'
     should also be done for the return value of functions.

     If this macro is defined, `FUNCTION_VALUE' must perform the same
     promotions done by `PROMOTE_MODE'.*/
#define PROMOTE_FUNCTION_RETURN 1

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY 32

/* Boundary (in *bits*) on which stack pointer should be aligned.  */
#define STACK_BOUNDARY 32

/* Allocation boundary (in *bits*) for the code of a function.  */
#define FUNCTION_BOUNDARY 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 32

/* Every structure's size must be a multiple of this.  */
#define STRUCTURE_SIZE_BOUNDARY 8

/* Minimum size in bits of the largest boundary to which any
   and all fundamental data types supported by the hardware
   might need to be aligned. No data type wants to be aligned
   rounder than this. */
#define BIGGEST_ALIGNMENT 64

/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* If bit field type is int, don't let it cross an int,
   and give entire struct the alignment of an int.  */
#define PCC_BITFIELD_TYPE_MATTERS 1

/*   An integer expression for the size in bits of the largest integer
     machine mode that should actually be used.  All integer machine
     modes of this size or smaller can be used for structures and
     unions with the appropriate sizes.  If this macro is undefined,
     `GET_MODE_BITSIZE (DImode)' is assumed. */
/*
#define MAX_FIXED_MODE_SIZE (GET_MODE_BITSIZE(SImode))
*/


/* Standard register usage.  */

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.

   Here we mark only the registers that have pervasive uses on all
   flavors of LANai processor AND the condition code register (CCR),
   which does not exist on the LANai9, because the CCR is treated
   specially by the compiler, and removing this register (register 3)
   from FIXED_REGS caused all sorts of trouble.  */

#define FIXED_REGISTERS							\
 {1, 1, 1, 1, 1, 1, 0, 0,						\
  0, 0, 0, 0, 0, 0, 0, 1,						\
  0, 0, 0, 0, 0, 0, 0, 0,						\
  0, 0, 0, 0, 0, 0, 0, 0}

/* Define this macro if function calls on the target machine do not
   preserve any registers; in other words, if `CALL_USED_REGISTERS'
   has 1 for all registers.  This macro enables `-fcaller-saves' by
   default.  Eventually that option will be enabled by default on all
   machines and both the option and this macro will be eliminated. */

#define DEFAULT_CALLER_SAVES 1

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   Uses:
   r0: zero register
   r1: ones register
   r2: pc
   r3: psw
   r4: sp  (stack pointer)
   r5: fp  (frame pointer)
   r6: argument pointer
   r7: return addr
   r8: static chain and return value
   r9: struct value
   r15: used for constant addrs
*/

#define CALL_USED_REGISTERS \
 {1, 1, 1, 1, 1, 1, 1, 1,   \
  1, 1, 1, 1, 1, 1, 1, 1,   \
  1, 1, 1, 1, 1, 1, 1, 1,   \
  1, 1, 1, 1, 1, 1, 1, 1}

/* Try to get a non-preserved register before trying to get one we will
   have to preserve. By default, registers are allocated in numerical
   order if this macros is undefined. */
#define REG_ALLOC_ORDER			\
 {31, 30, 29, 28, 27, 26, 25, 24,	\
  23, 22, 21, 20, 19, 18, 17, 16,	\
  15, 14, 13, 12, 11, 10,  9,  8,	\
   7,  6,  5,  4,  3,  2,  1,  0}

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers. */

#define HARD_REGNO_NREGS(REGNO, MODE)  \
	(((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD))

/* ?? */
/* #define REGNO_MODE_ALIGNED(REGNO, MODE) \
  (((REGNO) % ((GET_MODE_UNIT_SIZE (MODE) + 3) / 4)) == 0)
*/

/* Value is 1 if hard register REGNO can hold a value of machine-mode MODE.
   - On the LANai3, any type can go in any general-purpose register.  However,
   CCmode can only go in register 3.  The value here should not matter
   for the FIXED_REGISTERS */
#define HARD_REGNO_MODE_OK(REGNO, MODE) ((HOST_WIDE_INT) (32					\
						- (int) HARD_REGNO_NREGS (REGNO, MODE))	\
					 >= (HOST_WIDE_INT) REGNO)
/*    (GET_MODE_CLASS (MODE) == MODE_CC ? (REGNO) == 3 : (REGN0) > 3) */

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2) \
    (GET_MODE_CLASS (MODE1) == MODE_INT && GET_MODE_CLASS (MODE2) == MODE_INT)

/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */
#define PC_REGNUM 2

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM 4

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM 5

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED 1

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

#define RETURN_VALUE_REGNUM 8

/* LANai specific #define: function return address register. */
#define RETURN_ADDRESS_REGNUM 7

/* Register in which static-chain is passed to a function. */
/* Used to support Gnu's nested functions extension to C. */
#define STATIC_CHAIN_REGNUM 6

/* Register in which address to store a structure value
   is passed to a function.  */
/* #define STRUCT_VALUE_REGNUM 9 */

/* Notes:  By default (if DEFAULT_PCC_STRUCT_RETURN is not defined)
   structs are passed in memory.  The address of the struct is passed
   in STRUCT_VALUE_REGNUM unless STRUCT_VALUE is define, which tells
   the compiler where to pass the structure address. (0 means
   as an invisible 1st argument). */

#define STRUCT_VALUE 0

/* LANai specific #define: register to use to assemble constant
   addresses larger than 16 bits */
#define CONST_ADDR_REG 15

/* Interrupt mask register */
#define IMR_REGNUM 30

/* Interrupt status register */
#define ISR_REGNUM 31
#define IS_ISR(X) (GET_CODE (X) == REG					\
		   && TARGET_has_isr_imr_in_reg				\
		   && REGNO (X) == ISR_REGNUM)

/* Process status register number */
#define PS_REGNUM 3


/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS
   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.  */

/* The LANai3.0 has only one type of regs. */

enum reg_class
{ NO_REGS, ALL_REGS, LIM_REG_CLASSES };
#define GENERAL_REGS ALL_REGS

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.   */
#define REG_CLASS_NAMES \
 {"NO_REGS", "ALL_REGS" }

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */

#define REG_CLASS_CONTENTS {{0},{0xffffffff}}

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define REGNO_REG_CLASS(REGNO) (REGNO == PS_REGNUM			\
				? NO_REGS				\
				: GENERAL_REGS)

/* The class value for index registers, and the one for base regs.  */
#define INDEX_REG_CLASS GENERAL_REGS
#define BASE_REG_CLASS GENERAL_REGS

/* Get reg_class from a letter such as appears in the machine description.  */

/* For the LANai3, we don't introduce any new reg classes */
#define REG_CLASS_FROM_LETTER(C) NO_REGS

/****************
 * CONST_OK_FOR_LETTER_P
 ****************/

/* The letters I, J, K, L and M in a register constraint string can be
   used to stand for particular ranges of immediate operands.  This
   macro defines what the ranges are.  C is the letter, and VALUE is a
   constant value.  Return 1 if VALUE is in the range specified by
   C.

   We define CONST_OK_FOR_LETTER_P here in terms of other macros,
   which we also use in lanai3.c and lanai3.md.  We must be very
   careful to define these macros so they work on host machines with
   registers larger than 32 bits. */

#define SIGNED_INT(X,BITS) (-(1<<((BITS)-1)) <= (int)(X)		\
			    && (int)(X) < (1<<((BITS)-1)))
#define UNSIGNED_INT(X,BITS) (((int)(X) & ((1<<(BITS))-1)) == (int)(X))

/* LOGIC_INT consts have bits in only one halfword of a 32-bit word,
   and can be use in logical RI insns other than AND. */
#define LOGIC_INT(X) (((int)(X) & 0x0000ffff) == 0			\
		      || ((X) & 0xffff0000) == 0)

/* AND_INT consts have 1 halfword = 0xffff, and can be used in AND RI insns */
#define AND_INT(X) (((int)(X) | 0x0000ffff) == (int)(X)			\
		    || ((int)(X) | (int) 0xffff0000) == (int)(X))

/* an ARITH_INT is any int that can be used directly in an ADD/SUB RI
   insn or negated and used in a complementary SUB/ADD */
#define ARITH_INT(X) (LOGIC_INT (X) || LOGIC_INT (-(X)))

/* a SLI_INT int is unsigned 21 bits for SLI insns */
#define SLI_INT(X) UNSIGNED_INT (X, 21)

/* a SLS_INT int is signed 21 bits for SLS insns */
#define SLS_INT(X) SIGNED_INT (X, 21)

/* a SPLS_INT int is signed 10 bits for SPLS insns */
#define SPLS_INT(X) SIGNED_INT (X, 10)

/* a BR_INT int is unsigned 25 bits for absolute BR insns */
#define BR_INT(X) UNSIGNED_INT (X, 25)

/* a BR_R_INT int is signed 25 bits for relative BR insns */
#define BR_R_INT(X) SIGNED_INT (X, 25)

/* a SHIFT_INT is 6 bits signed SHIFT RR insns */
#define SHIFT_INT(X) SIGNED_INT (X, 6)

/* an RM_INT is 16 bits signed for RM insns */
#define RM_INT(X) SIGNED_INT (X, 16)

/* J = a constant that can be used in an ADD or logic immediate insn
   K = any constant that can be moved into a register in 1 instuction.
   L = the negative of a constant that can be used in an ADD.
   M = A constant that can be used in a SHIFT immediate insn.
   N = a constant that can be used in an AND immediate insn.
   O = the constant 0. */

#define CONST_OK_FOR_LETTER_P(VALUE, C)					\
     (  (C) == 'J' ? LOGIC_INT (VALUE)					\
      : (C) == 'K' ? SLI_INT (VALUE) || ARITH_INT (VALUE)		\
      : (C) == 'L' ? LOGIC_INT (-(VALUE)&0xffffffff)			\
      : (C) == 'M' ? SHIFT_INT (VALUE)					\
      : (C) == 'N' ? AND_INT (VALUE)					\
      : (C) == 'O' ? (VALUE) == 0 || ((VALUE)&0xffffffff) == 0xffffffff	\
      : 0)

/* Return non-zero if the given VALUE is acceptable for the
   constraint letter C.  The LANai3.0 does not
   support doubles */
#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C) 0

/*   A C expression that defines the optional machine-dependent
     constraint letters that can be used to segregate specific types of
     operands, usually memory references, for the target machine.
     Normally this macro will not be defined.  If it is required for a
     particular target machine, it should return 1 if VALUE corresponds
     to the operand type represented by the constraint letter C.  If C
     is not defined as an extra constraint, the value returned should
     be 0 regardless of VALUE. */
/* #define EXTRA_CONSTRAINT(VALUE,C) \
    ( (C) == 'Q' ? GET_CODE(VALUE) == MEM \
      && call_insn_operand(VALUE,FUNCTION_MODE) \
      : 0 ) */

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class.  */
#define PREFERRED_RELOAD_CLASS(X,CLASS)  CLASS

/* Return the register class of a scratch register needed to copy IN into
   a register in CLASS in MODE.  If it can be done directly, NO_REGS is
   returned.  */
/* On the lanai3, a scratch register is required for byte and halfword
   reloads if partword loads are disabled. */
/* #define SECONDARY_INPUT_RELOAD_CLASS(CLASS,MODE,IN) \
     (TARGET_no_partword_load && CLASS == MODE_INT && \
      ((MODE) == QImode || (MODE) == HImode) ? GENERAL_REGS : NO_REGS) */

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */
#define CLASS_MAX_NREGS(CLASS, MODE)	\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/************************
 ** Basic Stack Layout **
 ************************/

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Define this if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD 1

/*   Define this macro if successive arguments to a function occupy
     decreasing addresses on the stack. */
/* #define ARGS_GROW_DOWNWARD */

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
#define STARTING_FRAME_OFFSET -8

/* STACK_POINTER_OFFSET 0*/

/* Offset of first parameter from the argument pointer register value.  */
#define FIRST_PARM_OFFSET(FNDECL) 0

/*   Define this macro if an argument declared in a prototype as an
     integral type smaller than `int' should actually be passed as an
     `int'.  In addition to avoiding errors in certain cases of
     mismatch, it also makes for better code on certain machines.*/
#define PROMOTE_PROTOTYPES 1

/* If we generate an insn to push BYTES bytes,
   this says how many the stack pointer really advances by. */

/* #define PUSH_ROUNDING(BYTES) (BYTES) */

/*   If defined, the maximum amount of space required for outgoing
     arguments will be computed and placed into the variable
     `current_function_outgoing_args_size'.  No space will be pushed
     onto the stack for each call; instead, the function prologue should
     increase the stack frame size by this amount.

     Defining both `PUSH_ROUNDING' and `ACCUMULATE_OUTGOING_ARGS' is
     not proper. */

#if 0
/* This is disabled because it triggers a bug in the compiler core that
   causes libcalls to corrupt the stack in some cases on this architecture.
   I believe this bug is not known to the GCC maintainers because lanai-gcc
   is the only architecture that attempts to ACCUMULATE_OUTGOING_ARGS while
   passing libcall arguments on the stack. */
#define ACCUMULATE_OUTGOING_ARGS 1
#endif

/* Value is the number of bytes of arguments automatically
   popped when returning from a subroutine call.
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name.
   SIZE is the number of bytes of arguments passed on the stack.  */
#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) 0

/*      Define this macro if the function epilogue contains delay slots to
     which instructions from the rest of the function can be "moved".
     The definition should be a C expression whose value is an integer
     representing the number of delay slots there.
*/
/* #define DELAY_SLOTS_FOR_EPILOGUE 0 */

/*#define ELIGIBLE_FOR_EPILOGUE_DELAY (INSN, N) */

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */
/* return all values in the RETURN_VALUE_REGNUM */
/* Must do same type of promotion as PROMOTE_FUNCTION_RETURN for scalar types*/

#if 1
/* Promote integer return values. */
#define FUNCTION_VALUE(VALTYPE, FUNC)   \
    (GET_MODE_CLASS (TYPE_MODE (VALTYPE)) == MODE_INT \
     && GET_MODE_SIZE (TYPE_MODE (VALTYPE)) < UNITS_PER_WORD \
     ? gen_rtx (REG, word_mode          , RETURN_VALUE_REGNUM) \
     : gen_rtx (REG, TYPE_MODE (VALTYPE), RETURN_VALUE_REGNUM))
#endif

#if 0
#define FUNCTION_VALUE(VALTYPE, FUNC)   \
  gen_rtx (REG, TYPE_MODE (VALTYPE), RETURN_VALUE_REGNUM )
#endif

#if 0
/* modified from alpha.h */
#define FUNCTION_VALUE(VALTYPE, FUNC)   \
  gen_rtx (REG,                                         \
           ((TREE_CODE (VALTYPE) == INTEGER_TYPE        \
             || TREE_CODE (VALTYPE) == ENUMERAL_TYPE    \
             || TREE_CODE (VALTYPE) == BOOLEAN_TYPE     \
             || TREE_CODE (VALTYPE) == CHAR_TYPE        \
             || TREE_CODE (VALTYPE) == POINTER_TYPE     \
             || TREE_CODE (VALTYPE) == OFFSET_TYPE)     \
            && TYPE_PRECISION (VALTYPE) < BITS_PER_WORD) \
           ? word_mode : TYPE_MODE (VALTYPE),           \
           RETURN_VALUE_REGNUM)
#endif

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */
#define LIBCALL_VALUE(MODE) \
	gen_rtx (REG, MODE, RETURN_VALUE_REGNUM)

/* 1 if N is a possible register number for a function value
   as seen by the caller.  */

/* BAD: If this is the same as the fp, then it can be eliminated. */
#define FUNCTION_VALUE_REGNO_P(N) ((N) == RETURN_VALUE_REGNUM)

/* 1 if N is a possible register number for function argument passing. */
#define FUNCTION_ARG_REGNO_P(N) (6 <= (N) && (N) <= (6 + REGPARM_MAX))


/* Define a data type for recording info about an argument list
   during the scan of that argument list.  This data type should
   hold all necessary information about the function itself
   and about the args processed so far, enough to enable macros
   such as FUNCTION_ARG to determine where the next arg should go. */

struct lanai_cumulative_args {int pos; int limit;};
#define CUMULATIVE_ARGS struct lanai_cumulative_args

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.

     There is no need to record in `CUMULATIVE_ARGS' anything about the
     arguments that have been passed on the stack.  The compiler has
     other variables to keep track of that.  For target machines on
     which all arguments are passed on the stack, there is no need to
     store anything in `CUMULATIVE_ARGS'; however, the data structure
     must exist and should not be empty, so use `int'.
*/

#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,ignored) do {		\
  (CUM).pos = 0;							\
  (CUM).limit = 4*lanai_regparm;					\
  if (FNTYPE)								\
    {									\
      tree attr								\
	= lookup_attribute ("regparm", TYPE_ATTRIBUTES (FNTYPE));	\
									\
      if (attr)								\
	{								\
	  (CUM).limit							\
	    = 4 * TREE_INT_CST_LOW (TREE_VALUE (TREE_VALUE (attr)));	\
	}								\
    }									\
} while (0)

/* Machine-specific subroutines of the following macros.  */
#define CEILING(X,Y)  (((X) + (Y) - 1) / (Y))
#define ROUNDUP(X,Y)  (CEILING ((X), (Y)) * (Y))

/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)
   Floats, and doubleword ints, are returned in f regs;
   other ints, in r regs.
   Aggregates, even short ones, are passed in memory.  */

#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)			\
 ((TYPE) != 0 && (TREE_CODE ((TYPE)) == RECORD_TYPE			\
		  || TREE_CODE ((TYPE)) == UNION_TYPE)			\
  ? 0									\
  : GET_MODE_CLASS ((MODE)) == MODE_INT					\
  ? ((CUM).pos = (ROUNDUP ((CUM).pos, GET_MODE_SIZE ((MODE)))		\
		  + ROUNDUP (GET_MODE_SIZE (MODE), 4)))			\
  : 0)

/* Determine where to put an argument to a function.
   Value is zero to push the argument on the stack,
   or a hard register in which to store the argument.

   MODE is the argument's machine mode.
   TYPE is the data type of the argument (as a tree).
    This is null for libcalls where that information may
    not be available.
   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   NAMED is nonzero if this argument is a named parameter
    (otherwise it is an extra parameter matching an ellipsis).  */

/* On the lanai3, everything is passed on the stack
   NOTE: The i860 passed some args in registers. */
/* BAD?  Haven't checked if this works */
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED)				\
 ((TYPE) != 0 && (TREE_CODE ((TYPE)) == RECORD_TYPE			\
		  || TREE_CODE ((TYPE)) == UNION_TYPE)			\
  ? 0									\
  : (GET_MODE_CLASS ((MODE)) == MODE_INT				\
     && (CUM).pos + GET_MODE_SIZE((MODE)) <= (CUM).limit)		\
  ? gen_rtx_REG ((MODE), 6 + (CUM).pos/4)				\
  : 0)

/* For an arg passed partly in registers and partly in memory,
   this is the number of registers used.
   For args passed entirely in registers or entirely in memory, zero.  */

#define FUNCTION_ARG_PARTIAL_NREGS(CUM, MODE, TYPE, NAMED) 0

/* If defined, a C expression that gives the alignment boundary, in
   bits, of an argument with the specified mode and type.  If it is
   not defined,  `PARM_BOUNDARY' is used for all arguments.  */
/* #define FUNCTION_ARG_BOUNDARY(MODE, TYPE) */

/* BAD: I don't know that this is needed on the LANai3 */
/* Output a no-op just before the beginning of the function,
   to ensure that there does not appear to be a delayed branch there.
   Such a thing would confuse interrupt recovery.  */
#define ASM_OUTPUT_FUNCTION_PREFIX(FILE,NAME) \
  fprintf (FILE, "\t.align 4\n")

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */
#define FUNCTION_PROFILER(FILE, LABELNO)  \
   abort ();

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */
#define EXIT_IGNORE_STACK 1

/* Store in the variable DEPTH the initial difference between the
   frame pointer reg contents and the stack pointer reg contents,
   as of the start of the function body.  This depends on the layout
   of the fixed parts of the stack frame and on how registers are saved.

   On the i860, FRAME_POINTER_REQUIRED is always 1, so the definition of this
   macro doesn't matter.  But it must be defined.  */

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH) \
  do { (DEPTH) = 0; } while (0)


/*****************
 ** Trampolines **		NOT USED
 *****************/

/*	Trampolines are only required in languages like Pascal
and Algol that include nested functions.  C and C++ do not need them.
They must be defined, however.
*/

/* Output assembler code for a block containing the constant parts
   of a trampoline, leaving space for the variable parts.  */
/* On the LANai3.0, the trampoline contains four instructions:
	add	%r0,#TOP_OF_STATIC,%r6
	add	%r0,#TOP_OF_FUNCTION,%rca
	add	%rca,#BOTTOM_OF_FUNCTION,%pc
	add	%r6,#BOTTOM_OF_STATIC,r6
*/

#define TRAMPOLINE_TEMPLATE(FILE)                                   \
{                                                                   \
  ASM_OUTPUT_INT (FILE, gen_rtx (CONST_INT, VOIDmode, 0x03010000)); \
  ASM_OUTPUT_INT (FILE, gen_rtx (CONST_INT, VOIDmode, 0x07810000)); \
  ASM_OUTPUT_INT (FILE, gen_rtx (CONST_INT, VOIDmode, 0x013c0000)); \
  ASM_OUTPUT_INT (FILE, gen_rtx (CONST_INT, VOIDmode, 0x03180000)); \
}

/* Length in units of the trampoline for entering a nested function.  */
#define TRAMPOLINE_SIZE 16

/* Emit RTL insns to initialize the variable parts of a trampoline.
   FNADDR is an RTX for the address of the function's pure code.
   CXT is an RTX for the static chain value for the function.

   Store hi function at +0, low function at +4,
   hi static at +8, low static at +16  */

#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT)                       \
{                                                                       \
  rtx cxt = force_reg (Pmode, CXT);                                     \
  rtx fn = force_reg (Pmode, FNADDR);                                   \
  rtx hi_cxt = expand_shift (RSHIFT_EXPR, SImode, cxt,                  \
                             size_int (16), 0, 0);                      \
  rtx hi_fn = expand_shift (RSHIFT_EXPR, SImode, fn,                    \
                            size_int (16), 0, 0);                       \
  emit_move_insn (gen_rtx (MEM, HImode, plus_constant (TRAMP, 12)),     \
                  gen_lowpart (HImode, cxt));                           \
  emit_move_insn (gen_rtx (MEM, HImode, plus_constant (TRAMP, 8)),      \
                  gen_lowpart (HImode, fn));                            \
  emit_move_insn (gen_rtx (MEM, HImode, plus_constant (TRAMP, 0)),      \
                  gen_lowpart (HImode, hi_cxt));                        \
  emit_move_insn (gen_rtx (MEM, HImode, plus_constant (TRAMP, 4)),      \
                  gen_lowpart (HImode, hi_fn));                         \
}


/* Addressing modes, and classification of registers for them.  */

/*  BAD: I'm not using these now, b/c we allow arbitrary pre/post
	operations.  There may be a better way.  Even if there is, I
	might want to implement this to, but it will require changes to
	PRINT_OPERAND and GO_IF_LEGITIMATE_ADDRESS */
/*  These need not be defined to generate push and
    pop insns. Just define PUSH_ROUNDING() to generate them, and
    they will use the PRE_DEC family of rtl operators.
*/

#define HAVE_POST_INCREMENT 1
#define HAVE_PRE_DECREMENT 1
#define HAVE_POST_DECREMENT 1
#define HAVE_PRE_INCREMENT 1

/* Macros to check register numbers against specific register classes.  */

/* These assume that REGNO is a hard or pseudo reg number.
   They give nonzero only if REGNO is a hard reg of the suitable class
   or a pseudo reg currently allocated to a suitable hard reg.
   Since they use reg_renumber, they are safe only once reg_renumber
   has been allocated, which happens in local-alloc.c.  */

#define REGNO_OK_FOR_INDEX_P(REGNO) \
	((REGNO) < FIRST_PSEUDO_REGISTER \
	|| (unsigned) reg_renumber[REGNO] < FIRST_PSEUDO_REGISTER)
#define REGNO_OK_FOR_BASE_P(REGNO) \
	((REGNO) < FIRST_PSEUDO_REGISTER \
	|| (unsigned) reg_renumber[REGNO] < FIRST_PSEUDO_REGISTER)

#define REGNO_OK_FOR_FP_P(REGNO) 1

/* Now macros that check whether X is a register and also,
   strictly, whether it is in a specified class.

   These macros are specific to the i860, and may be used only
   in code for printing assembler insns and in conditions for
   define_optimization.  */

#define FP_REG_P(X) 1


/* Maximum number of registers that can appear in a valid memory address.  */

#define MAX_REGS_PER_ADDRESS 2

/* Recognize any constant value that is a valid address.  */
#define CONSTANT_ADDRESS_P(X)   					\
  (GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF		\
   || GET_CODE (X) == CONST_INT || GET_CODE (X) == CONST)
/*
   || GET_CODE (X) == HIGH)
*/

/* In fact, `1'
     is a suitable definition for this macro on machines where anything
     `CONSTANT_P' is valid.
*/
#define LEGITIMATE_CONSTANT_P(X) 1

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.

   Most source files want to accept pseudo regs in the hope that
   they will get allocated to the class that the insn wants them to be in.
   Source files for reload pass need to be strict.
   After reload, it makes no difference, since pseudo regs have
   been eliminated by then.  */

#ifndef REG_OK_STRICT

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  */
#define REG_OK_FOR_INDEX_P(X) 1
/* Nonzero if X is a hard reg that can be used as a base reg
   or if it is a pseudo reg.  */
#define REG_OK_FOR_BASE_P(X) 1

#else /* if REG_OK_STRICT defined */

/* Nonzero if X is a hard reg that can be used as an index.  */
#define REG_OK_FOR_INDEX_P(X) REGNO_OK_FOR_INDEX_P (REGNO (X))
/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define REG_OK_FOR_BASE_P(X) REGNO_OK_FOR_BASE_P (REGNO (X))

#endif /* REG_OK_STRICT */


/* GO_IF_LEGITIMATE_ADDRESS recognizes an RTL expression
   that is a valid memory address for an instruction.
   The MODE argument is the machine mode for the MEM expression
   that wants to use this address. */

#ifdef REG_OK_STRICT

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL) 			\
	{if( legitimate_address (MODE,X,1) ) goto LABEL;}

#else

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL) 			\
	{if( legitimate_address (MODE,X,0) ) goto LABEL;}

#endif

/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE and WIN are passed so that this macro can use
   GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this macro to do nothing.  It exists to recognize
   opportunities to optimize the output.  */

/* Take advantage of the LANai "reg op reg" addressing ability. */

#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN) do {			\
  enum tree_code lanai_code;						\
									\
  lanai_code = GET_CODE (X);						\
  if (0 && (lanai_code == PLUS						\
      || lanai_code == MINUS						\
      || lanai_code == AND						\
      || lanai_code == IOR						\
      || lanai_code == XOR))						\
    {									\
      (X) = gen_rtx (lanai_code,					\
		     Pmode,						\
		     force_reg (Pmode,					\
				force_operand (XEXP (X, 0),		\
					       NULL_RTX)),		\
		     force_reg (Pmode,					\
				force_operand (XEXP (X, 1),		\
					       NULL_RTX)));		\
      goto WIN;								\
    }									\
} while (0)

/* Go to LABEL if ADDR (a legitimate address expression)
   has an effect that depends on the machine mode it is used for.
*/
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL) {;}

/* Max number of args passed in registers.  We pass parameters in
   registers 6 to 14. */

#define REGPARM_MAX 9

/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE SImode

/* Define this if the tablejump instruction expects the table
   to contain offsets from the address of the table.
   Do not define this if the table should contain absolute addresses.  */
/* #define CASE_VECTOR_PC_RELATIVE */

#ifdef FIXME
/* Specify the tree operation to be used to convert reals to integers.  */
#define IMPLICIT_FIX_EXPR FIX_ROUND_EXPR

/* This is the kind of divide that is easiest to do in the general case.  */
#define EASY_DIV_EXPR TRUNC_DIV_EXPR
#endif

/* config.h can define LIB_SPEC to override the default libraries.  */
#define LIB_SPEC "-lgcc"

/* Must pass floats to libgcc functions as doubles.  Useful only if
	floats and ints are passed differetly.*/
/*
#define LIBGCC_NEEDS_DOUBLE 1
*/

/*
Use defaults from libgcc.a e.g. __divsi3 &c
#define MULSI3_LIBCALL "__mulsi3"
#define UMULSI3_LIBCALL "__umulsi3"
#define DIVSI3_LIBCALL "__divsi3"
#define UDIVSI3_LIBCALL "__udivsi3"
*/
/*
#define REMSI3_LIBCALL "*.rem"
#define UREMSI3_LIBCALL "*.urem"
*/

/* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 1

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
/* BAD: I'm not sure 0 is allowed.  Was 16 */
#define MOVE_MAX 4

/* Nonzero if access to memory by bytes is slow and undesirable.  */
#define SLOW_BYTE_ACCESS (TARGET_no_partword_load | TARGET_no_partword_store)

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* Value is 1 if it generates better code to perform an unsigned comparison
   on the given literal integer value in the given mode when we are only
   looking for an equal/non-equal result.  */
/* For the i860, if the immediate value has its high-order 27 bits zero,
   then we want to engineer an unsigned comparison for EQ/NE because
   such values can fit in the 5-bit immediate field of a bte or btne
   instruction (which gets zero extended before comparing).  For all
   other immediate values on the i860, we will use signed compares
   because that avoids the need for doing explicit xor's to zero_extend
   the non-constant operand in cases where it was (mem:QI ...) or a
   (mem:HI ...) which always gets automatically sign-extended by the
   hardware upon loading.  */

/*
#define LITERAL_COMPARE_BETTER_UNSIGNED(intval, mode)                   \
  (((unsigned) (intval) & 0x1f) == (unsigned) (intval))
*/

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode SImode

/*   Returns a mode from class `MODE_CC' to be used when comparison
     operation code OP is applied to rtx X and Y.

     On the LANai3, we have a "no-overflow" mode which is used when an
     add or subtract insn is used to set the condition code.
     Different branches are used in this case for some operations.
     */

#define SELECT_CC_MODE(OP,X,Y) \
 (GET_MODE_CLASS (GET_MODE (X)) != MODE_INT ? CCmode \
 : (GET_CODE(X)==PLUS || GET_CODE(X)==MINUS || GET_CODE(X)==NEG \
    ? CC_NOOVmode \
    : (GET_CODE(X)==ZERO_EXTRACT && XEXP (X,1) == const1_rtx \
       ? CC_BITTESTmode \
       : CCmode )))

/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */
#define FUNCTION_MODE QImode

/* Define this if addresses of constant functions
   shouldn't be put through pseudo regs where they can be cse'd.
   Desirable on machines where ordinary constants are expensive
   but a CALL with constant address is cheap.  */
/* BAD: This should be defined if the special instructions are
   implemented */
#define NO_FUNCTION_CSE 1

/* Compute the cost of computing a constant rtl expression RTX
   whose rtx-code is CODE.  The body of this macro is a portion
   of a switch statement.  If the code is computed here,
   return it with a return statement.  Otherwise, break from the switch.  */
/*
`RTX_COSTS (X, CODE, OUTER_CODE)'
     Like `CONST_COSTS' but applies to nonconstant RTL expressions.
     This can be used, for example, to indicate how costly a multiply
     instruction is.  In writing this macro, you can use the construct
     `COSTS_N_INSNS (N)' to specify a cost equal to N fast
     instructions.  OUTER_CODE is the code of the expression in which X
     is contained.

     This macro is optional; do not define it if the default cost
     assumptions are adequate for the target machine.
*/

#define RTX_COSTS(X,CODE,OUTER_CODE)                            \
  case MULT:                                                    \
  case DIV:                                                     \
  case UDIV:                                                    \
  case MOD:                                                     \
  case UMOD:                                                    \
      return COSTS_N_INSNS (40);				\
      /* Count mul/division by 2**n as a shift,			\
         because if we are considering it, 			\
	 we would output it as a shift.  */			\
      if(! (GET_CODE (XEXP (x, 1)) == CONST_INT			\
          && exact_log2 (INTVAL (XEXP (x, 1))) >= 0)){		\
	total += COSTS_N_INSNS (40);				\
	break;							\
      }

/* experiments have shown Glenn to use two as the lowest const_cost
and addr_cost. */

#define CONST_COSTS(RTX,CODE, OUTER_CODE)				\
  case CONST_INT:							\
  case CONST:								\
  case LABEL_REF:							\
  case SYMBOL_REF:							\
  case CONST_DOUBLE:							\
    return 2;

/* Specify the cost of a branch insn; roughly the number of extra insns that
   should be added to avoid a branch.

   Set this to 2 on the LANai3 since branches take two cycles.  */

#define BRANCH_COST 1

#if 0
/* Make addresses in register costly to encourace compiler
   use address computation instructions when possible.  Also, make
   RRM instructions a bit costlier to discourage compiler from keeping
   two live registers. */
#define ADDRESS_COST(X) ( \
     GET_CODE(X) == REG ? 4 : \
           ( GET_CODE (X) == PLUS || GET_CODE (X) == MINUS || \
	     GET_CODE (X) == AND  || GET_CODE (X) == IOR   || \
	     GET_CODE (X) == XOR  || GET_CODE (X) == ASHIFT|| \
	     GET_CODE (X) == LSHIFTRT ) \
           && GET_CODE(XEXP(X,0)) == REG && GET_CODE(XEXP(X,1)) == REG ? 0 : 0)
#endif

#if 0
/* Address costs nothing if legitimate and lots if illegitimate.  Thus,
   the compiler will choose the most complex legitimate address. */
#define ADDRESS_COST(X) (legitimate_address(GET_MODE(X),X,0)?0:1000)
#endif

#if 1
#define ADDRESS_COST(X) 0
#endif

#if 0
/* All address computations that can be done are free, but rtx cost returns
   the same for practically all of them.  So we weight the differnt types
   of address here in the order (most pref first):
   PRE/POST_INC/DEC, SHIFT or NON-INT sum, INT sum, REG, MEM or LABEL. */
#define ADDRESS_COST(X)                                                      \
  (10 - ((GET_CODE (X) == MEM || GET_CODE (X) == LABEL_REF                   \
          || GET_CODE (X) == SYMBOL_REF)                                     \
         ? 0                                                                 \
         : ((GET_CODE (X) == PRE_INC || GET_CODE (X) == PRE_DEC              \
             || GET_CODE (X) == POST_INC || GET_CODE (X) == POST_DEC)        \
            ? 10                                                             \
            : (((GET_CODE (X) == PLUS || GET_CODE (X) == MINUS)              \
                ? 6 + (GET_CODE (XEXP (X, 1)) == CONST_INT ? 2               \
                       : ((GET_RTX_CLASS (GET_CODE (XEXP (X, 0))) == '2'     \
                           || GET_RTX_CLASS (GET_CODE (XEXP (X, 0))) == 'c'  \
                           || GET_RTX_CLASS (GET_CODE (XEXP (X, 1))) == '2'  \
                           || GET_RTX_CLASS (GET_CODE (XEXP (X, 1))) == 'c') \
                          ? 1 : 0))                                          \
                : 4)))))
#endif


/* Tell final.c how to eliminate redundant test instructions.  */
/* BAD: I might want to look into supporting some of this */

/* We use one register to help construct constant addresses.  We call
   this register "rca".  Each time we set the value of this register,
   we also set rca_info to an EXP describing the memory location
   stored int the rca register, allowing later insn's the opportunity
   to use the old value of rca if doing so is helpful.  Any time
   rca might become invalid, we CLOBBER_RCA so we won't use its
   invalid contents. */
struct Rca_info
{
  unsigned int addr;
  unsigned int utility;
};
extern struct Rca_info rca_info;
#define CLOBBER_RCA \
  ( rca_info.addr =        0,             rca_info.utility =  0 )
#define RCA_SAME(X) \
  ( rca_info.addr == (int) XEXP((X),0) && rca_info.utility != 0 )
#define SET_RCA_COMPLETE(X) \
  ( rca_info.addr =  (int) XEXP((X),0),   rca_info.utility =  1 )
#define RCA_COMPLETE(X) \
  ( rca_info.addr == (int) XEXP((X),0) && rca_info.utility == 1 )
#define SET_RCA_WANTS_SIGNED(X) \
  ( rca_info.addr =  (int) XEXP((X),0),   rca_info.utility =  2 )
#define RCA_WANTS_SIGNED(X) \
  ( rca_info.addr == (int) XEXP((X),0) && rca_info.utility == 2 )
#define SET_RCA_WANTS_UNSIGNED(X) \
  ( rca_info.addr =  (int) XEXP((X),0),   rca_info.utility =  3 )
#define RCA_WANTS_UNSIGNED(X) \
  ( rca_info.addr == (int) XEXP((X),0) && rca_info.utility == 3 )

/* On the i860, only compare insns set a useful condition code.  */

/* glenn: deactivated as with CC_STATUS_PARTIAL_INIT */
/*
#define NOTICE_UPDATE_CC(EXP, INSN) \
{ cc_status.flags &= 0;	\
  cc_status.value1 = 0; cc_status.value2 = 0; }
*/

#define REVERSIBLE_CC_MODE(MODE) 1


/* Control the assembler format that we output.  */

/* Assembler pseudos to introduce constants of various size.  */

#define ASM_BYTE_OP "\t.byte"
#define ASM_SHORT "\t.short"
#define ASM_LONG "\t.long"
#define ASM_DOUBLE "\t.double"

/* Output at beginning of assembler file.  */
/* The .file command should always begin the output.  */

#define ASM_FILE_START(FILE)
#if 0
#define ASM_FILE_START(FILE)						\
  do { output_file_directive ((FILE), main_input_filename);		\
       if (optimize) ASM_FILE_START_1 (FILE);				\
     } while (0)
#endif

#define ASM_FILE_START_1(FILE)

/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */
#define ASM_APP_ON ""

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */
#define ASM_APP_OFF ""

/* Output before read-only data.  */
#define TEXT_SECTION_ASM_OP ".text"

/* Output before writable data.  */
#define DATA_SECTION_ASM_OP ".data"

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */
#define REGISTER_NAMES 							\
{"r0", "r1", "pc", "ps", "sp", "fp", "r6", "r7", "r8", "r9", 		\
 "r10", "r11", "r12", "r13", "r14", "rca", "r16", "r17", "r18", "r19",	\
 "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29",	\
 "r30", "r31"}

/*
#define FINAL_PRESCAN_INSN(INSN, OPVEC, NOPERANDS) 			\
	final_prescan_insn(INSN, OPVEC, NOPERANDS)
*/

#define IDENT_ASM_OP ".file"

/* Output #ident as a .ident.  */

#define ASM_OUTPUT_IDENT(FILE, NAME) \
  fprintf (FILE, "\t%s\t\"%s\"\n", IDENT_ASM_OP, NAME);

/*****************
 * Debugging
 *****************/

/* Generate DBX debugging information.  */
#define DBX_DEBUGGING_INFO 1

/* How to renumber registers for dbx and gdb.  */
#define DBX_REGISTER_NUMBER(REGNO) (REGNO)

#if 0
/* This is how to output the definition of a user-level label named NAME,
   such as the label on a static function or variable NAME.  */

#define ASM_OUTPUT_LABEL(FILE,NAME)					\
  do { assemble_name (FILE, NAME); fputs (":\n", FILE); } while (0)
#endif

/* This is how to output a command to make the user-level label named NAME
   defined for reference from other files.  */

#define GLOBAL_ASM_OP "\t.globl "

/* The prefix to add to user-visible assembler symbols.  */

#define USER_LABEL_PREFIX "_"

#if 0
/* This is how to output a reference to a user-level label named NAME.
   `assemble_name' uses this.

   This definition is overridden in i860v4.h because under System V
   Release 4, user-level symbols are *not* prefixed with underscores in
   the generated assembly code.  */

#define ASM_OUTPUT_LABELREF(FILE,NAME)					\
  {if((NAME)[0] == '*') fprintf(FILE,"%s",++(NAME));			\
  else fprintf (FILE, "_%s", NAME);}
#endif

/* This is how to output an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.  */

#define ASM_OUTPUT_INTERNAL_LABEL(FILE,PREFIX,NUM)			\
  fprintf (FILE, "%s%d:\n", PREFIX, NUM)

/* Output and element of an address vector (casesi jump table) */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE,VALUE)				\
  fprintf (FILE, "\t.long L%d\n", VALUE)

/* This is how to output an internal numbered label which
   labels a jump table.  */

#undef ASM_OUTPUT_CASE_LABEL
#define ASM_OUTPUT_CASE_LABEL(FILE, PREFIX, NUM, JUMPTABLE)		\
do { ASM_OUTPUT_ALIGN ((FILE), 2);					\
     ASM_OUTPUT_INTERNAL_LABEL ((FILE), PREFIX, NUM);			\
   } while (0)

/* Output at the end of a jump table.  */

#define ASM_OUTPUT_CASE_END(FILE,NUM,INSN)				\
  fprintf (FILE, ".text\n")

/* This is how to store into the string LABEL
   the symbol_ref name of an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.
   This is suitable for output with `assemble_name'.  */

#define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM)			\
  sprintf (LABEL, "*%s%d", PREFIX, NUM)

/* This is how to output an assembler line defining a `double' constant.  */

#define ASM_OUTPUT_DOUBLE(FILE,VALUE)  					\
  fprintf (FILE, "\t.double %.20e\n", (VALUE))

/* This is how to output an assembler line defining a `float' constant.  */

#define ASM_OUTPUT_FLOAT(FILE,VALUE)  					\
  fprintf (FILE, "\t.float %.12e\n", (VALUE))

/* This is how to output an assembler line defining an `int' constant.  */

#define ASM_OUTPUT_INT(FILE,VALUE)  					\
( fprintf (FILE, "\t.long "),						\
  output_addr_const (FILE, (VALUE)),					\
  fprintf (FILE, "\n"))

/* Likewise for `char' and `short' constants.  */

#define ASM_OUTPUT_SHORT(FILE,VALUE)  					\
( fprintf (FILE, "\t.short "),						\
  output_addr_const (FILE, (VALUE)),					\
  fprintf (FILE, "\n"))

#define ASM_OUTPUT_CHAR(FILE,VALUE)  					\
( fprintf (FILE, "\t.byte "),						\
  output_addr_const (FILE, (VALUE)),					\
  fprintf (FILE, "\n"))

/* This is how to output an assembler line for a numeric constant byte.  */

#define ASM_OUTPUT_BYTE(FILE,VALUE)  					\
  fprintf (FILE, "\t.byte 0x%x\n", (VALUE))

/* This is how to output code to push a register on the stack.
   It need not be very fast code.  */
#define ASM_OUTPUT_REG_PUSH(FILE,REGNO)					\
  fprintf (FILE, "\tst %s%s,[--%ssp]\n",				\
	lanai_reg_prefix, reg_names[REGNO], lanai_reg_prefix)

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */
#define ASM_OUTPUT_REG_POP(FILE,REGNO)					\
  fprintf (FILE, "\tld [%ssp++],%s%s\n",				\
	lanai_reg_prefix,						\
	lanai_reg_prefix, reg_names[REGNO])

#if 0
/* This is how to output an element of a case-vector that is absolute.  */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  				\
  fprintf (FILE, "\t.long L%d\n", VALUE)
#endif

/* This is how to output an element of a case-vector that is relative.
   (The i860 does not use such vectors,
   but we must define this macro anyway.)  */

#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL)  		\
  fprintf (FILE, "\t.word L%d-L%d\n", VALUE, REL)

/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG)					\
  if ((LOG) != 0)							\
    fprintf (FILE, "\t.align %d\n", 1 << (LOG))

#define ASM_OUTPUT_SKIP(FILE,SIZE)  					\
  fprintf (FILE, "\t.space %u\n", (SIZE))

/* This says how to output an assembler line
   to define a global common symbol.  */

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)			\
( fputs (".comm ", (FILE)),						\
  assemble_name ((FILE), (NAME)),					\
  fprintf ((FILE), ",%u\n", (ROUNDED)))

/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)  			\
( fputs (".lcomm ", (FILE)),						\
  assemble_name ((FILE), (NAME)),					\
  fprintf ((FILE), ",%u\n", (ROUNDED)))

/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */

#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)			\
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10),			\
  sprintf ((OUTPUT), "%s.%d", (NAME), (LABELNO)))

/* Print operand X (an rtx) in assembler syntax to file FILE.
   CODE is a letter or dot (`z' in `%z0') or 0 if no letter was specified.
   For `%' followed by punctuation, CODE is the punctuation and X is null.

   In the following comments, the term "constant address" is used frequently.
   For an exact definition of what constitutes a "constant address" see the
   output_addr_const routine in final.c

   On the LANai3, the following target-specific special codes are recognized:

	`r'	The operand can be anything, but if is is an immediate zero
		value (either integer or floating point) then it will be
		represented as `r0' or as `f0' (respectively).  If the
		value is integer -1, it will be represented as `r1'.

	`m'	The operand is a memory ref (to a constant address) but print
		its address as a constant.

	`L'	The operand is a numeric constant, a constant address, or
		a memory ref to a constant address.  Print the correct
		notation to yield the low part of the given value or
		address or the low part of the address of the referred
		to memory object.

	`H'	The operand is a numeric constant, a constant address, or
		a memory ref to a constant address.  Print the correct
		notation to yield the high part of the given value or
		address or the high part of the address of the referred
		to memory object.

	`h'	The operand is a numeric constant, a constant address, or
		a memory ref to a constant address.  Either print the
		correct notation to yield the plain high part of the
		given value or address (or the plain high part of the
		address of the memory object) or else print the correct
		notation to yield the "adjusted" high part of the given
		address (or of the address of the referred to memory object).

		The choice of what to print depends upon whether the address
		in question is relocatable or not.  If it is relocatable,
		print the notation to get the adjusted high part.  Otherwise
		just print the notation to get the plain high part.  Note
		that "adjusted" high parts are generally used *only* when
		the next following instruction uses the low part of the
		address as an offset, as in `offset(reg)'.

	`R'	The operand is a floating-pointer register.  Print the
		name of the next following (32-bit) floating-point register.
		(This is used when moving a value into just the most
		significant part of a floating-point register pair.)

	`?'	(takes no operand) Substitute the value of lanai_reg_prefix
		at this point.  The value of lanai_reg_prefix is typically
		a null string for most i860 targets, but for System V
		Release 4 the i860 assembler syntax requires that all
		names of registers be prefixed with a percent-sign, so
		for SVR4, the value of lanai_reg_prefix is initialized to
		"%" in i860.c.
*/

extern const char *lanai_reg_prefix;

/* ? prints the register prefix
   o prints an operator (add,sub,and,or,xor) based on the code
   # prints a nop to fill a shadow if needed. */
#define PRINT_OPERAND_PUNCT_VALID_P(CODE) \
	((CODE) == '?' || (CODE) == 'o' || (CODE) == '#')

#define PRINT_OPERAND_PART(FILE, X, PART_CODE)				\
  do { 									\
	fprintf (FILE, "%s%%", PART_CODE);				\
	output_address (X);						\
  } while (0)

#define OPERAND_LOW_PART	"l"
#define OPERAND_HIGH_PART	"h"
/* NOTE: All documentation available for the i860 sez that you must
   use "ha" to get the relocated high part of a relocatable, but
   reality sez different.  */
#define OPERAND_HIGH_ADJ_PART	"ha"

#define PRINT_OPERAND(FILE, X, CODE) do {				\
  if ((CODE) == '?')							\
    fprintf (FILE, "%s", lanai_reg_prefix);				\
  else if (CODE == 'o')							\
    {									\
      switch(GET_CODE(X)){						\
      case PLUS: fprintf (FILE, "add"); break;				\
      case MINUS: fprintf (FILE, "sub"); break;				\
      case AND: fprintf (FILE, "and"); break;				\
      case IOR: fprintf (FILE, "or"); break;				\
      case XOR: fprintf (FILE, "xor"); break;				\
      case ASHIFT: fprintf (FILE, "sha"); break;			\
      case LSHIFTRT: fprintf (FILE, "sh"); break;			\
      case ASHIFTRT: fprintf (FILE, "sha"); break;			\
      default: abort(); break;						\
      }									\
    }									\
  else if (CODE == '#')							\
    {									\
      int seq_len;							\
									\
      seq_len = dbr_sequence_length ();					\
      while (seq_len++ < TARGET_max_branch_delay)			\
	{								\
	  fprintf (FILE, "\n\tnop");					\
	}								\
    }									\
  else if (CODE == 'R')							\
    fprintf (FILE, "%s%s", lanai_reg_prefix, reg_names[REGNO (X) + 1]);	\
  else if (GET_CODE (X) == REG)						\
    fprintf (FILE, "%s%s", lanai_reg_prefix, reg_names[REGNO (X)]);	\
  else if ((CODE) == 'm')						\
    output_address (XEXP (X, 0));					\
  else if ((CODE) == 'L')						\
    {									\
      if (GET_CODE (X) == MEM)						\
	PRINT_OPERAND_PART (FILE, XEXP (X, 0), OPERAND_LOW_PART);	\
      else								\
	PRINT_OPERAND_PART (FILE, X, OPERAND_LOW_PART);			\
    }									\
  else if ((CODE) == 'H')						\
    {									\
      if (GET_CODE (X) == MEM)						\
	PRINT_OPERAND_PART (FILE, XEXP (X, 0), OPERAND_HIGH_PART);	\
      else								\
	PRINT_OPERAND_PART (FILE, X, OPERAND_HIGH_PART);		\
    }									\
  else if ((CODE) == 'h')						\
    {									\
      if (GET_CODE (X) == MEM)						\
	PRINT_OPERAND_PART (FILE, XEXP (X, 0), OPERAND_HIGH_ADJ_PART);	\
      else								\
	PRINT_OPERAND_PART (FILE, X, OPERAND_HIGH_ADJ_PART);		\
    }									\
  else if (GET_CODE (X) == MEM)						\
    {									\
      output_address (XEXP (X, 0));					\
    }									\
  else if ((CODE) == 'r')						\
    {									\
      if ( (X) == const0_rtx || (X) == CONST0_RTX (GET_MODE (X)))	\
	{								\
	  fprintf (FILE, "%sr0", lanai_reg_prefix);			\
	}								\
      else if ((X) == constm1_rtx)					\
	{								\
	  fprintf (FILE, "%sr1", lanai_reg_prefix);			\
	}								\
    }									\
  else									\
    {									\
      output_addr_const (FILE, X);					\
    }									\
} while (0)


/* Print a memory address as an operand to reference that memory location.  */

#define PRINT_OPERAND_ADDRESS(FILE, ADDR) {print_operand_address(FILE,ADDR);}

/* The following #defines are used when compiling the routines in
   libgcc1.c.

#define FLOAT_TYPE_VALUE	float
#define INTIFY(FLOATVAL)	(FLOATVAL)
#define FLOATIFY(INTVAL)	(INTVAL)
#define FLOAT_ARG_TYPE		float
*/


/* Optionally define this if you have added predicates to
   `MACHINE.c'.  This macro is called within an initializer of an
   array of structures.  The first field in the structure is the
   name of a predicate and the second field is an array of rtl
   codes.  For each predicate, list all rtl codes that can be in
   expressions matched by the predicate.  The list should have a
   trailing comma.  Here is an example of two entries in the list
   for a typical RISC machine:

   #define PREDICATE_CODES \
     {"gen_reg_rtx_operand", {SUBREG, REG}},  \
     {"reg_or_short_cint_operand", {SUBREG, REG, CONST_INT}},

   Defining this macro does not affect the generated code (however,
   incorrect definitions that omit an rtl code that may be matched
   by the predicate can cause the compiler to malfunction).
   Instead, it allows the table built by `genrecog' to be more
   compact and efficient, thus speeding up the compiler.  The most
   important predicates to include in the list specified by this
   macro are thoses used in the most insn patterns.  */

/* Built-in:
     {"general_operand", {CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF,
                          LABEL_REF, SUBREG, REG, MEM}},
     {"address_operand", {CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF,
                          LABEL_REF, SUBREG, REG, MEM, PLUS, MINUS, MULT}},
     {"register_operand", {SUBREG, REG}},
     {"scratch_operand", {SCRATCH, REG}},
     {"immediate_operand", {CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF,
                            LABEL_REF}},
     {"const_int_operand", {CONST_INT}},
     {"const_double_operand", {CONST_INT, CONST_DOUBLE}},
     {"nonimmediate_operand", {SUBREG, REG, MEM}},
     {"nonmemory_operand", {CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF,
                            LABEL_REF, SUBREG, REG}},
     {"push_operand", {MEM}},
     {"memory_operand", {SUBREG, MEM}},
     {"indirect_operand", {SUBREG, MEM}},
     {"comparison_operator", {EQ, NE, LE, LT, GE, GT, LEU, LTU, GEU, GTU}},
     {"mode_independent_operand", {CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF,
                                   LABEL_REF, SUBREG, REG, MEM}},
*/

/*
#define PREDICATE_CODES							\
   {"fairly_general_operand", {CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF,\
                          LABEL_REF, SUBREG, REG, MEM}},		\
   {"reg_0_m1_operand",		{REG, SUBREG, CONST_INT}},		\
   {"arith_operand",		{REG, SUBREG, CONST_INT}},		\
   {"logic_operand",		{REG, SUBREG, CONST_INT}},		\
   {"and_operand",		{REG, SUBREG, CONST_INT}},     		\
   {"shift_operand",		{REG, SUBREG, CONST_INT}},		\
   {"arith_immediate_operand",	{CONST_INT}},				\
   {"logic_immediate_operand",	{CONST_INT}},				\
   {"and_immediate_operand",	{CONST_INT}},				\
   {"indexed_operand",		{MEM}},					\
   {"load_operand",		{MEM}},					\
   {"op1_operand",		{PLUS,MINUS,AND,IOR,XOR,ASHIFT}},	\
   {"op2_operand",		{PLUS,MINUS,AND,IOR,XOR,ASHIFT,LSHIFTRT}},\
   {"di_operand", 	{SUBREG, REG, CONST_INT, CONST_DOUBLE, MEM}},   \
   {"df_operand", 	{SUBREG, REG, CONST_INT, CONST_DOUBLE, MEM}},   \
   {"call_insn_operand",	{MEM}},
   */

#define perform_mulsi3(a,b) 						\
{	int i,j,result;							\
	if(a<0) i = -a;							\
	else    i = a;							\
	for(i=a,j=b,result=;i;i>>1,j<<1){				\
		if(i&1) result += j;					\
	}								\
	return (a>0?result:-result);					\
}

/*   Define this macro if operations between registers with integral
     mode smaller than a word are always performed on the entire
     register.  Most RISC machines have this property and most CISC
     machines do not.
*/
#define WORD_REGISTER_OPERATIONS 1

/*   Define this macro to be a C expression indicating when insns that
     read memory in MODE, an integral mode narrower than a word, set the
     bits outside of MODE to be either the sign-extension or the
     zero-extension of the data read.  Return `SIGN_EXTEND' for values
     of MODE for which the insn sign-extends, `ZERO_EXTEND' for which
     it zero-extends, and `NIL' for other modes.

     This macro is not called with MODE non-integral or with a width
     greater than or equal to `BITS_PER_WORD', so you may return any
     value in this case.  Do not define this macro if it would always
     return `NIL'.  On machines where this macro is defined, you will
     normally define it as the constant `SIGN_EXTEND' or `ZERO_EXTEND'.
*/
/* On the lanai3, loads zero extend by default */
#define LOAD_EXTEND_OP(MODE) ZERO_EXTEND

/*   A list of names for sections other than the standard two, which are
     `in_text' and `in_data'.  You need not define this macro on a
     system with no other sections (that GCC needs to use). */

/*********************
 * CTORS/DTORS support
 *********************/

#define CTORS_SECTION_ASM_OP	".section\t.ctors"
#define DTORS_SECTION_ASM_OP	".section\t.dtors"

/* The LANai linker collects the .ctors and .dtors sections
   automatically, obviating the need to run collect2.  HOWEVER,
   because the CTORS pointers are 32bits and sections are aligned on
   64bit boundaries, there may be holes in the packet list to maintain
   alignement.  These wholes are filled with 0's, and such filler
   pointers are ignored by these replacement
   DO_GLOBAL_?TORS_BODY's. */


#define DO_GLOBAL_CTORS_BODY			\
  extern func_ptr __CTOR_END__[2];		\
  func_ptr *p;					\
  for (p = __CTOR_LIST__; p < __CTOR_END__; p++) \
    if (*p)					\
      (*p)();

#define DO_GLOBAL_DTORS_BODY			\
extern func_ptr __DTOR_END__[2];		\
  func_ptr *p;					\
  for (p = __DTOR_END__ - 1; p >= __DTOR_LIST__; p--) \
    if (*p)					\
      (*p)();

/*   A C statement to output something to the assembler file to switch
     to the section contained in STRING.  Some target formats do not
     support arbitrary sections.  Do not define this macro in such
     cases.

     At present this macro is only used to support section attributes.
     When this macro is undefined, section attributes are disabled. */

#define ASM_OUTPUT_SECTION_NAME(FILE, DECL, NAME, ignored) \
  fprintf (FILE, ".section\t%s\n", NAME)

#define INIT_SECTION_ASM_OP "\t.section\t.init"
#define FINI_SECTION_ASM_OP "\t.section\t.init"

/* Arrange to collect constructors/destructors using a named section. */
#define TARGET_ASM_NAMED_SECTION default_coff_asm_named_section

#define lanai_ld_delay			TARGET_ld_delay
#define lanai_min_branch_delay		TARGET_min_branch_delay
#define lanai_jump_delay		TARGET_min_branch_delay	/* true, for now */
#define lanai_max_indirect_jump_delay	TARGET_max_indirect_jump_delay

#define USE_LOAD_PRE_DECREMENT(mode)	1
#define USE_LOAD_POST_DECREMENT(mode)	1
#define USE_LOAD_PRE_INCREMENT(mode)	1
#define USE_LOAD_POST_INCREMENT(mode)	1
#define USE_STORE_PRE_DECREMENT(mode)	1
#define USE_STORE_POST_DECREMENT(mode)	1
#define USE_STORE_PRE_INCREMENT(mode)	1
#define USE_STORE_POST_INCREMENT(mode)	1

extern char *lanai_regparm_string;		/* # registers to use to pass args */
extern int lanai_regparm;			/* i386_regparm_string as a number */
extern void override_options (void);

/* Disable unsupported libgcc2 functions. */

#undef L_ffsdi2
#undef L_floatdisf
#undef L_fixunsdfdi
#undef L_fixunssfdi
#undef L_absvsi2
#undef L_absvdi2
#undef L_addvsi3
#undef L_addvdi3
#undef L_subvdi3
#undef L_subvsi3
#undef L_mulvsi3
#undef L_mulvdi3
#undef L_negvsi2
#undef L_negvdi2

#endif /* _lanai_h_ */
