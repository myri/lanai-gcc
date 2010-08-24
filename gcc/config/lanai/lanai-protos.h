#ifndef _lanai_protos_h_
#define _lanai_protos_h_ 1

extern int partword_load_first_time;

extern int free_insn_output_reg (int *last);
extern void lanai_output_function_prologue (register FILE *asm_file,
					    register unsigned SIZE);
extern void lanai_output_function_epilogue (register FILE *asm_file,
					    register unsigned SIZE);


#ifdef RTX_CODE
extern rtx lanai_compare_op0;
extern rtx lanai_compare_op1;
extern rtx lanai_insn;
extern const char *output_cbranch (rtx op, int label, int reversed,
				   int shadow_filled);
extern const char *output_movdi (rtx *operands);
extern const char *output_move (rtx *operands, enum machine_mode m);
extern const char *output_scc_insn (rtx operands[], rtx insn);
extern int and_operand (rtx op, enum machine_mode mode);
extern int arith_operand (rtx op, enum machine_mode mode);
extern int call_insn_operand (rtx op, enum machine_mode mode);
extern int legitimate_address (enum machine_mode MODE, rtx X, int STRICT);
extern int logic_operand (rtx op, enum machine_mode mode);
extern int noov_compare_op (rtx op, enum machine_mode mode);
extern int op1_operand (rtx op, enum machine_mode mode);
extern int op2_operand (rtx op, enum machine_mode mode);
extern int operand2_ok_for_operator (rtx operator, rtx operand);
extern int output_load (rtx *operands, enum machine_mode m);
extern int reg_0_m1_operand (rtx op, enum machine_mode mode);
extern int shift_operand (rtx op, enum machine_mode mode);
extern int trivial_move_operand (rtx op, enum machine_mode m);
extern int trivial_store_operand (rtx op, enum machine_mode mode);
extern struct rtx_def *gen_compare_reg (enum rtx_code code, rtx x, rtx y);
extern void emit_nops_if_needed (rtx dest, int dslots_number_nops);
extern void output_store (rtx *operands, enum machine_mode m);
extern void print_operand_address (FILE *f, rtx a);
#endif

#endif /* _lanai_protos_h_ */
