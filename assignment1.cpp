#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <utility>
#include <stdlib.h> ///for strtol
/* //////////////////////////////////////what todo

메모리 구현                                                                  ---------------------complete
data segment, text segment 시작주소 잡기                                   --------------complete
한줄한줄 읽을때마다 data segment에 저장하기. 4바이트                          --------------complete
main이 시작되었을때 txt segment에 저장하기
print_output 함수 정리하기                                                  -------------complete


sum:
sum_exit:
exit: 가 시작되었을때                                                  --------------complete

lb와 같이 offset 있는 명령어들 extract_number 함수 처리                     ----------------complete
la                                                                       ----------------complete
연산자들 조건

숫자들 한줄로 이어서 string으로 만들고 난 후 int로 바꾸고 16진수로 바꾸기   ----------------complete

한번 다 읽은 다음에 파싱해야함.                                           ----------------complete



///////////////////////////////////// */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////format 클래스 파일
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Myformats {
public:
    Myformats() {
        opcode = 0;
        instruction_name = "none";
    }
    virtual ~Myformats() {};

    int opcode;
    std::string instruction_name;

    void set_opcode(int op) {
        opcode = op;
    }

};

class R_format : public Myformats {
public:
    std::string type_name = "R";
    int rs, rt, rd, shamt, funct;

    R_format() {
        rs = rt = rd = shamt = funct = 0;
    }
    R_format(std::string name, int op, int funct_num) {
        rs = rt = rd = shamt = funct = 0;
        instruction_name = name;
        set_opcode(op);
        funct = funct_num;
    }
    virtual ~R_format() {};

};

class I_format : public Myformats {
public:
    I_format() {
        rs = rt = immediate_or_address = 0;
    }
    I_format(std::string name, int op_num) {
        rs = rt = immediate_or_address = 0;
        instruction_name = name;
        set_opcode(op_num);
    }
    virtual ~I_format() {};

    std::string type_name = "I";
    int rs, rt, immediate_or_address;
};

class J_format : public Myformats {
public:
    J_format() {
        jump_target = 0;
    }
    J_format(std::string name, int op_num) {
        jump_target = 0;
        instruction_name = name;
        set_opcode(op_num);
    }
    virtual ~J_format() {};

    std::string type_name = "J";
    int jump_target;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////함수파일
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> classify_bracket(std::string str) {
    int idx_first = 0;
    int idx_second = 0;
    for (auto iter = str.begin(); iter != str.end(); iter++) {
        if ((*iter) == '(') {
            idx_first = iter - str.begin();
        }
        if ((*iter) == ')') {
            idx_second = iter - str.begin();
        }
    }
    std::string offset;
    for (auto iter = str.begin(); iter != str.begin() + idx_first; iter++) {
        offset.push_back(*iter);
    }
    std::string rs;
    for (auto iter = str.begin() + idx_first + 1; iter != str.begin() + idx_second; iter++) {
        rs.push_back(*iter);
    }
    std::vector<std::string> classify = { offset, rs };
    return classify;
}

std::vector<std::vector<std::string>>& read_once(std::ifstream& sample_code, std::vector<std::vector<std::string>>& parsing_table) {
    if (sample_code.is_open()) {
        while (!sample_code.eof()) {
            std::string oneline;
            getline(sample_code, oneline);
            std::istringstream iss(oneline);

            std::vector<std::string> container;
            container.clear();
            std::string sub;
            while (iss >> sub) {
                container.push_back(sub);
            }

            if (container.size() == 0) {
                continue;
            }
            parsing_table.push_back(container);
        }
        sample_code.close();
    }
    /*
        if(parsing_table.back().size() == 0){ /////////////s파일 맨마지막 줄이 비었을 때
            parsing_table.pop_back();
        }
    */
    return parsing_table;
}

int num_extract(std::string str) {
    int first_num_idx = 0;
    int last_num_idx = str.length() - 1;

    if (str[0] == '$') {
        first_num_idx = 1;
    }
    if (str[last_num_idx] == ',') {
        last_num_idx--;
    }

    if (first_num_idx == 1 && last_num_idx == 1) {
        char one_char = str[1];
        int num = one_char - 48;
        return num;
    }

    else {
        int num_char = last_num_idx - first_num_idx + 1;
        char* extract_number = new char[num_char];
        for (int i = 0; i != num_char; i++) {
            extract_number[i] = str[first_num_idx + i];
        }
        std::string extract_num = extract_number;

        delete[] extract_number;
        //////////////16진수일때
        if (extract_num[0] == '0' && extract_num[1] == 'x') {
            const char* transform = extract_num.c_str();
            return strtol(transform, NULL, 16);
        }
        int num = stoi(extract_num);
        return num;
    }

}

std::string int_to_hex(int num) {   ///16진수 string으로 변환
    std::stringstream ss;
    std::string str;
    ss << std::hex << num;
    str = ss.str();
    return str;
}

int binary_to_int(std::string binary_str) {
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

int address_by_finding_far_from_main(std::vector<std::vector<std::string>> parsing_table, std::string str, std::vector<std::vector<std::vector<int>>> data_memory) {
    int mem_starting_add_txt = 0x00400000;
    int number_of_main = 0;
    int number_of_line = 0;

    static int la_number = 0;
    for (int n = 0; n < parsing_table.size(); n++) {
        number_of_line++;
        if (parsing_table[n][0] == "la") { //// lui,ori  instruction 몇개인지 구분하기 위해 위의 변수를 봐야함. 따라서 data memory 필요.
            std::string find_op = parsing_table[n][2];
            int number = 0;
            for (int i = 0; i < parsing_table.size(); i++) {
                if (parsing_table[i][0].back() == ':') {
                    number++;
                }
                if (parsing_table[i][0] == find_op + ':') {
                    break;
                }
            }

            /// lui에 상위 16비트/// ori에 하위 16비트

            int address_num_la = data_memory[number - 1][0][0];
            std::string hex_num = int_to_hex(address_num_la); ////16진수로 바꿈. 상위 4글자 하위 4글자.

            std::string eight_hex_num = "00000000";
            int idx = 0;
            for (auto iter = hex_num.rbegin(); iter != hex_num.rend(); iter++) {
                eight_hex_num[7 - idx] = (*iter);
                idx++;
            }
            std::string downfour;
            for (int i = 4; i < 8; i++) {
                downfour.push_back(eight_hex_num[i]);
            }

            if (downfour != "0000") {   ////ori를 해야할 경우
                number_of_line++;
            }

        }
        if (parsing_table[n][0] == "main:") {
            number_of_main = number_of_line - 1;
        }
        if (parsing_table[n][0].back() == ':' && parsing_table[n].size() == 1) {
            number_of_line--;
            if (parsing_table[n][0] == str + ':') {
                number_of_line++;
            }
        }
        if (parsing_table[n][0] == str + ':') {
            break;
        }
    }
    int byte_far_from_main = 4 * (number_of_line - number_of_main);
    return mem_starting_add_txt + byte_far_from_main;
}

std::string calculate_R_format(std::vector<R_format>::iterator iter, std::vector<std::string> container, std::vector<std::vector<std::string>> parsing_table) {
    int code_num = iter->opcode;
    if (iter->instruction_name == "jr") { // jr인 경우
        iter->rs = num_extract(container[1]);
    }

    else {
        if (iter->instruction_name == "sll" | iter->instruction_name == "srl") {
            iter->rd = num_extract(container[1]);
            iter->rt = num_extract(container[2]);
            iter->shamt = num_extract(container[3]);
        }
        else {
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

std::string calculate_I_format(std::vector<I_format>::iterator iter, std::vector<std::string> container, std::vector<std::vector<std::string>> parsing_table, std::vector<std::vector<std::vector<int>>> data_memory) {
    int code_num = iter->opcode;
    if (iter->instruction_name == "addiu" | iter->instruction_name == "andi" | iter->instruction_name == "ori" | iter->instruction_name == "sltiu") {
        iter->rt = num_extract(container[1]);
        iter->rs = num_extract(container[2]);
        iter->immediate_or_address = num_extract(container[3]);
    }

    else {
        if (iter->instruction_name == "lui") {
            iter->rt = num_extract(container[1]);
            iter->immediate_or_address = num_extract(container[2]);
        }

        else if (iter->instruction_name == "beq" | iter->instruction_name == "bne") {
            iter->rs = num_extract(container[1]);
            iter->rt = num_extract(container[2]);
            int beq_bne_address = address_by_finding_far_from_main(parsing_table, container[0], data_memory);
            int branch_address = address_by_finding_far_from_main(parsing_table, container[3], data_memory);
            int offset = (branch_address - beq_bne_address - 4) / 4;
            float rest = (branch_address - beq_bne_address - 4) % 4;
            if (rest != 0) {
                std::cout << "beq, bne instruction's offset calculation is wrong. 4로 나눌 때 나머지 != 0 " << ", 나머지 = " << rest << std::endl;
            }
            iter->immediate_or_address = offset;
        }

        else { ////////////////lw,lb,sw, sb처럼 offset존재하는경우

            iter->rt = num_extract(container[1]);
            std::vector<std::string> classified_string = classify_bracket(container[2]);
            iter->rs = num_extract(classified_string.back());
            iter->immediate_or_address = std::stoi(classified_string.front());
            std::cout << "immediate or address = " << iter->immediate_or_address << std::endl; ///////////////////////////for test
            std::cout << "rs = " << iter->rs << std::endl; ///////////////////////////for test
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

std::string calculate_J_format(std::vector<J_format>::iterator iter, std::vector<std::string> container, std::vector<std::vector<std::string>> parsing_table, std::vector<std::vector<std::vector<int>>> data_memory) {
    int code_num = iter->opcode;
    int jumping_address = address_by_finding_far_from_main(parsing_table, container[1], data_memory);
    iter->jump_target = jumping_address / 4;
    std::cout << code_num << " " << (iter)->jump_target << std::endl;
    std::bitset<6>bit_code_num(code_num);
    std::bitset<26>bit_jump_target((iter)->jump_target);

    std::string bit_instruction = bit_code_num.to_string() + bit_jump_target.to_string();
    std::cout << bit_instruction << std::endl;
    return bit_instruction; //std::stoll(bit_instruction);

}

std::string print_output(std::vector<R_format> R, std::vector<I_format> I, std::vector<J_format> J, std::vector<std::string> container, std::vector<std::vector<std::string>> parsing_table, std::vector<std::vector<std::vector<int>>> data_memory) {

    ////////////////                                                                  for test
    for (int i = 0; i < container.size(); i++)
    {
        std::cout << container[i] << " ";
    }
    std::cout << std::endl; //////////////////


    for (auto iter = R.begin(); iter != R.end(); iter++) {
        if (iter->instruction_name == container[0]) {
            return calculate_R_format(iter, container, parsing_table);
        }
    }
    for (auto iter = I.begin(); iter != I.end(); iter++) {
        if (iter->instruction_name == container[0]) {
            return calculate_I_format(iter, container, parsing_table, data_memory);
        }
    }

    for (auto iter = J.begin(); iter != J.end(); iter++) {
        if (iter->instruction_name == container[0]) {
            return calculate_J_format(iter, container, parsing_table, data_memory);
        }
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    std::string command;
    getline(std::cin, command);

    std::istringstream start(command);
    std::vector<std::string> split_command;
    while (start >> command) {
        split_command.push_back(command);
    }


    if (split_command[0] == "$") {
        if (split_command[1] == "./runfile") {
            split_command[2].erase(split_command[2].begin());
            split_command[2].pop_back();

            std::string file_name_extension = split_command[2];

            split_command[2].pop_back();
            split_command[2].pop_back();
            
            std::string file_name = split_command[2];
            std::ofstream fileout(file_name + ".o");                  ///////////////////////file 생성

            std::cout << "program start" << std::endl;

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// instruction들의 초기설정 
            R_format addu("addu", 0, 0x21), _and("and", 0, 0x24), jr("jr", 0, 8), nor("nor", 0, 0x27), _or("or", 0, 0x25), sltu("sltu", 0, 0x2b), sll("sll", 0, 0), srl("srl", 0, 2), subu("subu", 0, 0x23);
            I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
            J_format j("j", 2), jal("jal", 3);

            std::vector<R_format> R{ addu, _and, jr, nor, _or, sltu, sll, srl, subu };
            std::vector<I_format> I{ addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb };
            std::vector<J_format> J{ j, jal };

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 메모리 초기설정
            int mem_starting_add_txt = 0x00400000;
            int mem_starting_add_data = 0x10000000;
            int mem_end_stack = 0x7ffffffc;

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// parsing
            std::ifstream sample_code;
            sample_code.open(file_name_extension);

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
            for (auto iter = parsing_table.begin(); iter != parsing_table.end(); iter++) {
                std::vector<std::string> container = (*iter);

                std::string& first_word = container[0];

                if (first_word == ".data") {
                    data = true;
                    continue;
                }
                if (first_word == ".text") {
                    text = true;
                    data_memory.push_back(data_array); // "".text"가 나오는 순간 data영역이 끝나기 때문에 바로 아래의 if문에서 만들어진 마지막 data_array를 data_memory에 저장해주어야함
                    data_array.clear();
                    continue;
                }

                if (data == true && text == false) { //data segment
                    number_of_data_line++;
                    mem_now_add_data = mem_starting_add_data + 4 * (number_of_data_line);

                    if (first_word.back() == ':') {
                        if (data_array.size() != 0)
                        {
                            data_memory.push_back(data_array);
                            data_array.clear();
                        }
                        std::vector<int> variable = { mem_now_add_data, num_extract(container[2]) }; /////////////////////
                        data_array.push_back(variable);
                    }

                    if (first_word == ".word") {
                        std::vector<int> variable = { mem_now_add_data, num_extract(container[1]) };
                        data_array.push_back(variable);

                    }
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// main: 이 시작
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                if (data == true && text == true) { //instruction
                    if (first_word.back() == ':') {
                        if (txt_array.size() != 0) {
                            txt_memory.push_back(txt_array);
                            txt_array.clear();
                        }

                        if (first_word == "exit:") {
                            break;
                        }

                        if (container.size() != 1) {
                            std::vector<std::string> new_container;
                            new_container.clear();

                            for (int i = 1; i < container.size(); i++) {
                                new_container.push_back(container[i]);
                            }

                            container.clear();
                            container = new_container;
                            goto label;
                        }

                        continue;
                    }                                     /////////////sum: sltiu s1 s2 1

                    else {
                    label:
                        std::string bit_instruction;

                        number_of_instruction_line++;
                        mem_now_add_txt = mem_starting_add_txt + 4 * (number_of_instruction_line);


                        if (first_word == "la") {
                            std::string find_op = container[2];
                            int number = 0;

                            for (int n = 0; n < parsing_table.size(); n++) {
                                if (parsing_table[n][0].back() == ':') {
                                    number++;
                                }
                                if (parsing_table[n][0] == find_op + ':') {
                                    break;
                                }
                            }

                            /// lui에 상위 16비트/// ori에 하위 16비트

                            int address_num_la = data_memory[number - 1][0][0];
                            std::string hex_num = int_to_hex(address_num_la); ////16진수로 바꿈. 상위 4글자 하위 4글자.

                            std::string eight_hex_num = "00000000";
                            int idx = 0;
                            for (auto iter = hex_num.rbegin(); iter != hex_num.rend(); iter++) {
                                eight_hex_num[7 - idx] = (*iter);
                                idx++;
                            }


                            std::string upfour;
                            std::string downfour;
                            for (int i = 0; i < 4; i++) {
                                upfour.push_back(eight_hex_num[i]);
                            }

                            for (int i = 4; i < 8; i++) {
                                downfour.push_back(eight_hex_num[i]);
                            }

                            std::vector<std::string> new_container = { "lui", container[1], upfour }; ////////////////address_num_la를 16진수로 바꾸고 4자리씩 비교해야함.
                            bit_instruction = print_output(R, I, J, new_container, parsing_table, data_memory);
                            std::pair<int, std::string> value = std::make_pair(mem_now_add_txt, bit_instruction);
                            txt_array.push_back(value);
                            std::cout << "memory address = " << value.first << " bit instruction = " << value.second << std::endl;

                            if (downfour != "0000") {   ///////////ori 해야함.
                                number_of_instruction_line++;
                                mem_now_add_txt = mem_starting_add_txt + 4 * (number_of_instruction_line);
                                std::vector<std::string> second_new_container = { "ori", container[1], container[1], downfour };
                                bit_instruction = print_output(R, I, J, second_new_container, parsing_table, data_memory);
                                std::pair<int, std::string> value = std::make_pair(mem_now_add_txt, bit_instruction);
                                txt_array.push_back(value);
                                std::cout << "memory address = " << value.first << " bit instruction = " << value.second << std::endl;

                            }

                        }
                        else {
                            std::string bit_instruction = print_output(R, I, J, container, parsing_table, data_memory);
                            std::pair<int, std::string> value = std::make_pair(mem_now_add_txt, bit_instruction);
                            txt_array.push_back(value);
                            std::cout << "memory address = " << value.first << " bit instruction = " << value.second << std::endl;
                        }
                    }

                }
                if (iter == parsing_table.end() - 1) {
                    txt_memory.push_back(txt_array);
                    txt_array.clear();
                }
            }
            
            int text_section_size = mem_now_add_txt - mem_starting_add_txt;
            int data_section_size = mem_now_add_data - mem_starting_add_data;
            std::bitset<32> bit_text_section_size(text_section_size);
            std::bitset<32> bit_data_section_size(data_section_size);



            std::cout << "--------------------------------------------------------------------------------------------for the test--------------------------------------------------------------------------------------------" << std::endl;
            std::cout << "text_section_size = " << "0x"+int_to_hex(text_section_size) << std::endl;
            std::cout << "data_section_size = " << "0x" + int_to_hex(data_section_size) << std::endl;
            std::cout << "--------------------------------------------------------------------------------------------32bit code size--------------------------------------------------------------------------------------------" << std::endl;
            fileout << bit_text_section_size.to_string() << std::endl;
            fileout << bit_data_section_size.to_string() << std::endl;
            std::cout << "--------------------------------------------------------------------------------------------32bit instruction--------------------------------------------------------------------------------------------" << std::endl;
            std::cout << "txt_memory.size() = " << txt_memory.size() << std::endl;
            for (int m = 0; m < txt_memory.size(); m++) {
                for (int x = 0; x < txt_memory[m].size(); x++) {
                    fileout << txt_memory[m][x].second << std::endl;
                    std::cout << "0x" + int_to_hex(binary_to_int(txt_memory[m][x].second)) << std::endl;
                }
            }

            std::cout << "--------------------------------------------------------------------------------------------32bit value--------------------------------------------------------------------------------------------" << std::endl;
            std::cout << "data_memory.size() = " << data_memory.size() << std::endl;
            for (int m = 0; m < data_memory.size(); m++) {
                for (int x = 0; x < data_memory[m].size(); x++) {
                    fileout << std::bitset<32>(data_memory[m][x][1]) << std::endl;
                    std::cout << "0x" + int_to_hex(data_memory[m][x][1]) << std::endl;
                }
            }
            std::cout << "--------------------------------------------------------------------------------------------for the test--------------------------------------------------------------------------------------------" << std::endl;
            std::cout << "number of data line = " << number_of_data_line << std::endl;
            std::cout << "number of instruction line = " << number_of_instruction_line << std::endl;
            
            fileout.close();
        }
    }
    else {
    std::cout << "command is wrong" << std::endl;
    }
}
