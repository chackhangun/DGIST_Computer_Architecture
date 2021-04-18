#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int binary_to_int(std::string binary_str) {  ///동작 체크 필요
    int a = 1;
    int num = 0;
    for (int i = binary_str.length() - 1; i >= 0; i--) {
        for (int j = 0; j < binary_str.length() - i - 1; j++) {
            a *= 2;
        }
        if (binary_str[i] == '1') {
            num += a;
        }
        a = 1;
    }
    return num;
}

std::string int_to_hex(int num) {   ///16진수 string으로 변환 // 동작 체크필요
    std::stringstream ss;
    std::string str;
    ss << std::hex << num;
    str = ss.str();
    return str;
}

int change_instruction(std::string instruction, int start, int final) {
    std::string bit_num;
    for (auto iter = instruction.begin() + start; iter != instruction.begin() + final; iter++) {
        bit_num.push_back(*iter);
    }
    int int_num = binary_to_int(bit_num);
    return int_num;
}

std::vector<int> change_to_R_format_instruction(std::string instruction) {
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction, 0, 6);
    int int_rs = change_instruction(instruction, 6, 11);
    int int_rt = change_instruction(instruction, 11, 16);
    int int_rd = change_instruction(instruction, 16, 21);
    int int_shamt = change_instruction(instruction, 21, 26);
    int int_funct = change_instruction(instruction, 26, 32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(int_rs);
    decimal_instruction.push_back(int_rt);
    decimal_instruction.push_back(int_rd);
    decimal_instruction.push_back(int_shamt);
    decimal_instruction.push_back(int_funct);
    return decimal_instruction;
}

std::vector<int> change_to_I_format_instruction(std::string instruction) {
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction, 0, 6);
    int int_rs = change_instruction(instruction, 6, 11);
    int int_rt = change_instruction(instruction, 11, 16);
    int int_immediate = change_instruction(instruction, 16, 32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(int_rs);
    decimal_instruction.push_back(int_rt);
    decimal_instruction.push_back(int_immediate);
    return decimal_instruction;
}

std::vector<int> change_to_J_format_instruction(std::string instruction) {
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction, 0, 6);
    int jump_target = change_instruction(instruction, 6, 32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(jump_target);
    return decimal_instruction;
}

int find_address_index_for_branch(std::vector<int> instruction_or_data_address, int address_in_regi) {
    for (auto iter = instruction_or_data_address.begin(); iter != instruction_or_data_address.end(); iter++) {
        if (*iter == address_in_regi) {
            return iter - instruction_or_data_address.begin();
        }
    }
}

std::string value_sign_extension(std::string data, int quantity) {
    std::string num_extension;
    if (data.front() == '0') {
        for (int num = 0; num < quantity; num++) {
            num_extension.push_back('0');
        }
    }
    else {
        for (int num = 0; num < quantity; num++) {
            num_extension.push_back('1');
        }
    }
    return num_extension;
}

std::bitset<32> change_to_bitset(std::string instruction_32bit) {
    std::bitset<32> bitset_instruction = 0;
    for (int n = 0; n < 32; n++) {
        if (instruction_32bit[31 - n] == '1') {
            bitset_instruction.set(n);
        }
    }
    return bitset_instruction;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Myformats {
public:
    Myformats() {
        opcode = 0;
        instruction_name = "none";
    }
    virtual ~Myformats() {};

    int opcode;
    std::string instruction_name;
};

class R_format : public Myformats {
public:
    std::string type_name = "R";
    int rs, rt, rd, shamt, funct;
    void operation(std::vector<std::bitset<32>> my_register, std::vector<int> instruction_address) {                  ////register 상태와 pc table 받아야함.
        int rs_value = int(my_register[rs].to_ullong());
        int rt_value = int(my_register[rt].to_ullong());
        int rd_value = 0;

        if (instruction_name == "addu") {
            rd_value = rs_value + rt_value;
            my_register[rd] = std::bitset<32>(rd_value);
            return;
        }

        if (instruction_name == "and") {
            my_register[rd] = (my_register[rs] & my_register[rt]);
            return;
        }

        if (instruction_name == "nor") {
            my_register[rd] = (~(my_register[rs] | my_register[rt]));
            return;
        }

        if (instruction_name == "or") {
            my_register[rd] = (my_register[rs] | my_register[rt]);
            return;
        }

        if (instruction_name == "sltu") {
            if (rs_value < rt_value) {
                my_register[rd] = 1;
            }
            else {
                my_register[rd] = 0;
            }
            return;
        }

        if (instruction_name == "sll") {
            my_register[rd] = (my_register[rt] << shamt);
            return;
        }

        if (instruction_name == "srl") {
            my_register[rd] = (my_register[rt] >> shamt);
            return;
        }

        if (instruction_name == "subu") {
            rd_value = rs_value - rt_value;
            my_register[rd] = std::bitset<32>(rd_value);
            return;
        }
    }

    int operation_jr(std::vector<std::bitset <32>> my_register, std::vector<int> instruction_address) {///pc_num에서 rs안에 들어있는 주소와 같은 값의 index를 구한다.
        int decimal_num = binary_to_int(my_register[rs].to_string());
        int index = find_address_index_for_branch(instruction_address, decimal_num);
        return index;
    }

    R_format() {
        rs = rt = rd = shamt = funct = 0;
    }
    R_format(std::string name, int op, int funct_num) {
        rs = rt = rd = shamt = funct = 0;
        instruction_name = name;
        opcode = op;
        funct = funct_num;
    }
    virtual ~R_format() {};
};

class I_format : public Myformats {
public:
    std::string type_name = "I";
    int rs, rt, immediate_or_address;
    int operation_beq(std::vector<std::bitset<32>> my_register, std::vector<int> instruction_address, int now_index) { ///분기해야할 pc index찾아주기
        if (my_register[rs].to_string() == my_register[rt].to_string()) {
            int target_address = instruction_address[now_index] + 4 + immediate_or_address * 4;
            int branch_idx = find_address_index_for_branch(instruction_address, target_address);
            return branch_idx;
        }
        else {
            return -1;                    //rs와 rt가 다르면 -1 리턴.
        }
    }

    int operation_bne(std::vector<std::bitset<32>> my_register, std::vector<int> instruction_address, int now_index) { ///분기해야할 pc index찾아주기
        if (my_register[rs].to_string() != my_register[rt].to_string()) {
            int target_address = instruction_address[now_index] + 4 + immediate_or_address * 4;
            int branch_idx = find_address_index_for_branch(instruction_address, target_address);
            return branch_idx;
        }
        else {
            return -1;                    //rs와 rt가 다르면 -1 리턴.
        }
    }

    void operation_lw_or_lb(std::vector<std::bitset<32>> my_register, std::vector<std::string> data_memory, std::vector<int> data_address) {
        int rs_value = int(my_register[rs].to_ullong());
        int address = rs_value + immediate_or_address;
        int address_index = find_address_index_for_branch(data_address, address);
        if (instruction_name == "lw") {
            my_register[rt] = binary_to_int(data_memory[address_index]);                           ////std::vector<std::bitset<32> my_register[0] = 15; 하면 2진수로 들어가는가 체크체크하기
        }
        else {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++) {                                                              
                one_byte_data.push_back(data_memory[address_index][n]);
            }
            std::string num_extension = value_sign_extension(one_byte_data, 24);
            my_register[rt] = change_to_bitset(num_extension);
        }
    }

    void operation_sw_or_sb(std::vector<std::bitset<32>> my_register, std::vector<std::string> data_memory, std::vector<int> data_address) {
        int rt_value = int(my_register[rt].to_ullong());
        int rs_value = int(my_register[rs].to_ullong());
        int address = rs_value + immediate_or_address;
        int address_index = find_address_index_for_branch(data_address, address);
        if (instruction_name == "sw") {
            data_memory[address_index] = std::bitset<32>(rt_value).to_string();
        }
        else {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++) {
                one_byte_data.push_back(data_memory[address_index][n]);
            }
            data_memory[address_index] = value_sign_extension(one_byte_data, 24);
        }
    }

    void operation(std::vector<std::bitset<32>> my_register) {
        int rs_value = int(my_register[rs].to_ullong());
        int rt_value = int(my_register[rt].to_ullong());

        if (instruction_name == "addiu") { ///////rs와 sign-extened된 immediate의 합을 rt에 저장.
            rt_value = rs_value + immediate_or_address;
            my_register[rt] = std::bitset<32>(rt_value);
            return;
        }

        if (instruction_name == "andi") {
            my_register[rt] = my_register[rs] & std::bitset<32>(immediate_or_address);
            return;
        }
        

        if (instruction_name == "lui") {
            std::bitset<16> imm_16bit(immediate_or_address);
            for (int idx = 0; idx < 32; idx++) {
                if (idx < 16) {
                    my_register[rt][idx] = 0;
                }
                else {
                    my_register[rt][idx] = imm_16bit[idx - 16];
                }
            }
            return;
        }

        if (instruction_name == "ori") {
            my_register[rt] = my_register[rs] | std::bitset<32> (immediate_or_address);
            return;
        }

        if (instruction_name == "sltiu") {                       ///rs가 sign_extened imm보다 작으면 rt 1, 아니면 0
            if (rs < immediate_or_address) {
                my_register[rt] = 1;
                return;
            }
            else {
                my_register[rt] = 0;
                return;
            }
        }
    }

    I_format() {
        rs = rt = immediate_or_address = 0;
    }
    I_format(std::string name, int op_num) {
        rs = rt = immediate_or_address = 0;
        instruction_name = name;
        opcode = op_num;
    }
    virtual ~I_format() {};
};

class J_format : public Myformats {
public:
    std::string type_name = "J";
    int jump_target;
    std::string j_operation(std::vector<int> instruction_address, int now_index) {
        jump_target = jump_target * 4;

        std::bitset<28> jump_target_28bit(jump_target);
        std::string str_jump_target_28bit = jump_target_28bit.to_string();

        std::bitset<32> pc_32bit(instruction_address[now_index]);
        std::string str_pc_32bit = pc_32bit.to_string();

        std::string upper_pc_4bit;
        for (int i = 0; i < 4; i++) {
            upper_pc_4bit.push_back(str_pc_32bit[i]);
        }

        std::string jump_target_32bit = upper_pc_4bit + str_jump_target_28bit;
    }

    J_format() {
        jump_target = 0;
    }
    J_format(std::string name, int op_num) {
        jump_target = 0;
        instruction_name = name;
        opcode = op_num;
    }
    virtual ~J_format() {};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {

    int start_instruction_address = 0x400000;
    std::vector<int> instruction_address;
    int start_data_address = 0x10000000;
    std::vector<int> data_address;
    ///// pc 설정하기
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    R_format addu("addu", 0, 0x21), _and("and", 0, 0x24), jr("jr", 0, 8), nor("nor", 0, 0x27), _or("or", 0, 0x25), sltu("sltu", 0, 0x2b), sll("sll", 0, 0), srl("srl", 0, 2), subu("subu", 0, 0x23);
    I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
    J_format j("j", 2), jal("jal", 3);

    std::vector<R_format> R{ addu, _and, jr, nor, _or, sltu, sll, srl, subu };
    std::vector<I_format> I{ addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb };
    std::vector<J_format> J{ j, jal };


    std::vector<std::string> instruction_memory;
    std::vector<std::string> data_memory;
    int txt_section_size;
    int data_section_size;

    std::ifstream reading;
    reading.open("sample.o");
    if (reading.is_open()) {
        int line_num = 0;
        int end_line_instruction_memory = 0;
        std::string hex_num;
        int demical_num;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //// 바이너리 파일 읽어오기
        while (!reading.eof()) {
            getline(reading, hex_num);
            const char* transform = hex_num.c_str();
            demical_num = strtol(transform, NULL, 16);                           ////16진수 10진수로 바꾸기
            if (line_num == 0) {                                                   ////text section size 설정
                txt_section_size = demical_num;
                ///std::cout << txt_section_size << std::endl;
                end_line_instruction_memory = 2 + (txt_section_size / 4);                 /// input file에서 txt size, data size를 포함한 line number
                line_num++;
                continue;
            }
            if (line_num == 1) {                                                    ////data section size 설정
                data_section_size = demical_num;
                line_num++;
                continue;
            }
            else {
                std::bitset<32>bit_num(demical_num);                            //////32비트로 나타내기
                std::string bit_oneline = bit_num.to_string();                  /////bitset을 string으로.
                if (line_num < end_line_instruction_memory) {
                    instruction_memory.push_back(bit_oneline);                  /////instruction memory 설정                        
                    instruction_address.push_back(start_instruction_address + 4 * (line_num - 2));               /////pc 설정 (10진수)
                    line_num++;
                    continue;
                }

                else {
                    data_memory.push_back(bit_oneline);                         /////data memory 설정
                    data_address.push_back(start_data_address + 4 * (line_num - 2));                   ///data memory address 설정
                    line_num++;
                    continue;
                }
            }
        }
        ////pc_num과 ins_memory의 사이즈는 동일해야한다.
        reading.close();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    std::vector<std::bitset<32>> my_register(32, 0); ///0-31 reg 초기화
    int instruction_memory_index = 0;
    int data_address_index = 0;

    while (instruction_memory_index < instruction_memory.size()) {
        std::string type_checking = "none";
        if (type_checking == "none") {
            ////R format일 때 동작
            for (auto r = R.begin(); r != R.end(); r++) {
                std::vector<int> int_instruction = change_to_R_format_instruction(instruction_memory[instruction_memory_index]);         ///32비트 2진수 instruction을 r format 형태의 10진수 vector instruction으로 바꾸기.
                if (r->opcode == int_instruction[0] && (r->funct) == int_instruction[5]) {
                    type_checking = r->type_name;
                    r->rs = int_instruction[1];
                    r->rt = int_instruction[2];
                    r->rd = int_instruction[3];
                    r->shamt = int_instruction[4];
                    if (r->instruction_name == "jr") {
                        int index = r->operation_jr(my_register, instruction_address);
                        instruction_memory_index = index - 1;
                        break;
                            ///pc_num에서 rs와 같은ㅇ 값을 찾고 index만큼 iter을 instruction_memory.begin() + index
                    }
                    else {
                        r->operation(my_register, instruction_address);
                        break;
                    }
                }
            }
        }
        instruction_memory_index++;

        if (type_checking == "none") {
            for (auto i = I.begin(); i != I.end(); i++) {
                std::vector<int> int_instruction = change_to_I_format_instruction(instruction_memory[instruction_memory_index]);       ///32비트 2진수 instruction을 I format 형태의 10진수 vector instruction으로 바꾸기.
                if (i->opcode == int_instruction[0]) {
                    type_checking = i->type_name;
                    i->rs = int_instruction[1];
                    i->rt = int_instruction[2];
                    i->immediate_or_address = int_instruction[3];
                    if (i->instruction_name == "beq") {
                        i->operation_beq(my_register, instruction_address, )
                    }
                    if (i->instruction_name == "bne") {
                        i->operation_bne(my_register, instruction_address);
                    }
                    if (i->instruction_name == "lw" | i->instruction_name == "lb") {
                        i->operation_lw_or_lb(my_register, data_memory, data_address);
                    }
                    if (i->instruction_name == "sw" | i->instruction_name == "sb") {
                        i->operation_sw_or_sb(my_register, data_memory, data_address);
                    }
                    else {
                        i->operation(my_register);
                    }
                }
            }
        }

        if (type_checking == "none") {
            for (auto j = J.begin(); j != J.end(); j++) {
                std::vector<int> int_instruction = change_to_J_format_instruction(instruction_memory[instruction_memory_index]);        ///32비트 2진수 instruction을 J format 형태의 10진수 vector instruction으로 바꾸기.
                if (j->opcode == int_instruction[0]) {
                    type_checking = j->type_name;
                    j->jump_target = int_instruction[1];
                    if (j->instruction_name == "jal") {
                        my_register[31] = instruction_address[instruction_memory_index++];                                               ///ra에 다음 실행할 ins의 주소를 저장.
                    }

                    std::string jump_target_32bit = j->j_operation(instruction_address, instruction_memory_index);                       ////28비트의 jump_target과 instruction_or_data_address[instruction_memory_index]의 상위 4비트랑 합침
                    int jump_target = binary_to_int(jump_target_32bit);                                                     ////32비트 2진수를 int로 바꾸고
                    instruction_memory_index = find_address_index_for_branch(instruction_address, jump_target) - 1;                        ////pc_num에서 해당 target의 index를 찾고 다음 while문에서 실행될수있도록 1을 빼준다
                    break;
                }
            }
        }

    }

}
