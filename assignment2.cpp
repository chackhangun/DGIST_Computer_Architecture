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
//// format 클래스 만들어주기

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
        void operation(std::vector<int> my_register){
            if(insturction_name == "addu"){
                my_register[rd] = my_register[rs] + my_register[rt];
                return;
            }

            if(instruction_name == "and"){         //// rs와 rt의 logical and연산을 rd에 저장
                
            }

            if(instruction_name == 'jr'){
                
            }

            if(instruction_name == 'nor'){
                
            }

            if(instruction_name == 'or'){
                
            }

            if(instruction_name == 'sltu'){
                if(my_register[rs] < my_register[rt]){
                    my_register[rd] = 1;
                }
                else{
                    my_register[rd] = 0;
                }
            }

            if(instruction_name == 'sll'){
                my_register[rd] = my_register[rt] << shamt;
                return;
            }

            if(instruction_name == 'srl'){
                my_register[rd] = my_register[rt] >> shamt;
                return;
            }

            if(instruction_name == 'subu'){
                my_register[rd] = my_register[rs] - my_register[rt];
                return;
            }
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
        void operation(){}

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
        void operation(){}

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

int change_instruction(std::string instruction, int start, int final){
    std::string bit_num;
    for(auto iter = instruction.begin()+start; iter != instruction.begin()+fianl; iter++){
        bit_num.push_back(*iter);
    }
    int int_num = binary_to_int(bit_num);
    return int_num;
}
std::vector<int> change_to_decimal_instruction(std::string instruction){
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction,0,6);
    int int_rs = change_instruction(instruction,6,11);
    int int_rt = change_instruction(instruction,11,16);
    int int_rd = change_instruction(instruction,16,21);
    int int_shamt = change_instruction(instruction,21,26);
    int int_funct = change_instruction(instruction,26,32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(int_rs);
    decimal_instruction.push_back(int_rt);
    decimal_instruction.push_back(int_rd);
    decimal_instruction.push_back(int_shamt);
    decimal_instruction.push_back(int_funct);

    return decimal_instruction;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
int main(){
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

    std::ifstream reading;
    reading.open(object_file);
    if(reading.is_open()){
        int line_num = 0;
        int startline_data_section = 0;
        int txt_section_size;
        int data_section size;
        std::string hex_num;
        int demical_num;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //// 바이너리 파일 읽어오기
        while(!reading.eof()){
            getline(reading, hex_num);
            const char* transform = hex_num.c_str();
            demical_num = strtol(transform, NULL, 16);
            if(line_num == 0){
                txt_section_size = demical_num;
                startline_data_section = 1 + (demical_num / 4) + 1 ;
                line_num++;
                continue;
            }
            if(line_num = 1){
                data_section_size = demical_num;
                line_num++;
                continue;
            }
            else{                    
                std::bitset<32>bit_num(demical_num);
                std::string bit_oneline = bit_num.to_string();

                if(line_num < startline_data_section){
                    instruction_memory.push_back(bit_oneline);
                    line_num++
                    continue;
                }

                else{
                    data_memory.push_back(bit_oneline);
                    line_num++;
                    continue;
                }
            }
        }
        reading.close();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<int> my_register(31,0); ///0-31 reg 초기화

    for(auto iter = instruction_memory.begin(); iter != instruction_memory.end(); iter++){
        std::vector<int> int_instruction = change_to_decimal_instruction(*iter)
        std::string type_checking = NULL;
        std::string ins_name = NULL;

        if(type_checking == NULL){
            for(auto iter2 = R.begin(); iter2 != R.end(); iter2++){
                if(*iter2->opcode == int_instruction[0]){
                    if((*iter2->funct) == int_instruction[5]){
                        type_checking = *iter2->type_name;
                        ins_name = *iter2->instruction_name;
                        *iter2->rs = int_instruction[1];
                        *iter2->rt = int_instruction[2];
                        *iter2->rd = int_instruction[3];
                        *iter2->shamt = int_instruction[4];

                        *iter2->operation(my_register);
                    }
                }
            }
        }

        if(type_checking == NULL){
            for(auto iter3 = I.begin(); iter3 != I.end(); iter3++){
                if(*iter3->opcode == int_opcode){
                    type_checking = *iter3->type_name;
                    ins_name = *iter3->instruction_name;
                    break;
                }
            }
        }

        if(type_checking == NULL){
            for(auto iter4 = J.begin(); iter4 != J.end(); iter4++){
                if(*iter4->opcode == int_opcode){
                    type_checking = *iter4->type_name;
                    ins_name = *iter4->instruction_name;
                    break;
                }
            }
        }




    }


}


