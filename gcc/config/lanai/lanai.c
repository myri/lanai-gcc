/*************************************************************************
 *                                                                       *
 * lanai3.c: a source file to add LANai3 support to gcc                  *
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

#include <stdio.h>
#include <math.h>

#include "config.h"
#include "errors.h"
#include "system.h"
#include "tree.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "function.h"
#include "expr.h"
#include "optabs.h"
#include "libfuncs.h"
#include "recog.h"
#include "toplev.h"
#include "ggc.h"
#include "tm_p.h"
#include "debug.h"
#include "target.h"
#include "target-def.h"
#include "reload.h"

#ifndef LANAI_REG_PREFIX
#define LANAI_REG_PREFIX "%"
#endif

const char *lanai_reg_prefix = LANAI_REG_PREFIX;
const struct attribute_spec lanai_attribute_table[];

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE lanai_output_function_prologue
#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE lanai_output_function_epilogue
#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE lanai_attribute_table

struct gcc_target targetm = TARGET_INITIALIZER;

/* # of registers to use to pass arguments. */
char *lanai_regparm_string;

/* i386_regparm_string as a number */
int lanai_regparm;

/* Sometimes certain combinations of command options do not make
   sense on a particular target machine.  You can define a macro
   `OVERRIDE_OPTIONS' to take account of this.  This macro, if
   defined, is executed once just after all the command options have
   been parsed.

   Don't use this macro to turn on various extra optimizations for
   `-O'.  That is what `OPTIMIZATION_OPTIONS' is for.  */

void
override_options ()
{
  int i;

  /* Validate -mregparm= value. */
  if (lanai_regparm_string)
    {
      i = atoi (lanai_regparm_string);
      if (i < 0 || i > REGPARM_MAX)
	error ("-mregparm=%d is not between 0 and %d", i, REGPARM_MAX);
      else
	lanai_regparm = i;
    }
}

static tree
lanai_handle_regparm_attribute (tree *node, tree name, tree args,
				int flags, bool *no_add_attrs);

/* Handle a "regparm" attribute;
   arguments as in struct attribute_spec.handler.  */
static tree
lanai_handle_regparm_attribute (node, name, args, flags, no_add_attrs)
     tree *node;
     tree name;
     tree args;
     int flags ATTRIBUTE_UNUSED;
     bool *no_add_attrs;
{
  if (TREE_CODE (*node) != FUNCTION_TYPE
      && TREE_CODE (*node) != METHOD_TYPE
      && TREE_CODE (*node) != FIELD_DECL
      && TREE_CODE (*node) != TYPE_DECL)
    {
      warning ("`%s' attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }
  else
    {
      tree cst;

      cst = TREE_VALUE (args);
      if (TREE_CODE (cst) != INTEGER_CST)
	{
	  warning ("`%s' attribute requires an integer constant argument",
		   IDENTIFIER_POINTER (name));
	  *no_add_attrs = true;
	}
      else if (compare_tree_int (cst, REGPARM_MAX) > 0)
	{
	  warning ("argument to `%s' attribute larger than %d",
		   IDENTIFIER_POINTER (name), REGPARM_MAX);
	  *no_add_attrs = true;
	}
    }

  return NULL_TREE;
}

/* Table of valid machine attributes.  */
const struct attribute_spec lanai_attribute_table[] =
{
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler } */
  /* Regparm attribute specifies how many integer arguments are to be
     passed in registers.  */
  { "regparm",   1, 1, false, true,  true,  lanai_handle_regparm_attribute },
  { NULL,        0, 0, false, false, false, NULL }
};

#if 0
/* Return the regparm value for a fuction with the indicated TYPE.  */

static int lanai_fntype_regparm (tree type);

static int
lanai_fntype_regparm (type)
     tree type;
{
  tree attr;

  attr = lookup_attribute ("regparm", TYPE_ATTRIBUTES (type));
  if (attr)
    return TREE_INT_CST_LOW (TREE_VALUE (TREE_VALUE (attr)));
  else
    return lanai_regparm;
}
#endif



/* Return non-zero only if OP is a register of mode MODE, or one of
   the constants 0 or -1, which are in hard registers on the LANai3.  */
int
reg_0_m1_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (register_operand (op, mode) || op == const0_rtx
	  || op == constm1_rtx);
}

#if 0
/* Return non-zero only if OP is a register of mode MODE, or one of
   the constants 0 or -1, which are in hard registers on the LANai3.  */
static int reg_0_operand (rtx op, enum machine_mode mode);

static
int
reg_0_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (register_operand (op, mode) || op == const0_rtx);
}
#endif

#if 0
static int reg_m1_operand (rtx op, enum machine_mode);
static
int
reg_m1_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (register_operand (op, mode) || op == constm1_rtx);
}
#endif

#if 0
/* from arm.c */
/* s_register_operand is the same as register_operand, but it doesn't accept
   (SUBREG (MEM)...). */

static int s_register_operand (register rtx op, enum machine_mode mode);
static
int
s_register_operand (op, mode)
     register rtx op;
     enum machine_mode mode;
{
  if (GET_MODE (op) != mode && mode != VOIDmode)
    return 0;

  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);

  /* We don't consider registers whose class is NO_REGS
     to be a register operand.  */
  return (GET_CODE (op) == REG
	  && (REGNO (op) >= FIRST_PSEUDO_REGISTER
	      || REGNO_REG_CLASS (REGNO (op)) != NO_REGS));
}
#endif

/* Return truth value of whether OP can be used as an operands in a three
   address add/subtract insn (such as add %r4,7,%r5) of mode MODE or if
   the operation can be converted add<->subtract & the const. is
   acceptable.  */
int
arith_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (register_operand (op, mode)
	  || (GET_CODE (op) == CONST_INT && ARITH_INT (INTVAL (op))));
}

/* Return 1 if OP is a valid second operand for a logical insn of mode MODE.  */
int
logic_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (register_operand (op, mode)
	  || (GET_CODE (op) == CONST_INT && LOGIC_INT (INTVAL (op))));
}

/* Return 1 if OP is a valid second operand for an and insn of mode MODE.  */
int
and_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (register_operand (op, mode)
	  || (GET_CODE (op) == CONST_INT && AND_INT (INTVAL (op))));
}

/* Return 1 if OP is a valid second operand for a shift insn of mode MODE.  */
int
shift_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (register_operand (op, mode)
	  || (GET_CODE (op) == CONST_INT && SHIFT_INT (INTVAL (op))));
}

#if 0
/* Return 1 if OP is a valid second operand for a shift insn of mode MODE.  */
static int unresolved_operand (rtx op, enum machine_mode mode);

static
int
unresolved_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  (void) mode;
  return (GET_CODE (op) == CONST ||
	  GET_CODE (op) == SYMBOL_REF || GET_CODE (op) == LABEL_REF);
}
#endif

/* Return 1 if op is a const that can be built in a register with a
   single instruction, or a register */
int
trivial_move_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  if (GET_CODE (op) == SYMBOL_REF ||
      GET_CODE (op) == LABEL_REF || GET_CODE (op) == CONST)
    return 1;
  if (GET_CODE (op) == CONST_INT)
    {
      if (AND_INT (INTVAL (op)) ||
	  ARITH_INT (INTVAL (op)) || SLI_INT (INTVAL (op)))
	return 1;
      if (op == const0_rtx || op == constm1_rtx)
	return 1;
    }
  return register_operand (op, mode);
}

/* Return 1 is op can be stored to memory in 1 insn */
int
trivial_store_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  if (register_operand (op, mode) || op == const0_rtx || op == constm1_rtx)
    return 1;
  if (GET_CODE (op) == CONST_INT)
    {
      if ((mode == HImode && INTVAL (op) == 0xffff) ||
	  (mode == QImode && INTVAL (op) == 0xff))
	return 1;
    }
  return 0;
}

#if 0
/* Return 1 if OP is an indexed memory reference of mode MODE.  */
static int indexed_operand (rtx op, enum machine_mode mode);

static
int
indexed_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  if (GET_CODE (op) == MEM)
    {
      if (legitimate_address (mode, XEXP (op, 0), 0))
	return 1;
    }
  return 0;
}
#endif

#if 0
static int fairly_general_operand (rtx op, enum machine_mode mode);

static
int
fairly_general_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  if (memory_operand (op, mode))
    {
      if (legitimate_address (mode, XEXP (op, 0), 0))
	return 1;
    }
  else
    {
      return general_operand (op, mode);
    }
  return 0;
}
#endif

#if 0
/* Return 1 if OP is a suitable source operand for a load insn
   with mode MODE.  */
static int load_operand (rtx op, enum machine_mode mode);

static
int
load_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  return (indexed_operand (op, mode));
}
#endif

#if 0
/* For partword loads, if partword loads are disabled, require the address
   of the load to be in a register or be a constant. */
static int partword_source_operand (rtx op, enum machine_mode mode);

static
int
partword_source_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  if (TARGET_no_partword_load && memory_operand (op, mode))
    {
      if (GET_CODE (op) == MEM &&
	  (GET_CODE (XEXP (op, 0)) == REG
	   || GET_CODE (XEXP (op, 0)) == CONST_INT))
	return 1;
      return 0;
    }
  return general_operand (op, mode);
}
#endif

int
op1_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  enum rtx_code code = GET_CODE (op);
  if (GET_MODE (op) != mode)
    return 0;
  return (code == PLUS || code == MINUS || code == IOR
	  || code == XOR || code == ASHIFT);
}

int
op2_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  enum rtx_code code = GET_CODE (op);
  if (GET_MODE (op) != mode)
    return 0;
  return (code == PLUS || code == MINUS || code == AND || code == IOR
	  || code == XOR || code == ASHIFT);
}

int
operand2_ok_for_operator (operator, operand)
     rtx operator;
     rtx operand;
{
  enum rtx_code and = GET_CODE (operand);
  if (and == REG)
    return 1;
  if (and == CONST_INT)
    {
      switch (GET_CODE (operator))
	{
	case PLUS:
	case MINUS:
	  return arith_operand (operand, SImode);
	case AND:
	  return and_operand (operand, SImode);
	case IOR:
	case XOR:
	  return logic_operand (operand, SImode);
	case ASHIFT:
	case LSHIFTRT:
	case ASHIFTRT:
	  return shift_operand (operand, SImode);
	default:
	  return 0;
	}
    }
  return 0;
}

#if 0
/* Return truth value of whether OP is a integer which fits the
   range constraining immediate operands in logic insns other than and.  */
static int logic_int (rtx op, enum machine_mode mode);

static
int
logic_int (op, mode)
     rtx op;
     enum machine_mode mode;
{
  (void) mode;
  return (GET_CODE (op) == CONST_INT && LOGIC_INT (INTVAL (op)));
}
#endif

#if 0
/* Return truth value of whether OP is a integer which fits the
   range constraining immediate operands in and insns.  */
static int and_int (rtx op, enum machine_mode mode);

static
int
and_int (op, mode)
     rtx op;
     enum machine_mode mode;
{
  (void) mode;
  return (GET_CODE (op) == CONST_INT && AND_INT (INTVAL (op)));
}
#endif

/* Test for a valid operand for a call instruction.
   Don't allow the arg pointer register or virtual regs
   since they may change into reg + const, which the patterns
   can't handle yet.  BAD */
int
call_insn_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  (void) mode;

  if (GET_CODE (op) == MEM
      && (CONSTANT_ADDRESS_P (XEXP (op, 0))
	  || (GET_CODE (XEXP (op, 0)) == REG
	      && XEXP (op, 0) != arg_pointer_rtx
	      && !(REGNO (XEXP (op, 0)) >= FIRST_PSEUDO_REGISTER
		   && REGNO (XEXP (op, 0)) <= LAST_VIRTUAL_REGISTER))))
    return 1;
  return 0;
}


#if 0
static const char *output_movsf (rtx *operands);

static
const char *
output_movsf (operands)
     rtx *operands;
{
  output_asm_insn ("!*** Start invalid movsf ***", operands);
  return "!*** End invalid movsf ***";
}
#endif

#if 0
static const char *output_movdf (rtx *operands);

static
const char *
output_movdf (operands)
     rtx *operands;
{
  output_asm_insn ("!*** Start invalid movdf ***", operands);
  return "!*** End invalid movdf ***";
}
#endif

/* This function generates the assembly code for function entry.
   The macro FUNCTION_PROLOGUE in i860.h is defined to call this function.

   ASM_FILE is a stdio stream to output the code to.
   SIZE is an int: how many units of temporary storage to allocate.

   Refer to the array `regs_ever_live' to determine which registers
   to save; `regs_ever_live[I]' is nonzero if register number I
   is ever used in the function.  This macro is responsible for
   knowing which registers should not be saved even if used.

   NOTE: `frame_lower_bytes' is the count of bytes which will lie
   between the new `fp' value and the new `sp' value after the
   prologue is done.  `frame_upper_bytes' is the count of bytes
   that will lie between the new `fp' and the *old* `sp' value
   after the new `fp' is setup (in the prologue).  The upper
   part of each frame always includes at least 2 words (8 bytes)
   to hold the saved frame pointer and the saved return address.
   Glenn: This seems wrong.  The code puts it in the lower part.

   -The svr4 ABI for the i860 now requires that the values of the
   -stack pointer and frame pointer registers be kept aligned to
   -16-byte boundaries at all times.  We obey that restriction here.

   -The svr4 ABI for the i860 is entirely vague when it comes to specifying
   -exactly where the "preserved" registers should be saved.  The native
   -svr4 C compiler I now have doesn't help to clarify the requirements
   -very much because it is plainly out-of-date and non-ABI-compliant
   -(in at least one important way, i.e. how it generates function
   -epilogues).

   The native svr4 C compiler saves the "preserved" registers (i.e.
   r4-r15) in the lower part of a frame (i.e. at negative
   offsets from the frame pointer).

   Previous versions of GCC also saved the "preserved" registers in the
   "negative" part of the frame, but they saved them using positive
   offsets from the (adjusted) stack pointer (after it had been adjusted
   to allocate space for the new frame).  That's just plain wrong
   because if the current function calls alloca(), the stack pointer
   will get moved, and it will be impossible to restore the registers
   properly again after that.

   Both compilers handled parameter registers (i.e. r16-r27 and f8-f15)
   by copying their values either into various "preserved" registers or
   into stack slots in the lower part of the current frame (as seemed
   appropriate, depending upon subsequent usage of these values).

   Here we want to save the preserved registers at some offset from the
   frame pointer register so as to avoid any possible problems arising
   from calls to alloca().  We can either save them at small positive
   offsets from the frame pointer, or at small negative offsets from
   the frame pointer.  If we save them at small negative offsets from
   the frame pointer (i.e. in the lower part of the frame) then we
   must tell the rest of GCC (via STARTING_FRAME_OFFSET) exactly how
   many bytes of space we plan to use in the lower part of the frame
   for this purpose.  Since other parts of the compiler reference the
   value of STARTING_FRAME_OFFSET long before final() calls this function,
   we would have to go ahead and assume the worst-case storage requirements
   for saving all of the "preserved" registers (and use that number, i.e.
   `80', to define STARTING_FRAME_OFFSET) if we wanted to save them in
   the lower part of the frame.  That could potentially be very wasteful,
   and that wastefulness could really hamper people compiling for embedded
   i860 targets with very tight limits on stack space.  Thus, we choose
							---------------
   here to save the preserved registers in the upper part of the
   -------------------------------------------------------------
   frame, so that we can decide at the very last minute how much (or how
   -----
   little) space we must allocate for this purpose.

   -To satisfy the needs of the svr4 ABI "tdesc" scheme, preserved
   -registers must always be saved so that the saved values of registers
   -with higher numbers are at higher addresses.  We obey that restriction
   -here.

   There are two somewhat different ways that you can generate prologues
   here... i.e. pedantically ABI-compliant, and the "other" way.  The
   "other" way is more consistent with what is currently generated by the
   "native" svr4 C compiler for the i860.  That's important if you want
   to use the current (as of 8/91) incarnation of svr4 SDB for the i860.
   The SVR4 SDB for the i860 insists on having function prologues be
   non-ABI-compliant!

   To get fully ABI-compliant prologues, define I860_STRICT_ABI_PROLOGUES
   in the i860svr4.h file.  (By default this is *not* defined).

   The differences between the ABI-compliant and non-ABI-compliant prologues
   are that (a) the ABI version seems to require the use of *signed*
   (rather than unsigned) adds and subtracts, and (b) the ordering of
   the various steps (e.g. saving preserved registers, saving the
   return address, setting up the new frame pointer value) is different.

   For strict ABI compliance, it seems to be the case that the very last
   thing that is supposed to happen in the prologue is getting the frame
   pointer set to its new value (but only after everything else has
   already been properly setup).  We do that here, but only if the symbol
   I860_STRICT_ABI_PROLOGUES is defined.

     The function entry code is responsible for allocating any stack
     space required for the function.  This stack space consists of the
     regions listed below.  In most cases, these regions are allocated
     in the order listed, with the last listed region closest to the
     top of the stack (the lowest address if `STACK_GROWS_DOWNWARD' is
     defined, and the highest address if it is not defined).  You can
     use a different order for a machine if doing so is more convenient
     or required for compatibility reasons.  Except in cases where
     required by standard or by a debugger, there is no reason why the
     stack layout used by GCC need agree with that used by other
     compilers for a machine.

        * A region of `current_function_pretend_args_size' bytes of
          uninitialized space just underneath the first argument
          arriving on the stack.  (This may not be at the very start of
          the allocated stack region if the calling sequence has pushed
          anything else since pushing the stack arguments.  But
          usually, on such machines, nothing else has been pushed yet,
          because the function prologue itself does all the pushing.)
          This region is used on machines where an argument may be
          passed partly in registers and partly in memory, and, in some
          cases to support the features in `varargs.h' and `stdargs.h'.

        * An area of memory used to save certain registers used by the
          function.  The size of this area, which may also include
          space for such things as the return address and pointers to
          previous stack frames, is machine-specific and usually
          depends on which registers have been used in the function.
          Machines with register windows often do not require a save
          area.

        * A region of at least SIZE bytes, possibly rounded up to an
          allocation boundary, to contain the local variables of the
          function.  On some machines, this region and the save area
          may occur in the opposite order, with the save area closer to
          the top of the stack.

        * Optionally, when `ACCUMULATE_OUTGOING_ARGS' is defined, a
          region of `current_function_outgoing_args_size' bytes to be
          used for outgoing argument lists of the function.  *Note
          Stack Arguments::.

     Normally, it is necessary for the macros `FUNCTION_PROLOGUE' and
     `FUNCTION_EPILOGUE' to treat leaf functions specially.  The C
     variable `leaf_function' is nonzero for such a function.

Note: We don`t treat leaf functions specially because the most
effecient way to handle function calls on the lanai seems to
be having the function call itself push the return address.  This
makes leaf function returns SHORTER by an insn, but require
an extra memory access.  It makes leaf function calls the
same size, but require an extra memory access.  It does make
all function calls identical, which SIMPLIFIES THINGS.  It
makes code on double-clocked machines slightly slower by a
memory clock cycle, but
makes code on single-clocked machines slightly faster by an
instruction clock cycle.

Note: We don't support current_function_pretend_args_size because
all arguments are passed on the stack, so the region specified
by this variable is not used.

Note: We don't support current_function_outgoing_args_size because
ACCUMULATE_OUTGOING_ARGS is not defined, so the region specified
by this variable is not used.

before	after	after	after	after	stack
call	call	prolog	func	epilog	contents
------	------	------	------	------	---------
fp->	fp->			fp->	junk...
					  ...
					...end of junk
					saved...
					  ....
					...registers
					function...
					    ...
sp->		fp->	fp->	sp->	...args
	sp->				return address (or garbage)
					saved frame pointer
					SIZE bytes allocated...
						 ...
		sp->			...storage
					alloca'd...
					    ...
			sp->		...storage
*/

#ifndef STACK_ALIGNMENT
#define STACK_ALIGNMENT	4
#endif

extern char call_used_regs[];
extern int leaf_function_p (void);

char *current_function_original_name;

static int preserved_bytes;

/* This function generates the assembly code for function entry.
   FILE is a stdio stream to output the code to.
   SIZE is an int: how many units of temporary storage to allocate. */
void
lanai_output_function_prologue (asm_file, SIZE)
     register FILE *asm_file;
     register unsigned SIZE;
{
  /* NOTE: The return address is pushed by the caller onto
     the top of the stack just under the function args. */

  /* Round-up the frame_lower_bytes so that it's a multiple of
     STACK_ALIGNMENT. */
  preserved_bytes = (SIZE
#ifdef ACCUMULATE_OUTGOING_ARGS
		     + current_function_outgoing_args_size
#endif
		     + STACK_ALIGNMENT - 1) & -STACK_ALIGNMENT;

  fprintf (asm_file, "\tst %s%s,-4[*%s%s]	!push old FP\n",
	   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
	   lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM]);

  /* Generate the new frame pointer */
  fprintf (asm_file, "\tadd %s%s,8,%s%s	!generate new FP\n",
	   lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM],
	   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM]);

  /*  If needed, Allocate SIZE bytes of storage on the stack starting
     two word slots below the frame pointer. */
  if (preserved_bytes)
    {
      fprintf (asm_file, "\tsub %s%s,0x%X,%s%s	!allocate stack space\n",
	       lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM],
	       preserved_bytes,
	       lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM]);
    }
}


/* This function generates the assembly code for function exit.
   The macro FUNCTION_EPILOGUE in lanai3.h is defined to call this function.

   ASM_FILE is a stdio stream to output the code to.
   SIZE is an int: how many units of temporary storage to allocate.

   The function epilogue should not depend on the current stack pointer!
   It should use the frame pointer only.  This is mandatory because
   of alloca; we also take advantage of it to omit stack adjustments
   before returning.

   Note that when we go to restore the preserved register values we must
   not try to address their slots by using offsets from the stack pointer.
   That's because the stack pointer may have been moved during the function
   execution due to a call to alloca().  Rather, we must restore all
   preserved registers via offsets from the frame pointer value.

   Note also that when the current frame is being "popped" (by adjusting
   the value of the stack pointer) on function exit, we must (for the
   sake of alloca) set the new value of the stack pointer based upon
   the current value of the frame pointer.  We can't just add what we
   believe to be the (static) frame size to the stack pointer because
   if we did that, and alloca() had been called during this function,
   we would end up returning *without* having fully deallocated all of
   the space grabbed by alloca.  If that happened, and a function
   containing one or more alloca() calls was called over and over again,
   then the stack would grow without limit!

If return address is in stack frame and mem->pc is OK:
	ld -4[%fp],%pc
	ld -8[%fp],%fp
	mov %fp,%sp
If return address is in stack frame and mem->pc is not OK:
	ld -4[%fp],%rca
	ld -8[%fp],%fp
	mov %fp,%sp
	mov %rca,%pc
	nop
If return address is in register:
	ld -8[%fp],%fp
	mov %fp,%sp
	mov %ra,%pc
	nop
*/

void
lanai_output_function_epilogue (asm_file, SIZE)
     register FILE *asm_file;
     register unsigned SIZE;
{
  (void) SIZE;

#if 0
  if (leaf_function_p ())
    {
      /* restore the stack pointer */
      fprintf (asm_file, "\tmov %s%s,%s%s	!restore SP\n",
	       lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
	       lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM]);

      /* restore the frame pointer from stack (1 shadow) */
      fprintf (asm_file, "\tld -8[%s%s],%s%s	!restore FP (1 shadow)\n",
	       lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
	       lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM]);

      /* move saved return address from RCA to pc (1 shadows) */
      fprintf (asm_file, "\tmov %s%s,%s%s	!PC<-RA (1 shadow)\n",
	       lanai_reg_prefix, reg_names[RETURN_ADDRESS_REGNUM],
	       lanai_reg_prefix, reg_names[PC_REGNUM]);

      fprintf (asm_file, "\tnop		!fill shadow\n");

      return;
    }
#endif

  /*  Return address is in the stack frame */

  if (TARGET_no_pc_load)
    {
      /* Not using PC load */

      /* move saved return address from stack to rca */
      fprintf (asm_file, "\tld -4[%s%s],%s%s\n",
	       lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
	       lanai_reg_prefix, reg_names[CONST_ADDR_REG]);

      /* restore the stack pointer using the old frame pointer value */
      fprintf (asm_file, "\tadd %s%s,0x%X,%s%s\n",
	       lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
	       current_function_pops_args,
	       lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM]);

      /* restore the frame pointer from stack */
      fprintf (asm_file, "\tld -8[%s%s],%s%s\n",
	       lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
	       lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM]);

      /* move saved return address from RCA to pc */
      fprintf (asm_file, "\tadd %s%s,0x0,%s%s\n",
	       lanai_reg_prefix, reg_names[CONST_ADDR_REG],
	       lanai_reg_prefix, reg_names[PC_REGNUM]);

      /* fill jump delay slot */
      if (lanai_jump_delay == 2)
	{
	  fprintf (asm_file, "\tnop\n");
	  fprintf (asm_file, "\tnop\n");
	}
      else if (lanai_jump_delay == 1)
	{
	  fprintf (asm_file, "\tnop\n");
	}
      else if (lanai_jump_delay != 0)
	{
	  abort ();
	}
    }
  else
    {
      /* Using PC load */

      if (TARGET_old_value_usable_in_ld_delay)
	{
	  /* move saved return address from stack to PC */
	  fprintf (asm_file, "\tld -4[%s%s],%s%s\n",
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
		   lanai_reg_prefix, reg_names[PC_REGNUM]);

	  /* restore the frame pointer from stack */
	  fprintf (asm_file, "\tld -8[%s%s],%s%s\n",
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM]);

	  /* restore the stack pointer using the old frame pointer value */
	  fprintf (asm_file, "\tadd %s%s,0x%X,%s%s\n",
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
		   current_function_pops_args,
		   lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM]);

	  /* fill remaining pc load shadows */
	  if (lanai_max_indirect_jump_delay == 4)
	    {
	      fprintf (asm_file, "\tnop\n");
	      fprintf (asm_file, "\tnop\n");
	    }
	  else if (lanai_max_indirect_jump_delay != 2)
	    {
	      abort ();
	    }
	}
      else /* !TARGET_old_value_usable_in_ld_delay */
	{
	  /* restore the stack pointer using the old frame pointer value */
	  /* move saved return address from stack to PC */
	  fprintf (asm_file, "\tld -4[%s%s],%s%s\n",
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
		   lanai_reg_prefix, reg_names[PC_REGNUM]);

	  fprintf (asm_file, "\tadd %s%s,0x%X,%s%s\n",
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
		   current_function_pops_args,
		   lanai_reg_prefix, reg_names[STACK_POINTER_REGNUM]);

	  /* restore the frame pointer from stack */
	  fprintf (asm_file, "\tld -8[%s%s],%s%s\n",
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM],
		   lanai_reg_prefix, reg_names[FRAME_POINTER_REGNUM]);

	  if (lanai_max_indirect_jump_delay == 4)
	    {
	      fprintf (asm_file, "\tnop\n");
	    }
	  else if (lanai_max_indirect_jump_delay != 2)
	    {
	      abort ();
	    }
	}
    }
}

static int REG_OK (int STRICT, rtx X);

static
int
REG_OK (STRICT, X)
     int STRICT;
     rtx X;
{
  if (!STRICT)
    return 1;
  return REGNO_OK_FOR_INDEX_P (REGNO (X));
}

int
legitimate_address (MODE, X, STRICT)
     enum machine_mode MODE;
     rtx X;
     int STRICT;
{
  if (GET_CODE (X) == REG)
    {
      /* Address is in a register */
      if (REG_OK (STRICT, X))
	return 1;
    }
  else if (GET_CODE (X) == CONST_INT)
    {
      switch (GET_MODE_SIZE (MODE))
	{
	case 4:		/* Fullword access */
	  if (RM_INT (INTVAL (X))
	      && (INTVAL (X) & (GET_MODE_SIZE (MODE) - 1)) == 0)
	    return 1;
	  break;
	case 2:
	case 1:		/* Partword access */
	  if (SPLS_INT (INTVAL (X))
	      && (INTVAL (X) & (GET_MODE_SIZE (MODE) - 1)) == 0)
	    return 1;
	  break;
	default:
	  return 0;
	}
    }
  else if (TARGET_no_partword_load && GET_MODE_CLASS (MODE) == MODE_INT
	   && (GET_MODE_SIZE (MODE) == 1 || GET_MODE_SIZE (MODE) == 2))
    {
      /* Require address to be a constant or
         in a register for partword memory
         accesses. */
      return 0;
    }
  else if (GET_CODE (X) == PLUS && GET_CODE (XEXP (X, 1)) == CONST_INT)
    {
      if (GET_CODE (XEXP (X, 0)) == REG && REG_OK (STRICT, XEXP (X, 0)))
	{
	  switch (GET_MODE_SIZE (MODE))
	    {
	    case 4:		/* Fullword access */
	      if (RM_INT (INTVAL (XEXP (X, 1)))
		  && (INTVAL (XEXP (X, 1)) & (GET_MODE_SIZE (MODE) - 1)) == 0)
		return 1;
	      break;
	    case 2:
	    case 1:		/* Partword access */
	      if (SPLS_INT (INTVAL (XEXP (X, 1)))
		  && (INTVAL (XEXP (X, 1)) & (GET_MODE_SIZE (MODE) - 1)) == 0)
		return 1;
	      break;
	    default:
	      return 0;
	    }
	}
      else
	{
	  return 0;
	}
    }
  else if (GET_CODE (X) == PLUS || GET_CODE (X) == MINUS ||
	   GET_CODE (X) == AND || GET_CODE (X) == IOR ||
	   GET_CODE (X) == XOR || GET_CODE (X) == ASHIFT ||
	   GET_CODE (X) == LSHIFTRT)
    {
      if (GET_CODE (XEXP (X, 0)) == REG && REG_OK (STRICT, XEXP (X, 0)))
	{
	  if (GET_CODE (XEXP (X, 1)) == REG && REG_OK (STRICT, XEXP (X, 1)))
	    {
	      /* Do not allow "REG OP REG" addressing for doublewords, since
		 it is way too complictated to support. */

	      return (GET_MODE_SIZE (MODE) <= 4);
	    }
	}
    }
  else if (GET_CODE (X) == PRE_DEC || GET_CODE (X) == POST_DEC)
    {
      /* At least one LANai does not support partword PRE and POST
	 decrement.  Catch that case. */

      if (TARGET_broken_spls_q_neg && GET_MODE_SIZE (MODE) < 4)
	{
	  return 0;
	}

      /* Do no support pre/post for doubleword, since doing so is too complex
	 and not very useful since we ACCUMULATE_OUTGOING_ARGS. */

      if (MODE == DImode)
	{
	  return 0;
	}

      if (GET_CODE (XEXP (X, 0)) == REG && REG_OK (STRICT, XEXP (X, 0)))
	{
	  return 1;
	}
    }
  else if (GET_CODE (X) == PRE_INC || GET_CODE (X) == POST_INC)
    {
      /* Do no support pre/post for doubleword, since doing so is too complex
	 and not very useful since we ACCUMULATE_OUTGOING_ARGS. */

      if (MODE == DImode)
	{
	  return 0;
	}

      if (GET_CODE (XEXP (X, 0)) == REG && REG_OK (STRICT, XEXP (X, 0)))
	{
	  return 1;
	}
    }
  else if (GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF)
    {
      return (GET_MODE_SIZE (MODE) >= 4 ? 1 : 0);
    }
  else if (CONSTANT_ADDRESS_P (X))
    {
      return (GET_MODE_SIZE (MODE) >= 4 ? 1 : 0);;
    }
  return 0;
}

static int mode_dependent_address (rtx addr);

static
int
mode_dependent_address (addr)
     rtx addr;
{
  switch (GET_CODE (addr))
    {
    case MEM:
      return mode_dependent_address (XEXP (addr, 0));
    case PLUS:
    case MINUS:
      return (mode_dependent_address (XEXP (addr, 0))
	      || mode_dependent_address (XEXP (addr, 1)));
    case REG:
    case CONST:
    case CONST_INT:
    case LABEL_REF:
    case SYMBOL_REF:
      return 0;
/* IMPLICIT:
  case PRE_DEC:
  case PRE_INC:
  case POST_DEC:
  case POST_INC:
*/
    default:
      return 1;
    }
}

static void print_const (FILE *f, rtx a);

static
void
print_const (f, a)
     FILE *f;
     rtx a;
{
  switch (GET_CODE (a))
    {
    case CONST:
      print_const (f, XEXP (a, 0));
      break;
    case PLUS:
      fprintf (f, "(");
      print_const (f, XEXP (a, 0));
      fprintf (f, "+");
      print_const (f, XEXP (a, 1));
      fprintf (f, ")");
      break;
    case MINUS:
      fprintf (f, "(");
      print_const (f, XEXP (a, 0));
      fprintf (f, "-");
      print_const (f, XEXP (a, 1));
      fprintf (f, ")");
      break;
    case LABEL_REF:
    case SYMBOL_REF:
      assemble_name (f, (char *) XEXP (a, 0));
      break;
    case CONST_INT:
      fprintf (f, "0x%X", INTVAL (a));
      break;
    default:
      abort ();
    }
}

void
print_operand_address (f, a)
     FILE *f;
     rtx a;
{
  register rtx addr = a;
  enum tree_code code = GET_CODE (addr);
  switch (code)
    {
    case REG:
      fprintf (f, "[%s%s]", lanai_reg_prefix, reg_names[REGNO (addr)]);
      break;
    case CONST_INT:
      fprintf (f, "[0x%X]", INTVAL (addr));
      break;
    case CONST:
      fprintf (f, "[");
      print_const (f, XEXP (addr, 0));
      fprintf (f, "]");
      /* fprintf(f,"[%sr0]",lanai_reg_prefix); */
      break;
    case SYMBOL_REF:
    case LABEL_REF:
      fputc ('[', f);
      assemble_name (f, (char *) XEXP (addr, 0));
      fputc (']', f);
      break;
    case PRE_INC:
      fprintf (f, "[++%s%s]",
	       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))]);
      break;
    case PRE_DEC:
      fprintf (f, "[--%s%s]",
	       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))]);
      break;
    case POST_INC:
      fprintf (f, "[%s%s++]",
	       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))]);
      break;
    case POST_DEC:
      fprintf (f, "[%s%s--]",
	       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))]);
      break;
    case PLUS:
      if (GET_CODE (XEXP (addr, 0)) == REG)
	{
	  switch (GET_CODE (XEXP (addr, 1)))
	    {
	    case CONST_INT:
	      if (INTVAL (XEXP (addr, 1)) >> 31)
		{
		  fprintf (f, "-0x%X[%s%s]", -INTVAL (XEXP (addr, 1)),
			   lanai_reg_prefix,
			   reg_names[REGNO (XEXP (addr, 0))]);
		}
	      else
		{
		  fprintf (f, "0x%X[%s%s]", INTVAL (XEXP (addr, 1)),
			   lanai_reg_prefix,
			   reg_names[REGNO (XEXP (addr, 0))]);
		}
	      break;
	    case LABEL_REF:
	    case SYMBOL_REF:
	      /* FIXME: scary cast. */
	      fprintf (f, "_%s[%s%s]", (char *)XEXP (addr, 1),
		       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))]);
	      break;
	    case CONST:
	      print_const (f, XEXP (addr, 1));
	      fprintf (f, "[%s%s]",
		       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))]);
	      break;
	    case REG:
	      fprintf (f, "[%s%s add %s%s]",
		       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))],
		       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 1))]);
	      break;
	    default:
	      abort ();
	    }
	}
      else
	{
	  abort ();
	}
      break;
    case MINUS:
    case AND:
    case IOR:
    case XOR:
    case ASHIFT:
    case LSHIFTRT:
      fprintf (f, "[%s%s %s %s%s]",
	       lanai_reg_prefix, reg_names[REGNO (XEXP (addr, 0))],
	       code == MINUS ? "sub" : code == AND ? "and" : code ==
	       IOR ? "or" : code == XOR ? "xor" : code ==
	       ASHIFT ? "sh" : "sha", lanai_reg_prefix,
	       reg_names[REGNO (XEXP (addr, 1))]);
      break;
    default:
      abort ();
      fprintf (f, "*** ERROR 2 IN PRINT_OPERAND_ADDRESS (%s) ***",
	       tree_code_name[code]);
    }
}

/* Function to output any store */
void
output_store (operands, m)
     rtx *operands;
     enum machine_mode m;
{
  if (m == SImode || m == SFmode)
    output_asm_insn ("st %1,%0", operands);
  else if (m == DImode)
    {
      rtx fake_operands[4];

      fake_operands[0] = gen_lowpart (SImode, operands[0]);
      fake_operands[1] = gen_lowpart (SImode, operands[1]);
      fake_operands[2] = gen_highpart (SImode, operands[0]);
      fake_operands[3] = gen_highpart (SImode, operands[1]);
      output_store (fake_operands, SImode);
      output_store (fake_operands+2, SImode);
    }
  else
    {
      if (TARGET_no_partword_store)
	{
	  rtx o[5];

	  if (m != QImode && m != HImode)
	    abort ();

	  o[0] = operands[0];
	  o[1] = operands[1];

	  /* Pick 3 registers not used by the operands to use
	     as temporaries.  The last of them should be register
	     15, the scratch register. */

	  {
	    rtx *_o;
	    int regno = 9;
	    rtx reg;

	    _o = &o[2];
	    do
	      {
		reg = gen_rtx (REG, SImode, regno);
		if (reg_overlap_mentioned_for_reload_p (reg, o[0])
		    || reg_overlap_mentioned_for_reload_p (reg, o[1])
		    || regno == 15)
		  {
		    regno++;
		    continue;
		  }
		*_o = reg;
		regno++;
		_o++;
	      }
	    while (_o < &o[4]);
	  }
	  o[4] = gen_rtx (REG, SImode, 15);

	  output_asm_insn ("! begin partword store for reload", o);
	  output_asm_insn ("st %2,[--%?sp]", o);
	  output_asm_insn ("st %3,[--%?sp]", o);
	  if (m == QImode)
	    {
	      output_asm_insn ("mov 0x18100000,%2", o);
	      output_asm_insn ("or %2,0x0800,%2", o);
	    }
	  else
	    {
	      output_asm_insn ("mov 0x00001000,%2", o);
	    }
	  if (! TARGET_old_value_usable_in_ld_delay)
	    {
	      abort ();
	    }
	  output_asm_insn ("ld %0,%2 ! %2 <- backing", o);
	  output_asm_insn ("st %2,%0 ! memory word <- 0x00081018", o);
	  output_asm_insn ("ld.b %0,%3 ! %3 <- shift amount", o);
	  output_asm_insn (m == QImode ? "mov 0xff,%4" : "mov 0xffff,%4", o);
	  output_asm_insn ("sh %4,%3,%4 ! %4 <- shifted mask", o);
	  output_asm_insn ("sh %1,%3,%3 ! %3 <- shifted data", o);
	  output_asm_insn ("and %3,%4,%3 ! %3 <- masked shifted data", o);
	  output_asm_insn ("xor %4,%?r1,%4 ! %4 <- complementary mask", o);
	  output_asm_insn ("and %2,%4,%2 ! %2 <- masked backing", o);
	  output_asm_insn ("or %2,%3,%2 ! %2 <- word_to_write", o);
	  output_asm_insn ("ld [%?sp++],%3", o);
	  output_asm_insn ("ld [%?sp++],%2", o);
	  output_asm_insn ("st %2,%0 ! finally, perform the store", o);
	  output_asm_insn ("! end partword store for reload", o);
	}
      else
	{
	  if (m == QImode || m == QFmode)
	    output_asm_insn ("st.b %1,%0", operands);
	  else if (m == HImode || m == HFmode)
	    output_asm_insn ("st.h %1,%0", operands);
	  else
	    abort ();
	}
    }
}

/* Function to output any load.  Returns the number of clock cycles
   before the result of the load is available. */
int
output_load (operands, m)
     rtx *operands;
     enum machine_mode m;
{
  if (m == SImode || m == SFmode)
    {
      output_asm_insn ("ld %1,%0", operands);
      return lanai_ld_delay;
    }
  else if (m == QImode || m == QFmode)
    {
      if (TARGET_no_partword_load)
	{
	  output_asm_insn ("ld %1, %0", operands);
	  /* Now use the register holding the address. */
	  operands[1] = XEXP (operands[1], 0);
	  if (GET_CODE (operands[1]) != REG)
	    abort ();
	  output_asm_insn ("sha %1,30,%?rca", operands);
	  output_asm_insn ("sh %?rca,-27,%?rca", operands);
	  output_asm_insn ("sha %0,%?rca,%0", operands);
	  output_asm_insn ("sh %0,-24,%0", operands);
	  return 0;
	}
      else
	{
	  output_asm_insn ("uld.b %1,%0", operands);
	  return lanai_ld_delay;
	}
    }
  else if (m == HImode || m == HFmode)
    {
      if (TARGET_no_partword_load)
	{
	  output_asm_insn ("ld %1, %0", operands);
	  /* Now use the register holding the address. */
	  operands[1] = XEXP (operands[1], 0);
	  if (GET_CODE (operands[1]) != REG)
	    abort ();
	  output_asm_insn ("sha %1,30,%?rca", operands);
	  output_asm_insn ("sh %?rca,-27,%?rca", operands);
	  output_asm_insn ("sha %0,%?rca,%0", operands);
	  output_asm_insn ("sh %0,-16,%0", operands);
	  return 0;
	}
      else
	{
	  output_asm_insn ("uld.h %1,%0", operands);
	  return lanai_ld_delay;
	}
    }
  else if (m == DImode)
    {
      rtx fake_operands[4];

      fake_operands[0] = gen_lowpart (SImode, operands[0]);
      fake_operands[1] = gen_lowpart (SImode, operands[1]);
      fake_operands[2] = gen_highpart (SImode, operands[0]);
      fake_operands[3] = gen_highpart (SImode, operands[1]);

      /* Check if the register "fake_operands[0]" is used in the address of
	 the second load.  If so, perform the loads in reverse order.  This
	 works because the 2 load regs are different, and for DI loads, there
	 is at most one register in the addr.  However, reversing the order
	 requires an extra load delay slot. */

      if (!TARGET_old_value_usable_in_ld_delay
	  && reg_overlap_mentioned_for_reload_p (fake_operands[0],
						 fake_operands[3]))
	{
	  output_load (fake_operands+2, SImode);
	  output_load (fake_operands, SImode);
	  return lanai_ld_delay;
	}


      output_load (fake_operands, SImode);
      output_load (fake_operands+2, SImode);
      return lanai_ld_delay;
    }
  else
    abort ();
}

/* Function to output any possible move */
const char *
output_move (operands, m)
     rtx *operands;
     enum machine_mode m;
{
  rtx fake_operands[2];

  if (GET_CODE (operands[0]) == REG)
    {
      if (GET_CODE (operands[1]) == REG)
	return "mov %1,%0";
      else if (GET_CODE (operands[1]) == MEM)
	{
	  emit_nops_if_needed (operands[0], output_load (operands, m));
	  return "";
	}
      else if (GET_CODE (operands[1]) == CONST_INT)
	{
	  int high;
	  int low;
	  /* Try to move const into reg in 1 insn */
	  if (SLI_INT (INTVAL (operands[1])))
	    return "mov %1,%0";
	  if (operands[1] == constm1_rtx)
	    return "mov %?r1,%0";
	  if (ARITH_INT (INTVAL (operands[1])))
	    {
	      if ((INTVAL (operands[1]) & 0x0000ffff) == 0 ||
		  (INTVAL (operands[1]) & 0xffff0000) == 0)
		return "add %?r0,%1,%0";
	      else if ((-INTVAL (operands[1]) & 0x0000ffff) == 0 ||
		       (-INTVAL (operands[1]) & 0xffff0000) == 0)
		{
		  operands[1] = gen_rtx (CONST_INT, VOIDmode,
					 -INTVAL (operands[1]));
		  output_asm_insn ("sub %?r0,%1,%0", operands);
		  return "";
		}
	      else
		abort ();
	    }
	  if (AND_INT (INTVAL (operands[1])))
	    return "and %?r1,%1,%0";
	  /* Move const into reg in 2 insns */
	  low = INTVAL (operands[1]) & 0xffff0000;
	  high = INTVAL (operands[1]) & 0x0000ffff;
	  if (!high || !low)
	    abort ();
	  operands[1] = GEN_INT (high);
	  output_asm_insn ("mov %1,%0", operands);
	  operands[1] = GEN_INT (low);
	  return "or %0,%1,%0";
	}
      else if (GET_CODE (operands[1]) == CONST ||
	       GET_CODE (operands[1]) == SYMBOL_REF ||
	       GET_CODE (operands[1]) == LABEL_REF)
	return "mov %1,%0";
      else
	abort ();
    }
  else if (GET_CODE (operands[0]) == MEM)
    {
      if (GET_CODE (operands[1]) == REG)
	{
	  output_store (operands, m);
	  return "";
	}
      else if (GET_CODE (operands[1]) == MEM)
	{
	  fake_operands[0] = gen_rtx (REG, SImode, 15);
	  fake_operands[1] = operands[1];

	  emit_nops_if_needed (operands[0], output_load (fake_operands, m));
	  fake_operands[1] = fake_operands[0];
	  fake_operands[0] = operands[0];
	  output_store (fake_operands, m);
	  return "";
	}
      else if (GET_CODE (operands[1]) == CONST_INT ||
	       GET_CODE (operands[1]) == CONST ||
	       GET_CODE (operands[1]) == SYMBOL_REF ||
	       GET_CODE (operands[1]) == LABEL_REF)
	{
	  if (operands[1] == const0_rtx)
	    {
	      operands[1] = gen_rtx (REG, SImode, 0);
	      output_store (operands, m);
	      return "";
	    }
	  else if (operands[1] == constm1_rtx ||
		   (m == HImode && INTVAL (operands[1]) == 0xffff) ||
		   (m == QImode && INTVAL (operands[1]) == 0xff))
	    {
	      operands[1] = gen_rtx (REG, SImode, 1);
	      output_store (operands, m);
	      return "";
	    }
	  else
	    {
	      fake_operands[1] = operands[1];
	      fake_operands[0] = gen_rtx (REG, SImode, 15);
	      output_asm_insn (output_move (fake_operands, m), fake_operands);

	      fake_operands[1] = fake_operands[0];
	      fake_operands[0] = operands[0];
	      output_store (fake_operands, m);
	      return "";
	    }
	}
      else
	abort ();
    }
  else
    abort ();
}


const char *
output_movdi (operands)
     rtx *operands;
{
  rtx fake_operands[6];
  const enum machine_mode m = DImode;

  output_asm_insn ("! movdi %1->%0", operands);

  if (GET_CODE (operands[0]) == REG)
    {
      if (GET_CODE (operands[1]) == REG)
	{
	  fake_operands[0] = gen_lowpart (SImode, operands[0]);
	  fake_operands[1] = gen_lowpart (SImode, operands[1]);
	  fake_operands[2] = gen_highpart (SImode, operands[0]);
	  fake_operands[3] = gen_highpart (SImode, operands[1]);
	  if (REGNO (operands[0]) + 1 == REGNO (operands[1]))
	    output_asm_insn ("mov %3,%2\n\tmov %1,%0", fake_operands);
	  else
	    output_asm_insn ("mov %1,%0\n\tmov %3,%2", fake_operands);
	  return "";
	}
      else if (GET_CODE (operands[1]) == MEM)
	{
	  emit_nops_if_needed (operands[0], output_load (operands, m));
	  return "";
	}
      else if (GET_CODE (operands[1]) == CONST_INT)
	{
	  fake_operands[0] = gen_lowpart (SImode, operands[0]);
	  fake_operands[1] = gen_lowpart (SImode, operands[1]);
	  fake_operands[2] = gen_highpart (SImode, operands[0]);

#if HOST_BITS_PER_WIDE_INT == 64
	  fake_operands[3] = gen_rtx_CONST_INT (VOIDmode, INTVAL (operands[1])>>32);
#else
#if HOST_BITS_PER_WIDE_INT == 32
	  if (INTVAL (operands[1]) < 0)
	    fake_operands[3] = constm1_rtx;
	  else
	    fake_operands[3] = const0_rtx;
#else
 #error Unsupported HOST_BITS_PER_WIDE_INT value.
#endif
#endif

	  output_asm_insn (output_move (fake_operands, SImode), fake_operands);
	  output_asm_insn (output_move (fake_operands+2, SImode),
			   fake_operands+2);
	  return "";
	}
      else if (GET_CODE (operands[1]) == CONST_DOUBLE)
	{
	  fake_operands[0] = gen_lowpart (SImode, operands[0]);
	  fake_operands[1] = GEN_INT (CONST_DOUBLE_LOW (operands[1]));
	  fake_operands[2] = gen_highpart (SImode, operands[0]);
	  fake_operands[3] = GEN_INT (CONST_DOUBLE_HIGH (operands[1]));
	  output_asm_insn (output_move (fake_operands, SImode), fake_operands);
	  output_asm_insn (output_move (fake_operands+2, SImode),
			   fake_operands+2);
	  return "";
	}
      else if (GET_CODE (operands[1]) == CONST ||
	       GET_CODE (operands[1]) == SYMBOL_REF ||
	       GET_CODE (operands[1]) == LABEL_REF)
	{
	  fake_operands[0] = gen_lowpart (SImode, operands[0]);
	  fake_operands[1] = operands[1];
	  fake_operands[2] = gen_highpart (SImode, operands[0]);
	  fake_operands[3] = const0_rtx;

	  output_asm_insn ("mov %1,%0\n\tmov %3,%2", fake_operands);
	  return "";
	}
      else
	abort ();
    }
  else if (GET_CODE (operands[0]) == MEM)
    {
      if (GET_CODE (operands[1]) == REG)
	{
	  output_store (operands, m);
	  return "";
	}
      else if (GET_CODE (operands[1]) == MEM)
	{
	  int delay;

	  fake_operands[0] = gen_lowpart (SImode, operands[0]);
	  fake_operands[1] = gen_rtx (REG, SImode, 15);
	  fake_operands[2] = gen_lowpart (SImode, operands[1]);

	  fake_operands[3] = gen_highpart (SImode, operands[0]);
	  fake_operands[4] = fake_operands[1];
	  fake_operands[5] = gen_highpart (SImode, operands[1]);

	  if (TARGET_old_value_usable_in_ld_delay)
	    {
	      output_load (fake_operands+1, SImode);
	      output_load (fake_operands+4, SImode);
	      for (delay = lanai_ld_delay-1; delay > 0; delay--)
		output_asm_insn ("nop", fake_operands);
	      output_store (fake_operands+0, SImode);
	      output_store (fake_operands+3, SImode);
	    }
	  else
	    {
	      /* Note that this can produce incorrect results if the
		 source and destination partially overlap.  However
		 that means that one of the pointers must be
		 misaligned, which is a programming error, so we don't
		 worry about it. */

	      output_load (fake_operands+1, SImode);
	      for (delay = lanai_ld_delay; delay > 0; delay--)
		output_asm_insn ("nop", fake_operands);
	      output_store (fake_operands+0, SImode);
	      output_load (fake_operands+4, SImode);
	      for (delay = lanai_ld_delay; delay > 0; delay--)
		output_asm_insn ("nop", fake_operands);
	      output_store (fake_operands+3, SImode);
	    }
	  return "";
	}
      else
	abort ();
    }
  else
    abort ();
}

/* Nonzero if X contains any volatile memory or global register
   references UNSPEC_VOLATILE operations or volatile ASM_OPERANDS
   expressions.  */

static int volatile_memory_refs_p (rtx x);

static
int
volatile_memory_refs_p (x)
     rtx x;
{
  register RTX_CODE code;

  code = GET_CODE (x);
  switch (code)
    {
    case LABEL_REF:
    case SYMBOL_REF:
    case CONST_INT:
    case CONST:
    case CONST_DOUBLE:
    case CC0:
    case PC:
    case SCRATCH:
    case CLOBBER:
    case ASM_INPUT:
    case ADDR_VEC:
    case ADDR_DIFF_VEC:
    case REG:
      return 0;

      /* case CALL: */
    case UNSPEC_VOLATILE:
      /* case TRAP_IF: This isn't clear yet.  */
      return 1;

    case MEM:
    case ASM_OPERANDS:
      return MEM_VOLATILE_P (x);

    default:
      ;
    }

  /* Recursively scan the operands of this expression.  */

  {
    register const char *fmt = GET_RTX_FORMAT (code);
    register int i;

    for (i = GET_RTX_LENGTH (code) - 1; i >= 0; i--)
      {
	if (fmt[i] == 'e')
	  {
	    if (volatile_memory_refs_p (XEXP (x, i)))
	      return 1;
	  }
	if (fmt[i] == 'E')
	  {
	    register int j;
	    for (j = 0; j < XVECLEN (x, i); j++)
	      if (volatile_memory_refs_p (XVECEXP (x, i, j)))
		return 1;
	  }
      }
  }
  return 0;
}

rtx lanai_insn;
rtx lanai_proxy_dest;
int lanai_proxy_delay_slots;

void
emit_nops_if_needed (dest, dslots_number_nops)
     rtx dest;
     int dslots_number_nops;
{
  rtx next;

  if (!TARGET_explicit_ld_stalls_required)
    return;

  if (!lanai_insn)
    abort ();
  if (GET_CODE (dest) != REG)
    abort ();

  for (next = next_insn (lanai_insn);
       next && (dslots_number_nops > 0 || lanai_proxy_delay_slots > 0);
       next = next_insn (next))
    {
      int len;

      len = get_attr_length (next);
      if (!len)
	continue;

      /* output nops to prevent ASMs in delay slots */

      if (get_attr_asm (next))
	{
	  while (dslots_number_nops > 0 || lanai_proxy_delay_slots > 0)
	    {
	      output_asm_insn ("nop", 0);
	      --dslots_number_nops;
	      --lanai_proxy_delay_slots;
	    }
	}

      /* output any other required NOPs */

      while ((dslots_number_nops > 0
	      && reg_referenced_p (dest, PATTERN (next)))
	     || (lanai_proxy_delay_slots > 0
		 && reg_referenced_p (lanai_proxy_dest, PATTERN (next))))
	{
	  output_asm_insn ("nop", 0);
	  --dslots_number_nops;
	  --lanai_proxy_delay_slots;
	}

      /* compute the delay slot requirements after NEXT is emitted */

      dslots_number_nops -= len;
      lanai_proxy_delay_slots -= len;
      if (lanai_proxy_delay_slots < 0)
	{
	  lanai_proxy_delay_slots = 0;
	  lanai_proxy_dest = 0;
	}

      /* Emit remaining NOPs by proxy, if necessary and possible. */

      if (dslots_number_nops > 0
	  && lanai_proxy_dest == 0
	  && get_attr_type (next) == TYPE_LOAD
	  && get_attr_length (next) == 1 && next == next_insn (lanai_insn))
	{
	  lanai_proxy_dest = dest;
	  lanai_proxy_delay_slots = dslots_number_nops;
	  break;
	}
    }
  lanai_insn = 0;
  return;
}

/* This is used by the define_expands for the bCC instructions
   to specify where to find the condition code to determine which
   sort of branch to perform. */
rtx gen_compare_reg (code, x, y)
     enum rtx_code code;
     rtx x, y;
{
  enum machine_mode mode;
  rtx cc_reg;

  (void) code;

  /* catch DI compare case, where the compare was already emitted. */

  if (!x)
    return gen_rtx (REG, CCmode, 3);

  mode = SELECT_CC_MODE (code, x, y);
  cc_reg = gen_rtx (REG, mode, 3);
  emit_insn (gen_rtx (SET, VOIDmode, cc_reg, gen_rtx (COMPARE, mode, x, y)));
  return cc_reg;
}

/* Return 1 if this is a comparison operator.  This allows the use of
   MATCH_OPERATOR to recognize all the branch insns.  */
int
noov_compare_op (op, mode)
     register rtx op;
     enum machine_mode mode;
{
  enum rtx_code code = GET_CODE (op);

  (void) mode;

  if (GET_RTX_CLASS (code) != '<')
    return 0;

  if (GET_MODE (XEXP (op, 0)) == CC_NOOVmode)
    /* These are the only branches which work with CC_NOOVmode.  */
    return (code == EQ || code == NE || code == GE || code == LT);

  if (GET_MODE (XEXP (op, 0)) == CC_BITTESTmode)
    /* These are the only branches which work with CC_BITTESTmode.  */
    return (code == EQ || code == NE);

  return 1;
}

#if 0
/* Return nonzero if OP is an operator of mode MODE which can set
   the condition codes explicitly.  We do not include PLUS and MINUS
   because these require CC_NOOVmode, which we handle explicitly.  */

static int cc_arithop (rtx op, enum machine_mode mode);

static
int
cc_arithop (op, mode)
     rtx op;
     enum machine_mode mode;
{
  (void) mode;

  if (GET_CODE (op) == AND || GET_CODE (op) == IOR || GET_CODE (op) == XOR)
    return 1;

  return 0;
}
#endif


/* Return the string to output a conditional branch to LABEL, which is
   the operand number of the label.  OP is the conditional expression.  The
   mode of register 0 says what kind of comparison we made.

   REVERSED is non-zero if we should reverse the sense of the comparison.

   MODE is one of {CCmode, CC_NATIVEmode, CC_NOOVmode}.

   It is CCmode if a logical or shift instruction like
     xor.f reg,reg,%r0
   was used to perform
     (set (reg:CC 3) (compare:CC (xor (reg) (reg)) (const_int 0)))
   In which case the "C" bit is not set correctly, but alternative branches
   can be used instead.

   It is CC_NOOVmode if an arithmetic instruction like
     sub.f reg,reg,%r0
   was used to perform
     (set (reg:CC_NOOV 3) (compare:CC_NOOV (xor (reg) (reg)) (const_int 0)))
   In which case the "V" bit is not set correctly, but alternative branches
   can be used instead.

   It is CC_NATIVEmode if a subtract instruction like
     sub.f reg,reg,%r0
   was used to perform
     (set (reg:CC_NOOV 3) (compare:CC_NOOV (reg) (reg)))
   in which case all flags are set correctly, and the normal branches can
   be used.

   Also note: If CC_NOOV is used, the overflow bit should be
   assumed to be 0.
*/

const char *
output_cbranch (op, label, reversed, shadow_filled)
     rtx op;
     int label;
     int reversed;
     int shadow_filled;
{
  static char string[40];
  enum rtx_code code = GET_CODE (op);
  enum machine_mode mode = GET_MODE (XEXP (op, 0));
  static char lanai3_labelno[] = " %lX";
  char *labelno;

  string[0] = '\0';

  if (reversed)
    code = reverse_condition (code), reversed = 0;

  /* Start by writing the branch condition.  */
  switch (code)
    {
    case NE:
      if (mode == CC_BITTESTmode)
	strcpy (string, "bmi");
      else
	strcpy (string, "bne");
      break;

    case EQ:
      if (mode == CC_BITTESTmode)
	strcpy (string, "bpl");
      else
	strcpy (string, "beq");
      break;

      /* CC_BITTEST mode is never used with the following operators */
    case GE:
      if (mode == CC_NOOVmode)
	strcpy (string, "bpl");
      else
	strcpy (string, "bge");
      break;

    case GT:
      strcpy (string, "bgt");
      break;

    case LE:
      strcpy (string, "ble");
      break;

    case LT:
      if (mode == CC_NOOVmode)
	strcpy (string, "bmi");
      else
	strcpy (string, "blt");
      break;

      /* CC_NOOVmode is never used with the following operators */
    case GEU:
      strcpy (string, "buge");
      break;

    case GTU:
      strcpy (string, "bugt");
      break;

    case LEU:
      strcpy (string, "bule");
      break;

    case LTU:
      strcpy (string, "bult");
      break;

    default:
      ;
    }

  labelno = lanai3_labelno;
  /* Set the char indicating the number of the operand containing the
     label_ref.  */
  labelno[3] = label + '0';
  strcat (string, labelno);
  if (!shadow_filled)
    strcat (string, "%#");

  if (mode == CC_NOOVmode)
    switch (code)
      {
      case GE:
	strcat (string, " ! NOOV bge");
	break;
      case LT:
	strcat (string, " ! NOOV blt");
	break;
      default:
	break;
      }
  return string;
}

/* Output reasonable peephole for set-on-condition-code insns.
   Note that these insns assume a particular way of defining
   labels.  Therefore, *both* lanai3.h and this function must
   be changed if a new syntax is needed.    */

const char *
output_scc_insn (operands, insn)
     rtx operands[];
     rtx insn;
{
  static char string[100];
  rtx label = 0, next = insn;
  int need_label = 0;

  /* Use the scc family of instructions, if they are available. */

  if (TARGET_has_scc)
    {
      enum machine_mode mode = GET_MODE (XEXP (operands[1], 0));

      switch (GET_CODE (operands[1]))
	{
	case NE:
	  return (mode == CC_BITTESTmode) ? "smi %0" : "sne %0";

	case EQ:
	  return (mode == CC_BITTESTmode) ? "spl %0" : "seq %0";

	  /* CC_BITTEST mode is never used with the following operators */
	case GE:
	  return (mode == CC_NOOVmode) ? "spl %0" : "sge %0";

	case GT:
	  return "sgt %0";

	case LE:
	  return "sle %0";

	case LT:
	  return (mode == CC_NOOVmode) ? "smi %0" : "slt %0";

	  /* CC_NOOVmode is never used with the following operators */
	case GEU:
	  return "suge %0";

	case GTU:
	  return "sugt %0";

	case LEU:
	  return "sule %0";

	case LTU:
	  return "sult %0";

	default:
	  break;
	}
    }

  /* Try doing a jump optimization which jump.c can't do for us
     because we did not expose that setcc works by using branches.

     If this scc insn is followed by an unconditional branch, then have
     the jump insn emitted here jump to that location, instead of to
     the end of the scc sequence as usual.  */

  do
    {
      if (GET_CODE (next) == CODE_LABEL)
	label = next;
      next = NEXT_INSN (next);
      if (next == 0)
	break;
    }
  while (GET_CODE (next) == NOTE || GET_CODE (next) == CODE_LABEL);

  /* If we are in a sequence, and the following insn is a sequence also,
     then just following the current insn's next field will take us to the
     first insn of the next sequence, which is the wrong place.  We don't
     want to optimize with a branch that has had its delay slot filled.
     Avoid this by verifying that NEXT_INSN (PREV_INSN (next)) == next
     which fails only if NEXT is such a branch.  */

  if (next && GET_CODE (next) == JUMP_INSN && simplejump_p (next)
      && (!final_sequence || NEXT_INSN (PREV_INSN (next)) == next))
    label = JUMP_LABEL (next);
  /* If not optimizing, jump label fields are not set.  To be safe, always
     check here to whether label is still zero.  */
  if (label == 0)
    {
      label = gen_label_rtx ();
      need_label = 1;
    }

  LABEL_NUSES (label) += 1;

  operands[2] = label;

  /* If we are in a delay slot, assume it is the delay slot of an fpcc
     insn since our type isn't allowed anywhere else.  */

  /* ??? Fpcc instructions no longer have delay slots, so this code is
     probably obsolete.  */

  /* The fastest way to emit code for this is an annulled branch followed
     by two move insns.  This will take two cycles if the branch is taken,
     and three cycles if the branch is not taken.

     However, if we are in the delay slot of another branch, this won't work,
     because we can't put a branch in the delay slot of another branch.
     The above sequence would effectively take 3 or 4 cycles respectively
     since a no op would have be inserted between the two branches.
     In this case, we want to emit a move, annulled branch, and then the
     second move.  This sequence always takes 3 cycles, and hence is faster
     when we are in a branch delay slot.  */

  string[0] = 0;
  if (final_sequence)
    {
      strcat (string, "nop");
    }
  strcat (string, output_cbranch (operands[1], 2, 0, 1));
  if (TARGET_max_branch_delay == 1)
    {
      strcat (string, "\n\tmov 1,%0\n\tmov 0,%0");
    }
  else if (TARGET_max_branch_delay == 2)
    {
      strcat (string, "\n\tmov 1,%0\n\tnop\n\tmov 0,%0");
    }
  else
    abort ();

  if (need_label)
    strcat (string, "\n%l2:");

  return string;
}

/* Return a free register number or -1. */

int
free_insn_output_reg (last)
     int *last;
{
  int i;

  for (i = (last ? *last + 1 : 0); i < FIRST_PSEUDO_REGISTER; i++)
    {
      if (!regs_ever_live[i] && !global_regs[i] && !fixed_regs[i])
	return i;
    }
  return -1;
}

rtx lanai_compare_op0, lanai_compare_op1;
int partword_load_first_time = 1;
