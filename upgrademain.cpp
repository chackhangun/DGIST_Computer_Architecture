#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <utility>
#include <stdlib.h> ///for strtol
/* //////////////////////////////////////what todo

메모리 구현
data segment, text segment 시작주소 잡기                                   --------------complete
한줄한줄 읽을때마다 data segment에 저장하기. 4바이트                          --------------complete
main이 시작되었을때 txt segment에 저장하기
print_output 함수 정리하기                                                  -------------complete


sum:
sum_exit:
exit: 가 시작되었을때                                                  --------------complete

lb와 같이 offset 있는 명령어들 extract_number 함수 처리
la를 위한 함수
연산자들 조건

숫자들 한줄로 이어서 string으로 만들고 난 후 int로 바꾸고 16진수로 바꾸기   ----------------complete

한번 다 읽은 다음에 파싱해야함.                                           ----------------complete



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

std::vector<std::vector<std::string>>& read_once (std::ifstream& sample_code,std::vector<std::vector<std::string>>& parsing_table){
    if(sample_code.is_open()){
        while(!sample_code.eof()){
            std::string oneline;
            getline(sample_code, oneline);
            std::istringstream iss(oneline);

            std::vector<std::string> container;
            container.clear();
            std::string sub;
            while(iss >> sub){
                container.push_back(sub);
            }
            parsing_table.push_back(container);
        }
        sample_code.close();
    }

    if(parsing_table.back().size() == 0){ /////////////s파일 맨마지막 줄이 비었을 때
        parsing_table.pop_back();
    }

    return parsing_table;
}

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
            const char* transform = extract_num.c_str();
            return strtol(transform, NULL, 16);
        }
        int num = stoi(extract_num);
        return num;
    }

}

std::string calculate_R_format(std::vector<R_format>::iterator iter, std::vector<std::string> container){
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
    std::cout << std::bitset<6>(code_num) << " " << std::bitset<5>((iter)->rs) << " " << std::bitset<5>((iter)->rt) << " " << std::bitset<5>((iter)->rd) << " " << std::bitset<5>((iter)->shamt) << " " << std::bitset<6>((iter)->funct) << std::endl;

    std::bitset<6>bit_op(code_num);
    std::bitset<5>bit_rs((iter)->rs); 
    std::bitset<5>bit_rt((iter)->rt);
    std::bitset<5>bit_rd((iter)->rd); 
    std::bitset<5>bit_shamt((iter)->shamt); 
    std::bitset<6>bit_funct((iter)->funct);

    std::string bit_instruction = bit_op.to_string() + bit_rs.to_string() + bit_rt.to_string() + bit_rd.to_string() + bit_shamt.to_string() + bit_funct.to_string();
    std::cout << bit_instruction << std::endl;
    return bit_instruction;

}

std::string calculate_I_format(std::vector<I_format>::iterator iter, std::vector<std::string> container){
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
    std::cout << std::bitset<6>(code_num) << " " << std::bitset<5>((iter)->rs) << " " << std::bitset<5>((iter)->rt) << " " << std::bitset<16>((iter)->immediate_or_address) << std::endl;
    std::bitset<6>bit_code_num(code_num); 
    std::bitset<5>bit_rs((iter)->rs);
    std::bitset<5>bit_rt((iter)->rt); 
    std::bitset<16>bit_immediate((iter)->immediate_or_address); 
    
    std::string bit_instruction = bit_code_num.to_string() + bit_rs.to_string() + bit_rt.to_string() + bit_immediate.to_string();
    std::cout << bit_instruction << std::endl;
    return bit_instruction; //std::stoll(bit_instruction);

}

std::string calculate_J_format(std::vector<J_format>::iterator iter, std::vector<std::string> container){
    int code_num = iter->opcode;
    iter->jump_target = num_extract(container[1]);
    std::cout << code_num << " " << (iter)->jump_target << std::endl; 
    std::bitset<6>bit_code_num(code_num);
    std::bitset<26>bit_jump_target((iter)->jump_target); 

    std::string bit_instruction = bit_code_num.to_string() + bit_jump_target.to_string();
    std::cout << bit_instruction << std::endl;
    return bit_instruction; //std::stoll(bit_instruction);
     
}

std::string print_output(std::vector<R_format> R, std::vector<I_format> I,std::vector<J_format> J, std::vector<std::string> container){
    for(auto iter = R.begin(); iter != R.end(); iter++){
        if(iter->instruction_name ==  container[0]){
            return calculate_R_format(iter, container);
        }
    }
    for(auto iter = I.begin(); iter != I.end(); iter++){
        if(iter->instruction_name ==  container[0]){
            return calculate_I_format(iter, container);
        }
    }

    for(auto iter = J.begin(); iter != J.end(); iter++){
        if(iter->instruction_name ==  container[0]){
            return calculate_J_format(iter, container);
        }
    }       
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// instruction들의 초기설정 
    R_format addu("addu",0, 0x21), _and("and", 0, 0x24), jr("jr",0,8), nor("nor",0, 0x27), _or("or", 0, 0x25), sltu("sltu",0, 0x2b), sll("sll",0,0), srl("srl",0,2), subu("subu",0,0x23);
    I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
    J_format j("j", 2), jal("jal", 3);

    std::vector<R_format> R{addu, _and, jr, nor, _or, sltu, sll, srl, subu};
    std::vector<I_format> I{addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb};
    std::vector<J_format> J{j, jal};

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 메모리 초기설정
    int mem_starting_add_txt = 0x00400000;
    int mem_starting_add_data = 0x10000000;
    int mem_end_stack = 0x7ffffffc;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// parsing
    std::ifstream sample_code;
    sample_code.open("sample.s");

    std::vector<std::vector<std::string>> parsing_table;
    parsing_table = read_once(sample_code, parsing_table);

    int number_of_instruction_line = 0;
    int number_of_data_line = 0;

    int mem_now_add_txt = 0x00400000;
    int mem_now_add_data = 0x10000000;

    bool data = false;
    bool text = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// data memory 설정
    std::vector<std::vector<int>> data_array;
    std::vector<std::vector<std::vector<int>>> data_memory; 
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// text memeory 설정
    std::vector<std::pair<int, std::string>> txt_array;
    std::vector<std::vector<std::pair<int, std::string>>> txt_memory;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for(auto iter = parsing_table.begin(); iter != parsing_table.end(); iter++){
        std::vector<std::string> container = (*iter);

        if (container.size() == 0){
            continue;
        }

        std::string &first_word = container[0];

        if (first_word == ".data"){
            data = true;
            continue;
        }
        if (first_word == ".text"){
            text = true;
            data_memory.push_back(data_array); // "".text"가 나오는 순간 data영역이 끝나기 때문에 바로 아래의 if문에서 만들어진 마지막 data_array를 data_memory에 저장해주어야함
            data_array.clear();
            continue;
        }

        if (data == true && text == false){ //data segment
            number_of_data_line++;
            mem_now_add_data = mem_starting_add_data - 4 * (number_of_data_line - 1);

            if (first_word.back() == ':'){
                if (data_array.size() != 0)
                {
                    data_memory.push_back(data_array);
                    data_array.clear();
                }
                std::vector<int> variable = {mem_now_add_data, num_extract(container[2])}; /////////////////////
                data_array.push_back(variable);
                /*
                    std::cout << "container[1] Memory address = " << variable[0];
                    std::cout << "container[2], value = " << num_extract(container[2]) << "," << variable[1] << std::endl;
                    std::cout << variable.back() << std::endl; // for test
                    */
            }

            if (first_word == ".word"){
                std::vector<int> variable = {mem_now_add_data, num_extract(container[1])};
                data_array.push_back(variable);
                /*
                    std::cout << "container[0] Memory address = " << variable[0];
                    std::cout << "container[1], value = " << num_extract(container[1]) << "," << variable[1] << std::endl;
                    std::cout << variable.back() << std::endl; /// for test;
                    */
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// main: 이 시작
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (data == true && text == true){ //instruction
            if(first_word.back() == ':'){
                if(txt_array.size() != 0){
                    txt_memory.push_back(txt_array);
                    txt_array.clear();
                }
                
                if(first_word == "exit:"){
                    break;
                }

                continue;
            }

            else{ ////////////////////////////////////////////////////////2진수 string으로 만들고 다시 int로 만들어준 후 array에 추가.
                std::string bit_to_int_instruction;

                number_of_instruction_line++;
                mem_now_add_txt = mem_starting_add_txt - 4 * (number_of_instruction_line - 1);


                if(first_word == "la"){
                    std::string find_op = container[2];
                    int number = 0;

                    for(int n = 0; n < parsing_table.size(); n++ ){
                        if(parsing_table[n][0].back() == ':'){
                            number++;
                        }
                        if(parsing_table[n][0] == find_op + ':'){
                            break;
                        }
                    }
                    int address_num_la = data_memory[number-1][0][0];

                    std::vector<std::string> new_container = {"lui", container[1], std::to_string(address_num_la)}; ////////////////address_num_la를 16진수로 바꾸고 4자리씩 비교해야함.
                    bit_to_int_instruction = print_output(R,I,J, new_container);
                    std::pair<int, std::string> value = std::make_pair(mem_now_add_txt, bit_to_int_instruction);
                    txt_array.push_back(value);
                    std::cout << "memory address = " << value.first << "bit instruction = " << value.second << std::endl;
                    /*
                    const char* checking_ori = std::to_string(address_num_la).c_str();
                    strtol(checking_ori, NULL, 16);
                    */

                    if(address_num_la % 16 != 0){ ///ori 계산
                        number_of_instruction_line++;
                        mem_now_add_txt = mem_starting_add_txt - 4 * (number_of_instruction_line - 1);


                    }
                }
                else{
                    std::string bit_to_int_instruction = print_output(R, I, J, container);
                    std::pair<int, std::string> value = std::make_pair(mem_now_add_txt, bit_to_int_instruction);
                    txt_array.push_back(value);
                    std::cout << "memory address = " << value.first << "bit instruction = " << value.second << std::endl;
                }
            }
            
            
            ////////////////                                                                  for test
            for (int i = 0; i < container.size(); i++)
            {
                std::cout << container[i] << " ";
            }
            std::cout << std::endl; //////////////////
            
        }
    }

    /*
    for(int i = 0; i != parsing_table.size(); i++){
        for(int n = 0; n!= parsing_table[i].size(); n++){
            std::cout << parsing_table[i][n] << " " ;
        }
        std::cout<< std::endl;
    }
    */



   
    std::cout << "number of data line = " << number_of_data_line << std::endl;
    std::cout << "number of instruction line = " << number_of_instruction_line << std::endl;
     /*
    std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
    for(auto i = data_memory.begin(); i != data_memory.end(); i++){
        std::cout << (*i).front().back() << std::endl;
    }
    */
    
    return 0 ;
}
