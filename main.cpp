#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
/////////////////////////////////////////////////////////format 클래스 파일
class Myformats{
    public:
        Myformats(){
            opcode = 0;
            instruction_name = "none";
        }
        virtual ~Myformats(){}; 

        int opcode;
        std::string instruction_name;

        void set_opcode(int op){
            opcode = op;
        }

};

class R_format: public Myformats{
    public:
        R_format() {
            rs = rt = rd = shamt = funct = 0;
        }
        R_format(std::string name, int op, int funct_num){
            instruction_name = name;
            set_opcode(op);
            funct = funct_num;
        }
        virtual ~R_format(){};

        std::string type_name = "R";
        int rs, rt, rd, shamt, funct;
};

class I_format: public Myformats{
    public:
        I_format() {
            rs = rt = immediate = 0;
        }
        I_format(std::string name, int op_num){
            instruction_name = name;
            set_opcode(op_num);
        }
        virtual ~I_format(){};

        std::string type_name = "I";
        int rs, rt, immediate;
};

class J_format: public Myformats{
    public:
        J_format() {
            jump_target = 0;
        }
        J_format(std::string name, int op_num){
            instruction_name = name;
            set_opcode(op_num);
        }
        virtual ~J_format(){};

        std::string type_name = "J";
        int jump_target;
};

/////////////////////////////////////////////////////함수파일

void print_opcode(std::vector<Myformats> instructions, std::vector<std::string> container){
    for(auto iter = instructions.begin(); iter != instructions.end(); iter++){
        if(iter->instruction_name ==  container[0]){
            int code_num = iter->opcode;


            std::cout << std::bitset<5>(code_num) << std::endl;
            break;
            }
    }   
}

void mytranslation(std::string file_name, std::vector<Myformats> instructions){
    std::ifstream sample_code;
    sample_code.open(file_name);

    int main_line = 0;

    if(sample_code.is_open()){
        while(!sample_code.eof()){
            std::string oneline;
            getline(sample_code, oneline);

            if(oneline.length() == 0){
                break;
            }

            else{
                std::istringstream iss(oneline);
                std::vector<std::string> container;
                container.clear();

                std::string sub;
            
                while(iss >> sub)
                {
                    container.push_back(sub);
                }

                if(sub == "main:"){
                    main_line = 1;
                    continue;
                }

                if(main_line == 1){
                    print_opcode(instructions, container);
                }
            }
        }
    sample_code.close();
    }
}

std::vector<Myformats> make_instruction_class(){
    R_format addu("addu",0, 0x21), _and("and", 0, 0x24), jr("jr",0,8), nor("nor",0, 0x27), _or("or", 0, 0x25), sltu("sltu",0, 0x2b), sll("sll",0,0), srl("srl",0,2), subu("subu",0,0x23);
    I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
    J_format j("j", 2), jal("jal", 3);

    std::vector<Myformats> container{addu, _and, jr, nor, _or, sltu, sll, srl, subu, addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb, j, jal};
    return container;
}


////////////////////큰 format class 만들고 세부적인 클래스 만들기
////////////////////std::vector<myformat> container 
int main(){
    std::vector<Myformats> all_instruction = make_instruction_class();
    mytranslation("sample.s", all_instruction);
}
