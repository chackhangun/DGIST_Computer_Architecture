#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
/* //////////////////////////////////////what todo


array

array2 처리


main이 시작되었을때
print_output 함수 정리하기 /done;

lb와 같이 offset 있는 명령어들 extract_number 함수 처리
la를 위한 함





///////////////////////////////////// */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////format 클래스 파일
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        std::string type_name = "R";
        int rs, rt, rd, shamt, funct;

        R_format() {
            rs = rt = rd = shamt = funct = 0;
        }
        R_format(std::string name, int op, int funct_num){
            rs = rt = rd = shamt = funct = 0;
            instruction_name = name;
            set_opcode(op);
            funct = funct_num;
        }
        virtual ~R_format(){};

};

class I_format: public Myformats{
    public:
        I_format() {
            rs = rt = immediate_or_address = 0;
        }
        I_format(std::string name, int op_num){
            rs = rt = immediate_or_address = 0;
            instruction_name = name;
            set_opcode(op_num);
        }
        virtual ~I_format(){};

        std::string type_name = "I";
        int rs, rt, immediate_or_address;
};

class J_format: public Myformats{
    public:
        J_format() {
            jump_target = 0;
        }
        J_format(std::string name, int op_num){
            jump_target = 0;
            instruction_name = name;
            set_opcode(op_num);
        }
        virtual ~J_format(){};

        std::string type_name = "J";
        int jump_target;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////함수파일
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int num_extract(std::string str){
    int first_num_idx = 0;
    int last_num_idx = str.length() - 1;

    if(str[0] == '$'){
        first_num_idx = 1;
    }
    if(str[last_num_idx] == ','){
        last_num_idx--;
    }

    if(first_num_idx == 1 && last_num_idx == 1){
        char one_char = str[1];
        int num = one_char - 48;
        return num;
    }

    else{
        int num_char = last_num_idx - first_num_idx + 1;
        char* extract_number = new char[num_char];
        for(int i = 0; i != num_char; i++){
            extract_number[i] = str[first_num_idx+i];
        }
        std::string extract_num = extract_number;

        delete[] extract_number;
        //////////////16진수일때
        if(extract_num[0] == '0' && extract_num[1] == 'x'){
            unsigned char hex_num = 0;
            for(int i = 0; extract_num[i] != 0; i++){
                if(extract_num[i] >= 'a' && extract_num[i] <= 'f'){
                    hex_num = hex_num * 16 + extract_num[i] - 'a' + 10;
                }
                else if(extract_num[i] >= '0' && extract_num[i] <= '9'){
                    hex_num = hex_num * 16 + extract_num[i] - '0';
                }
            }
            return hex_num;
        }
        //////////////////////////////////
        int num = stoi(extract_num);
        return num;
    }

}


void print_R_format(std::vector<R_format>::iterator iter, std::vector<std::string> container){
    int code_num = iter->opcode;
    if(iter->instruction_name == "jr"){ // jr인 경우
        iter->rs = num_extract(container[1]);
    }

    else{
        if(iter->instruction_name == "sll" | iter->instruction_name == "srl"){
            iter->rd = num_extract(container[1]);
            iter->rt = num_extract(container[2]);
            iter->shamt = num_extract(container[3]);
        }
        else{
            iter->rd = num_extract(container[1]);
            iter->rs = num_extract(container[2]);
            iter->rt = num_extract(container[3]);
        }
    }
    std::cout << std::bitset<6>(code_num) << " " << (iter)->rs << " " << (iter)->rt << " " << (iter)->rd << " " << (iter)->shamt << " " << (iter)->funct <<std::endl;
}

void print_I_format(std::vector<I_format>::iterator iter, std::vector<std::string> container){
    int code_num = iter->opcode;
    if(iter->instruction_name == "addiu" | iter->instruction_name == "andi" | iter->instruction_name == "ori" | iter->instruction_name == "sltiu"){
        iter->rt  = num_extract(container[1]);
        iter->rs = num_extract(container[2]);
        iter->immediate_or_address  = num_extract(container[3]);
    }

    else{
        if(iter->instruction_name == "lui"){
            iter->rt = num_extract(container[1]);
            iter->immediate_or_address = num_extract(container[2]);
        }

        if(iter->instruction_name == "beq" | iter->instruction_name == "bne"){
            iter->rs = num_extract(container[1]);
            iter->rt = num_extract(container[2]);
            iter->immediate_or_address = num_extract(container[3]);
        }

        else{ ////////////////lw,lb,sw, sb인 경우
            iter->rt = num_extract(container[1]);
            iter->immediate_or_address = num_extract(container[2]);
            std::cout << "immediate or address = " << iter->immediate_or_address << std::endl; ///////////////////////////for test
        }
    }
    std::cout << std::bitset<6>(code_num) << " " << (iter)->rs << " " << (iter)->rt << " " << (iter)->immediate_or_address << std::endl;
}

void print_J_format(std::vector<J_format>::iterator iter, std::vector<std::string> container){
    int code_num = iter->opcode;
    iter->jump_target = num_extract(container[1]);
    std::cout << std::bitset<6>(code_num) << " " << (iter)->jump_target << std::endl;
}

int print_output(std::vector<R_format> R, std::vector<I_format> I,std::vector<J_format> J, std::vector<std::string> container){
    for(auto iter = R.begin(); iter != R.end(); iter++){
        if(iter->instruction_name ==  container[0]){
            print_R_format(iter, container);
            return 1;
        }
    }
    for(auto iter = I.begin(); iter != I.end(); iter++){
        if(iter->instruction_name ==  container[0]){
            print_I_format(iter, container);
            return 2;
        }
    }

    for(auto iter = J.begin(); iter != J.end(); iter++){
        if(iter->instruction_name ==  container[0]){
            print_J_format(iter, container);
            return 3;
        }
    }       
}

void mytranslation(std::string file_name, std::vector<R_format> R, std::vector<I_format> I, std::vector<J_format> J){
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
                    ////////////////                                                                  for test
                    for(int i = 0; i < container.size(); i++){
                        std::cout << container[i] << " ";
                    }
                    std::cout << std::endl;//////////////////
                    int program_execution = print_output(R,I,J, container);
                }
            }
        }
    sample_code.close();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
std::vector<Myformats> make_instruction_class(){
    R_format addu("addu",0, 0x21), _and("and", 0, 0x24), jr("jr",0,8), nor("nor",0, 0x27), _or("or", 0, 0x25), sltu("sltu",0, 0x2b), sll("sll",0,0), srl("srl",0,2), subu("subu",0,0x23);
    I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
    J_format j("j", 2), jal("jal", 3);

    std::vector<Myformats> container{addu, _and, jr, nor, _or, sltu, sll, srl, subu, addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb, j, jal};
    return container;
}
*/

////////////////////큰 format class 만들고 세부적인 클래스 만들기
////////////////////std::vector<myformat> container 
int main(){

    R_format addu("addu",0, 0x21), _and("and", 0, 0x24), jr("jr",0,8), nor("nor",0, 0x27), _or("or", 0, 0x25), sltu("sltu",0, 0x2b), sll("sll",0,0), srl("srl",0,2), subu("subu",0,0x23);
    I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
    J_format j("j", 2), jal("jal", 3);

    std::vector<R_format> R{addu, _and, jr, nor, _or, sltu, sll, srl, subu};
    std::vector<I_format> I{addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb};
    std::vector<J_format> J{j, jal};

    mytranslation("sample.s", R,I,J);
}
