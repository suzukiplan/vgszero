CPP = g++ -std=c++17 -g
MAK = @make --no-print-directory
DEPS = common.hpp ../src/*
TEST_MODULES =\
	assignment \
	binary \
	bracket_to_address \
	bracket_eliminate \
	decimal \
	define_parse \
	define_replace \
	enum \
	formulas_evaluate \
	formulas_evaluate_array \
	increment \
	label_parse \
	label_parse_jump \
	label_extract_anonymous \
	literal \
	nametable \
	numeric \
	offset_parse \
	offset_replace \
	operand \
	org \
	sizeof_parse \
	sizeof_replace

all:
	${MAK} execute
	${MAK} execute >result.txt
	${MAK} show_result_count
	${MAK} show_result_count >>result.txt

show_result_count:
	@echo Normal case: `cat result.txt | grep -o -c "^N:"`
	@echo Error case: `cat result.txt | grep -o -c -e "^E:" -e "^Error:"`
	@echo Total: `cat result.txt | grep -o -c "^[NE]:"`

execute: ${TEST_MODULES}
	./assignment
	./binary
	./bracket_to_address
	./bracket_eliminate
	./decimal
	./define_parse
	./define_replace
	./enum
	./formulas_evaluate
	./formulas_evaluate_array
	./increment
	./label_parse
	./label_parse_jump
	./label_extract_anonymous
	./literal
	./nametable
	./numeric
	./offset_parse
	./offset_replace
	./operand
	./org
	./sizeof_parse
	./sizeof_replace
	${MAK} struct_tests
	${MAK} macro_tests
	${MAK} mnemonic

single: sizeof_replace
	./sizeof_replace

clean:
	rm -f ${TEST_MODULES}

assignment: assignment.cpp ${DEPS}
	${CPP} $< -o $@

binary: binary.cpp ${DEPS}
	${CPP} $< -o $@

bracket_to_address: bracket_to_address.cpp ${DEPS}
	${CPP} $< -o $@

bracket_eliminate: bracket_eliminate.cpp ${DEPS}
	${CPP} $< -o $@

decimal: decimal.cpp ${DEPS}
	${CPP} $< -o $@

define_parse: define_parse.cpp ${DEPS}
	${CPP} $< -o $@

define_replace: define_replace.cpp ${DEPS}
	${CPP} $< -o $@

enum: enum.cpp ${DEPS}
	${CPP} $< -o $@

formulas_evaluate: formulas_evaluate.cpp ${DEPS}
	${CPP} $< -o $@

formulas_evaluate_array: formulas_evaluate_array.cpp ${DEPS}
	${CPP} $< -o $@

increment: increment.cpp ${DEPS}
	${CPP} $< -o $@

label_parse: label_parse.cpp ${DEPS}
	${CPP} $< -o $@

label_parse_jump: label_parse_jump.cpp ${DEPS}
	${CPP} $< -o $@

label_extract_anonymous: label_extract_anonymous.cpp ${DEPS}
	${CPP} $< -o $@

literal: literal.cpp ${DEPS}
	${CPP} $< -o $@

nametable: nametable.cpp ${DEPS}
	${CPP} $< -o $@

numeric: numeric.cpp ${DEPS}
	${CPP} $< -o $@

offset_parse: offset_parse.cpp ${DEPS}
	${CPP} $< -o $@

offset_replace: offset_replace.cpp ${DEPS}
	${CPP} $< -o $@

operand: operand.cpp ${DEPS}
	${CPP} $< -o $@

org: org.cpp ${DEPS}
	${CPP} $< -o $@

sizeof_parse: sizeof_parse.cpp ${DEPS}
	${CPP} $< -o $@

sizeof_replace: sizeof_replace.cpp ${DEPS}
	${CPP} $< -o $@

check: check.cpp
	${CPP} $< -o $@

../vgsasm: ../src/*
	cd .. && make

mnemonic:
	${MAK} mnemonic_bit_tests
	${MAK} mnemonic_calc_tests
	${MAK} mnemonic_data_tests
	${MAK} mnemonic_djnz_tests
	${MAK} mnemonic_ex_tests
	${MAK} mnemonic_im_tests
	${MAK} mnemonic_inc_tests
	${MAK} mnemonic_dec_tests
	${MAK} mnemonic_in_tests
	${MAK} mnemonic_out_tests
	${MAK} mnemonic_jp_tests
	${MAK} mnemonic_jr_tests

mnemonic_jr_tests: check ../vgsasm
	${MAK} normal_case src=m_jr_n
	@./check "`../vgsasm m_jr_n_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_jr_n_range2.asm`" "Numerical range incorrect: 128"
	${MAK} normal_case src=m_jr_l
	${MAK} normal_case src=m_jr_osn
	@./check "`../vgsasm m_jr_osn_ill.asm`" "Invalid condition: PE"
	${MAK} normal_case src=m_jr_osl
	@./check "`../vgsasm m_jr_osl_ill.asm`" "Invalid condition: PO"
	@./check "`../vgsasm m_jr_ill.asm`" "Illegal JR instruction."

mnemonic_jp_tests: check ../vgsasm
	${MAK} normal_case src=m_jp_n
	@./check "`../vgsasm m_jp_n_range.asm`" "Numerical range incorrect: 65536"
	${MAK} normal_case src=m_jp_l
	${MAK} normal_case src=m_jp_osn
	${MAK} normal_case src=m_jp_osl
	${MAK} normal_case src=m_jp_aoa
	${MAK} normal_case src=m_jp_o
	@./check "`../vgsasm m_jp_ill.asm`" "Illegal JP instruction."

mnemonic_out_tests: check ../vgsasm
	${MAK} normal_case src=m_out_anaso
	@./check "`../vgsasm m_out_anaso_range.asm`" "Numerical range incorrect: 256"
	${MAK} normal_case src=m_out_aoaso
	@./check "`../vgsasm m_out_aoaso_ill.asm`" "Illegal OUT instruction."
	${MAK} normal_case src=m_out_osaoa
	${MAK} normal_case src=m_out_aoa
	@./check "`../vgsasm m_out_ill.asm`" "Illegal OUT instruction."

mnemonic_in_tests: check ../vgsasm
	${MAK} normal_case src=m_in_osana
	@./check "`../vgsasm m_in_osana_range.asm`" "Numerical range incorrect: 256"
	${MAK} normal_case src=m_in_osaoa
	@./check "`../vgsasm m_in_osaoa_ill.asm`" "Illegal IN instruction."
	@./check "`../vgsasm m_in_ill.asm`" "Illegal IN instruction."

mnemonic_dec_tests: check ../vgsasm
	${MAK} normal_case src=m_dec_o
	@./check "`../vgsasm m_dec_o_ill.asm`" "Illegal DEC instruction."
	${MAK} normal_case src=m_dec_aoa
	@./check "`../vgsasm m_dec_aoa_ill.asm`" "Illegal DEC instruction."
	${MAK} normal_case src=m_dec_ana
	@./check "`../vgsasm m_dec_ana_range.asm`" "Numerical range incorrect: 65536"
	${MAK} normal_case src=m_dec_aopna
	@./check "`../vgsasm m_dec_aopna_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_dec_aopna_range2.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_dec_aopna_ill.asm`" "Illegal DEC instruction."

mnemonic_inc_tests: check ../vgsasm
	${MAK} normal_case src=m_inc_o
	@./check "`../vgsasm m_inc_o_ill.asm`" "Illegal INC instruction."
	${MAK} normal_case src=m_inc_aoa
	@./check "`../vgsasm m_inc_aoa_ill.asm`" "Illegal INC instruction."
	${MAK} normal_case src=m_inc_ana
	@./check "`../vgsasm m_inc_ana_range.asm`" "Numerical range incorrect: 65536"
	${MAK} normal_case src=m_inc_aopna
	@./check "`../vgsasm m_inc_aopna_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_inc_aopna_range2.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_inc_aopna_ill.asm`" "Illegal INC instruction."

mnemonic_im_tests: check ../vgsasm
	${MAK} normal_case src=m_im
	@./check "`../vgsasm m_im_ill1.asm`" "Unsupported interrupt mode: 3"
	@./check "`../vgsasm m_im_ill2.asm`" "Unexpected operand: A"

mnemonic_ex_tests: check ../vgsasm
	${MAK} normal_case src=m_ex
	@./check "`../vgsasm m_ex_ill1.asm`" "Excessive or insufficient number of operands."
	@./check "`../vgsasm m_ex_ill2.asm`" "Unexpected operand: B"
	@./check "`../vgsasm m_ex_ill3.asm`" "Incorrect EX syntax."
	@./check "`../vgsasm m_ex_ill4.asm`" "Incorrect EX syntax."
	${MAK} normal_case src=m_ex_aoaso
	@./check "`../vgsasm m_ex_aoaso_ill1.asm`" "Unexpected operand: 4660"
	@./check "`../vgsasm m_ex_aoaso_ill2.asm`" "Incorrect EX syntax."

mnemonic_djnz_tests: check ../vgsasm
	@echo Testing: mnemonic djnz
	${MAK} normal_case src=m_djnz
	@./check "`../vgsasm m_djnz_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_djnz_range2.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_djnz_ill1.asm`" "Illegal DJNZ instruction."
	@./check "`../vgsasm m_djnz_ill2.asm`" "Illegal DJNZ instruction."

mnemonic_data_tests: check ../vgsasm
	@echo Testing: mnemonic data
	${MAK} normal_case src=m_db
	@./check "`../vgsasm m_db_nodata.asm`" "No binary data."
	@./check "`../vgsasm m_db_range1.asm`" "An invalid range of number specified: -129"
	@./check "`../vgsasm m_db_range2.asm`" "An invalid range of number specified: 256"
	@./check "`../vgsasm m_db_unex.asm`" "Unexpected symbols detected in DB: HL"
	@./check "`../vgsasm m_db_nosp.asm`" "Symbols are not specified as comma-separated: 2"
	@./check "`../vgsasm m_db_nosym.asm`" "No symbol is specified after the comma."
	@./check "`../vgsasm m_db_nodq.asm`" "String literals are not closed with double quotes."
	${MAK} normal_case src=m_dw
	@./check "`../vgsasm m_dw_nodata.asm`" "No binary data."
	@./check "`../vgsasm m_dw_range1.asm`" "An invalid range of number specified: -32769"
	@./check "`../vgsasm m_dw_range2.asm`" "An invalid range of number specified: 65536"
	@./check "`../vgsasm m_dw_unex.asm`" "Unexpected symbols detected in DW: BC"
	@./check "`../vgsasm m_dw_nosp.asm`" "Symbols are not specified as comma-separated: 2"
	@./check "`../vgsasm m_dw_nosym.asm`" "No symbol is specified after the comma."
	@./check "`../vgsasm m_dw_nodq.asm`" "String literals are not closed with double quotes."
	@./check "`../vgsasm m_dw_nolabel.asm`" "Label \`@LABEL3\` is undefined in \`LABEL1\`."

mnemonic_calc_tests: check ../vgsasm
	@echo Testing: mnemonic calc8
	${MAK} normal_case src=m_calc8_o
	@./check "`../vgsasm m_calc8_o_ill.asm`" "Illegal 8-bit arithmetic instruction."
	${MAK} normal_case src=m_calc8_n
	@./check "`../vgsasm m_calc8_n_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_calc8_n_range2.asm`" "Numerical range incorrect: 256"
	${MAK} normal_case src=m_calc8_n_range3
	${MAK} normal_case src=m_calc8_ao
	@./check "`../vgsasm m_calc8_ao_ill1.asm`" "Illegal 8-bit arithmetic instruction."
	@./check "`../vgsasm m_calc8_ao_ill2.asm`" "Illegal 8-bit arithmetic instruction."
	@./check "`../vgsasm m_calc8_ao_ill3.asm`" "Illegal 8-bit arithmetic instruction."
	${MAK} normal_case src=m_calc8_os
	@./check "`../vgsasm m_calc8_os_ill.asm`" "Illegal 8-bit arithmetic instruction."
	@./check "`../vgsasm m_calc8_ill.asm`" "Illegal 8-bit arithmetic instruction."
	@echo Testing: mnemonic calc16
	${MAK} normal_case src=m_calc16_add_bc_a
	${MAK} normal_case src=m_calc16_add_de_a
	${MAK} normal_case src=m_calc16_add_hl_a
	${MAK} normal_case src=m_calc16_hl_rr
	@./check "`../vgsasm m_calc16_hl_rr_ill.asm`" "Illegal 16-bit arithmetic instruction."
	${MAK} normal_case src=m_calc16_ix_rr
	@./check "`../vgsasm m_calc16_ix_rr_ill.asm`" "Illegal 16-bit arithmetic instruction."
	${MAK} normal_case src=m_calc16_iy_rr
	@./check "`../vgsasm m_calc16_iy_rr_ill.asm`" "Illegal 16-bit arithmetic instruction."
	@./check "`../vgsasm m_calc16_oso_ill.asm`" "Illegal 16-bit arithmetic instruction."
	${MAK} normal_case src=m_calc16_osn
	@./check "`../vgsasm m_calc16_osn_range1.asm`" "Numerical range incorrect: -32769"
	@./check "`../vgsasm m_calc16_osn_range2.asm`" "Numerical range incorrect: 65536"
	@./check "`../vgsasm m_calc16_osn_ill.asm`" "Illegal 16-bit arithmetic instruction."
	${MAK} normal_case src=m_calc16_osaoa
	@./check "`../vgsasm m_calc16_osaoa_ill.asm`" "Illegal 16-bit arithmetic instruction."
	${MAK} normal_case src=m_calc16_osaopna
	@./check "`../vgsasm m_calc16_osaopna_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_calc16_osaopna_range2.asm`" "Numerical range incorrect: 127"
	@./check "`../vgsasm m_calc16_osaopna_ill.asm`" "Illegal 16-bit arithmetic instruction."
	@./check "`../vgsasm m_calc16_ill.asm`" "Illegal 16-bit arithmetic instruction."
	@./check "`../vgsasm m_calc_ill1.asm`" "Illegal arithmetic instruction."
	@./check "`../vgsasm m_calc_ill2.asm`" "Illegal arithmetic instruction."
	${MAK} normal_case src=m_calc_ana
	@./check "`../vgsasm m_calc_ana_range.asm`" "Numerical range incorrect: 65536"

mnemonic_bit_tests: check ../vgsasm
	@echo Testing: mnemonic bits
	${MAK} normal_case src=m_bit_nso
	@./check "`../vgsasm m_bit_nso_range.asm`" "Numerical range incorrect: 8"
	@./check "`../vgsasm m_bit_nso_ill.asm`" "Illegal BIT/SET/RES instruction."
	${MAK} normal_case src=m_bit_nsaoa
	@./check "`../vgsasm m_bit_nsaoa_range.asm`" "Numerical range incorrect: 8"
	@./check "`../vgsasm m_bit_nsaoa_ill.asm`" "Illegal BIT/SET/RES instruction."
	${MAK} normal_case src=m_bit_nsaopna
	@./check "`../vgsasm m_bit_nsaopna_range1.asm`" "Numerical range incorrect: 8"
	@./check "`../vgsasm m_bit_nsaopna_range2.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_bit_nsaopna_range3.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_bit_ill.asm`" "Illegal BIT/SET/RES instruction."

macro_tests: check ../vgsasm
	@echo Testing: macro
	@./check "`../vgsasm macro_notop.asm`" "\`#macro\` must appear at the beginning of the line."
	@./check "`../vgsasm macro_only.asm`" "Invalid #macro syntax."
	@./check "`../vgsasm macro_dup.asm`" "Duplicate #macro names are defined: FOO"
	@./check "`../vgsasm macro_nobrk.asm`" "Invalid #macro syntax. (Parentheses)"
	@./check "`../vgsasm macro_nocma.asm`" "Invalid #macro syntax. (Arguments not separated by commas)"
	@./check "`../vgsasm macro_argop.asm`" "Invalid #macro syntax. (Invalid argument name: HL)"
	@./check "`../vgsasm macro_argdup.asm`" "Invalid #macro syntax. (Duplicate argument names: ARG1)"
	@./check "`../vgsasm macro_nobrk2.asm`" "\`(\` is not closed with \`)\`."
	@./check "`../vgsasm macro_nobrk3.asm`" "Unspecified bracket for macro call."
	@./check "`../vgsasm macro_nobrk4.asm`" "Unexpected symbol: A"
	@./check "`../vgsasm macro_unexpect.asm`" "Unexpected symbol: B"
	@./check "`../vgsasm macro_conflict.asm`" "A macro argument name conflict with the name used for structs, defines, etc.: ARG1"
	@./check "`../vgsasm macro_noscp.asm`" "Detect symbols that are not in the start scope of the macro: LD"
	@./check "`../vgsasm macro_circulating.asm`" "Macro calls are circulating: HOGE"
	@./check "`../vgsasm macro_nosplit.asm`" "Unexpected macro argument symbol: 2"
	@./check "`../vgsasm macro_nospa.asm`" "Invalid macro argument type: )"
	@./check "`../vgsasm macro_itype.asm`" "Invalid macro argument type: HL"
	@./check "`../vgsasm macro_noarg.asm`" "Macro argument mismatch: FOO"
	${MAK} normal_case src=macro_normal

struct_tests: check ../vgsasm
	@echo Testing: struct
	@./check "`../vgsasm struct_notop.asm`" "\`struct\` must appear at the beginning of the line."
	@./check "`../vgsasm struct_dup_field.asm`" "Duplicate field name \`VAR1\` in struct \`HOGE\`"
	@./check "`../vgsasm struct_name_dot.asm`" "Invalid structure name: HOGE.COM"
	@./check "`../vgsasm struct_arrow_no.asm`" "Incorrect syntax for struct arrow."
	@./check "`../vgsasm struct_arrow_same.asm`" "Incorrect syntax for struct arrow."
	@./check "`../vgsasm struct_arrow_inval.asm`" "Incorrect syntax for struct arrow."
	@./check "`../vgsasm struct_arrow_array_empty.asm`" "Unexpected symbol: ]"
	@./check "`../vgsasm struct_arrow_array_inval.asm`" "Unexpected symbol: HIGE"
	@./check "`../vgsasm struct_arrow_array_uex.asm`" "Unexpected symbol: 2"
	@./check "`../vgsasm struct_arrow_array_zero.asm`" "struct array must have a value of 1 or more: 0"
	@./check "`../vgsasm struct_noaddr.asm`" "Incorrect syntax for struct: ADDR"
	${MAK} normal_case src=struct_noaddr2
	@./check "`../vgsasm struct_noscope1.asm`" "Invalid \`}\` without corresponding \`{\`."
	@./check "`../vgsasm struct_noscope2.asm`" "The scope \`{\` specified in this line is not closed with \`}\`."
	@./check "`../vgsasm struct_arrow_seq1.asm`" "It must be defined on the line after the definition of struct specified by the arrow operator."
	@./check "`../vgsasm struct_arrow_seq2.asm`" "It must be defined on the line after the definition of struct specified by the arrow operator."
	@./check "`../vgsasm struct_field_dot.asm`" "Invalid structure field name: VAR.FOO"
	@./check "`../vgsasm struct_field_inv1.asm`" "Invalid field VAR1 in structure HOGE"
	@./check "`../vgsasm struct_field_inv2.asm`" "Invalid field VAR1 in structure HOGE"
	@./check "`../vgsasm struct_field_nnum.asm`" "Specify non-numeric number of field VAR1 in structure HOGE: AAA"
	@./check "`../vgsasm struct_field_nstr.asm`" "Unknown structure name FOO specified in field VAR1 of structure HOGE"
	@./check "`../vgsasm struct_field_cir.asm`" "Detects circular references between structures"
	@./check "`../vgsasm struct_field_uk.asm`" "Unknown field name in structure HOGE: VAR2"
	@./check "`../vgsasm struct_array_empty.asm`" "Illegal array structure element: ]"
	@./check "`../vgsasm struct_array_ill.asm`" "Illegal array structure element: FOO"
	@./check "`../vgsasm struct_array_mul.asm`" "Illegal array structure element: 2"
	@./check "`../vgsasm struct_array_nd.asm`" "Invalid array field designation: VAR1"
	@./check "`../vgsasm struct_field_nf.asm`" "Unknown field name in structure HOGE: VAR2"
	${MAK} normal_case src=struct_normal

normal_case:
	@echo N: ${src}
	../vgsasm ${src}.asm -v
	diff ${src}.bin ${src}.exp
	@#z88dk-dis ${src}.exp > ${src}.dis
