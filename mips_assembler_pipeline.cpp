#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include <stdlib.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////function/////////////////////////////////////////////////////////////////////////////////////
int binary_to_int(std::string binary_str)
{ ///동작 ok
    int a = 1;
    int num = 0;
    for (int i = binary_str.length() - 1; i >= 0; i--)
    {
        for (int j = 0; j < binary_str.length() - i - 1; j++)
        {
            a *= 2;
        }
        if (binary_str[i] == '1')
        {
            num += a;
        }
        a = 1;
    }
    return num;
}

std::string int_to_hex(int num)
{ ///16진수 string으로 변환 //동작 ok
    std::stringstream ss;
    std::string str;
    ss << std::hex << num;
    str = ss.str();
    return str;
}

int change_instruction(std::string instruction, int start, int final)
{ /// 동작 ok
    std::string bit_num;
    for (auto iter = instruction.begin() + start; iter != instruction.begin() + final; iter++)
    {
        bit_num.push_back(*iter);
    }
    int int_num = binary_to_int(bit_num);
    return int_num;
}

std::vector<int> change_to_R_format_instruction(std::vector<std::string> string_vec_instruction)
{ //동작 ok
    std::string instruction;
    for (int i = 0; i < string_vec_instruction.size(); i++)
    {
        instruction += string_vec_instruction[i];
    }
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

std::vector<int> change_to_I_format_instruction(std::vector<std::string> string_vec_instruction)
{ ///동작 ok
    std::string instruction;
    for (int i = 0; i < string_vec_instruction.size(); i++)
    {
        instruction += string_vec_instruction[i];
    }
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

std::vector<int> change_to_J_format_instruction(std::vector<std::string> string_vec_instruction)
{ ///동작 ok
    std::string instruction;
    for (int i = 0; i < string_vec_instruction.size(); i++)
    {
        instruction += string_vec_instruction[i];
    }
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction, 0, 6);
    int jump_target = change_instruction(instruction, 6, 32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(jump_target);
    return decimal_instruction;
}

std::vector<int> find_address_index_for_branch(std::vector<std::vector<int>> instruction_or_data_address, int address_in_regi)
{
    std::vector<int> index;
    for (auto iter = instruction_or_data_address.begin(); iter != instruction_or_data_address.end(); iter++)
    {
        for (int n = 0; n < (*iter).size(); n++)
        {
            if ((*iter)[n] == address_in_regi)
            {
                index.push_back(iter - instruction_or_data_address.begin());
                index.push_back(n);
                return index;
            }
        }
    }
}

std::string value_sign_extension(std::string data, int quantity)
{ ///checked
    std::string num_extension;
    if (data.front() == '0')
    {
        for (int num = 0; num < quantity; num++)
        {
            num_extension.push_back('0');
        }
    }
    else
    {
        for (int num = 0; num < quantity; num++)
        {
            num_extension.push_back('1');
        }
    }
    return num_extension + data;
}

std::bitset<32> change_to_bitset(std::string instruction_32bit)
{ ///checked
    std::bitset<32> bitset_instruction = 0;
    for (int n = 0; n < 32; n++)
    {
        if (instruction_32bit[31 - n] == '1')
        {
            bitset_instruction.set(n);
        }
    }
    return bitset_instruction;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Myformats
{
public:
    Myformats()
    {
        opcode = 0;
        instruction_name = "none";
    }
    virtual ~Myformats() {};

    int opcode;
    std::string instruction_name;
};

class R_format : public Myformats
{
public:
    std::string type_name = "R";
    int rs, rt, rd, shamt, funct;
    std::bitset<32> operation(std::vector<std::bitset<32>>& my_register)
    { ////register 상태와 pc table 받아야함.
        int rs_value = int(my_register[rs].to_ullong()); //ullong
        int rt_value = int(my_register[rt].to_ullong()); //ullong
        int rd_value = 0;

        if (instruction_name == "addu")
        {
            rd_value = rs_value + rt_value;
            return std::bitset<32>(rd_value);
        } ////checked

        if (instruction_name == "and")
        {
            return (my_register[rs] & my_register[rt]);
        } ///checked

        if (instruction_name == "nor")
        {
            return (~(my_register[rs] | my_register[rt]));
        } ///checked

        if (instruction_name == "or")
        {
            return (my_register[rs] | my_register[rt]);
        } ///checked

        if (instruction_name == "sltu")
        {
            if (rs_value < rt_value)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        } ///checked

        if (instruction_name == "sll")
        {
            return (my_register[rt] << shamt);
        } ///checked

        if (instruction_name == "srl")
        {
            return (my_register[rt] >> shamt);
        } ///checked

        if (instruction_name == "subu")
        {
            rd_value = rs_value - rt_value;
            return std::bitset<32>(rd_value);
        } ///checked

        if (instruction_name == "jr") {//////////////////////////////////////////rs로 분기하는것 좀더 생각필요
            return my_register[rs];
        }
    }

    std::vector<int> operation_jr(std::vector<std::bitset<32>> my_register, std::vector<std::vector<int>> instruction_address)
    { ///pc_num에서 rs안에 들어있는 주소와 같은 값의 index를 구한다.

        ///return my_register[rs];
        int decimal_num = int(my_register[rs].to_ullong());
        std::vector<int> index = find_address_index_for_branch(instruction_address, decimal_num);
        return index;
    } ///checked 만약 찾지못하면??


    R_format()
    {
        rs = rt = rd = shamt = funct = 0;
    }
    R_format(std::string name, int op, int funct_num)
    {
        rs = rt = rd = shamt = funct = 0;
        instruction_name = name;
        opcode = op;
        funct = funct_num;
    }
    virtual ~R_format() {};
};

class I_format : public Myformats
{
public:
    std::string type_name = "I";
    int rs, rt, immediate_or_address;
    std::vector<int> operation_beq(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<int>>& instruction_address, int big_index, int small_index)
    { ///분기해야할 pc index찾아주기
        if (my_register[rs].to_string() == my_register[rt].to_string())
        {
            int32_t target_address = instruction_address[big_index][small_index] + 4 + immediate_or_address * 4;
            std::vector<int> branch_idx = find_address_index_for_branch(instruction_address, target_address);
            return branch_idx;
        }
        else
        {
            return { -1, 0 }; //rs와 rt가 다르면 -1 리턴.
        }
    } ///checked

    std::vector<int> operation_bne(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<int>>& instruction_address, int big_index, int small_index)
    { ///분기해야할 pc index찾아주기
        if (my_register[rs].to_string() != my_register[rt].to_string())
        {
            int target_address = instruction_address[big_index][small_index] + 4 + immediate_or_address * 4;
            std::vector<int> branch_idx = find_address_index_for_branch(instruction_address, target_address);
            return branch_idx;
        }
        else
        {
            return { -1, 0 }; //rs와 rt가 다르면 -1 리턴.
        }
    } ///checked

    std::bitset<32> operation_lw_or_lb(std::vector<std::vector<std::string>>& data_memory, std::vector<int> address_index)
    {
        if (instruction_name == "lw")
        {
            std::string word;
            for (int a = 0; a < 4; a++) {

                word += data_memory[address_index[0]][a];
            }
            return change_to_bitset(word);
            //my_register[rt] = change_to_bitset(word); ////std::vector<std::bitset<32> my_register[0] = 15; 하면 2진수로 들어가는가 체크체크하기
        }
        else
        {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++)
            {
                one_byte_data.push_back(data_memory[address_index[0]][address_index[1]][n]);
            }
            std::string num_extension = value_sign_extension(one_byte_data, 24);
            return change_to_bitset(num_extension);
            //my_register[rt] = change_to_bitset(num_extension);
        }
    } //checked
    void operation_sw_or_sb(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<std::string>>& data_memory, std::vector<int> address_index)
    {
        int32_t rt_value = int32_t(my_register[rt].to_ullong());
        if (instruction_name == "sw")
        {
            std::string a = std::bitset<32>(rt_value).to_string();
            std::string temp;
            std::vector<std::string> cont;
            for (int num = 0; num < 4; num++) {
                for (int num2 = 0 + num * 8; num2 < 8 + num * 8; num2++) {
                    temp.push_back(a[num2]);
                }
                cont.push_back(temp);
                temp.clear();
            }
            data_memory[address_index[0]] = cont;
        }
        else
        {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++)
            {
                one_byte_data.push_back(data_memory[address_index[0]][address_index[1]][n]);
            }
            data_memory[address_index[0]][address_index[1]] = one_byte_data;
        }
    }

    std::vector<int> address_idx_cal_lw_lb_sw_sb(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<int>>& data_address){
        int32_t rs_value = int32_t(my_register[rs].to_ullong());
        int32_t address = rs_value + immediate_or_address;
        std::vector<int> address_index = find_address_index_for_branch(data_address, address);
        return address_index;
    }///data memory와 data address 크기 늘려주기. assign만 되면 된다.

    std::bitset<32> operation(std::vector<std::bitset<32>>& my_register)
    {
        int32_t rs_value = int32_t(my_register[rs].to_ullong());
        int32_t rt_value = int32_t(my_register[rt].to_ullong());

        if (instruction_name == "addiu")
        {                                                                                                        ///////rs와 sign-extened된 immediate의 합을 rt에 저장.
            std::string extension = value_sign_extension(std::bitset<16>(immediate_or_address).to_string(), 16); ///32비트 extension된 immediate
            std::bitset<32> bitset_extension = change_to_bitset(extension);
            int32_t rt_value = (int32_t)bitset_extension.to_ullong() + rs_value;
            return rt_value;
            //my_register[rt] = std::bitset<32>(rt_value);
            //return;
        } ////checked

        if (instruction_name == "andi")
        {
            return (my_register[rs] & std::bitset<32>(immediate_or_address));
            //my_register[rt] = (my_register[rs] & std::bitset<32>(immediate_or_address));
            //return;
        } ///checked

        if (instruction_name == "lui")
        {
            std::bitset<32> container;
            std::bitset<16> imm_16bit(immediate_or_address);
            for (int idx = 0; idx < 32; idx++)
            {
                if (idx < 16)
                {
                    container[idx] = 0;
                    //my_register[rt][idx] = 0;
                }
                else
                {
                    container[idx] = imm_16bit[idx - 16];
                    //my_register[rt][idx] = imm_16bit[idx - 16];
                }
            }
            return container;
        } ///checked

        if (instruction_name == "ori")
        {
            return (my_register[rs] | std::bitset<32>(immediate_or_address));
            //my_register[rt] = (my_register[rs] | std::bitset<32>(immediate_or_address));
            //return;
        } ///checked

        if (instruction_name == "sltiu")
        { ///rs가 sign_extened imm보다 작으면 rt 1, 아니면 0
            std::string a = (value_sign_extension(std::bitset<16>(immediate_or_address).to_string(), 16));
            std::bitset<32> b = change_to_bitset(a);
            if (rs_value < int32_t(b.to_ullong()))
            {   
                return 1;
                //my_register[rt] = 1;
                //return;
            }
            else
            {   
                return 0;
                //my_register[rt] = 0;
                //return;
            }
        } ///checked
    }

    I_format()
    {
        rs = rt = immediate_or_address = 0;
    }
    I_format(std::string name, int op_num)
    {
        rs = rt = immediate_or_address = 0;
        instruction_name = name;
        opcode = op_num;
    }
    virtual ~I_format() {};
};

class J_format : public Myformats
{
public:
    std::string type_name = "J";
    int jump_target;
    std::string j_operation(int pc)
    { ///checked
        jump_target = jump_target * 4;

        std::bitset<28> jump_target_28bit(jump_target);
        std::string str_jump_target_28bit = jump_target_28bit.to_string();

        std::bitset<32> pc_32bit(pc);
        std::string str_pc_32bit = pc_32bit.to_string();

        std::string upper_pc_4bit;
        for (int i = 0; i < 4; i++)
        {
            upper_pc_4bit.push_back(str_pc_32bit[i]);
        }

        std::string jump_target_32bit = upper_pc_4bit + str_jump_target_28bit;
        return jump_target_32bit;
    }

    J_format()
    {
        jump_target = 0;
    }
    J_format(std::string name, int op_num)
    {
        jump_target = 0;
        instruction_name = name;
        opcode = op_num;
    }
    virtual ~J_format() {};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class Single_cycle_path {
public:
    std::vector<std::string> str_instruction;
    R_format* r_pointer = nullptr;
    I_format* i_pointer = nullptr;
    J_format* j_pointer = nullptr;
    std::string instruction_type = "none";
    std::string instruction_name = "none";
    std::vector<int> int_instruction = { 0, };
    int big_index = -1;
    int small_index = -1;
    int pc = 0;

    Single_cycle_path() {
        str_instruction = { "none" };
    }

    Single_cycle_path(std::vector<std::string> one_instruction, std::vector<int*> index, int pc_num) {
        str_instruction = one_instruction;
        big_index = *index[0];
        small_index = *index[1];
        pc = pc_num;
        std::cout << "Single_cycle_path 인스턴스 생성" << std::endl;
        std::cout << "이 instruction의 big_index = " << big_index << std::endl;
        std::cout << "이 instruction의 small_index = " << small_index << std::endl;
    }

    void if_stage(std::vector<int*>& index) {
        if (pc == 0) {
            return;
        }
        *index[0] ++; ///////////////////pc+4해준것
        *index[1] = 0;
        std::cout << "fetching stage complete" << std::endl;
        std::cout << "instruction is " << str_instruction[0] + str_instruction[1] + str_instruction[2] + str_instruction[3] << std::endl;
    }

    void id_stage(std::vector<R_format> R, std::vector<I_format> I, std::vector<J_format> J) {
        if (pc == 0) {
            return;
        }
        if (instruction_type == "none") {
            int_instruction = change_to_R_format_instruction(str_instruction);
            for (auto r = R.begin(); r != R.end(); r++) {
                if ((r->opcode) == int_instruction[0] && (r->funct) == int_instruction[5]) {
                    std::cout << r->instruction_name << std::endl; ////test////
                    r->rs = int_instruction[1];
                    r->rt = int_instruction[2];
                    r->rd = int_instruction[3];
                    r->shamt = int_instruction[4];
                    instruction_type = (r->type_name);
                    instruction_name = (r->instruction_name);
                    r_pointer = &(*r);
                    break;
                }
            }
        }
        if (instruction_type == "none") {
            int_instruction = change_to_I_format_instruction(str_instruction);
            for (auto i = I.begin(); i != I.end(); i++) {
                if (i->opcode == int_instruction[0]) {
                    std::cout << i->instruction_name << std::endl; ////test////
                    i->rs = int_instruction[1];
                    i->rt = int_instruction[2];
                    i->immediate_or_address = int_instruction[3];
                    instruction_type = (i->type_name);
                    instruction_name = (i->instruction_name);
                    i_pointer = &(*i);
                    break;
                }
            }
        }
        if (instruction_type == "none") {
            ///////////////////////////////////////////////////////j foramt은 id stage에서 종료됨///////////////////////////////////////////////////////////
            int_instruction = change_to_J_format_instruction(str_instruction);
            for (auto j = J.begin(); j != J.end(); j++) {
                if (j->opcode == int_instruction[0]) {
                    std::cout << j->instruction_name << std::endl;
                    j->jump_target = int_instruction[1];
                    instruction_type = (j->type_name);
                    instruction_name = (j->instruction_name);
                    j_pointer = &(*j);
                    if(instruction_name == "jump"){
                        j_pointer->j_operation(pc);
                    }
                    break;
                }
            }
        }
        std::cout << "---------------------------------Now state---------------------------------" << std::endl;
        std::cout << "str_instruction = " << str_instruction[0] + str_instruction[1] + str_instruction[2] + str_instruction[3] << std::endl;
        std::cout << "r_pointer = " << r_pointer << std::endl;
        std::cout << "i_pointer = " << i_pointer << std::endl;
        std::cout << "j_pointer = " << j_pointer << std::endl;
        std::cout << "instruction_type = " << instruction_type << std::endl;
        std::cout << "instruction_name = " << instruction_name << std::endl;
        std::cout << "int_instruction = ";
        for (int i = 0; i < int_instruction.size(); i++) {
            std::cout << int_instruction[i] << ",";
        }
        std::cout << std::endl;
        std::cout << "pc = " << pc << std::endl;
    }

    std::bitset<32> ex_stage(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<int>>& instruction_address, std::vector<std::vector<int>>& data_address) {//// arithmetic or 주소만 계산
        if (pc == 0) {
            return 0;
        }

        if (instruction_type == "R") {
            return r_pointer->operation(my_register); ///jr 처리해주기
        }
        if (instruction_type == "I") {
            if (instruction_name == "lw" || instruction_name == "lb" || instruction_name == "sw" || instruction_name == "sb") {
                std::vector<int> new_index = i_pointer->address_idx_cal_lw_lb_sw_sb(my_register, data_address); 
                
                
                int address = data_address[new_index[0]][new_index[1]]; //address찾았음
                std::cout << instruction_name << "'s ex_stage(calculation address) complete" << std::endl;

                return std::bitset<32>(address);
            }

            if (instruction_name == "beq" || instruction_name == "bne") {
                std::vector<int> new_index;
                if(instruction_name == "beq"){
                    new_index = i_pointer->operation_beq(my_register, instruction_address, big_index, small_index);
                }
                if(instruction_name == "bne"){
                    new_index = i_pointer->operation_bne(my_register, instruction_address, big_index, small_index);
                }
                if(new_index[0]!= -1){ //beq가 성립한다는 뜻
                    ///new_index의 주소 리턴
                }
                else{
                    return;///아무동작 하지않아야한다.
                }
            }
            else{
                return i_pointer->operation(my_register);
            }
        }
        if (instruction_type == "J"){
            return; //아무동작 하지않도록 
        }
    }

    std::string mem_stage(std::bitset<32> result_ex_stage, std::vector<std::vector<std::string>>& data_memory, std::vector<std::vector<int>>& data_address, std::vector<std::bitset<32>>& my_register) {
        if (pc == 0) {
            return "none";
        }
        if(instruction_type == "I"){
            int32_t address = int32_t(result_ex_stage.to_ullong());
            std::vector<int> address_index = find_address_index_for_branch(data_address, address);
            //////////////////////////////////////////////////////// lw, lb는 wb stage에서 register에 써줘야함////////////////////////////////////////////////////
            if (instruction_name == "lw" || instruction_name == "lb") {
                std::bitset<32> word = i_pointer->operation_lw_or_lb(data_memory, address_index);

                std::cout <<  instruction_name << " is started " << std::endl;
                std::cout << "the index of address is " << address_index[0] << "," << address_index[1] << std::endl;
                return word.to_string(); ///////////  string으로 된 32bit data memory의 주소 리턴.
            }
            //////////////////////////////////////////////////////// sw, sb는 mem stage에서 종료///////////////////////////////////////////////////////////////////
            if (instruction_name == "sw" || instruction_name == "sb") {
                i_pointer->operation_sw_or_sb(my_register, data_memory, address_index);
                if(instruction_name == "sw"){
                    return "sw complete";
                }
                else{
                    return "sb complete";
                }
            }
        }
        else{
            std::cout << "mem_stage of " << instruction_name << ". This instruction doesn't need memory access" << std::endl;
            return "none";
        }
    }

    void wb_stage(std::bitset<32> result_ex_stage, std::string result_mem_stage, std::vector<std::bitset<32>>& my_register, std::vector<std::vector<std::string>>& data_memory) {
        if (pc == 0) {
            return;
        }

        if (instruction_type == "R") {
            int rd = int_instruction[3];
            my_register[rd] = result_ex_stage; // e
            std::cout << instruction_name << "'s wb stage is completed" << std::endl;
            return;
        }
        if (instruction_type == "I") {
            int rt = int_instruction[2];
            if (instruction_name == "lw" || instruction_name == "lb") {
                my_register[rt] = change_to_bitset(result_mem_stage);
                std::cout << instruction_name << "'s lw or lb stage is completed" << std::endl;
                return;
            }
        }
        if(instruction_type == "J"){
            return;
        }
    }
};
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::string command;
    getline(std::cin, command);

    std::istringstream start_command(command);
    std::vector<std::string> command_parsing;
    while (start_command >> command)
    {
        command_parsing.push_back(command);
    }

    if (command_parsing[0] == "./runfile")
    {
        std::cout << "program_start" << std::endl; //// test code
        bool exist_m = false;
        bool exist_d = false;
        bool exist_n = false;
        int memory_n;
        int command_n_instruction;
        for (int n = 0; n < command_parsing.size(); n++)
        {
            if (command_parsing[n] == "-m")
            {
                exist_m = true;
                memory_n = n + 1;
                continue;
            }
            if (command_parsing[n] == "-d")
            {
                exist_d = true;
                continue;
            }
            if (command_parsing[n] == "-n")
            {
                exist_n = true;
                command_n_instruction = stoi(command_parsing[n + 1]);
                continue;
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////0-31 reg 초기화 및 pc 설정///////////////////////////////////////////////////////////////

        std::vector<std::bitset<32>> my_register(32, 0);

        int start_instruction_address = 0x400000; ///txt 영역 시작주소 (pc)
        int end_instruction_address;
        std::vector<int> byte_instruction_address(4);
        byte_instruction_address[0] = start_instruction_address;
        std::vector<std::vector<int>> instruction_address;
        instruction_address.push_back(byte_instruction_address); ////////////////asdfasdfasdf

        int start_data_address = 0x10000000; ///data영역 시작주소
        int end_data_address;
        std::vector<int> byte_data_address;
        std::vector<std::vector<int>> data_address;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        R_format addu("addu", 0, 0x21), _and("and", 0, 0x24), jr("jr", 0, 8), nor("nor", 0, 0x27), _or("or", 0, 0x25), sltu("sltu", 0, 0x2b), sll("sll", 0, 0), srl("srl", 0, 2), subu("subu", 0, 0x23);
        I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
        J_format j("j", 2), jal("jal", 3);

        std::vector<R_format> R{ addu, _and, jr, nor, _or, sltu, sll, srl, subu };
        std::vector<I_format> I{ addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb };
        std::vector<J_format> J{ j, jal };

        std::string one_byte_instruction;
        std::vector<std::string> four_byte_instruction;
        std::vector<std::vector<std::string>> instruction_memory;
        instruction_memory.clear();

        std::string one_byte_data;
        std::vector<std::string> four_byte_data;
        std::vector<std::vector<std::string>> data_memory;
        data_memory.clear();

        int txt_section_size;
        int data_section_size;

        std::ifstream reading;
        reading.open(command_parsing.back());
        if (reading.is_open())
        {
            int line_num = 0;                           ///object파일의 몇번째 줄인지 확인
            int end_line_instruction_memory_plus_1 = 0; ////instruction의 끝 줄 확인
            std::string hex_num;                        ////getline을 사용
            __int64 decimal_num;
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //// 바이너리 파일 읽어오기
            while (!reading.eof())
            {
                getline(reading, hex_num);
                const char* transform = hex_num.c_str();
                decimal_num = strtoul(transform, NULL, 16); ////16진수 10진수로 바꾸기
                if (line_num == 0)
                { ////text section size 설정
                    txt_section_size = decimal_num;
                    end_instruction_address = start_instruction_address + txt_section_size;
                    end_line_instruction_memory_plus_1 = 2 + (txt_section_size / 4); /// input file에서 txt size, data size를 포함한 line number
                    line_num++;
                    continue;
                }
                if (line_num == 1)
                { ////data section size 설정
                    data_section_size = decimal_num;
                    end_data_address = start_data_address + data_section_size;
                    line_num++;
                    continue;
                }
                if (line_num > 1)
                {
                    std::bitset<32> bit_num(decimal_num);          //////32비트로 나타내기
                    std::string bit_oneline = bit_num.to_string(); ///// 32비트 이진수를 string으로

                    if (line_num < end_line_instruction_memory_plus_1)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            for (int n = 0 + i * 8; n < 8 + i * 8; n++)
                            {
                                one_byte_instruction.push_back(bit_oneline[n]);
                            }
                            four_byte_instruction.push_back(one_byte_instruction);
                            one_byte_instruction.clear();
                        }
                        instruction_memory.push_back(four_byte_instruction);
                        four_byte_instruction.clear();

                        if (line_num == 2)
                        {
                            instruction_address[0][1] = byte_instruction_address[0] + 1;
                            instruction_address[0][2] = byte_instruction_address[0] + 2;
                            instruction_address[0][3] = byte_instruction_address[0] + 3;
                        }
                        if (line_num > 2)
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                byte_instruction_address.push_back(start_instruction_address + 4 * (line_num - 2) + i);
                            }
                            instruction_address.push_back(byte_instruction_address); /////pc 설정 (10진수)
                        }
                        byte_instruction_address.clear();
                        line_num++;
                        continue;
                    }


                    else
                    {
                        if (line_num == end_line_instruction_memory_plus_1) {
                            int end_address = (instruction_address.back())[3];
                            std::vector<int> end = { end_address + 1,end_address + 2, end_address + 3, end_address + 4 };
                            instruction_address.push_back(end);
                        }

                        if (line_num < end_line_instruction_memory_plus_1 + data_section_size / 4)
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                for (int n = 0 + i * 8; n < 8 + i * 8; n++)
                                {
                                    one_byte_data.push_back(bit_oneline[n]);
                                }
                                four_byte_data.push_back(one_byte_data);
                                one_byte_data.clear();
                            }
                            data_memory.push_back(four_byte_data);
                            four_byte_data.clear();

                            for (int i = 0; i < 4; i++)
                            {
                                byte_data_address.push_back(start_data_address + 4 * (line_num - end_line_instruction_memory_plus_1) + i);
                            }
                            data_address.push_back(byte_data_address); ///data memory address 설정
                            byte_data_address.clear();
                            line_num++;
                            continue;
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }
            reading.close();
        }


        /////////////////////////////////////////////////instruction memory, data memory 생성됨 ///////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////.///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int instruction_memory_big_index = 0;
        int instruction_memory_small_index = 0;
        int data_memory_big_index = 0;
        int data_memory_small_index = 0;

        std::vector<int*> index = {&instruction_memory_big_index, &instruction_memory_small_index};

        /////////////////////////////////////////////////////////pipeline test 코드///////////////////////////////////////////////////////////////////


        /* 클래스로 구현 가능*/

        //////////////////////////////////////////////////////////////////////////// IF/ID 상태레지스터 ///////////////////////////////////
        std::vector<std::string> string_instruction = {"none",}; 
        int IF_pc = 0;
        std::vector<int> IF_index = {-1,-1};

        int IF_ID_pc = 0;  
        std::vector<int> IF_ID_index = {-1,-1};   
        //////////////////////////////////////////////////////////////////////////// ID STAGE 레지스터, ID/EX 상태레지스터 ///////////////////////////////////
        std::string ID_instruction_name = "none";
        std::string ID_instruction_type = "none";
        R_format* ID_r_format = nullptr;
        I_format* ID_i_format = nullptr;
        J_format* ID_j_format = nullptr;
        bool ID_register_write = false;
        int ID_rs = -1;
        int ID_rt = -1;
        int ID_imm = 0;
        int ID_rd = -1;
        int ID_shamt = 0;

        int ID_EX_pc = 0; 
        std::vector<int> ID_EX_index = {-1, -1};
        std::string ID_EX_instruction_type = "none";
        std::string ID_EX_instruction_name = "none";
        R_format* ID_EX_r_formats = nullptr; //어떤 format의 인스턴스인지 확인하기 위해서
        I_format* ID_EX_i_formats = nullptr;
        J_format* ID_EX_j_formats = nullptr;
        bool ID_EX_register_write = false;
        int ID_EX_rs = -1;
        int ID_EX_rt = -1;
        int ID_EX_rd = -1;

        //////////////////////////////////////////////////////////////////////////// EX STAGE 레지스터, EX/MEM 상태레지스터 ////////////////////////////////////////////////////
        std::bitset<32> EX_result = 0;

        std::string EX_MEM_instruction_type = "none";
        std::string EX_MEM_instruction_name = "none";
        int EX_MEM_pc = 0;
        std::vector<int> EX_MEM_index = {-1,-1};
        bool EX_MEM_register_write = false;
        int EX_MEM_rt = -1;
        int EX_MEM_rd = -1;
        std::bitset<32> EX_MEM_ALU_result = 0;
        I_format* EX_MEM_i_formats = nullptr;
        //////////////////////////////////////////////////////////////////////////// MEM STAGE 레지스터, MEM/WB 상태레지스터////////////////////////////////////////////////////////
        std::bitset<32> MEM_result_for_lw = 0;

        int MEM_WB_pc = 0;
        std::vector<int> MEM_WB_index {-1,-1};
        std::string MEM_WB_instruction_name = "none";
        std::string MEM_WB_instruction_type = "none";
        std::bitset<32> MEM_WB_ALU_result = 0; //arithmatic인 경우 사용
        bool MEM_WB_register_write = false;
        int MEM_WB_rt = -1; 
        int MEM_WB_rd = -1;       
        std::bitset<32> MEM_WB_data = 0; //lw,lb인 경우 사용
        
        ////////////////////////////////////////////////////////////////////////////
        std::deque<std::string> instruction_pipeline = {"none", "none", "none", "none", "none"};


        while (instruction_address[instruction_memory_big_index][0] <= instruction_address.back()[0]) {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //DATA HAZARD 감지 해야함 RAW -> 이름 비교 or rs_rt 비교해야함

            ////////////////////////////////////////////////////////////IF STAGE 시작 //////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////Fetching 단계에서는 pc+4와 instruction memory에 접근해서 instruction을 꺼내옴.////////////
            instruction_pipeline.pop_back();
            std::string instruction_32bit_string;
            for(int n = 0; n < 4; n++){
                instruction_32bit_string += instruction_memory[instruction_memory_big_index][n];             /////instruction memory에 접근 + 32bit instruction 가져옴.
            }
            instruction_pipeline.push_front(instruction_32bit_string);

            IF_pc = instruction_address[instruction_memory_big_index][instruction_memory_small_index];
            IF_index = {instruction_memory_big_index, instruction_memory_small_index}; 



            ////////////////////////////////////////////////////////////ID STAGE 시작 //////////////////////////////////////////////////////////////////////////
            //pipeline[1].id_stage(R,I,J);
            if (IF_ID_pc != 0) {
                std::vector<int> int_instruction;
                std::string ID_stage_type_checking = "none";
                ID_register_write = false; // 초기화 시켜주기 위해서

                if (ID_stage_type_checking == "none") {
                    int_instruction = change_to_R_format_instruction(string_instruction);
                    for (auto r = R.begin(); r != R.end(); r++) {
                        if ((r->opcode) == int_instruction[0] && (r->funct) == int_instruction[5]) {
                            std::cout << r->instruction_name << std::endl; ////test////
                            ID_rs = r->rs = int_instruction[1];
                            ID_rt = r->rt = int_instruction[2];
                            ID_rd = r->rd = int_instruction[3];
                            ID_shamt = r->shamt = int_instruction[4];
                            ID_instruction_type = (r->type_name);
                            ID_instruction_name = (r->instruction_name);
                            ID_r_format = &(*r);
                            ID_i_format = nullptr;
                            ID_j_format = nullptr;
                            if(r->rd != -1){
                                ID_register_write = true;
                            }
                            break;
                        }
                    }
                }

                if (ID_stage_type_checking == "none") {
                    int_instruction = change_to_I_format_instruction(string_instruction);
                    for (auto i = I.begin(); i != I.end(); i++) {
                        if (i->opcode == int_instruction[0]) {
                            std::cout << i->instruction_name << std::endl; ////test////
                            ID_rs = i->rs = int_instruction[1];
                            ID_rt = i->rt = int_instruction[2];
                            ID_imm = i->immediate_or_address = int_instruction[3];
                            ID_instruction_type = (i->type_name);
                            ID_instruction_name = (i->instruction_name);
                            ID_r_format = nullptr;
                            ID_i_format = &(*i);
                            ID_j_format = nullptr;
                            
                            if(i->rt != -1){
                                if(ID_instruction_name != "sw" || ID_instruction_name != "sb" ){
                                    ID_register_write = true;
                                }
                            }
                            break;
                        }
                    }
                }
                if (ID_stage_type_checking == "none") {
                    ///////////////////////////////////////////////////////j foramt은 id stage에서 종료됨///////////////////////////////////////////////////////////
                    int_instruction = change_to_J_format_instruction(string_instruction);
                    for (auto j = J.begin(); j != J.end(); j++) {
                        if (j->opcode == int_instruction[0]) {
                            std::cout << j->instruction_name << std::endl;
                            ID_imm =j->jump_target = int_instruction[1];
                            ID_instruction_type = (j->type_name);
                            ID_instruction_name = (j->instruction_name);
                            ID_r_format = nullptr;
                            ID_i_format = nullptr;
                            ID_j_format = &(*j);
                            if (j->instruction_name == "jump") {
                                j->j_operation(IF_ID_pc);
                            }
                            break;
                        }
                    }
                }
            }


            ////////////////////////////////////////////////////////////EX STAGE 시작 //////////////////////////////////////////////////////////////////////////
            if (ID_EX_pc != 0){
                if (ID_EX_instruction_type == "R") {
                    EX_result = ID_EX_r_formats->operation(my_register); ///jr 처리해주기
                }
                if (ID_EX_instruction_type == "I") {

                    if (ID_EX_instruction_name == "lw" || ID_EX_instruction_name == "lb" || ID_EX_instruction_name == "sw" || ID_EX_instruction_name == "sb") {
                        std::vector<int> new_index = ID_EX_i_formats->address_idx_cal_lw_lb_sw_sb(my_register, data_address);
                        //EX_MEM_BRANCH_TARGET = new_index;
                        
                        std::cout << ID_EX_instruction_name << "'s ex_stage(calculation address) complete" << std::endl;
                        int address = data_address[new_index[0]][new_index[1]]; //address찾았음
                        EX_result = std::bitset<32>(address);
                    }

                    if (ID_EX_instruction_name == "beq" || ID_EX_instruction_name == "bne") {
                        std::vector<int> new_index;
                        if (ID_EX_instruction_name == "beq") {
                            new_index = ID_EX_i_formats->operation_beq(my_register, instruction_address, ID_EX_index[0], ID_EX_index[1]);
                        }
                        if (ID_EX_instruction_name == "bne") {
                            new_index = ID_EX_i_formats->operation_bne(my_register, instruction_address, ID_EX_index[0], ID_EX_index[1]);
                        }
                        if (new_index[0] != -1) { //beq가 성립한다는 뜻
                            int address = instruction_address[new_index[0]][new_index[1]];///new_index의 주소 리턴
                            EX_result = std::bitset<32>(address);
                        }
                        else {
    
                        }
                    }
                    else {
                        EX_result = ID_EX_i_formats->operation(my_register);
                    }
                }
                if (ID_EX_instruction_type == "J") {

                }                
            }


            ////////////////////////////////////////////////////////////MEM STAGE 시작 //////////////////////////////////////////////////////////////////////////
            if(EX_MEM_pc !=0){
                if (EX_MEM_instruction_type == "I") {
                    int32_t address = int32_t(EX_MEM_ALU_result.to_ullong());
                    std::vector<int> address_index = find_address_index_for_branch(data_address, address);
                    //////////////////////////////////////////////////////// lw, lb는 wb stage에서 register에 써줘야함////////////////////////////////////////////////////
                    if (EX_MEM_instruction_name == "lw" || EX_MEM_instruction_name == "lb") {
                        std::bitset<32> word = EX_MEM_i_formats->operation_lw_or_lb(data_memory, address_index); ///data를 가져왔음.

                        std::cout << EX_MEM_instruction_name << " is started " << std::endl;
                        std::cout << "the index of address is " << address_index[0] << "," << address_index[1] << std::endl;
                        
                        MEM_result_for_lw = word; ///////////  string으로 된 32bit data memory의 주소 리턴.
                    }
                    //////////////////////////////////////////////////////// sw, sb는 mem stage에서 종료///////////////////////////////////////////////////////////////////
                    if (EX_MEM_instruction_name == "sw" || EX_MEM_instruction_name == "sb") {
                        EX_MEM_i_formats->operation_sw_or_sb(my_register, data_memory, address_index);
                        if (EX_MEM_instruction_name == "sw") {
                            std::cout << "sw complete" << std::endl;
                        }
                        else {
                            std::cout << "sb complete" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "mem_stage of " << EX_MEM_instruction_name << ". This instruction doesn't need memory access" << std::endl;
                }
            }

            ////////////////////////////////////////////////////////////WB STAGE 시작 //////////////////////////////////////////////////////////////////////////
            if(MEM_WB_pc != 0){
                if (MEM_WB_instruction_type == "R") {
                    int rd = MEM_WB_rd;
                    my_register[rd] = MEM_WB_ALU_result; // e
                    std::cout << MEM_WB_instruction_name << "'s wb stage is completed" << std::endl;
                }
                if (MEM_WB_instruction_type == "I") {
                    int rt = MEM_WB_rt;
                    if (MEM_WB_instruction_name == "lw" || MEM_WB_instruction_name == "lb") {
                        my_register[rt] = MEM_WB_data;
                        std::cout << MEM_WB_instruction_name << "'s lw or lb stage is completed" << std::endl;
                    }
                }
                if (MEM_WB_instruction_type == "J") {
                }
            }

            //////////////////////////////////////////////////////////상태레지스터 업데이트//////////////////////////////////////////////////////////////////////////
            MEM_WB_pc = EX_MEM_pc;
            MEM_WB_index = EX_MEM_index;
            MEM_WB_instruction_name = EX_MEM_instruction_name;
            MEM_WB_instruction_type = EX_MEM_instruction_type;
            MEM_WB_ALU_result = EX_MEM_ALU_result;
            MEM_WB_rt = EX_MEM_rt;
            MEM_WB_rd = EX_MEM_rd;
            MEM_WB_data = MEM_result_for_lw; // MEM STAGE의 결과값 저장.
            MEM_WB_register_write = EX_MEM_register_write;

            EX_MEM_pc = ID_EX_pc;
            EX_MEM_index = ID_EX_index;
            EX_MEM_instruction_name = ID_EX_instruction_name;
            EX_MEM_rt = ID_EX_rt;
            EX_MEM_rd = ID_EX_rd;
            EX_MEM_ALU_result = EX_result; // EX stage의 결과값을 저장.
            EX_MEM_i_formats = ID_EX_i_formats;
            EX_MEM_register_write = ID_EX_register_write;
            

            ID_EX_pc = IF_ID_pc;
            ID_EX_index = IF_ID_index;
            ID_EX_instruction_type = ID_instruction_type;
            ID_EX_instruction_name = ID_instruction_name;
            ID_EX_rs = ID_rs;
            ID_EX_rt = ID_rt;
            ID_EX_rd = ID_rd;
            ID_EX_r_formats = ID_r_format; //어떤 format의 인스턴스인지 확인하기 위해서
            ID_EX_i_formats = ID_i_format;
            ID_EX_j_formats = ID_j_format;
            ID_EX_register_write = ID_register_write;

            IF_ID_pc = IF_pc;
            IF_ID_index = IF_index;//if stage에 있던 pc를 if/id stage에 전달

            instruction_memory_big_index++ ; //pc+4와 동일

            ///해저드체킹
            /////EX/MEM TO EX FORWARDING
            if(EX_MEM_register_write == true){
                if(EX_MEM_instruction_type == "R"){
                    if(EX_MEM_rd != -1){
                        if(EX_MEM_rd == ID_EX_rt){
                            std::cout << "EX/MEM rd, ID_EX rt 데이터 해저드 발생" << std::endl;
                            my_register[ID_EX_rt] = EX_MEM_ALU_result; 
                        }
                        if(EX_MEM_rd == ID_EX_rs){
                            std::cout << "EX/MEM rd, ID_EX_rs 데이터 해저드 발생" << std::endl;
                            my_register[ID_EX_rs] = EX_MEM_ALU_result;
                        }
                    }
                }
                if(EX_MEM_instruction_type == "I"){//////////////////////addiu, andi, lui,ori, sltiu 인 경우
                    if(EX_MEM_rt != -1){
                        if(EX_MEM_rt == ID_EX_rt){
                            std::cout << "EX/MEM rt, ID/EX rt 데이터 해저드 발생" << std::endl;
                            my_register[ID_EX_rt] = EX_MEM_ALU_result; 
                        }
                        if(EX_MEM_rt == ID_EX_rs){
                            std::cout << "EX/MEM rt, ID/EX rs 데이터 해저드 발생" << std::endl;
                            my_register[ID_EX_rs] = EX_MEM_ALU_result; 
                        }
                    }
                }

                ////MEM/WB to EX FORWARDING 
                if(MEM_WB_register_write == true){
                    if(MEM_WB_instruction_type == "R"){
                        if(MEM_WB_rd != -1){
                            if(EX_MEM_rd != ID_EX_rs){
                                if(MEM_WB_rd == ID_EX_rs){
                                    std::cout << "MEM/WB FORWARDING, ID/EX rs 데이터 해저드 발생" << std::endl;
                                    my_register[ID_EX_rs] = MEM_WB_ALU_result;
                                }
                            }
                            if(EX_MEM_rd != ID_EX_rt){
                                if(MEM_WB_rd == ID_EX_rt){
                                    std::cout << "MEM/WB FORWARDING, 데이터 해저드 발생" << std::endl;
                                    my_register[ID_EX_rt] = MEM_WB_ALU_result;
                                }
                            }
                        }
                    }
                    if(MEM_WB_instruction_type == "I"){
                        if(MEM_WB_rt != -1){
                            if(EX_MEM_rt != ID_EX_rs){
                                if(MEM_WB_rt == ID_EX_rs){
                                    std::cout << "MEM/WB FORWARDING, ID/EX rs 데이터 해저드 발생" << std::endl;
                                    my_register[ID_EX_rs] = MEM_WB_ALU_result;
                                }
                            }
                            if(EX_MEM_rt != ID_EX_rt){
                                if(MEM_WB_rt == ID_EX_rt){
                                    std::cout << "MEM/WB FORWARDING, 데이터 해저드 발생" << std::endl;
                                    my_register[ID_EX_rt] = MEM_WB_ALU_result;
                                }
                            }
                        }
                    }
                }

                //////////////////////
            }





        
        }
        
        


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////.///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    Label:
        while (instruction_memory_big_index < instruction_memory.size())
        {
            bool branch_switch = false;

            if (exist_n == true)
            {
                if (instruction_memory_big_index >= command_n_instruction)
                {
                    break;
                }
            } ///실행할 instruction 개수(-n 명령어)

            std::string type_checking = "none";
            if (type_checking == "none")
            {
                ////R format일 때 동작
                std::vector<int> int_instruction = change_to_R_format_instruction(instruction_memory[instruction_memory_big_index]); ///32비트 2진수 instruction을 r format 형태의 10진수 vector instruction으로 바꾸기.
                for (auto r = R.begin(); r != R.end(); r++)
                {
                    if ((r->opcode) == int_instruction[0] && (r->funct) == int_instruction[5])
                    {
                        std::cout << r->instruction_name << std::endl;
                        type_checking = r->type_name;
                        r->rs = int_instruction[1];
                        r->rt = int_instruction[2];
                        r->rd = int_instruction[3];
                        r->shamt = int_instruction[4];
                        if (r->instruction_name == "jr")
                        {
                            std::vector<int> index = r->operation_jr(my_register, instruction_address);
                            //minus_big_small_index(index);
                            instruction_memory_big_index = index[0];
                            instruction_memory_small_index = index[1];
                            break;
                            ///pc_num에서 rs와 같은ㅇ 값을 찾고 index만큼 iter을 instruction_memory.begin() + index
                        }
                        else
                        {
                            r->operation(my_register);

                            instruction_memory_big_index++;
                            instruction_memory_small_index = 0;
                            break;
                        }


                    }
                }
            } ///checked

            if (type_checking == "none")
            {
                std::vector<int> int_instruction = change_to_I_format_instruction(instruction_memory[instruction_memory_big_index]); ///32비트 2진수 instruction을 I format 형태의 10진수 vector instruction으로 바꾸기.
                for (auto i = I.begin(); i != I.end(); i++)
                {
                    if (i->opcode == int_instruction[0])
                    {
                        std::cout << i->instruction_name << std::endl;
                        type_checking = i->type_name;
                        i->rs = int_instruction[1];
                        i->rt = int_instruction[2];
                        i->immediate_or_address = int_instruction[3];
                        if (i->instruction_name == "beq")
                        {
                            std::vector<int> new_instruction_memory_idx = i->operation_beq(my_register, instruction_address, instruction_memory_big_index, instruction_memory_small_index);
                            if (new_instruction_memory_idx[0] != -1)
                            {
                                branch_switch = true;
                                instruction_memory_big_index = new_instruction_memory_idx[0];
                                instruction_memory_small_index = new_instruction_memory_idx[1];
                            }
                            else {
                                instruction_memory_big_index++;
                                instruction_memory_small_index = 0;
                            }
                            break;
                        } ///checked
                        if (i->instruction_name == "bne")
                        {
                            std::vector<int> new_instruction_memory_idx = i->operation_bne(my_register, instruction_address, instruction_memory_big_index, instruction_memory_small_index);
                            if (new_instruction_memory_idx[0] != -1)
                            {
                                branch_switch = true;
                                instruction_memory_big_index = new_instruction_memory_idx[0];
                                instruction_memory_small_index = new_instruction_memory_idx[1];
                            }
                            else {
                                instruction_memory_big_index++;
                                instruction_memory_small_index = 0;
                            }
                            break;
                        } ///checked
                        if (i->instruction_name == "lw" | i->instruction_name == "lb")
                        {
                            i->operation_lw_or_lb(my_register, data_memory, data_address);

                            instruction_memory_big_index++;
                            instruction_memory_small_index = 0;
                            break;
                        } ///checked
                        if (i->instruction_name == "sw" | i->instruction_name == "sb")
                        {
                            i->operation_sw_or_sb(my_register, data_memory, data_address);

                            instruction_memory_big_index++;
                            instruction_memory_small_index = 0;
                            break;
                        }
                        else
                        {
                            i->operation(my_register);

                            instruction_memory_big_index++;
                            instruction_memory_small_index = 0;
                            break;
                        } //checked


                    }
                }
            }

            if (type_checking == "none")
            {
                std::vector<int> int_instruction = change_to_J_format_instruction(instruction_memory[instruction_memory_big_index]); ///32비트 2진수 instruction을 J format 형태의 10진수 vector instruction으로 바꾸기.
                for (auto j = J.begin(); j != J.end(); j++)
                {
                    if (j->opcode == int_instruction[0])
                    {
                        std::cout << j->instruction_name << std::endl;
                        type_checking = j->type_name;
                        j->jump_target = int_instruction[1];
                        if (j->instruction_name == "jal")
                        {
                            std::vector<int> new_idx = { instruction_memory_big_index, instruction_memory_small_index }; ///돌아올 pc를 저장해야한다.
                            //plus_big_small_index(new_idx);
                            my_register[31] = instruction_address[new_idx[0] + 1][0]; ///ra에 다음 실행할 ins의 주소를 저장. [new_idx[1]
                        }
                        std::string jump_target_32bit = j->j_operation(instruction_address, instruction_memory_big_index, instruction_memory_small_index); ////28비트의 jump_target과 instruction_or_data_address[instruction_memory_index]의 상위 4비트랑 합침
                        std::bitset<32> bitset_jump_target_32bit = change_to_bitset(jump_target_32bit);
                        int jump_target = int(bitset_jump_target_32bit.to_ullong());                                  ////32비트 2진수를 int로 바꾸고
                        std::vector<int> next_idx = find_address_index_for_branch(instruction_address, jump_target); ////pc_num에서 해당 target의 index를 찾고 다음 while문에서 실행될수있도록 1을 빼준다
                        branch_switch = true;
                        ///minus_big_small_index(new_idx);
                        instruction_memory_big_index = next_idx[0];
                        instruction_memory_small_index = next_idx[1];
                        break;
                    }


                }
            } /// checked

            if (exist_d == true)
            {
                std::cout << "Current register value: " << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;
                std::cout << "PC: 0x" << int_to_hex(instruction_address[instruction_memory_big_index][0]) << std::endl;
                std::cout << "Register:" << std::endl;
                for (int n = 0; n < my_register.size(); n++)
                {
                    std::cout << "R" << n << ": 0x" << int_to_hex(int(my_register[n].to_ullong())) << std::endl;
                }
            }

            if (branch_switch == true)
            {
                goto Label;
            }
        }

        if (exist_d == false)
        {
            std::cout << "Current register value: " << std::endl;
            std::cout << "---------------------------------------------------------------" << std::endl;
            std::cout << "PC: 0x" << int_to_hex(instruction_address[instruction_memory_big_index][0]) << std::endl;
            std::cout << "Register:" << std::endl;
            for (int n = 0; n < my_register.size(); n++)
            {
                std::cout << "R" << n << ":"
                    << " 0x" << int_to_hex(int(my_register[n].to_ullong())) << std::endl;
            }
        }

        if (exist_m == true)
        {
            std::string address_range = command_parsing[memory_n];
            int token_index;
            for (int n = 0; n < address_range.size(); n++)
            {
                if (address_range[n] == ':')
                {
                    token_index = n;
                }
            }
            std::string first_address;
            std::string second_address;
            for (int n = 0; n < token_index; n++)
            {
                first_address.push_back(address_range[n]);
            }
            for (int n = token_index + 1; n < address_range.size(); n++)
            {
                second_address.push_back(address_range[n]);
            }

            const char* first = first_address.c_str();
            int d_first_address = strtol(first, NULL, 16);

            const char* second = second_address.c_str();
            int d_second_address = strtol(second, NULL, 16);

            std::cout << "Memory content [" << first_address << ".." << second_address << "]" << std::endl;
            std::cout << "Current register value: " << std::endl;
            std::cout << "---------------------------------------------------------------" << std::endl;
            for (int n = d_first_address; n < d_second_address + 1; n += 4)
            {
                std::cout << "0x" << int_to_hex(n) << ":";
                if (n >= start_data_address && n <= end_data_address)
                {
                    std::vector<int> index = find_address_index_for_branch(data_address, n);
                    std::vector<std::string> temp = data_memory[index[0]];
                    std::cout << temp[0] + temp[1] + temp[2] + temp[3] << std::endl;
                }
                else
                {
                    if (n >= start_instruction_address && n <= end_instruction_address)
                    {
                        std::vector<int> index = find_address_index_for_branch(instruction_address, n);
                        std::vector<std::string> temp = instruction_memory[index[0]];
                        std::cout << temp[0] + temp[1] + temp[2] + temp[3] << std::endl;
                    }
                    else
                    {
                        std::cout << "0x0" << std::endl;
                    }
                }
            }
        }
    }
}
