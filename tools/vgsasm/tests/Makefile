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
	${MAK} all_instructions
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
	${MAK} test55
	${MAK} test56
	${MAK} test57
	${MAK} test58
	${MAK} test59
	${MAK} test60

test60: check ../vgsasm
	${MAK} normal_case src=test60

test59: check ../vgsasm
	${MAK} normal_case src=ifdef
	@./check "`../vgsasm ifdef_err1.asm`" "#ifdef must appear at the beginning of the line."
	@./check "`../vgsasm ifdef_err2.asm`" "Definition name not specified in #ifdef."
	@./check "`../vgsasm ifdef_err3.asm`" "Multiple definition names specified in #ifdef: B"
	@./check "`../vgsasm ifdef_err4.asm`" "The corresponding #ifdef is not defined."
	@./check "`../vgsasm ifdef_err5.asm`" "The corresponding #ifdef is not defined."
	@./check "`../vgsasm ifdef_err6.asm`" "#else must appear at the beginning of the line."
	@./check "`../vgsasm ifdef_err7.asm`" "Unexpected symbol has specified: B"
	@./check "`../vgsasm ifdef_err8.asm`" "#endif must appear at the beginning of the line."
	@./check "`../vgsasm ifdef_err9.asm`" "Unexpected symbol has specified: Y"
	@./check "`../vgsasm ifdef_err10.asm`" "#ifdef is specified as nested."
	@./check "`../vgsasm ifdef_err11.asm`" "#ifdef is specified as nested."
	@./check "`../vgsasm ifdef_err12.asm`" "#ifdef is not closed with #endif."

test58: check ../vgsasm
	${MAK} normal_case src=test58

test57: check ../vgsasm
	${MAK} normal_case src=m_add_b_n
	${MAK} normal_case src=m_add_c_n
	${MAK} normal_case src=m_add_d_n
	${MAK} normal_case src=m_add_e_n
	${MAK} normal_case src=m_add_h_n
	${MAK} normal_case src=m_add_l_n
	@./check "`../vgsasm m_add_b_n_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_add_b_n_range2.asm`" "Numerical range incorrect: 256"
	@./check "`../vgsasm m_adc_b_n.asm`" "Illegal 8-bit arithmetic instruction."
	@./check "`../vgsasm m_add_ixh_n.asm`" "Illegal arithmetic instruction."
	@./check "`../vgsasm m_add_ixl_n.asm`" "Illegal arithmetic instruction."
	@./check "`../vgsasm m_add_iyh_n.asm`" "Illegal arithmetic instruction."
	@./check "`../vgsasm m_add_iyl_n.asm`" "Illegal arithmetic instruction."


test56: check ../vgsasm
	${MAK} normal_case src=m_add_bc_nn
	${MAK} normal_case src=m_add_de_nn
	@./check "`../vgsasm m_add_bc_nn_range1.asm`" "Numerical range incorrect: -32769"
	@./check "`../vgsasm m_add_bc_nn_range2.asm`" "Numerical range incorrect: 65536"
	@./check "`../vgsasm m_add_de_nn_range1.asm`" "Numerical range incorrect: -32769"
	@./check "`../vgsasm m_add_de_nn_range2.asm`" "Numerical range incorrect: 65536"

test55: check ../vgsasm
	${MAK} normal_case src=m_sizeof
	@./check "`../vgsasm m_sizeof_nofield.asm`" "Undefined field XXXX of strcture VRAM is specified in sizeof."
	@./check "`../vgsasm m_sizeof_nostr.asm`" "Undefined structure YYYYY is specified in sizeof."

all_instructions: check ../vgsasm
	${MAK} normal_case src=all

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
	${MAK} mnemonic_rst_tests
	${MAK} mnemonic_shift_tests
	${MAK} mnemonic_stack_tests
	${MAK} mnemonic_sub_tests
	${MAK} mnemonic_vgs_tests
	${MAK} mnemonic_load_tests
	${MAK} mnemonic_other_tests

mnemonic_other_tests: check ../vgsasm
	${MAK} normal_case src=m_single
	@./check "`../vgsasm m_far_m129.asm`" "Relative jump destination address is too far away: -129"
	${MAK} normal_case src=m_far_m128
	${MAK} normal_case src=m_far_p127
	@./check "`../vgsasm m_far_p128.asm`" "Relative jump destination address is too far away: 128"

mnemonic_load_tests: check ../vgsasm
	${MAK} normal_case src=m_ld_oso8_a
	${MAK} normal_case src=m_ld_oso8_b
	${MAK} normal_case src=m_ld_oso8_c
	${MAK} normal_case src=m_ld_oso8_d
	${MAK} normal_case src=m_ld_oso8_e
	${MAK} normal_case src=m_ld_oso8_h
	${MAK} normal_case src=m_ld_oso8_l
	${MAK} normal_case src=m_ld_oso8_ixh
	${MAK} normal_case src=m_ld_oso8_ixl
	${MAK} normal_case src=m_ld_oso8_iyh
	${MAK} normal_case src=m_ld_oso8_iyl
	@./check "`../vgsasm m_ld_oso8_ill01.asm`" "LD {H|L},{IXH|IXL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill02.asm`" "LD {H|L},{IXH|IXL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill03.asm`" "LD {H|L},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill04.asm`" "LD {H|L},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill05.asm`" "LD {H|L},{IXH|IXL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill06.asm`" "LD {H|L},{IXH|IXL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill07.asm`" "LD {H|L},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill08.asm`" "LD {H|L},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill09.asm`" "LD {IXH|IXL},{H|L} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill10.asm`" "LD {IXH|IXL},{H|L} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill11.asm`" "LD {IXH|IXL},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill12.asm`" "LD {IXH|IXL},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill13.asm`" "LD {IXH|IXL},{H|L} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill14.asm`" "LD {IXH|IXL},{H|L} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill15.asm`" "LD {IXH|IXL},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill16.asm`" "LD {IXH|IXL},{IYH|IYL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill17.asm`" "LD {IYH|IYL},{H|L} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill18.asm`" "LD {IYH|IYL},{H|L} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill19.asm`" "LD {IYH|IYL},{IXH|IXL} is not supported."
	@./check "`../vgsasm m_ld_oso8_ill20.asm`" "LD {IYH|IYL},{IXH|IXL} is not supported."
	${MAK} normal_case src=m_ld_oso16_bc
	${MAK} normal_case src=m_ld_oso16_de
	${MAK} normal_case src=m_ld_oso16_hl
	${MAK} normal_case src=m_ld_oso16_ix
	${MAK} normal_case src=m_ld_oso16_iy
	${MAK} normal_case src=m_ld_oso16_sp
	@./check "`../vgsasm m_ld_oso16_bc_ill.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_oso16_de_ill.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_oso16_hl_ill.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_oso16_ix_ill.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_oso16_iy_ill.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_oso16_sp_ill.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_oso16_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_osaoa_hl
	@./check "`../vgsasm m_ld_osaoa_hl_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_aoaso_hl
	@./check "`../vgsasm m_ld_aoaso_hl_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_a_bc
	${MAK} normal_case src=m_ld_a_de
	${MAK} normal_case src=m_ld_bc_a
	${MAK} normal_case src=m_ld_de_a
	${MAK} normal_case src=m_ld_osn8
	@./check "`../vgsasm m_ld_osn8_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_osn8_range2.asm`" "Numerical range incorrect: 256"
	${MAK} normal_case src=m_ld_osn16
	@./check "`../vgsasm m_ld_osn16_range1.asm`" "Numerical range incorrect: -32769"
	@./check "`../vgsasm m_ld_osn16_range2.asm`" "Numerical range incorrect: 65536"
	@./check "`../vgsasm m_ld_osn_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_osl
	@./check "`../vgsasm m_ld_osl_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_osana
	@./check "`../vgsasm m_ld_osana_range1.asm`" "Numerical range incorrect: 65536"
	@./check "`../vgsasm m_ld_osana_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_osala
	@./check "`../vgsasm m_ld_osala_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_anaso
	@./check "`../vgsasm m_ld_anaso_range.asm`" "Numerical range incorrect: 65536"
	@./check "`../vgsasm m_ld_anaso_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_alaso
	@./check "`../vgsasm m_ld_alaso_ill.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_anasn
	@./check "`../vgsasm m_ld_anasn_range1.asm`" "Numerical range incorrect: 65536"
	@./check "`../vgsasm m_ld_anasn_range2.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_anasn_range3.asm`" "Numerical range incorrect: 256"
	${MAK} normal_case src=m_ld_alasn
	@./check "`../vgsasm m_ld_alasn_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_alasn_range2.asm`" "Numerical range incorrect: 256"
	${MAK} normal_case src=m_ld_osaopna8_ix
	${MAK} normal_case src=m_ld_osaopna8_iy
	${MAK} normal_case src=m_ld_osaopna16_ix
	${MAK} normal_case src=m_ld_osaopna16_iy
	@./check "`../vgsasm m_ld_osaopna8_ix_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna8_ix_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna8_ix_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna8_ix_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna8_iy_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna8_iy_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna8_iy_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna8_iy_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna16_ix_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna16_ix_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna16_iy_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna16_iy_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaopna_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_osaopna_range2.asm`" "Numerical range incorrect: 128"
	${MAK} normal_case src=m_ld_osaoa_ix
	${MAK} normal_case src=m_ld_osaoa_iy
	@./check "`../vgsasm m_ld_osaoa_ix_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_ix_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_ix_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_ix_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_ix_ill5.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_iy_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_iy_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_iy_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_iy_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_osaoa_iy_ill5.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_aopnaso_ix
	${MAK} normal_case src=m_ld_aopnaso_iy
	@./check "`../vgsasm m_ld_aopnaso_ix_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_ill5.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_ill6.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_ill7.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_ill8.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill5.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill6.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill7.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_iy_ill8.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aopnaso_ix_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_aopnaso_ix_range2.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_ld_aopnaso_iy_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_aopnaso_iy_range2.asm`" "Numerical range incorrect: 128"
	${MAK} normal_case src=m_ld_aoaso_ix
	${MAK} normal_case src=m_ld_aoaso_iy
	@./check "`../vgsasm m_ld_aoaso_ix_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_ix_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_ix_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_ix_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_ix_ill5.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_ix_ill6.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_ix_ill7.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_ix_ill8.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill1.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill2.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill3.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill4.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill5.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill6.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill7.asm`" "Illegal LD instruction."
	@./check "`../vgsasm m_ld_aoaso_iy_ill8.asm`" "Illegal LD instruction."
	${MAK} normal_case src=m_ld_aopnasn_ix
	${MAK} normal_case src=m_ld_aopnasn_iy
	@./check "`../vgsasm m_ld_aopnasn_ix_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_aopnasn_ix_range2.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_ld_aopnasn_ix_range3.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_aopnasn_ix_range4.asm`" "Numerical range incorrect: 256"
	@./check "`../vgsasm m_ld_aopnasn_iy_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_aopnasn_iy_range2.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_ld_aopnasn_iy_range3.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_aopnasn_iy_range4.asm`" "Numerical range incorrect: 256"
	${MAK} normal_case src=m_ld_aoasn
	@./check "`../vgsasm m_ld_aoasn_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_ld_aoasn_range2.asm`" "Numerical range incorrect: 256"
	@./check "`../vgsasm m_ld_ill.asm`" "Illegal LD instruction."

mnemonic_vgs_tests: check ../vgsasm
	${MAK} normal_case src=m_hage_oso_hl
	${MAK} normal_case src=m_hage_oso_a
	${MAK} normal_case src=m_hage_oso_b
	${MAK} normal_case src=m_hage_oso_c
	${MAK} normal_case src=m_hage_oso_d
	${MAK} normal_case src=m_hage_oso_e
	${MAK} normal_case src=m_hage_oso_h
	${MAK} normal_case src=m_hage_oso_l
	@./check "`../vgsasm m_hage_oso_hl_ill.asm`" "Illegal MUL instruction."
	@./check "`../vgsasm m_hage_oso_a_ill.asm`" "Illegal MUL instruction."
	@./check "`../vgsasm m_hage_oso_b_ill.asm`" "Illegal MULS instruction."
	@./check "`../vgsasm m_hage_oso_c_ill.asm`" "Illegal DIV instruction."
	@./check "`../vgsasm m_hage_oso_d_ill.asm`" "Illegal DIVS instruction."
	@./check "`../vgsasm m_hage_oso_e_ill.asm`" "Illegal MOD instruction."
	@./check "`../vgsasm m_hage_oso_h_ill.asm`" "Illegal MUL instruction."
	@./check "`../vgsasm m_hage_oso_l_ill.asm`" "Illegal MULS instruction."
	@./check "`../vgsasm m_hage_oso_ill.asm`" "Illegal MUL instruction."
	${MAK} normal_case src=m_hage_osn
	@./check "`../vgsasm m_hage_osn_mod.asm`" "MOD cannot operate on negative numbers."
	@./check "`../vgsasm m_hage_osn_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_hage_osn_range2.asm`" "Numerical range incorrect: 256"
	@./check "`../vgsasm m_hage_osn_ill.asm`" "Illegal MUL instruction."
	${MAK} normal_case src=m_atn2_oso
	@./check "`../vgsasm m_atn2_oso_ill.asm`" "Illegal ATN2 instruction."
	@./check "`../vgsasm m_atn2_oso_ill2.asm`" "Illegal ATN2 instruction."
	@./check "`../vgsasm m_atn2_ill.asm`" "Illegal ATN2 instruction."
	${MAK} normal_case src=m_sin_o
	@./check "`../vgsasm m_sin_o_ill.asm`" "Illegal SIN instruction."
	${MAK} normal_case src=m_sin_oso
	@./check "`../vgsasm m_sin_oso_ill1.asm`" "Illegal SIN instruction."
	@./check "`../vgsasm m_sin_oso_ill2.asm`" "Illegal SIN instruction."
	@./check "`../vgsasm m_sin_ill.asm`" "Illegal SIN instruction."
	${MAK} normal_case src=m_cos_o
	@./check "`../vgsasm m_cos_o_ill.asm`" "Illegal COS instruction."
	${MAK} normal_case src=m_cos_oso
	@./check "`../vgsasm m_cos_oso_ill1.asm`" "Illegal COS instruction."
	@./check "`../vgsasm m_cos_oso_ill2.asm`" "Illegal COS instruction."
	@./check "`../vgsasm m_cos_ill.asm`" "Illegal COS instruction."

mnemonic_sub_tests: check ../vgsasm
	${MAK} normal_case src=m_call_n
	@./check "`../vgsasm m_call_n_range.asm`" "Numerical range incorrect: 65536"
	${MAK} normal_case src=m_call_l
	${MAK} normal_case src=m_call_osn
	@./check "`../vgsasm m_call_osn_ill.asm`" "Invalid condition: A"
	@./check "`../vgsasm m_call_osn_range.asm`" "Numerical range incorrect: 65536"
	${MAK} normal_case src=m_call_osl
	@./check "`../vgsasm m_call_ill.asm`" "Illegal CALL instruction."

mnemonic_stack_tests: check ../vgsasm
	${MAK} normal_case src=m_push_o
	@./check "`../vgsasm m_push_o_ill.asm`" "Unsupported PUSH operand: A"
	@./check "`../vgsasm m_push_ill.asm`" "Excessive or insufficient number of operands."
	${MAK} normal_case src=m_pop_o
	@./check "`../vgsasm m_pop_o_ill.asm`" "Unsupported POP operand: SP"
	@./check "`../vgsasm m_pop_ill.asm`" "Excessive or insufficient number of operands."

mnemonic_shift_tests: check ../vgsasm
	${MAK} normal_case src=m_shift_o
	@./check "`../vgsasm m_shift_o_ill.asm`" "Illegal shift/rotate instruction."
	${MAK} normal_case src=m_shift_osn
	${MAK} normal_case src=m_shift_aoa
	@./check "`../vgsasm m_shift_aoa_ill.asm`" "Illegal shift/rotate instruction."
	${MAK} normal_case src=m_shift_aoasn
	${MAK} normal_case src=m_shift_aopna
	@./check "`../vgsasm m_shift_aopna_range1.asm`" "Numerical range incorrect: -129"
	@./check "`../vgsasm m_shift_aopna_range2.asm`" "Numerical range incorrect: 128"
	@./check "`../vgsasm m_shift_aopna_ill.asm`" "Illegal shift/rotate instruction."
	${MAK} normal_case src=m_shift_aopna_o
	@./check "`../vgsasm m_shift_ill.asm`" "Illegal shift/rotate instruction."

mnemonic_rst_tests: check ../vgsasm
	${MAK} normal_case src=m_rst
	@./check "`../vgsasm m_rst_ill.asm`" "Illegal RST instruction."

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
	@./check "`../vgsasm m_calc8_os_ill.asm`" "Illegal arithmetic instruction."
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
	@./check "`../vgsasm m_calc_ill1.asm`" "Illegal 8-bit arithmetic instruction."
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
	@./check "`../vgsasm struct_array_nd.asm`" "Illegal LD instruction."
	@./check "`../vgsasm struct_field_nf.asm`" "Unknown field name in structure HOGE: VAR2"
	${MAK} normal_case src=struct_normal

normal_case:
	@echo N: ${src}
	../vgsasm ${src}.asm -v
	diff ${src}.bin ${src}.exp
	@#z88dk-dis ${src}.exp > ${src}.dis
