;;- Machine description for Lanai7 chip for GNU C compiler -*-lisp-*-
;;/*************************************************************************
;; *									   *
;; * lanai7.md: Lanai7 machine description file for gcc			   *
;; *									   *
;; * Copyright 1994,95,96,97,98,99,2000 by Myricom, Inc.		   *
;; * All rights reserved.						   *
;; *									   *
;; * This program is free software; you can redistribute it and/or modify  *
;; * it under the terms of version 2 of the GNU General Public License	   *
;; * as published by the Free Software Foundation.  Myricom requests that  *
;; * all modifications of this software be returned to Myricom, Inc. for   *
;; * redistribution.  The name of Myricom, Inc. may not be used to endorse *
;; * or promote products derived from this software without specific prior *
;; * written permission.						   *
;; *									   *
;; * Myricom, Inc. makes no representations about the suitability of this  *
;; * software for any purpose.						   *
;; *									   *
;; * THIS FILE IS PROVIDED "AS-IS" WITHOUT WARRANTY OF ANY KIND, WHETHER   *
;; * EXPRESSED OR IMPLIED, INCLUDING THE WARRANTY OF MERCHANTABILITY OR	   *
;; * FITNESS FOR A PARTICULAR PURPOSE.	MYRICOM, INC. SHALL HAVE NO	   *
;; * LIABILITY WITH RESPECT TO THE INFRINGEMENT OF COPYRIGHTS, TRADE	   *
;; * SECRETS OR ANY PATENTS BY THIS FILE OR ANY PART THEREOF.		   *
;; *									   *
;; * In no event will Myricom, Inc. be liable for any lost revenue	   *
;; * or profits or other special, indirect and consequential damages, even *
;; * if Myricom has been advised of the possibility of such damages.	   *
;; *									   *
;; * Other copyrights might apply to parts of this software and are so	   *
;; * noted when applicable.						   *
;; *									   *
;; * Myricom, Inc.		      Email: info@myri.com		   *
;; * 325 N. Santa Anita Ave.	      World Wide Web: http://www.myri.com/ *
;; * Arcadia, CA 91024							   *
;; *************************************************************************/
;; /* file first release 5/95 */

;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.

;; *********************
;; ** Attribute types **
;; *********************

;; Define the different types of instructions that we need to know about
;; for delay slot scheduling

;;;;;;;;;;;;;;;;
;; Type definitions:
;;   load: Cannot go in branch delay slot
;;   branch: Has 2 delay slots and cannot got in branch delay slot
;;   common: Can go in branch delay slot if length is 1.  (Catch-All type)
;;;;;;;;;;;;;;;;

(define_attr "type" "common,load,branch" (const_string "common"))

(define_attr "nops_after_branches" "no,yes"
  (const (if_then_else (symbol_ref "TARGET_nops_after_branches")
		       (const_string "yes")
		       (const_string "no"))))

(define_attr "min_branch_delay" "one,two"
  (const (if_then_else (eq (symbol_ref "TARGET_min_branch_delay")
			   (const_int 1))
		       (const_string "one")
		       (const_string "two"))))

;; Length (in # of insns).
(define_attr "length" "" (const_int 1))

(define_attr "asm" "" (const_int 0))

(define_asm_attributes
  [(set_attr "length" "1")
  (set_attr "asm" "1")])

;; Attributes for instruction and branch scheduling

;; Describe the branch delays

(define_delay (eq_attr "type" "branch")
  [(and (and (eq_attr "type" "common")
	     (eq_attr "length" "1"))
	(eq_attr "nops_after_branches" "no"))
  (nil)
  (nil)
  (and (and (and (eq_attr "type" "common")
		 (eq_attr "length" "1"))
	    (eq_attr "nops_after_branches" "no"))
       (eq_attr "min_branch_delay" "two"))
  (nil)
  (nil)])

;; *******************************
;; ** Load Shadow Specification **
;; *******************************

;; Specify that loads return values are not available for next insn.
;; (define_function_unit "memory" 1 0 (eq_attr "type" "load") 2 0)
;;			 ^^^^^^^^ ^ ^ ^^^^^^^^^^^^^^^^^^^^^^^ ^ ^
;;			    |	  | |		 |	      | +-issue delay
;;			    |	  | |		 |	      +---ready delay
;;			    |	  | |		 +-attr test. (when applic.)
;;			    |	  | +- 0=>pipelined
;;			    |	  +- multiplicity: There is one memory
;;			    +-- name

;; Define a pipelined memory with LANAI_LD_DELAY delay slots

(define_function_unit	 "memory" 1 0 (eq_attr "type" "load") 3 0)

;; **********************************
;; **********************************
;; **********************************
;; **********************************
;; **** Instruction Declarations ****
;; **********************************
;; **********************************
;; **********************************
;; **********************************

;; Compare instructions.
;; This controls RTL generation and register allocation.

;; We generate RTL for comparisons and branches by having the cmpxx
;; patterns store away the operands.  Then, the scc and bcc patterns
;; emit RTL for both the compare and the branch.
;;
;; We do this because we want to generate different code for an sne and
;; seq insn.  In those cases, if the second operand of the compare is not
;; const0_rtx, we want to compute the xor of the two operands and test
;; it against zero.
;;
;; We start with the DEFINE_EXPANDs, then the DEFINE_INSNs to match
;; the patterns.  Finally, we have the DEFINE_SPLITs for some of the scc
;; insns that actually require more than one machine instruction.

;; Put cmpsi first among compare insns so it matches two CONST_INT operands.

(define_expand "cmpsi"
  [(set (reg:CC 3)
	(compare:CC (match_operand:SI 0 "register_operand" "" )
		    (match_operand:SI 1 "logic_operand" "" )))]
  ""
  "{
    lanai_compare_op0 = operands[0];
    lanai_compare_op1 = operands[1];
    DONE;
  }")

;; Next come the scc insns.  For seq, sne, sgeu, and sltu, we can do this
;; without jumps using the addc/sub.f instructions.  For seq/sne on v9 we use
;; the same code as v8 (the addc/sub.f method has more applications).  The
;; exception to this is "reg != 0" which can be done in one instruction on v9
;; (so we do it).  For the rest, on v9 we use conditional moves; on v8, we do
;; branches.

;; Seq_special[_xxx] and sne_special[_xxx] clobber the CC reg, because they
;; generate addcc/sub.f instructions.

;; ??? v9: Operand 0 needs a mode, so SImode was chosen.
;; However, the code handles both SImode and DImode.
(define_expand "seq"
  [(set (match_operand:SI 0 "register_operand" "")
	(eq:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  operands[1] = gen_compare_reg (EQ, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sne"
  [(set (match_operand:SI 0 "register_operand" "")
	(ne:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  operands[1] = gen_compare_reg (NE, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sgt"
  [(set (match_operand:SI 0 "register_operand" "")
	(gt:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  rtx tem;

  /* We can do lt easily, so if both operands are registers, swap them and
     do a LT.  */
  if (lanai_compare_op0
      && (GET_CODE (lanai_compare_op0) == REG
	  || GET_CODE (lanai_compare_op0) == SUBREG)
      && lanai_compare_op1
      && (GET_CODE (lanai_compare_op1) == REG
	  || GET_CODE (lanai_compare_op1) == SUBREG))
    {
      tem = lanai_compare_op0;
      lanai_compare_op0 = lanai_compare_op1;
      lanai_compare_op1 = tem;
      emit_insn (gen_slt (operands[0]));
      DONE;
    }
  operands[1] = gen_compare_reg (GT, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "slt"
  [(set (match_operand:SI 0 "register_operand" "")
	(lt:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  operands[1] = gen_compare_reg (LT, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sge"
  [(set (match_operand:SI 0 "register_operand" "")
	(ge:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  operands[1] = gen_compare_reg (GE, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sle"
  [(set (match_operand:SI 0 "register_operand" "")
	(le:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
      rtx tem;

      /* We can do ge easily, so if both operands are registers, swap them and
	 do a GE.  */
      if (lanai_compare_op0
	  && (GET_CODE (lanai_compare_op0) == REG
	      || GET_CODE (lanai_compare_op0) == SUBREG)
	  && lanai_compare_op1
	  && (GET_CODE (lanai_compare_op1) == REG
	      || GET_CODE (lanai_compare_op1) == SUBREG))
	{
	  tem = lanai_compare_op0;
	  lanai_compare_op0 = lanai_compare_op1;
	  lanai_compare_op1 = tem;
	  emit_insn (gen_sge (operands[0]));
	  DONE;
	}
  operands[1] = gen_compare_reg (LE, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sgtu"
  [(set (match_operand:SI 0 "register_operand" "")
	(gtu:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  rtx tem;

  /* We can do ltu easily, so if both operands are registers, swap them and
     do a LTU.	*/
  if (lanai_compare_op0
      && (GET_CODE (lanai_compare_op0) == REG
	  || GET_CODE (lanai_compare_op0) == SUBREG)
      && lanai_compare_op1
      && (GET_CODE (lanai_compare_op1) == REG
	  || GET_CODE (lanai_compare_op1) == SUBREG))
    {
      tem = lanai_compare_op0;
      lanai_compare_op0 = lanai_compare_op1;
      lanai_compare_op1 = tem;
      emit_insn (gen_sltu (operands[0]));
      DONE;
    }
  operands[1] = gen_compare_reg (GTU, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sltu"
  [(set (match_operand:SI 0 "register_operand" "")
	(ltu:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  operands[1] = gen_compare_reg (LTU, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sgeu"
  [(set (match_operand:SI 0 "register_operand" "")
	(geu:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
  operands[1] = gen_compare_reg (GEU, lanai_compare_op0, lanai_compare_op1);
}")

(define_expand "sleu"
  [(set (match_operand:SI 0 "register_operand" "")
	(leu:SI (match_dup 1) (const_int 0)))]
  ""
  "
{
      rtx tem;

      /* We can do geu easily, so if both operands are registers, swap them and
	 do a GEU.  */
      if (lanai_compare_op0
	  && (GET_CODE (lanai_compare_op0) == REG
	      || GET_CODE (lanai_compare_op0) == SUBREG)
	  && lanai_compare_op1
	  && (GET_CODE (lanai_compare_op1) == REG
	      || GET_CODE (lanai_compare_op1) == SUBREG))
	{
	  tem = lanai_compare_op0;
	  lanai_compare_op0 = lanai_compare_op1;
	  lanai_compare_op1 = tem;
	  emit_insn (gen_sgeu (operands[0]));
	  DONE;
	}
  operands[1] = gen_compare_reg (LEU, lanai_compare_op0, lanai_compare_op1);
}")

;; Now the DEFINE_INSNs for the compare and scc cases.	First the compare.
;; Note that we cannot use add.f for negative constants because it would
;; create the wrong C flag value.
(define_insn ""
  [ (set (reg:CC 3)
	 (compare:CC (match_operand:SI 0 "register_operand" "r" )
		     (match_operand:SI 1 "logic_operand" "rJ" )))]
  ""
  "sub.f %0,%1,%?r0")

;; The SEQ and SNE patterns are special because they can be done
;; without any branching and do not involve a COMPARE.

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ne:SI (match_operand:SI 1 "register_operand" "r")
	       (const_int 0)))
   (clobber (reg:CC 3))]
  "!TARGET_one_alu"
  "sub.f %?r0,%1,%?r0\;put %?r0 sub (%?r1 addc %?r0),%0"
  [(set_attr "length" "2")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(neg:SI (ne:SI (match_operand:SI 1 "register_operand" "r")
		       (const_int 0))))
   (clobber (reg:CC 3))]
  ""
  "sub.f %?r0,%1,%?r0\;addc %?r1,0,%0"
  [(set_attr "length" "2")])

;; We can also do (x + (i == 0)) and related, so put them in.

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (ne:SI (match_operand:SI 1 "register_operand" "r")
			(const_int 0))
		 (match_operand:SI 2 "register_operand" "r")))
   (clobber (reg:CC 3))]
  "!TARGET_one_alu"
  "sub.f %?r0,%1,%?r0\;put %2 sub (%?r1 addc %?r0),%0"
  [(set_attr "length" "2")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(minus:SI (match_operand:SI 2 "register_operand" "r")
		  (ne:SI (match_operand:SI 1 "register_operand" "r")
			 (const_int 0))))
   (clobber (reg:CC 3))]
  ""
  "sub.f %?r0,%1,%?r0\;addc %?r1,%2,%0"
  [(set_attr "length" "2")])

;; We can also do EQ directly, but this operates after a compare.
;; Here we extract the zero bit from PS.  Either CC mode is sufficient
;; since only the Z bit is used.
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(eq:SI (reg:CC 3) (const_int 0)))]
  "TARGET_has_ps"
  "and %?ps,0xffff0001,%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(eq:SI (reg:CC_NOOV 3) (const_int 0)))]
  "TARGET_has_ps"
  "and %?ps,0xffff0001,%0")

;; Similarly for NE
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ne:SI (reg:CC 3) (const_int 0)))]
  "!TARGET_one_alu && TARGET_has_ps"
  "and %?ps,0xffff0001,%0\;put %?r0 sub (%?r1 add %0),%0"
  [(set_attr "length" "2")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ne:SI (reg:CC_NOOV 3) (const_int 0)))]
  "!TARGET_one_alu && TARGET_has_ps"
  "and %?ps,0xffff0001,%0\;put %?r0 sub (%?r1 add %0),%0"
  [(set_attr "length" "2")])

;; We can also do GEU and LTU directly, but these operate after a compare.
;; The condition code mode must be CCmode because CC_NOOVmode does not
;; set C correctly.
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ltu:SI (reg:CC 3) (const_int 0)))]
  "!TARGET_one_alu"
  "put %?r0 sub (%?r1 addc %?r0),%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(neg:SI (ltu:SI (reg:CC 3) (const_int 0))))]
  ""
  "addc %?r1,%?r0,%0")

;; ??? Combine should canonicalize these next two to the same pattern.
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(minus:SI (neg:SI (ltu:SI (reg:CC 3) (const_int 0)))
		  (match_operand:SI 1 "register_operand" "r")))]
  ""
  "subb %?r0,%1,%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(neg:SI (plus:SI (ltu:SI (reg:CC 3) (const_int 0))
			 (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "subb %?r0,%1,%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(geu:SI (reg:CC 3) (const_int 0)))]
  ""
  "addc %?r0,0,%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(neg:SI (geu:SI (reg:CC 3) (const_int 0))))]
  "!TARGET_one_alu"
  "put %?r0 sub (%?r0 addc %?r0),%0")

;; We can also do (x + ((unsigned) i >= 0)) and related, so put them in.
;; ??? The addc/sub.f insns use the 32 bit carry flag so there are no DImode
;; versions for v9.

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (ltu:SI (reg:CC 3) (const_int 0))
		 (match_operand:SI 1 "arith_operand" "rJ")))]
  "!TARGET_one_alu"
  "put %1 sub (%?r1 addc %?r0),%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (ltu:SI (reg:CC 3) (const_int 0))
		 (plus:SI (match_operand:SI 1 "arith_operand" "%r")
			  (match_operand:SI 2 "arith_operand" "rJ"))))]
  "!TARGET_one_alu"
  "put %1 sub (%?r1 subb %2),%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(minus:SI (match_operand:SI 1 "register_operand" "r")
		  (ltu:SI (reg:CC 3) (const_int 0))))]
  ""
  "addc %?r1,%1,%0")

;; ??? Combine should canonicalize these next two to the same pattern.
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(minus:SI (minus:SI (match_operand:SI 1 "register_operand" "r")
			    (match_operand:SI 2 "register_operand" "r"))
		  (ltu:SI (reg:CC 3) (const_int 0))))]
  "!TARGET_one_alu"
  "put %?r1 addc (%1 sub %2),%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(minus:SI (match_operand:SI 1 "register_operand" "r")
		  (plus:SI (ltu:SI (reg:CC 3) (const_int 0))
			   (match_operand:SI 2 "register_operand" "r"))))]
  "!TARGET_one_alu"
  "put %?r1 addc (%1 sub %2),%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (geu:SI (reg:CC 3) (const_int 0))
		 (match_operand:SI 1 "register_operand" "r")))]
  ""
  "addc %?r0,%1,%0")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(minus:SI (match_operand:SI 1 "register_operand" "r")
		  (geu:SI (reg:CC 3) (const_int 0))))]
  "!TARGET_one_alu"
  "put %1 sub (%?r0 addc %?r0),%0")

;; Simple patterns for slt and sge.  These do not require a compare.
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(lt:SI (match_operand:SI 1 "register_operand" "r")
	       (const_int 0)))]
  ""
  "sh %1,-31,%0")

;; ??? Need to add a define_split for this.
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ge:SI (match_operand:SI 1 "register_operand" "r")
	       (const_int 0)))]
  ""
  "sha %1,-31,%0\;add %0,1,%0"
  [(set_attr "length" "2")])

;; Now we have the generic scc insns.
;; We have to exclude the cases above, since we will not want combine to
;; turn something that does not require a jump into something that does.

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(match_operator:SI 1 "noov_compare_op" [(reg 3) (const_int 0)]))]
  ""
  "* return output_scc_insn (operands, insn); "
  [(set_attr "length" "4")])

;; These control RTL generation for conditional jump insns
;; ***********
;; ** bCOND **
;; ***********

(define_expand "beq"
  [(set (pc)
    (if_then_else (eq (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1] =gen_compare_reg (EQ, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "bne"
  [(set (pc)
    (if_then_else (ne (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1] =gen_compare_reg (NE, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "bgt"
  [(set (pc)
    (if_then_else (gt (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1] =gen_compare_reg (GT, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "blt"
  [(set (pc)
    (if_then_else (lt (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1] =gen_compare_reg (LT, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "ble"
  [(set (pc)
    (if_then_else (le (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1] =gen_compare_reg (LE, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "bge"
  [(set (pc)
    (if_then_else (ge (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1] =gen_compare_reg (GE, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "bgtu"
  [(set	 (pc)
    (if_then_else (gtu (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1]=gen_compare_reg (GTU, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "bltu"
  [(set (pc)
    (if_then_else (ltu (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1]=gen_compare_reg (LTU, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "bgeu"
  [(set (pc)
    (if_then_else (geu (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1]=gen_compare_reg (GEU, lanai_compare_op0, lanai_compare_op1);"
)

(define_expand "bleu"
  [(set (pc)
    (if_then_else (leu (match_dup 1) (const_int 0))
	      (label_ref (match_operand 0 "" ""))
	      (pc)))]
  ""
  "operands[1]=gen_compare_reg (LEU, lanai_compare_op0, lanai_compare_op1);"
)

;; Now match both normal and inverted branches.
(define_insn ""
  [(set (pc)
	(if_then_else (match_operator 0 "noov_compare_op"
				      [(reg 3) (const_int 0)])
		      (label_ref (match_operand 1 "" ""))
		      (pc)))]
  ""
  "*
{
  return output_cbranch (operands[0], 1, 0, 0);
}"
  [(set_attr "type" "branch")])

(define_insn ""
  [(set (pc)
	(if_then_else (match_operator 0 "noov_compare_op"
				      [(reg 3) (const_int 0)])
		      (pc)
		      (label_ref (match_operand 1 "" ""))))]
  ""
  "*
{
  return output_cbranch (operands[0], 1, 1, 0);
}"
  [(set_attr "type" "branch")])

;; *********
;; ** RRR **
;; *********

;; Define RRR instructions that don't modify the flags.
;; Note that the -mone_alu compiler option disables this instruction.
(define_insn ""
  [(set
	(match_operand:SI 0 "register_operand" "=r")
	(match_operator:SI 1 "op2_operand"
	    [
		(match_operand:SI 2 "reg_0_m1_operand" "rO")
		(match_operator:SI 3 "op1_operand"
		    [
			(match_operand:SI 4 "reg_0_m1_operand" "rO")
			(match_operand:SI 5 "reg_0_m1_operand" "rO")
		    ]
		)
	    ]
	)
   )
  ]
  "!TARGET_one_alu"
  "put %r2 %o1 (%r4 %o3 %r5),%0"
)

;; ??? BAD: Need to add flag-setting variants.

;; ***************************
;; ** Bit-field extraction  **
;; ***************************

;; Operand 0 is dest, operand 1 is source, operand 2 is field width, operand
;; 3 is field start position

;; sign-extend extraction
(define_expand "extv"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(sign_extract:SI (match_operand:SI 1 "register_operand" "")
			 (match_operand:SI 2 "immediate_operand" "")
			 (match_operand:SI 3 "immediate_operand" "")))]
  ""
  "{
    rtx temp = gen_reg_rtx (SImode);
    int len, pos;
    len = INTVAL (operands[2]);
    pos = INTVAL (operands[3]);
    /* operands[1] = copy_to_mode_reg (SImode, operands[1]); */
    operands[2] = GEN_INT (32-len-pos);
    operands[3] = GEN_INT (32-len);
    emit_insn (gen_ashlsi3 (temp, operands[1], operands[2]));
    emit_insn (gen_ashrsi3 (operands[0], temp, operands[3]));
    DONE;
  }"
)

;; zero-extend extraction
(define_expand "extzv"
  [(set (match_operand:SI 0 "register_operand" "")
	(zero_extract:SI (match_operand:SI 1 "register_operand" "")
			 (match_operand:SI 2 "immediate_operand" "")
			 (match_operand:SI 3 "immediate_operand" "")))]
  ""
  "{
    rtx temp = gen_reg_rtx (SImode);
    int len, pos;
    len = INTVAL (operands[2]);
    pos = INTVAL (operands[3]);
    operands[2] = GEN_INT (32-len-pos);
    operands[3] = GEN_INT (32-len);
    emit_insn (gen_ashlsi3 (temp, operands[1], operands[2]));
    emit_insn (gen_lshrsi3 (operands[0], temp, operands[3]));
    DONE;
  }"
)

;; Special patterns for optimizing bit-field compares.	This is needed
;; because combine uses this as a canonical form.

;; ???: {Can we,would it help to} remove clobber when the reg is not used?

;; If we need a scratch register (Takes two instructions)
(define_insn ""
  [(set (reg:CC 3)
	(compare:CC
	 (zero_extract:SI (match_operand:SI 0 "register_operand" "r")
			  (match_operand:SI 1 "immediate_operand" "n")
			  (match_operand:SI 2 "immediate_operand" "n"))
	 (const_int 0)))
  ;;  (clobber (match_operand:SI 3 "register_operand" "=r"))
  ]

  ""
  "*
{
  int len = INTVAL (operands[1]);
  int pos = INTVAL (operands[2]);

  /* ;; Can we do this with a single shift? */
  if (pos+len == 32)
    {
      operands[2] = GEN_INT (-pos);
      return \"sh.f %0,%2,%?r0\";
    }

  /* ;; Can we do this with a single and? */
  if ( pos == 0 && len >= 16 )
    {
      int mask;
      mask = ((1 << len) - 1) << pos;
      operands[1] = GEN_INT (mask);
      return \"and.f %0,%1,%?r0\";
    }

  /* ;; Do the extraction with two shifts */
  operands[2] = GEN_INT (32-len-pos);
  output_asm_insn (\"sh %0,%2,%?r15\", operands);
  operands[0] = GEN_INT (len-32);
  return \"sh.f %?r15,%0,%?r0\";
}"
  ;; BAD: should have different attributes for 1 insn case.  Currently,
  ;; we are conservative.
  [(set_attr "length" "2")]
)

;; Special pattern for testing a single bit
(define_insn ""
  [(set (reg:CC_BITTEST 3)
	(compare:CC_BITTEST
	 (zero_extract:SI (match_operand:SI 0 "register_operand" "r")
			  (match_operand:SI 1 "immediate_operand" "n")
			  (match_operand:SI 2 "immediate_operand" "n"))
	 (const_int 0)))
  ;;  (clobber (match_operand:SI 3 "register_operand" "=r"))
  ]

  "operands[1] == const1_rtx"
  "*
{
  int pos = INTVAL (operands[2]);
  operands[2] = GEN_INT (31-pos);
  return \"sh.f %0,%2,%?r0\";
}"
  ;; BAD: should have different attributes for 1 insn case.  Currently,
  ;; we are conservative.
  [(set_attr "length" "2")]
)

;; Define how to split a zero extract for pipeline filling.
(define_split
  [(set (reg:CC 3)
	(compare:CC
	 (zero_extract:SI (match_operand:SI 0 "register_operand" "")
			  (match_operand:SI 1 "immediate_operand" "")
			  (match_operand:SI 2 "immediate_operand" ""))
	 (const_int 0)))
   (clobber (match_operand:SI 3 "register_operand" ""))]
   ""
   [(set (match_dup 3) (ashift:SI (match_dup 0) (match_dup 1)))
   (parallel [(set (reg:CC 3)
		   (compare:CC (ashiftrt:SI (match_dup 3) (match_dup 2))
			       (const_int 0)))
	     (set (reg:SI 0)
		  (ashiftrt:SI (match_dup 3) (match_dup 2)))])]
   "
{
  int len, pos;
  len = INTVAL (operands[1]);
  pos = INTVAL (operands[2]);

  /* ; Override default definition if 1 instruction can be used. */
  if (pos == 0 && len >= 16)
    {
      rtx mask;
      mask = GEN_INT ( (1<<len) - 1 );
      /* ; This massive insn is merely \"and.f %0,mask,%?r0\" */
      emit_insn
	(gen_rtx_PARALLEL
	 (VOIDmode,
	  gen_rtvec (2,
		     gen_rtx_SET (VOIDmode,
				  gen_rtx_REG (CCmode, 3),
				  gen_rtx_COMPARE (CCmode,
						   gen_rtx_AND (SImode,
								operands[0],
								mask),
						   const0_rtx)),
		     gen_rtx_SET (VOIDmode,
				  gen_rtx_REG (SImode, 0),
				  gen_rtx_AND (SImode, operands[0], mask)))));
      DONE;
    }

  /* ; Must use two instructions, but can use the template with
     ; modified operands */
  operands[1] = GEN_INT (32-len-pos); /* Amt to shift left */
  operands[2] = GEN_INT (len-32); /* Amt to shift right */
}")

;; ********************
;; ********************
;; ** zero extension **
;; ********************
;; ********************

;; Below, the named instructions (used to expand the names) accept
;; only operands in registers, and we rely on combine to optimize
;; memory->reg extensions.  We did this because the sparc.md file
;; suggests it and it does seem to eliminate almost all
;; sign extensions in registers.

;; **************
;; ** HI -> SI **
;; **************
;; Extend halfword to fullword
(define_expand "zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(zero_extend:SI (match_operand:HI 1 "register_operand" "")))]
  ""
  "
{
  rtx constant = gen_rtx_CONST_INT (VOIDmode, 0xffff);
  int op1_subreg_byte = 0;

  if (GET_CODE (operand1) == SUBREG)
    {
      op1_subreg_byte = SUBREG_BYTE (operand1);
      op1_subreg_byte -= op1_subreg_byte % GET_MODE_SIZE (SImode);
      operand1 = XEXP (operand1, 0);
    }

  emit_insn (gen_andsi3 (operand0, gen_rtx_SUBREG (SImode, operand1,
					    op1_subreg_byte), constant));

  DONE;
}")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(zero_extend:SI (match_operand:HI 1 "memory_operand" "m")))]
  "!TARGET_no_partword_load"
  "*{
	output_asm_insn(\"uld.h %1,%0 !zero_extendhisi2\",operands);
	lanai_insn=insn;
	emit_nops_if_needed(operands[0],lanai_ld_delay);
	return \"\";
  }"
  [(set_attr "type" "load")]
)

;; **************
;; ** QI -> HI **
;; **************

;; Extend byte to halfword
;; Convert zero extensions to rtl to be handled below
(define_expand "zero_extendqihi2"
  [(set (match_operand:HI 0 "register_operand" "")
	(zero_extend:HI (match_operand:QI 1 "register_operand" "")))]
  ""
  "
{
  rtx temp = gen_reg_rtx (SImode);
  rtx shift_24 = gen_rtx_CONST_INT (VOIDmode, 24);
  int op0_subreg_byte = 0;

  if (GET_CODE (operand0) == SUBREG)
    {
      op0_subreg_byte = SUBREG_BYTE (operand0);
      op0_subreg_byte -= op0_subreg_byte % GET_MODE_SIZE (SImode);
      operand0 = XEXP (operand0, 0);
    }
  if (GET_CODE (operand1) == SUBREG)
    {
      operand1 = XEXP (operand1, 0);
    }

  emit_insn (gen_ashlsi3 (temp,
			  gen_lowpart (SImode, force_reg (QImode, operand1)),
			  shift_24));
  if (GET_MODE (operand0) != SImode)
    operand0 = gen_rtx_SUBREG (SImode, operand0, op0_subreg_byte);
  emit_insn (gen_lshrsi3 (operand0, temp, shift_24));
  DONE;
}")

;; If the operand to be zero-extended is a byte in memory, then perform
;; zero extension using an unsigned load
(define_insn ""
  [ (set (match_operand:HI 0 "register_operand" "=r")
	  (zero_extend:HI (match_operand:QI 1 "memory_operand" "m"))
    )
  ]
  "!TARGET_no_partword_load"
  "* {
	output_asm_insn(\"uld.b %1,%0 !zero_extendqihi2\",operands);
	lanai_insn=insn;
	emit_nops_if_needed(operands[0],lanai_ld_delay);
	return \"\";
     }
  "
  [(set_attr "type" "load")]
)

;; **************
;; ** QI -> SI **
;; **************
(define_expand "zero_extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(zero_extend:SI (match_operand:QI 1 "register_operand" "")))]
  ""
  "
{
  rtx temp = gen_reg_rtx (SImode);
  rtx shift_24 = gen_rtx_CONST_INT (VOIDmode, 24);

  if (GET_CODE (operand1) == SUBREG)
    {
      operand1 = XEXP (operand1, 0);
    }

  emit_insn (gen_ashlsi3 (temp,
			  gen_lowpart (SImode, force_reg (QImode, operand1)),
			  shift_24));
  emit_insn (gen_lshrsi3 (operand0, temp, shift_24));
  DONE;
}")

;; If the operand to be zero-extended is a byte in memory, then perform
;; zero extension using an unsigned load
(define_insn ""
  [ (set (match_operand:SI 0 "register_operand" "=r")
	  (zero_extend:SI (match_operand:QI 1 "memory_operand" "m"))
    )
  ]
  "!TARGET_no_partword_load"
  "* {
	output_asm_insn(\"uld.b %1,%0 !zero_extendqihi2\",operands);
	lanai_insn=insn;
	emit_nops_if_needed(operands[0],lanai_ld_delay);
	return \"\";
     }
  "
  [(set_attr "type" "load")]
)

;; ********************
;; ********************
;; ** sign extension **
;; ********************
;; ********************

(define_expand "extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(sign_extend:SI (match_operand:HI 1 "register_operand" "")))]
  ""
  "
{
  rtx temp = gen_reg_rtx (SImode);
  rtx shift_16 = gen_rtx_CONST_INT (VOIDmode, 16);

  if (GET_CODE (operand1) == SUBREG)
    {
      operand1 = XEXP (operand1, 0);
    }

  emit_insn (gen_ashlsi3 (temp,
			  gen_lowpart (SImode, force_reg (HImode, operand1)),
			  shift_16));
  emit_insn (gen_ashrsi3 (operand0, temp, shift_16));
  DONE;
}")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(sign_extend:SI (match_operand:HI 1 "memory_operand" "m")))]
  "!TARGET_no_partword_load"
  "* {
     output_asm_insn(\"ld.h %1,%0 ! extendhisi2\",operands);
     lanai_insn=insn;
     emit_nops_if_needed(operands[0],lanai_ld_delay);
     return \"\";
   }
  "
  [(set_attr "type" "load")])

(define_expand "extendqihi2"
  [(set (match_operand:HI 0 "register_operand" "")
	(sign_extend:HI (match_operand:QI 1 "register_operand" "")))]
  ""
  "
{
  rtx temp = gen_reg_rtx (SImode);
  rtx shift_24 = gen_rtx_CONST_INT (VOIDmode, 24);
  int op0_subreg_byte = 0;

  if (GET_CODE (operand1) == SUBREG)
    {
      operand1 = XEXP (operand1, 0);
    }
  if (GET_CODE (operand0) == SUBREG)
    {
      op0_subreg_byte = SUBREG_BYTE (operand0);
      op0_subreg_byte -= op0_subreg_byte % GET_MODE_SIZE (SImode);
      operand0 = XEXP (operand0, 0);
    }
  emit_insn (gen_ashlsi3 (temp,
			  gen_lowpart (SImode, force_reg (QImode, operand1)),
			  shift_24));
  if (GET_MODE (operand0) != SImode)
    operand0 = gen_rtx_SUBREG (SImode, operand0, op0_subreg_byte);
  emit_insn (gen_ashrsi3 (operand0, temp, shift_24));
  DONE;
}")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(sign_extend:HI (match_operand:QI 1 "memory_operand" "m")))]
  "!TARGET_no_partword_load"
  "*{
     output_asm_insn(\"ld.b %1,%0 ! extendqihi2\",operands);
     lanai_insn=insn;
     emit_nops_if_needed(operands[0],lanai_ld_delay);
     return \"\";
   }
  "
  [(set_attr "type" "load")])

(define_expand "extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(sign_extend:SI (match_operand:QI 1 "register_operand" "")))]
  ""
  "
{
  rtx temp = gen_reg_rtx (SImode);
  rtx shift_24 = gen_rtx_CONST_INT (VOIDmode, 24);

  if (GET_CODE (operand1) == SUBREG)
    {
      operand1 = XEXP (operand1, 0);
    }

  emit_insn (gen_ashlsi3 (temp,
			  gen_lowpart (SImode, force_reg (QImode, operand1)),
			  shift_24));
  emit_insn (gen_ashrsi3 (operand0, temp, shift_24));
  DONE;
}")

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(sign_extend:SI (match_operand:QI 1 "memory_operand" "m")))]
  "!TARGET_no_partword_load"
  "*{
     output_asm_insn(\"ld.b %1,%0 ! extendqisi2\",operands);
     lanai_insn=insn;
     emit_nops_if_needed(operands[0],lanai_ld_delay);
     return \"\";
   }
  "
  [(set_attr "type" "load")])

;;*****************************
;;** arithmetic instructions **
;;*****************************

;;;; Split large constant ADDs in two
;;(define_split
;;  [(set (match_operand:SI 0 "register_operand" "")
;;	  (plus:SI (match_operand:SI 1 "register_operand" "")
;;		  (match_operand:SI 2 "const_int_operand" "")))]
;;  "GET_CODE (operands[2]) == CONST_INT && !ARITH_INT (INTVAL (operands[2]))"
;;  [(set (match_dup 0) (plus:SI (match_dup 1) (match_dup 3)))
;;   (set (match_dup 0) (plus:SI (match_dup 0) (match_dup 4)))]
;;  " {
;;	int low =  (INTVAL (operands[2]) & 0x0000ffff);
;;	int high = (INTVAL (operands[2]) & 0xffff0000);
;;	operands[3] = gen_rtx_CONST_INT (VOIDmode, high);
;;	operands[4] = gen_rtx_CONST_INT (VOIDmode, low);
;;    }
;;  "
;;)

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (match_operand:SI 1 "register_operand" "%r,r")
		 (match_operand:SI 2 "arith_operand" "rJ,L")))]
  ""
  "*
{
    if(which_alternative == 0){
	return \"add %1,%2,%0\";
    }else{
	operands[2] = gen_rtx_CONST_INT (VOIDmode, - INTVAL (operands[2]));
	return \"sub %1,%2,%0\";
    }
} ")

(define_insn ""
  [(set (reg:CC_NOOV 3)
	(compare:CC_NOOV (plus:SI (match_operand:SI 0 "register_operand" "%r,r")
				  (match_operand:SI 1 "arith_operand" "rJ,L"))
			 (const_int 0)))]
  ""
  "*
{
    if(which_alternative == 0){
	return \"add.f %0,%1,%?r0\";
    }else{
	operands[1] = gen_rtx_CONST_INT (VOIDmode, - INTVAL (operands[1]));
	return \"sub.f %0,%1,%?r0\";
    }
} ")

(define_insn ""
  [(set (reg:CC_NOOV 3)
	(compare:CC_NOOV
	 (plus:SI (match_operand:SI 1 "register_operand" "%r,r")
		  (match_operand:SI 2 "arith_operand" "rJ,L"))
	 (const_int 0)))
   (set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (match_dup 1)
		 (match_dup 2)))]
  ""
  "*
{
    if(which_alternative == 0){
	return \"add.f %1,%2,%0\";
    }else{
	operands[2] = gen_rtx_CONST_INT (VOIDmode, - INTVAL (operands[2]));
	return \"sub.f %1,%2,%0\";
    }
} ")

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(minus:SI (match_operand:SI 1 "register_operand" "r,r")
		  (match_operand:SI 2 "arith_operand" "rJ,L")))]
  ""
  "*
{
    if(which_alternative == 0){
	return \"sub %1,%2,%0\";
    }else{
	operands[2] = gen_rtx_CONST_INT (VOIDmode, - INTVAL (operands[2]));
	return \"add %1,%2,%0\";
    }
}")

(define_insn ""
  [(set (reg:CC_NOOV 3)
	(compare:CC_NOOV (minus:SI (match_operand:SI 0 "register_operand" "r,r")
				   (match_operand:SI 1 "arith_operand" "rJ,L"))
			 (const_int 0)))]
  ""
  "*
{
    if(which_alternative == 0){
	return \"sub.f %0,%1,%?r0\";
    }else{
	operands[1] = gen_rtx_CONST_INT (VOIDmode, - INTVAL (operands[1]));
	return \"add.f %0,%1,%?r0\";
    }
}")

(define_insn ""
  [ (set (reg:CC_NOOV 3)
	 (compare:CC_NOOV (minus:SI (match_operand:SI 1 "register_operand" "r,r")
				    (match_operand:SI 2 "arith_operand" "rJ,L"))
			  (const_int 0)))
  (set (match_operand:SI 0 "register_operand" "=r,r")
       (minus:SI (match_dup 1) (match_dup 2)))]
  ""
  "*
{
    if(which_alternative == 0){
	return \"sub.f %1,%2,%0\";
    }else{
	operands[2] = gen_rtx_CONST_INT (VOIDmode, - INTVAL (operands[2]));
	return \"add.f %1,%2,%0\";
    }
}")

;; Note that we cannot use add.f for negative constants because it would
;; create the wrong C flag value.
(define_insn ""
  [ (set (reg:CC 3)
	 (compare:CC (match_operand:SI 1 "register_operand" "r")
		     (match_operand:SI 2 "arith_operand" "rJ")))
  (set (match_operand:SI 0 "register_operand" "=r")
       (minus:SI (match_dup 1)
		 (match_dup 2)))]
  ""
  "sub.f %1,%2,%0")
;; NOTE: need to consider change to a define_expand if there is any
;; optimization that can be performed with an immediate and.  I
;; believe there is.
;;- and instructions (with compliment also)

;; *******************************
;; ** Binary Logic Instructions **
;; *******************************

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_operand:SI 1 "register_operand" "%r")
		(match_operand:SI 2 "and_operand" "rN")))]
  ""
  "*
{
	return \"and %1,%2,%0\";
}")

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
	(and:SI (match_operand:SI 1 "register_operand" "")
		(match_operand:SI 2 "" "")))
   (clobber (match_operand:SI 3 "register_operand" ""))]
  "GET_CODE (operands[2]) == CONST_INT
   && !AND_INT (INTVAL (operands[2]))
   && !IS_ISR (operands[0])"
  [(set (match_dup 3) (and:SI (match_dup 1) (match_dup 2)))
   (set (match_dup 4) (and:SI (match_dup 3) (match_dup 4)))]
  "{
    operands[4] = GEN_INT (0x0000ffff | INTVAL (operands[2]));
    operands[2] = GEN_INT (0xffff0000 | INTVAL (operands[2]));
  }")

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC (and:SI (match_operand:SI 1 "register_operand" "%r")
			    (match_operand:SI 2 "and_operand" "rN"))
		    (const_int 0)))
  (set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_dup 1)
		(match_dup 2)))]
  ""
  "and.f %1,%2,%0")

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC
	 (and:SI (match_operand:SI 0 "register_operand" "%r")
		 (match_operand:SI 1 "and_operand" "rN"))
	 (const_int 0)))]
  ""
  "and.f %0,%1,%?r0"
)

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (match_operand:SI 1 "register_operand" "%r")
		(match_operand:SI 2 "logic_operand" "rJ")))]
  ""
  "or %1,%2,%0")

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
	(ior:SI (match_operand:SI 1 "register_operand" "")
		(match_operand:SI 2 "" "")))
   (clobber (match_operand:SI 3 "register_operand" ""))]
  "GET_CODE (operands[2]) == CONST_INT
   && !LOGIC_INT (INTVAL (operands[2]))
   && !IS_ISR (operands[0])"
  [(set (match_dup 3) (ior:SI (match_dup 1) (match_dup 2)))
   (set (match_dup 4) (ior:SI (match_dup 3) (match_dup 4)))]
  "{
    operands[4] = GEN_INT (0x0000ffff & INTVAL (operands[2]));
    operands[2] = GEN_INT (0xffff0000 & INTVAL (operands[2]));
  }")

(define_insn ""
  [ (set (reg:CC 3)
	 (compare:CC (ior:SI (match_operand:SI 1 "register_operand" "%r")
			     (match_operand:SI 2 "logic_operand" "rJ"))
			  (const_int 0)))
  (set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (match_dup 1)
		(match_dup 2)))]
  ""
  "or.f %1,%2,%0"
)

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC
	 (ior:SI (match_operand:SI 0 "register_operand" "%r")
		 (match_operand:SI 1 "logic_operand" "rN"))
	 (const_int 0)))]
  ""
  "or.f %0,%1,%?r0"
)

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(xor:SI (match_operand:SI 1 "register_operand" "%r")
		(match_operand:SI 2 "logic_operand" "rJ")))]
  ""
  "xor %1,%2,%0")

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
	(xor:SI (match_operand:SI 1 "register_operand" "")
		(match_operand:SI 2 "" "")))
   (clobber (match_operand:SI 3 "register_operand" ""))]
  "GET_CODE (operands[2]) == CONST_INT
   && !LOGIC_INT (INTVAL (operands[2]))
   && !IS_ISR (operands[0])"
  [(set (match_dup 3) (xor:SI (match_dup 1) (match_dup 2)))
   (set (match_dup 4) (xor:SI (match_dup 3) (match_dup 4)))]
  "{
    operands[4] = GEN_INT (0x0000ffff & INTVAL (operands[2]));
    operands[2] = GEN_INT (0xffff0000 & INTVAL (operands[2]));
  }")

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC (xor:SI (match_operand:SI 1 "register_operand" "%r")
			    (match_operand:SI 2 "logic_operand" "rJ"))
		    (const_int 0)))
  (set (match_operand:SI 0 "register_operand" "=r")
       (xor:SI (match_dup 1)
	       (match_dup 2)))]
  ""
  "xor.f %1,%2,%0"
)

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC
	 (xor:SI (match_operand:SI 0 "register_operand" "%r")
		 (match_operand:SI 1 "logic_operand" "rN"))
	 (const_int 0)))]
  ""
  "xor.f %0,%1,%?r0"
)

;; xnor instructions
;; DAB: duplicate
(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(not:SI (xor:SI (match_operand:SI 1 "reg_0_m1_operand" "%rO")
			(match_operand:SI 2 "register_operand" "r"))))]
  "!TARGET_one_alu"
  "put %?r1 xor ( %r0 xor %1 ),%0 ! xnor ")

;; ??? Only has implicit equivalent in sparc.md
(define_insn ""
  [(set (reg:CC 3)
	(compare:CC
	 (not:SI (xor:SI (match_operand:SI 0 "reg_0_m1_operand" "%rO")
			 (match_operand:SI 1 "register_operand" "r")))
	 (const_int 0)))]
  "!TARGET_one_alu"
  "put %?r1 xor.f ( %r0 xor %1 ), %?r0 ! xnor ")

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC
	 (not:SI (xor:SI (match_operand:SI 1 "reg_0_m1_operand" "%rO")
			 (match_operand:SI 2 "register_operand" "r")))
	 (const_int 0)))
   (set (match_operand:SI 0 "register_operand" "=r")
	(not:SI (xor:SI (match_dup 1) (match_dup 2))))]
  "!TARGET_one_alu"
  "put %?r1 xor.f ( %r1 xor %2 ), %0 ! xnor ")

;; ??? Here, sparc.md include some generalized compare versions of the ialu
;; insns.  We include individual patterns for now to more easily specify
;; the acceptable operands.  In the future, however, we could speed up
;; the compiler by combining them all into one define_insn.

;; Compiler should never want to negate constants, so we require
;; input to be in a register.

(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(neg:SI (match_operand:SI 1 "arith_operand" "rJ")))]
  ""
  "sub %?r0,%1,%0")

(define_insn ""
  [(set (reg:CC_NOOV 3)
	(compare:CC_NOOV (neg:SI (match_operand:SI 0 "arith_operand" "rJ"))
			 (const_int 0)))]
  ""
  "sub.f %?r0,%0,%?r0"
)

(define_insn ""
  [(set (reg:CC_NOOV 3)
	(compare:CC_NOOV (neg:SI (match_operand:SI 1 "arith_operand" "rJ"))
			 (const_int 0)))
   (set (match_operand:SI 0 "register_operand" "=r")
	(neg:SI (match_dup 1)))]
  ""
  "sub.f %?r0,%1,%0")

;; not

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(not:SI (match_operand:SI 1 "arith_operand" "rJ")))]
  ""
  "xor %?r1,%1,%0")

(define_insn ""
  [(set (reg:CC 3)

	(compare:CC (not:SI (match_operand:SI 0 "arith_operand" "rJ"))
		    (const_int 0)))]
  ""
  "xor.f %?r1,%0,%?r0")

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC (not:SI (match_operand:SI 1 "arith_operand" "rJ"))
		    (const_int 0)))
   (set (match_operand:SI 0 "register_operand" "=r")
	(not:SI (match_dup 1)))]
  ""
  "xor.f %?r1,%1,%0")

;; ************************
;; ************************
;; ** Shift instructions **
;; ************************
;; ************************

;; ********
;; ** SI **
;; ********

;; We cannot use left shifts for compares because they do not set C
;; correctly based on the comparison of the result with zero.
;; We do the one case we can handle: left shift by one using add.f

;; Arithmetic shift right
(define_expand "ashrsi3"
   [(set (match_operand:SI 0 "register_operand" "=r,r")
	 (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		      (match_operand:SI 2 "shift_operand" "r,i")))]
  ""
  "{
    if (TARGET_one_alu && register_operand (operands[2], SImode))
      {
	rtx temp = gen_reg_rtx (SImode);
	emit_insn (gen_negsi2 (temp, operands[2]));
	emit_insn (gen_ashlsi3 (operands[0], operands[1], temp));
	DONE;
      }
  }")

; Pattern for LANais with 2 ALUs
(define_insn ""
   [(set (match_operand:SI 0 "register_operand" "=r,r")
	 (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		      (match_operand:SI 2 "shift_operand" "r,i")))]
  "!TARGET_one_alu"
   "@
    put %1 sha (%?r0 sub %2), %0
    sha %1,-%2,%0")

; Pattern for LANais with 1 ALU
(define_insn ""
   [(set (match_operand:SI 0 "register_operand" "=&r,r")
	 (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		      (match_operand:SI 2 "shift_operand" "r,i")))]
  ""
   "@
    sub %?r0,%2,%0\;sha %1,%0,%0
    sha %1,-%2,%0"
   [(set_attr "length" "2,1")])

;; arithmetic shift right and set flags
(define_insn ""
   [(set (reg:CC 3)
	 (compare:CC (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
				  (match_operand:SI 2 "shift_operand" "r,i"))
		     (const_int 0)))
    (set (match_operand:SI 0 "register_operand" "=r,r")
	 (ashiftrt:SI (match_dup 1) (match_dup 2)))]
  "!TARGET_one_alu"
   "@
    put %1 sha.f (%?r0 sub %2), %0
    sha.f %1,-%2,%0"
)

(define_insn ""
   [(set (reg:CC 3)
	 (compare:CC (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
				  (match_operand:SI 2 "shift_operand" "r,i"))
		     (const_int 0)))
    (set (match_operand:SI 0 "register_operand" "=&r,r")
	 (ashiftrt:SI (match_dup 1) (match_dup 2)))]
  ""
   "@
    sub %?r0,%2,%0\;sha.f %1,%0,%0
    sha.f %1,-%2,%0"
   [(set_attr "length" "2,1")]
)

;; Shift left
(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ashift:SI (match_operand:SI 1 "register_operand" "r")
		   (match_operand:SI 2 "shift_operand" "rM")))]
  ""
  "sha %1,%2,%0"
)
;; Shift left and set flags
;; This does not set the C or V flags correctly for a compare with zero,
;; but that is OK if CC_NOOVmode is used.
(define_insn ""
  [(set (reg:CC_NOOV 3)
	(compare:CC_NOOV (ashift:SI (match_operand:SI 1 "register_operand" "r")
				    (match_operand:SI 2 "shift_operand" "rM"))
			 (const_int 0)))
   (set (match_operand:SI 0 "register_operand" "=r")
	(ashift:SI (match_dup 1) (match_dup 2)))]
  ""
  "sha.f %1,%2,%0"
)

;; logical shift right

(define_expand "lshrsi3"
  [ (set (match_operand:SI 0 "register_operand" "")
	 (lshiftrt:SI (match_operand:SI 1 "register_operand" "")
		      (match_operand:SI 2 "shift_operand" "")))]
  ""
  "")

(define_insn ""
  [ (set (match_operand:SI 0 "register_operand" "=r,r")
	 (lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		      (match_operand:SI 2 "shift_operand" "M,r")))]
  "!TARGET_one_alu"
  "@
   sh %1,-%2,%0
   put %1 sh (%?r0 sub %2),%0"
)

(define_insn ""
  [ (set (match_operand:SI 0 "register_operand" "=r,&r")
	 (lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		      (match_operand:SI 2 "shift_operand" "M,r")))]
  ""
  "@
   sh %1,-%2,%0
   sub %?r0,%2,%0\;sh %1,%0,%0"
  [(set_attr "length" "1,2")]
)

;; logical shift right and set flags
(define_insn ""
  [ (set (reg:CC 3)
	 (compare:CC (lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
				  (match_operand:SI 2 "shift_operand" "M,r"))
		     (const_int 0)))
    (set (match_operand:SI 0 "register_operand" "=r,r")
	 (lshiftrt:SI (match_dup 1) (match_dup 2)))]
  "!TARGET_one_alu"
  "@
   sh.f %1,-%2,%0
   put %1 sh.f (%?r0 sub %2),%0"
)

(define_insn ""
  [ (set (reg:CC 3)
	 (compare:CC (lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
				  (match_operand:SI 2 "shift_operand" "M,r"))
		     (const_int 0)))
    (set (match_operand:SI 0 "register_operand" "=r,&r")
	 (lshiftrt:SI (match_dup 1) (match_dup 2)))]
  ""
  "@
   sh.f %1,-%2,%0
   sub %?r0,%2,%0\;sh.f %1,%0,%0"
  [(set_attr "length" "1,2")]
)
;; Unconditional and other jump instructions
;; BAD: From here to end of file needs severe review

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "bt %0%#"
  [(set_attr "type" "branch")]
)

(define_insn "tablejump"
  [(set (pc) (match_operand:SI 0 "register_operand" "r"))
   (use (label_ref (match_operand 1 "" "")))
  ]
  ""
  "mov %0,%?pc !tablejump %l1%#"
  [(set_attr "type" "branch")])

;; Pretend this is an arithmetic instruction to prevent nops from being
;; emitted after it.  This is needed so the PC points to the beginning
;; of the table in the "!TARGET_no_pc_load" case, and it is OK because it
;; is guaranteed to be followed by the jump table.  The elements of the jump
;; table are effectively nop insn's, but they might clobber the flags, which
;; is OK since we mark the flags as clobbered above and we are guaranteed
;; there are more than two entries in the table because if there are
;; fewer than 4 entries, a tablejump is not used.

;;- jump to subroutine
(define_expand "call"
  [(call (match_operand:QI 0 "call_insn_operand" "")
	 (match_operand 1 "" ""))]
  ;; operand[2] is next_arg_register
  ""
  "
{
  /* Make sure the address is just one reg and will stay that way.  */
  if (! call_insn_operand (operands[0], QImode))
    operands[0]
      = change_address (operands[0], VOIDmode,
			copy_to_mode_reg (Pmode, XEXP (operands[0], 0)));
}"
)

;;- jump to subroutine
;; Zeroth operand is the address (which will be in a register)
;; First is number of bytes of arguments
(define_insn ""
  [ (call (mem:QI (match_operand:SI 0 "nonmemory_operand" "ri"))
	  (match_operand 1 "" "")
    )
  ]
  ;; operand[2] is next_arg_register
  ""
  "*{
     if (TARGET_min_branch_delay == 2)
       return \"mov %0,%?pc\;mov .+8,%?rca\;st %?rca,[--%?sp]\";
     else if (TARGET_min_branch_delay == 1)
       {
	 if (TARGET_max_branch_delay == 1)
	   return \"mov .+12,%?rca\;mov %0,%?pc\;st %?rca,[--%?sp]\";
	 else if (TARGET_max_branch_delay == 2)
	   return \"mov .+16,%?rca\;mov %0,%?pc\;st %?rca,[--%?sp]\;nop\";
	 else
	   abort ();
       }
     else
       abort ();
    }"
  ;; Don't mark as a call since has no shadow.
  [(set_attr "length" "3")]
)

(define_expand "call_value"
  [(set (match_operand 0 "register_operand" "=r")
	(call (match_operand:QI 1 "call_insn_operand" "")
	      (match_operand:SI 2 "" "")))]
  ;; operand 3 is next_arg_register
  ""
  "
{
  /* Make sure the address is just one reg and will stay that way. */
   if (! call_insn_operand (operands[1], QImode))
    operands[1]
      = change_address (operands[1], VOIDmode,
			copy_to_mode_reg (Pmode, XEXP (operands[1], 0)));
}"
)

(define_insn ""
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:QI (match_operand:SI 1 "nonmemory_operand" "ri"))
	      (match_operand 2 "" "")))]
  ;; operand 3 is next_arg_register
  ""
  "*{
     if (TARGET_min_branch_delay == 2)
       return \"mov %1,%?pc\;mov .+8,%?rca\;st %?rca,[--%?sp]\";
     else if (TARGET_min_branch_delay == 1)
       {
	 if (TARGET_max_branch_delay == 1)
	   return \"mov .+12,%?rca\;mov %1,%?pc\;st %?rca,[--%?sp]\";
	 else if (TARGET_max_branch_delay == 2)
	   return \"mov .+16,%?rca\;mov %1,%?pc\;st %?rca,[--%?sp]\;nop\";
	 else
	   abort ();
       }
     else
       abort ();
  }"
  [(set_attr "length" "3")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;- jump to subroutine
(define_expand "call_pop"
  [(parallel [(call (match_operand:QI 0 "call_insn_operand" "")
		    (match_operand 1 "" ""))
	     (set (reg:SI 4)
		  (plus:SI (reg:SI 4)
			   (match_operand:SI 3 "immediate_operand" "")))])]
  ;; operand[2] is next_arg_register
  ;; operand[3] is n_pop
  ""
  "
{
  /* Make sure the address is just one reg and will stay that way.  */
  if (! call_insn_operand (operands[0], QImode))
    operands[0]
      = change_address (operands[0], VOIDmode,
			copy_to_mode_reg (Pmode, XEXP (operands[0], 0)));
}"
)

;;- jump to subroutine
;; Zeroth operand is the address (which will be in a register)
;; First is number of bytes of arguments
(define_insn ""
  [(call (mem:QI (match_operand:SI 0 "nonmemory_operand" "ri"))
	 (match_operand 1 "" ""))
  (set (reg:SI 4)
       (plus:SI (reg:SI 4)
		(match_operand:SI 3 "immediate_operand" "")))
  (use (match_operand:SI 2 "general_operand" ""))
  ]
  ;; operand[2] is next_arg_register
  ;; operand[3] is n_pop
  ""
  "*
{
  if (TARGET_min_branch_delay == 2)
    return \"mov %0,%?pc\;mov .+8,%?rca\;st %?rca,[--%?sp]\";
  else if (TARGET_min_branch_delay == 1)
    {
      if (TARGET_max_branch_delay == 1)
	return \"mov .+12,%?rca\;mov %0,%?pc\;st %?rca,[--%?sp]\";
      else if (TARGET_max_branch_delay == 2)
	return \"mov .+16,%?rca\;mov %0,%?pc\;st %?rca,[--%?sp]\;nop\";
      else
	abort ();
    }
  else
      abort ();
}"
;; Don't mark as a call since has no shadow.
  [(set_attr "length" "3")]
)

;;(define_expand "call_value_pop"
;;  [(parallel [(set (match_operand 0 "register_operand" "")
;;		      (call (match_operand:QI 1 "call_insn_operand" "")
;;			    (match_operand:SI 2 "" "")))
;;		(set (reg:SI 4)
;;		     (plus:SI (reg:SI 4)
;;			      (match_operand:SI 4 "immediate_operand" "")))])
;;  ]
;;  ;; operand 3 is next_arg_register
;;  ;; operand[4] is n_pop
;;  ""
;;  "
;;{
;;  /* Make sure the address is just one reg and will stay that way. */
;;   if (! call_insn_operand (operands[1], QImode))
;;    operands[1]
;;	 = change_address (operands[1], VOIDmode,
;;			   copy_to_mode_reg (Pmode, XEXP (operands[1], 0)));
;;}"
;;)
;;
;;(define_insn ""
;;  [(parallel [(set (match_operand 0 "register_operand" "=r")
;;		      (call (mem:QI (match_operand:SI 1 "nonmemory_operand" "ri"))
;;			    (match_operand 2 "" "")))
;;		(set (reg:SI 4)
;;		     (plus:SI (reg:SI 4)
;;			      (match_operand:SI 4 "immediate_operand" "")))])
;;  ]
;;  ;; operand 3 is next_arg_register
;;  ;; operand[4] is n_pop
;;  ""
;;  "*{
;;	if (TARGET_branch_delay == 2)
;;	  return "mov %1,%?pc\;mov .+8,%?rca\;st %?rca,[--%?sp]";
;;	else if (TARGET_branch_delay == 1)
;;	  return "add %?pc,8,%?rca\;mov %1,%?pc\;st %?rca,[--%?sp]"; BAD
;;	else
;;	  abort ();
;;    }"
;;  ;; Don't mark as a call since has no shadow
;;  [ (set_attr "length" "3") ]
;;)

;; UNSPEC_VOLATILE is considered to use and clobber all hard registers and
;; all of memory.  This blocks insns from being moved across this point.

(define_insn "blockage"
  [(unspec_volatile [(const_int 0)] 0)]
  ""
  "")

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop ! (const_int 0)"
)

;; BAD???  sparc.md takes an "address_operand" here.
(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "register_operand" "r"))]
  ""
  "mov %0,%?pc%#"
  [(set_attr "type" "branch")]
)

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;; Casesi
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; ;; The "(const_int 700)" inside the parallel statements below prevent
; ;; the compiler from using the regular patterns for these operations,
; ;; which prevents the compiler from optimizing the code, which
; ;; Allows us to optimize it ourselves reliably with a special
; ;; peephole optimization.  The downside is that we must define
; ;; 3 special "(const_int 700)" matching instructions for the case
; ;; when optimization is not used.
;
; (define_expand "casesi"
;   [(set (match_dup 5)
;	(minus:SI (match_operand:SI 0 "register_operand" "r")
;		  (match_operand:SI 1 "arith_operand" "rJ")))
;   (set (match_dup 6) (ashift:SI (match_dup 5) (const_int 2)))
;   (set (match_dup 7) (label_ref (match_operand 3 "" "")))
;   (set (reg:CC 3) (compare:CC (match_dup 5)
;			      (match_operand:SI 2 "arith_operand" "rJ")))
;   (set (match_dup 8) (mem:SI (plus:SI (match_dup 6) (match_dup 7))))
;   (set (pc)
;	 (if_then_else (gtu (reg:CC 3)
;			  (const_int 0))
;		     (label_ref (match_operand 4 "" ""))
;		     (pc)))
;   (set (pc) (match_dup 8))]
;   ""
;   "{
;     operands[3] = force_reg (SImode, operands[3]);
;     operands[5] = gen_reg_rtx (SImode);
;     operands[6] = gen_reg_rtx (SImode);
;     operands[7] = gen_reg_rtx (SImode);
;     operands[8] = gen_reg_rtx (SImode);
;   }")
;
; ;;;;;;;;;;;;;;;;
; ;; "(const_int 700)" instructions, to catch the case that the peephole
; ;; optimizer is not being used.
; ;;;;;;;;;;;;;;;;
;
; (define_insn ""
;   [(set (match_operand:SI 0 "register_operand" "=&r")
;	(ashift:SI (match_operand:SI 1 "register_operand" "r")
;		   (const_int 2)))
;   (set (match_operand:SI 2 "register_operand" "=&r")
;	 (label_ref:SI (match_operand 3 "" "")))
;   (set (reg:CC 3)
;	 (compare:CC (match_operand:SI 4 "register_operand" "r")
;		   (match_operand:SI 5 "arith_operand" "rJ")))
;   (clobber (mem:BLK (const_int 0)))
;   (const_int 700)]
;   ""
;   "sh %1,2,%0\;mov %3,%2\;sub.f %4,%5,%?r0"
;   [(set_attr "length" "3")])
;
; ; Don't mark this as a branch so it won't be optimized, confusing the
; ; peephole optimizer.
;
; (define_insn ""
;   [(set (pc)
;	(if_then_else (gtu (reg:CC 3)
;			   (const_int 0))
;		      (label_ref (match_operand 0 "" ""))
;		      (pc)))
;   (const_int 700)]
;   ""
;   "bugt %0\;nop\;nop"
;   ; Do not mark this as a branch, to avoid delayed branch scheduling which
;   ; would confuse the peephole optimizer
;   [(set_attr "length" "3")])
;
; (define_insn ""
;   [(set (pc) (match_operand:SI 0 "memory_operand" "m"))
;   (const_int 700)
;   (use (label_ref (match_operand 1 "" "")))]
;   ""
;   "*{
;     int i;
;     if (TARGET_no_pc_load)
;	{
;	  output_asm_insn (\"ld %0,%?r15\", operands);
;	  for (i = TARGET_max_branch_delay; i; i--)
;	    output_asm_insn (\"nop\", operands);
;	  output_asm_insn (\"mov %?r15,%?pc\", operands);
;	  for (i = TARGET_max_branch_delay; i; i--)
;	    output_asm_insn (\"nop\", operands);
;	}
;     else
;	{
;	output_asm_insn (\"ld %0,%?pc\", operands);
;	  for (i = TARGET_max_indirect_jump_delay; i; i--)
;	    output_asm_insn (\"nop\", operands);
;	}
;     return \"\";
;   }"
;   [(set_attr "length" "5")])		; big enough to prevent DBR scheduling

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; casesi peephole optimizations
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; (define_peephole
;   [(parallel [(set (match_operand:SI 0 "register_operand" "=&r")
;		  (ashift:SI (match_operand:SI 1 "register_operand" "r")
;			     (const_int 2)))
;	    (set (match_operand:SI 2 "register_operand" "=&r")
;		 (label_ref (match_operand 3 "" "")))
;	    (set (reg:CC 3)
;		 (compare:CC (match_operand:SI 4 "register_operand" "r")
;			     (match_operand:SI 5 "arith_operand" "rJ")))
;	    (clobber (mem:BLK (const_int 0)))
;	    (const_int 700)])
;   (set (match_operand:SI 8 "register_operand" "=r") (match_dup 0))
;   (parallel [(set (pc)
;		  (if_then_else (gtu (reg:CC 3) (const_int 0))
;				(label_ref (match_operand 6 "" ""))
;				(pc)))
;	    (const_int 700)])
;   (parallel [(set (pc) (mem:SI (plus:SI
;				(match_dup 8)
;				(match_dup 2))))
;	    (const_int 700)
;	    (use (label_ref (match_operand 7 "" "")))])]
;   "TARGET_no_pc_load
;    || (TARGET_min_branch_delay == 1 && TARGET_max_branch_delay == 1)
;    || (TARGET_min_branch_delay == 2 && TARGET_max_branch_delay == 2)"
;   "*{
;     int i;
;
;     output_asm_insn (\"mov %3,%2\", operands);
;     if (TARGET_no_pc_load)
;	{
;	  output_asm_insn (\"sh %1,2,%0\", operands);
;	  output_asm_insn (\"ld [%0 add %2],%?r15\", operands);
;	  output_asm_insn (\"sub.f %4,%5,%?r0\", operands);
;	  output_asm_insn (\"bugt %6\", operands);
;	  output_asm_insn (\"mov %0,%8\", operands);
;	  for (i = TARGET_max_branch_delay-1; i; i--)
;	    output_asm_insn (\"nop\", operands);
;	  output_asm_insn (\"mov %?r15,%?pc\", operands);
;	  for (i = TARGET_max_branch_delay; i; i--)
;	    output_asm_insn (\"nop\", operands);
;	}
;     else
;	{
;	  /* PC loads are OK */
;
;	if (TARGET_min_branch_delay == 1
;	      && TARGET_max_branch_delay == 1)
;	    {
;	      output_asm_insn (\"sh %1,2,%0\", operands);
;	      output_asm_insn (\"sub.f %4,%5,%?r0\", operands);
;	      output_asm_insn (\"ld [%0 add %2],%?pc\", operands);
;	      output_asm_insn (\"bugt %6\", operands);
;	      output_asm_insn (\"mov %0,%8\", operands);
;	    }
;	else if (TARGET_min_branch_delay == 2
;		   && TARGET_max_branch_delay == 2)
;	    {
;	      /* Hack: take advantage of the fact that no read has
;		 a hardware side effect on LANais with 2 delay slots. */
;
;	      output_asm_insn (\"sh %1,2,%0\", operands);
;	      output_asm_insn (\"ld [%0 add %2],%?pc\", operands);
;	      output_asm_insn (\"sub.f %4,%5,%?r0\", operands);
;	      output_asm_insn (\"bugt %6\", operands);
;	      output_asm_insn (\"mov %0,%8\", operands);
;	      output_asm_insn (\"nop\", operands);
;	    }
;	  else
;	    {
;	      abort ();
;	    }
;	}
;     return \"\";
;   }"
;   [(set_attr "length" "4")])		; large enough to not matter

;; ***********
;; ***********
;; ** Moves **
;; ***********
;; ***********

;; *** float ***

(define_insn "movdf"
  [(set (match_operand:DF 0 "nonimmediate_operand" "=g")
	(match_operand:DF 1 "general_operand" "g"))]
  ""
  "! unimplemented DF move"
)

;; ********************
;; ** Fullword moves **
;; ********************

(define_expand "movsi"
  [(set (match_operand:SI 0 "general_operand" "")
	(match_operand:SI 1 "general_operand" ""))]
  ""
  "if (!reload_in_progress
       && memory_operand (operands[0],SImode)
       && !trivial_store_operand (operands[1], SImode))
     operands[1] = force_reg (SImode,operands[1]);

   else if (IS_ISR (operands[0])
	    && !trivial_move_operand (operands[1], SImode))
     {
       /* Perform big_const/mem->ISR using an intermediate register. */
       rtx r = gen_reg_rtx (SImode);
       emit_insn (gen_movsi (r, operands[1]));
       emit_insn (gen_movsi (operands[0], r));
       DONE;
     }
  "
)

;; How to split big_constant->reg into two insns.
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
	(match_operand:SI 1 "const_int_operand" ""))]
  "!trivial_move_operand (operands[1], SImode)"
  [(set (match_dup 0) (match_dup 2))
   (set (match_dup 0) (plus:SI (match_dup 0) (match_dup 3)))]
  " {
	operands[2]=gen_rtx_CONST_INT (VOIDmode,trunc_int_for_mode (INTVAL(operands[1])&0xffff0000, SImode));
	operands[3]=gen_rtx_CONST_INT (VOIDmode,INTVAL(operands[1])&0x0000ffff);
    }
  "
)

;;; How to split small_const->mem
;(define_split
;  [(set (match_operand:SI 0 "memory_operand" "=")
;	(match_operand:SI 1 "trivial_const_build_operand" ""))]
;  ""
;  [(set (match_dup 2) (match_dup 1))
;  (set (match_dup 0) (match_dup 2))]
;  "
;{
;  operands[2] = gen_reg_rtx (SImode);
;} ")

;;; How to split big_constant->mem into three insns.
;(define_split
;  [(set (match_operand:SI 0 "register_operand" "=")
;	(match_operand:SI 1 "const_int_operand" ""))]
;  "!trivial_const_build_operand (operands[1], SImode)"
;  [(set (match_dup 4) (match_dup 2))
;   (set (match_dup 4) (plus:SI (match_dup 0) (match_dup 3)))
;   (set (match_dup 0) (match_dup 4))]
;  " {
;	operands[2]=gen_rtx_CONST_INT (VOIDmode,INTVAL(operands[1])&0xffff0000);
;	operands[3]=gen_rtx_CONST_INT (VOIDmode,INTVAL(operands[1])&0x0000ffff);
;	operands[4]=gen_reg_rtx (SImode);
;    }
;  "
;)

(define_insn ""
  [(set (reg:CC_NOOV 3) (compare:CC_NOOV
		    (match_operand:SI 1 "nonmemory_operand" "rJ,L,n")
		    (const_int 0)))
   (set (match_operand:SI 0 "register_operand" "=r,r,r") (match_dup 1))]
  "GET_CODE (operands[1]) != CONST_INT
	  || (GET_CODE (operands[1]) == CONST_INT
	  && ARITH_INT (INTVAL (operands[1])))"
  "*switch(which_alternative){
    case 0:
      return \"add.f %?r0,%1,%0\";
    case 1:
      operands[1] = GEN_INT (-INTVAL (operands[1]));
      return \"sub.f %?r0,%1,%0\";
    default:
      abort(); /* This should never happen */
    }")

(define_insn ""
  [(set (reg:CC 3) (compare:CC
		    (match_operand:SI 1 "immediate_operand" "i")
		    (const_int 0)))
   (set (match_operand:SI 0 "register_operand" "=r") (match_dup 1))]
  "AND_INT (INTVAL (operands[1]))"
  "and.f %?r1,%1,%0")

;; A pattern to let the compiler know it can do 0->mem and -1->mem in
;; just one instruction.  We also match all reg->mem moves here to
;; speed up the compiler a bit.
(define_insn ""
  [(set (match_operand:SI 0 "memory_operand" "=m")
	(match_operand:SI 1 "trivial_store_operand" "rO"))]
  ""
  "*
{
  lanai_insn = insn;
  return output_move (operands, SImode);
}
  ")

;; Here I describe how to do all possible moves.  The constraints
;; force the compiler to break all complex moves into simple ones.
(define_insn ""
  [(set (match_operand:SI 0 "nonimmediate_operand"  "=r,r,r,m")
	(match_operand:SI 1 "general_operand" "rKs,i,m,r"))]
  "!IS_ISR (operands[0])"
  "*
{
  lanai_insn = insn;
  return output_move (operands, SImode);
}
  "
  [(set_attr "type" "common,common,load,common")
  (set_attr "length" "1,2,1,1")])

;; Special pattern to allow only single-instruction moves into ISR.
;; Loads into ISR are forbidden.  This would break if the value
;; in ISR were ever reloaded.  This will not happen because ISR
;; is a global register variable.
;;
;; This really should be handled using a special register class for ISR
;; to avoid the extra instruction patterns and checks for ISR throughout.
(define_insn ""
  [(set (match_operand:SI 0 "register_operand"	"=r")
	(match_operand:SI 1 "general_operand" "rK"))]
  "IS_ISR (operands[0])"
  "*
{
  lanai_insn = insn;
  return output_move (operands, SImode);
}
  ")

;; *** float ***

(define_insn "movsf"
  [(set (match_operand:SF 0 "nonimmediate_operand" "=g")
	(match_operand:SF 1 "general_operand" "g"))]
  ""
  "! unimplemented SF move"
)

;; ********************
;; ** Halfword Moves **
;; ********************

(define_expand "movhi"
  [(set (match_operand:HI 0 "general_operand" "")
	(match_operand:HI 1 "general_operand" ""))]
  ""
  "if (!reload_in_progress
       && memory_operand (operands[0], HImode)
       && !trivial_store_operand (operands[1], HImode))
     operands[1] = force_reg (HImode, operands[1]);

   if (TARGET_no_partword_store
       && memory_operand (operands[0], HImode)
       && !reload_in_progress)
     {
       rtx addr, inv_addr, containing_word, backing, pre_shift, shift;
       rtx pre_mask, mask, shifted_data, masked_shifted_data, inv_mask;
       rtx masked_backing, store_value, data;

       addr = copy_addr_to_reg (XEXP (operand0, 0));
       inv_addr = gen_reg_rtx (SImode);
       containing_word
	= change_address (operand0, SImode,
			  gen_rtx_AND (SImode,
				   addr,
				   gen_rtx_CONST_INT (VOIDmode, -4)));
       backing = gen_reg_rtx (SImode);
       pre_shift = gen_reg_rtx (SImode);
       shift = gen_reg_rtx (SImode);
       pre_mask = gen_reg_rtx (SImode);
       mask = gen_reg_rtx (SImode);
       data = gen_reg_rtx (SImode);
       shifted_data = gen_reg_rtx (SImode);
       masked_shifted_data = gen_reg_rtx (SImode);
       inv_mask = gen_reg_rtx (SImode);
       masked_backing = gen_reg_rtx (SImode);
       store_value = gen_reg_rtx (SImode);

       if (BYTES_BIG_ENDIAN)
	 emit_insn (gen_xorsi3 (inv_addr, addr, GEN_INT (2)));
       else
	 inv_addr = addr;
       emit_insn (gen_movsi (backing, containing_word));
       emit_insn (gen_ashlsi3 (pre_shift, inv_addr, GEN_INT (30)));
       emit_insn (gen_lshrsi3 (shift, pre_shift, GEN_INT (27)));
       emit_insn (gen_movsi (pre_mask, GEN_INT (0xffff)));
       emit_insn (gen_ashlsi3 (mask, pre_mask, shift));
       if (GET_MODE (operand1) == VOIDmode)
	 data = force_reg (SImode, operand1);
       else
	 emit_insn (gen_extendhisi2 (data, operand1));
       emit_insn (gen_ashlsi3 (shifted_data, data, shift));
       emit_insn (gen_andsi3 (masked_shifted_data, shifted_data, mask));
       emit_insn (gen_xorsi3 (inv_mask, mask, force_reg (SImode, constm1_rtx)));
       emit_insn (gen_andsi3 (masked_backing, backing, inv_mask));
       emit_insn (gen_iorsi3 (store_value, masked_backing, masked_shifted_data));
       emit_insn (gen_movsi (containing_word, store_value));

       DONE;
     }

   if (TARGET_no_partword_load
       && !reload_in_progress
       && GET_CODE (operands[1]) == MEM)
     {
       rtx temp;

       temp = gen_reg_rtx (SImode);
       emit_insn (gen_reload_inhi (operands[0], operands[1], temp));
       DONE;
     }
  "
)

;; The pattern is basically irrelevant, since it is not emitted.
(define_expand "reload_inhi"
  [(parallel [(match_operand:HI 0 "register_operand" "")
	     (match_operand:HI 1 "memory_operand" "")
	     (match_operand:SI 2 "register_operand" "")])]
  "TARGET_no_partword_load"
  "
  {
       rtx rca, temp;

       if (partword_load_first_time && !TARGET_no_partword_load_warning)
	 {
	   warning (\"Using non-optimal partword load workaround.\\nDon't use char and short memory accesses or use the\\n`-mno_partword_load_warning' option to avoid this message.\\nYou will be warned only once per compilation.\");
	   partword_load_first_time = 0;
	 }

       temp = operands[2];

       if (reload_in_progress)
	   rca = gen_rtx_REG (SImode, 15);
       else
	 rca = gen_reg_rtx (SImode);

       /* ; Put the address in %rca */
       rca = copy_to_mode_reg (SImode, XEXP (operands[1], 0));
       /* ; Put the fullword containing the memory into temp */
       emit_insn (gen_movsi (temp,
			     change_address (operands[1], SImode, NULL)));
       /* ; Compute the number of bits to shift left so that the
	  ; desired byte is the MS halfword */
       emit_insn (gen_ashlsi3 (rca, rca, GEN_INT (30)));
       emit_insn (gen_lshrsi3 (rca, rca, GEN_INT (27)));
       /* ; sign extend the desired byte into the LS halfword */
       emit_insn (gen_ashlsi3 (temp, temp, rca));
       emit_insn (gen_ashrsi3 (temp, temp, GEN_INT (16)));
       emit_insn (gen_movhi (operands[0], gen_lowpart (HImode, temp)));
       DONE;
  }
"
)

;; A pattern to let the compiler know it can do 0->mem and -1->mem in
;; just one instruction.  We also match all reg->mem moves here to
;; speed up the compiler a bit.
(define_insn ""
  [(set (match_operand:HI 0 "memory_operand" "=m")
	(match_operand:HI 1 "trivial_store_operand" "rO"))]
  ""
  "*
{
  lanai_insn = insn;
  return output_move (operands, HImode);
}
  ")

;; Here I describe how to do all possible moves.  The constraints
;; force the compiler to break all complex moves into simple ones.
(define_insn ""
  [(set (match_operand:HI 0 "nonimmediate_operand" "=r,r,m")
	(match_operand:HI 1 "general_operand" "ri,m,r"))]
  ""
  "*
{
  lanai_insn = insn;
  return output_move (operands, HImode);
}
  "
  [(set_attr "type" "common,load,common")])

;; *** float ***

(define_insn "movhf"
  [(set (match_operand:HF 0 "nonimmediate_operand" "=g")
	(match_operand:HF 1 "general_operand" "g"))]
  ""
  "! unimplemented HF move"
)

;; ****************
;; ** Byte Moves **
;; ****************

(define_expand "movqi"
  [(set (match_operand:QI 0 "general_operand" "")
	(match_operand:QI 1 "general_operand" ""))]
  ""
  "if (!reload_in_progress
       && memory_operand (operands[0], QImode)
       && !trivial_store_operand (operands[1], QImode))
     operands[1] = force_reg (QImode, operands[1]);

   if (TARGET_no_partword_store
       && memory_operand (operands[0], QImode)
       && !reload_in_progress)
     {
       rtx addr, inv_addr, containing_word, backing, pre_shift, shift;
       rtx pre_mask, mask, shifted_data, masked_shifted_data, inv_mask;
       rtx masked_backing, store_value, data;

       addr = copy_addr_to_reg (XEXP (operand0, 0));
       inv_addr = gen_reg_rtx (SImode);
       containing_word
	= change_address (operand0, SImode,
			  gen_rtx_AND (SImode,
				   addr,
				   gen_rtx_CONST_INT (VOIDmode, -4)));
       backing = gen_reg_rtx (SImode);
       pre_shift = gen_reg_rtx (SImode);
       shift = gen_reg_rtx (SImode);
       pre_mask = gen_reg_rtx (SImode);
       mask = gen_reg_rtx (SImode);
       data = gen_reg_rtx (SImode);
       shifted_data = gen_reg_rtx (SImode);
       masked_shifted_data = gen_reg_rtx (SImode);
       inv_mask = gen_reg_rtx (SImode);
       masked_backing = gen_reg_rtx (SImode);
       store_value = gen_reg_rtx (SImode);

       if (BYTES_BIG_ENDIAN)
	 emit_insn (gen_xorsi3 (inv_addr, addr, GEN_INT (3)));
       else
	 inv_addr = addr;
       emit_insn (gen_movsi (backing, containing_word));
       emit_insn (gen_ashlsi3 (pre_shift, inv_addr, GEN_INT (30)));
       emit_insn (gen_lshrsi3 (shift, pre_shift, GEN_INT (27)));
       emit_insn (gen_movsi (pre_mask, GEN_INT (0xff)));
       emit_insn (gen_ashlsi3 (mask, pre_mask, shift));
       if (GET_MODE (operand1) == VOIDmode)
	 data = force_reg (SImode, operand1);
       else
	 emit_insn (gen_extendqisi2 (data, operand1));
       emit_insn (gen_ashlsi3 (shifted_data, data, shift));
       emit_insn (gen_andsi3 (masked_shifted_data, shifted_data, mask));
       emit_insn (gen_xorsi3 (inv_mask, mask, force_reg (SImode, constm1_rtx)));
       emit_insn (gen_andsi3 (masked_backing, backing, inv_mask));
       emit_insn (gen_iorsi3 (store_value, masked_backing, masked_shifted_data));
       emit_insn (gen_movsi (containing_word, store_value));

       DONE;
     }

  if ( TARGET_no_partword_load
       && !reload_in_progress
       && GET_CODE (operands[1]) == MEM )
    {
       rtx temp;

       temp = gen_reg_rtx (SImode);
       emit_insn (gen_reload_inqi (operands[0], operands[1], temp));
       DONE;
    }
  "
)

;; The pattern here irrelevant, since it is never emitted.
(define_expand "reload_inqi"
  [(parallel [(match_operand:QI 0 "register_operand" "")
	     (match_operand:QI 1 "memory_operand" "")
	     (match_operand:SI 2 "register_operand" "")])]
  "TARGET_no_partword_load"
  "
  {
       rtx rca, temp;

       if (partword_load_first_time && !TARGET_no_partword_load_warning)
	 {
	   warning (\"Using non-optimal partword load workaround.\\nDon't use char and short memory accesses or use the\\n`-mno_partword_load_warning' option to avoid this message.\\nYou will be warned only once per compilation.\");
	   partword_load_first_time = 0;
	 }

       temp = operands[2];

       if (reload_in_progress)
	 rca = gen_rtx_REG (SImode, 15);
       else
	 rca = gen_reg_rtx (SImode);

       /* ; Put the address in %rca */
       rca = copy_to_mode_reg (SImode, XEXP (operands[1], 0));
       /* ; Put the fullword containing the memory into temp */
       emit_insn (gen_movsi (temp,
			     change_address (operands[1], SImode, NULL)));
       /* ; Compute the number of bits to shift left so that the
	  ; desired byte is the MS halfword */
       emit_insn (gen_ashlsi3 (rca, rca, GEN_INT (30)));
       emit_insn (gen_lshrsi3 (rca, rca, GEN_INT (27)));
       /* ; sign extend the desired byte into the LS halfword */
       emit_insn (gen_ashlsi3 (temp, temp, rca));
       emit_insn (gen_ashrsi3 (temp, temp, GEN_INT (24)));
       emit_insn (gen_movqi (operands[0], gen_lowpart (QImode, temp)));
       DONE;
  }
"
)

;; A pattern to let the compiler know it can do 0->mem and -1->mem in
;; just one instruction.  We also match all reg->mem moves here to
;; speed up the compiler a bit.
(define_insn ""
  [(set (match_operand:QI 0 "memory_operand" "=m")
	(match_operand:QI 1 "reg_0_m1_operand" "rn"))]
  ""
  "*
{
  lanai_insn = insn;
  return output_move (operands, QImode);
}
  ")

;; Here I describe how to do all possible moves.  The constraints
;; force the compiler to break all complex moves into simple ones.
(define_insn ""
  [(set (match_operand:QI 0 "nonimmediate_operand" "=r,r,m,m")
	(match_operand:QI 1 "general_operand" "ri,m,r,i"))]
  ""
  "*
{
  lanai_insn = insn;
  return output_move (operands, QImode);
}
  "
  [(set_attr "type" "common,load,common,common")
  (set_attr "length" "1,1,1,2")])


;; *** float ***

(define_insn "movqf"
  [(set (match_operand:QF 0 "nonimmediate_operand" "=g")
	(match_operand:QF 1 "general_operand" "g"))]
  ""
  "! unimplemented QF move"
)

; ****************************************************************
; * Doubleword Support
; ****************************************************************/

(define_expand "movdi"
  [(set (match_operand: DI 0 "general_operand" "=g")
	(match_operand: DI 1 "general_operand" "g"))]
  ""
  "if (!reload_in_progress
       && memory_operand (operands[0], DImode)
       && !trivial_store_operand (operands[1], DImode))
     {
       operands[1] = force_reg (DImode, operands[1]);
     }")

(define_insn ""
  [(set (match_operand: DI 0 "nonimmediate_operand" "=r,r,m,r,m")
	(match_operand: DI 1 "general_operand" "r,m,r,i,m"))]
  ""
  "*{
      lanai_insn = insn;
      return output_movdi (operands);
    }"
  [(set_attr "length" "2,2,2,2,6")
  (set_attr "type" "common,load,common,common,common")])

; This is near the end since doublewords are typically not used much,
; and putting them earlier would slow down the compiler.
;
; Most 64-bit operations are handled efficiently by the built-in
; support in the compiler.  Here, we only handle the pseudo-instructions
; that the compiler does not handle efficiently.

; ****************
; * subdi3
; ****************

(define_insn "subdi3"
  [(set (match_operand:DI 0 "register_operand" "=&r")
	(minus:DI (match_operand:DI 1 "register_operand" "r")
		  (match_operand:DI 2 "register_operand" "r")))
  (clobber (reg:CC 3))]
  ""
  "*
{
  rtx myops[6];

  /* low parts */
  myops[0] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
  myops[1] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
  myops[2] = gen_rtx_REG (SImode, REGNO (operands[2]) + 1);
  /* high parts */
  myops[3] = gen_rtx_REG (SImode, REGNO (operands[0]));
  myops[4] = gen_rtx_REG (SImode, REGNO (operands[1]));
  myops[5] = gen_rtx_REG (SImode, REGNO (operands[2]));
  output_asm_insn (\"sub.f %1,%2,%0 !subdi3\", myops);
  output_asm_insn (\"subb %4,%5,%3 !subdi3\", myops);
  return \"\";
}"
  [(set_attr "length" "2")])

; ; Define how to split the preceeding 64 bit operation into 2 32 operations
;
; (define_split
;   [(set (match_operand:DI 0 "register_operand" "=r")
;	       (minus (match_operand:DI 1 "register_operand" "r")
;		      (match_operand:DI 2 "register_operand" "r")))]
;   ""
;   [(parallel [(set (match_operand:SI 3 "register_operand" "=r")
;			 (minus:SI (match_operand:SI 4 "register_operand" "r")
;				   (match_operand:SI 5 "register_operand" "r")))
;		   (set (reg:CC_CARRY 3) (compare:CC_CARRY (match_dup 4) (match_dup 5)))])
;   (parallel [(set (match_operand:SI 6 "register_operand" "=r")
;			(minus (match_operand:SI 7 "register_operand" "r")
;			       (match_operand:SI 8 "register_operand" "r")))
;		  (use (reg:CC_CARRY 3))])
;   ]
;   "{
;      operands[3] = gen_lowpart (SImode, operands[0]);
;      operands[4] = gen_lowpart (SImode, operands[1]);
;      operands[5] = gen_lowpart (SImode, operands[2]);
;      operands[6] = gen_highpart (SImode, operands[0]);
;      operands[7] = gen_highpart (SImode, operands[1]);
;      operands[8] = gen_highpart (SImode, operands[2]);
;    }")
;
; (define_insn "subdi3_low"
;   [(set (match_operand:DI 0 "register_operand" "=r")
;	       (minus (match_operand:DI 1 "register_operand" "r")
;		      (match_operand:DI 2 "register_operand" "r")))
;   (set (reg:CC_CARRY 3) (compare:CC_CARRY (match_dup 1) (match_dup 2)))]
;   ""
;   "*
; {
;   rtx myops[3];
;
;   myops[0] = gen_lowpart (SImode, operands[0]);
;   myops[1] = gen_lowpart (SImode, operands[1]);
;   myops[2] = gen_lowpart (SImode, operands[2]);
;   output_asm_insn (\"sub.f %1,%2,%0 !subdi3_low\", myops);
;   return \"\";
; }")
;
; (define_insn "subdi3_high"
;   [(set (match_operand:DI 0 "register_operand" "=r")
;	       (minus (match_operand:DI 1 "register_operand" "r")
;		      (match_operand:DI 2 "register_operand" "r")))
;   (use (reg:CC_CARRY 3))]
;   ""
;   "*
; {
;   rtx myops[3];
;
;   myops[0] = gen_highpart (SImode, operands[0]);
;   myops[1] = gen_highpart (SImode, operands[1]);
;   myops[2] = gen_highpart (SImode, operands[2]);
;   output_asm_insn (\"subb %1,%2,%0 !subdi3_high\", myops);
;
;   return \"\";
; }")

; ****************
; * adddi3
; ****************

; Like subdi3 above

(define_insn "adddi3"
  [(set (match_operand:DI 0 "register_operand" "=&r")
	(plus:DI (match_operand:DI 1 "register_operand" "r")
		 (match_operand:DI 2 "register_operand" "r")))
  (clobber (reg:CC 3))]
  ""
  "*
{
  rtx myops[6];

  /* low parts */
  myops[0] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
  myops[1] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
  myops[2] = gen_rtx_REG (SImode, REGNO (operands[2]) + 1);
  /* high parts */
  myops[3] = gen_rtx_REG (SImode, REGNO (operands[0]));
  myops[4] = gen_rtx_REG (SImode, REGNO (operands[1]));
  myops[5] = gen_rtx_REG (SImode, REGNO (operands[2]));
  output_asm_insn (\"add.f %1,%2,%0 !adddi3\", myops);
  output_asm_insn (\"addc %4,%5,%3 !adddi3\", myops);
  return \"\";
}"
  [(set_attr "length" "2")])

; ; Define how to split the preceeding 64 bit operation into 2 32 operations
;
; (define_split
;   [(set (match_operand:DI 0 "register_operand" "=r")
;	      (plus (match_operand:DI 1 "register_operand" "r")
;		     (match_operand:DI 2 "register_operand" "r")))]
;   ""
;   [(parallel [(set (match_operand:SI 3 "register_operand" "=r")
;			(plus:SI (match_operand:SI 4 "register_operand" "r")
;				  (match_operand:SI 5 "register_operand" "r")))
;		  (set (reg:CC_CARRY 3) (compare:CC_CARRY (match_dup 4) (match_dup 5)))])
;   (parallel [(set (match_operand:SI 6 "register_operand" "=r")
;		       (plus (match_operand:SI 7 "register_operand" "r")
;			      (match_operand:SI 8 "register_operand" "r")))
;		 (use (reg:CC_CARRY 3))])
;   ]
;   "{
;      operands[3] = gen_lowpart (SImode, operands[0]);
;      operands[4] = gen_lowpart (SImode, operands[1]);
;      operands[5] = gen_lowpart (SImode, operands[2]);
;      operands[6] = gen_highpart (SImode, operands[0]);
;      operands[7] = gen_highpart (SImode, operands[1]);
;      operands[8] = gen_highpart (SImode, operands[2]);
;    }")
;
; (define_insn "adddi3_low"
;   [(set (match_operand:DI 0 "register_operand" "=r")
;	      (plus (match_operand:DI 1 "register_operand" "r")
;		     (match_operand:DI 2 "register_operand" "r")))
;   (set (reg:CC_CARRY 3) (compare:CC_CARRY (match_dup 1) (match_dup 2)))]
;   ""
;   "*
; {
;   rtx myops[3];
;
;   myops[0] = gen_lowpart (SImode, operands[0]);
;   myops[1] = gen_lowpart (SImode, operands[1]);
;   myops[2] = gen_lowpart (SImode, operands[2]);
;   output_asm_insn (\"add.f %1,%2,%0 !adddi3_low\", myops);
;   return \"\";
; }")
;
; (define_insn "adddi3_high"
;   [(set (match_operand:DI 0 "register_operand" "=r")
;	      (plus (match_operand:DI 1 "register_operand" "r")
;		     (match_operand:DI 2 "register_operand" "r")))
;   (use (reg:CC_CARRY 3))]
;   ""
;   "*
; {
;   rtx myops[3];
;
;   myops[0] = gen_highpart (SImode, operands[0]);
;   myops[1] = gen_highpart (SImode, operands[1]);
;   myops[2] = gen_highpart (SImode, operands[2]);
;   output_asm_insn (\"addc %1,%2,%0 !adddi3_high\", myops);
;
;   return \"\";
; }")

; ****************
; * cmpdi
; ****************/

; We do 64-bit compares with a 4-instruction sequence,
; due to a LANai7 bug in its treatment of the Zero flag.

(define_expand "cmpdi"
  [(set (reg:CC 3)
	(compare:CC (match_operand:DI 0 "register_operand" "r")
		    (match_operand:DI 1 "register_operand" "r")))]
  ""
  "{
    /* Record that we have handled the compare in the normal fashion.
       I.e.: by emitting an insn pattern.  This tells gen_compare_reg()
       to do nothing. */
    lanai_compare_op0 = 0;
    lanai_compare_op1 = 0;
  }")

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC (match_operand:DI 0 "register_operand" "r")
		    (match_operand:DI 1 "register_operand" "r")))]
  "TARGET_working_subb_f"
  "*
{
  rtx myops[4];

  myops[0] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
  myops[1] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
  myops[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
  myops[3] = gen_rtx_REG (SImode, REGNO (operands[1]));
  output_asm_insn (\"sub.f %0,%1,%?r0 !cmpdi\", myops);
  output_asm_insn (\"subb.f %2,%3,%?r0 !cmpdi\", myops);
  return \"\";
}"
  [(set_attr "length" "2")])

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC (match_operand:DI 0 "register_operand" "r")
		    (match_operand:DI 1 "register_operand" "r")))]
  "TARGET_has_ps"
  "*
{
  rtx myops[4];

  myops[0] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
  myops[1] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
  myops[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
  myops[3] = gen_rtx_REG (SImode, REGNO (operands[1]));
  output_asm_insn (\"sub.f %0,%1,%?r0 !cmpdi\", myops);
  output_asm_insn (\"or %?ps,0xfffe,%?r15 !cmpdi save zero flag\", myops);
  output_asm_insn (\"subb.f %2,%3,%?r0 !cmpdi\", myops);
  output_asm_insn (\"and %?ps,%?r15,%?ps !cmpdi fix zero flag\", myops);
  return \"\";
}"
  [(set_attr "length" "4")])

; LANai4 cmpdi

;(define_insn "subb_f"
;  [(set (reg:CC 3) (compare:CC (match_operand:SI 0 "register_operand" "r")
;				 (match_operand:SI 1 "register_operand" "r")))
;  (const_int 998)]
;  ""
;  "subb.f %0,%1,%?r0")
;
;(define_split
;  [(set (reg:CC 3)
;	  (compare:CC (match_operand:DI 0 "register_operand" "r")
;		      (match_operand:DI 1 "register_operand" "r")))]
;  ""
;  [(set (reg:CC 3)
;	  (compare:CC (match_dup 2) (match_dup 3)))
;  (set (match_dup 4) (ior:SI (reg:SI 3) (const_int 0xfffe)))
;  (parallel [(set (reg:CC 3) (compare:CC (match_dup 5) (match_dup 6)))
;	      (const_int 998)])
;  (set (reg:SI 3) (and:SI (reg:SI 3) (match_dup 4)))]
;  "{
;   operands[2] = gen_lowpart (SImode, operands[0]);
;   operands[3] = gen_lowpart (SImode, operands[1]);
;   /* Cannot use gen_reg_rtx() in define_split, so use RCA */
;   operands[4] = gen_rtx_REG (SImode, 15);
;   operands[5] = gen_highpart (SImode, operands[0]);
;   operands[6] = gen_highpart (SImode, operands[1]);
;   }")

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC (match_operand:DI 0 "register_operand" "r")
		    (match_operand:DI 1 "register_operand" "r")))]
  "TARGET_max_branch_delay == 1"
  "*
{
  rtx myops[4];

  myops[0] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
  myops[1] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
  myops[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
  myops[3] = gen_rtx_REG (SImode, REGNO (operands[1]));
  output_asm_insn (\"sub.f %2,%3,%?r0 !cmpdi\", myops);
  output_asm_insn (\"beq 1f !cmpdi\", myops);
  output_asm_insn (\"sub.f %0,%1,%?r0 !cmpdi\", myops);
  output_asm_insn (\"subb.f %2,%3,%?r0 !cmpdi\", myops);
  output_asm_insn (\"1:\", myops);
  return \"\";
}"
  [(set_attr "length" "4")])

; any LANai cmpdi

(define_insn ""
  [(set (reg:CC 3)
	(compare:CC (match_operand:DI 0 "register_operand" "r")
		    (match_operand:DI 1 "register_operand" "r")))]
  "TARGET_max_branch_delay == 2"
  "*
{
  rtx myops[4];

  myops[0] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
  myops[1] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
  myops[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
  myops[3] = gen_rtx_REG (SImode, REGNO (operands[1]));
  output_asm_insn (\"sub.f %2,%3,%?r0 !cmpdi\", myops);
  output_asm_insn (\"beq 1f !cmpdi\", myops);
  output_asm_insn (\"sub.f %0,%1,%?r0 !cmpdi\", myops);
  output_asm_insn (\"nop !cmpdi\", myops);
  output_asm_insn (\"subb.f %2,%3,%?r0 !cmpdi\", myops);
  output_asm_insn (\"1:\", myops);
  return \"\";
}"
  [(set_attr "length" "5")])

;; *****************
;; ** String move **
;; *****************

;; The sparc md sez they did better letting gcc generate the code.
;; I don't presume to think I could easily do better. --Glenn

;;;; ************
;;;; ** Floats **
;;;; ************
;;
;;(define_insn "floatdisf2"
;;  [(set (match_operand:SF 0 "general_operand" "")
;;	(match_operand:DI 1 "general_operand" ""))]
;;  ""
;;  "! floatdisf2 not supported"
;;)

;;;; ****************************
;;;; ****************************
;;;; ** Peephole Optimizations **
;;;; ****************************
;;;; ****************************

;; sparc.md handles movs followed by compares using peepholes

(define_peephole
  [
  (set (match_operand:SI 0 "register_operand" "")
	 (plus (match_operand:SI 1 "register_operand" "")
	       (match_operand:SI 2 "arith_operand" "")))
  (set (match_operand:SI 3 "register_operand" "")
	 (mem:SI (match_dup 0)))
  ]
  "!TARGET_explicit_ld_stalls_required
   && (operands[1] == operands[0]
       || operands[2] == operands[0]
       || operands[0] == operands[3])"
  "*
{
  if (operands[1] == operands[0])
    return \"ld %2[*%0],%3\";
  else if (operands[2] == operands[0])
    return \"ld %1[*%0],%3\";
  else if (operands[0] == operands[3])
    return \"ld %1[%2],%3\";
  else
    abort ();
}")
