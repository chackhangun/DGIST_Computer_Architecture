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
/*
void plus_big_small_index(std::vector<int>& index) { ////check one more
    if (index[1] == 3) {
        index[0] ++;
        index[1] = 0;
    }
    else {
        index[1] ++;
    }
}
void minus_big_small_index(std::vector<int>& index) { ///check one more
    if (index[0] != 0 && index[1] == 0) {
        index[0] --;
        index[1] = 3;
    }
    else{
        if(index[1] != 0){
            index[1] --;
        }
    }
}
*/

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
    void operation(std::vector<std::bitset<32>>& my_register)
    { ////register 상태와 pc table 받아야함.
        int rs_value = int(my_register[rs].to_ullong());
        int rt_value = int(my_register[rt].to_ullong());
        int rd_value = 0;

        if (instruction_name == "addu")
        {
            rd_value = rs_value + rt_value;
            my_register[rd] = std::bitset<32>(rd_value);
            return;
        } ////checked

        if (instruction_name == "and")
        {
            my_register[rd] = (my_register[rs] & my_register[rt]);
            return;
        } ///checked

        if (instruction_name == "nor")
        {
            my_register[rd] = (~(my_register[rs] | my_register[rt]));
            return;
        } ///checked

        if (instruction_name == "or")
        {
            my_register[rd] = (my_register[rs] | my_register[rt]);
            return;
        } ///checked

        if (instruction_name == "sltu")
        {
            if (rs_value < rt_value)
            {
                my_register[rd] = 1;
            }
            else
            {
                my_register[rd] = 0;
            }
            return;
        } ///checked

        if (instruction_name == "sll")
        {
            my_register[rd] = (my_register[rt] << shamt);
            return;
        } ///checked

        if (instruction_name == "srl")
        {
            my_register[rd] = (my_register[rt] >> shamt);
            return;
        } ///checked

        if (instruction_name == "subu")
        {
            rd_value = rs_value - rt_value;
            my_register[rd] = std::bitset<32>(rd_value);
            return;
        } ///checked
    }

    std::vector<int> operation_jr(std::vector<std::bitset<32>> my_register, std::vector<std::vector<int>> instruction_address)
    { ///pc_num에서 rs안에 들어있는 주소와 같은 값의 index를 구한다.
        int decimal_num = int(my_register[rs].to_ulong());
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
    std::vector<int> operation_beq(std::vector<std::bitset<32>> my_register, std::vector<std::vector<int>> instruction_address, int big_index, int small_index)
    { ///분기해야할 pc index찾아주기
        if (my_register[rs].to_string() == my_register[rt].to_string())
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

    std::vector<int> operation_bne(std::vector<std::bitset<32>> my_register, std::vector<std::vector<int>> instruction_address, int big_index, int small_index)
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

    void operation_lw_or_lb(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<std::string>>& data_memory, std::vector<std::vector<int>>& data_address)
    {
        int rs_value = int(my_register[rs].to_ullong());
        int address = rs_value + immediate_or_address;
        std::vector<int> address_index = find_address_index_for_branch(data_address, address);
        if (instruction_name == "lw")
        {
            my_register[rt] = change_to_bitset(data_memory[address_index[0]][address_index[1]]); ////std::vector<std::bitset<32> my_register[0] = 15; 하면 2진수로 들어가는가 체크체크하기
        }
        else
        {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++)
            {
                one_byte_data.push_back(data_memory[address_index[0]][address_index[1]][n]);
            }
            std::string num_extension = value_sign_extension(one_byte_data, 24);
            my_register[rt] = change_to_bitset(num_extension);
        }
    } //checked

    void operation_sw_or_sb(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<std::string>>& data_memory, std::vector<std::vector<int>>& data_address)
    {
        int rt_value = int32_t(my_register[rt].to_ulong());
        int rs_value = int32_t(my_register[rs].to_ulong());
        int address = rs_value + immediate_or_address;
        std::vector<int> address_index = find_address_index_for_branch(data_address, address);
        if (instruction_name == "sw")
        {
            data_memory[address_index[0]][address_index[1]] = std::bitset<32>(rt_value).to_string();
        }
        else
        {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++)
            {
                one_byte_data.push_back(data_memory[address_index[0]][address_index[1]][n]);
            }
            data_memory[address_index[0]][address_index[1]] = value_sign_extension(one_byte_data, 24);
        }
    }

    void operation(std::vector<std::bitset<32>>& my_register)
    {
        int rs_value = int(my_register[rs].to_ullong());
        int rt_value = int(my_register[rt].to_ullong());

        if (instruction_name == "addiu")
        {                                                                                                        ///////rs와 sign-extened된 immediate의 합을 rt에 저장.
            std::string extension = value_sign_extension(std::bitset<16>(immediate_or_address).to_string(), 16); ///32비트 extension된 immediate
            std::bitset<32> bitset_extension = change_to_bitset(extension);
            int32_t rt_value = (int32_t)bitset_extension.to_ulong() + rs_value;
            my_register[rt] = std::bitset<32>(rt_value);
            return;
        } ////checked

        if (instruction_name == "andi")
        {
            my_register[rt] = (my_register[rs] & std::bitset<32>(immediate_or_address));
            return;
        } ///checked

        if (instruction_name == "lui")
        {
            std::bitset<16> imm_16bit(immediate_or_address);
            for (int idx = 0; idx < 32; idx++)
            {
                if (idx < 16)
                {
                    my_register[rt][idx] = 0;
                }
                else
                {
                    my_register[rt][idx] = imm_16bit[idx - 16];
                }
            }
            return;
        } ///checked

        if (instruction_name == "ori")
        {
            my_register[rt] = (my_register[rs] | std::bitset<32>(immediate_or_address));
            return;
        } ///checked

        if (instruction_name == "sltiu")
        { ///rs가 sign_extened imm보다 작으면 rt 1, 아니면 0
            std::string a = (value_sign_extension(std::bitset<16>(immediate_or_address).to_string(), 16));
            std::bitset<32> b = change_to_bitset(a);
            if (rs_value < int32_t(b.to_ulong()))
            {
                my_register[rt] = 1;
                return;
            }
            else
            {
                my_register[rt] = 0;
                return;
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
    std::string j_operation(std::vector<std::vector<int>> instruction_address, int big_index, int small_index)
    { ///checked
        jump_target = jump_target * 4;

        std::bitset<28> jump_target_28bit(jump_target);
        std::string str_jump_target_28bit = jump_target_28bit.to_string();

        std::bitset<32> pc_32bit(instruction_address[big_index][small_index]);
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
        std::vector<std::bitset<32>> my_register(32, 0); ///0-31 reg 초기화

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
            int decimal_num;
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //// 바이너리 파일 읽어오기
            while (!reading.eof())
            {   
                getline(reading, hex_num);
                const char* transform = hex_num.c_str();
                decimal_num = strtol(transform, NULL, 16); ////16진수 10진수로 바꾸기
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
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////./////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int instruction_memory_big_index = 0;
        int instruction_memory_small_index = 0;
        int data_memory_big_index = 0;
        int data_memory_small_index = 0;

        while (instruction_memory_big_index < instruction_memory.size())
        {
        Label:
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
                for (auto r = R.begin(); r != R.end(); r++)
                {
                    std::vector<int> int_instruction = change_to_R_format_instruction(instruction_memory[instruction_memory_big_index]); ///32비트 2진수 instruction을 r format 형태의 10진수 vector instruction으로 바꾸기.
                    if ((r->opcode) == int_instruction[0] && (r->funct) == int_instruction[5])
                    {
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
                            break;
                        }
                    }
                }
            } ///checked

            if (type_checking == "none")
            {
                for (auto i = I.begin(); i != I.end(); i++)
                {
                    std::vector<int> int_instruction = change_to_I_format_instruction(instruction_memory[instruction_memory_big_index]); ///32비트 2진수 instruction을 I format 형태의 10진수 vector instruction으로 바꾸기.
                    if (i->opcode == int_instruction[0])
                    {
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
                            break;
                        } ///checked
                        if (i->instruction_name == "lw" | i->instruction_name == "lb")
                        {
                            i->operation_lw_or_lb(my_register, data_memory, data_address);
                            break;
                        } ///checked
                        if (i->instruction_name == "sw" | i->instruction_name == "sb")
                        {
                            i->operation_sw_or_sb(my_register, data_memory, data_address);
                            break;
                        }
                        else
                        {
                            i->operation(my_register);
                            break;
                        } //checked
                    }
                }
            }

            if (type_checking == "none")
            {
                for (auto j = J.begin(); j != J.end(); j++)
                {
                    std::vector<int> int_instruction = change_to_J_format_instruction(instruction_memory[instruction_memory_big_index]); ///32비트 2진수 instruction을 J format 형태의 10진수 vector instruction으로 바꾸기.
                    if (j->opcode == int_instruction[0])
                    {
                        type_checking = j->type_name;
                        j->jump_target = int_instruction[1];
                        if (j->instruction_name == "jal")
                        {
                            std::vector<int> new_idx = { instruction_memory_big_index, instruction_memory_small_index }; ///돌아올 pc를 저장해야한다.
                            //plus_big_small_index(new_idx);
                            my_register[31] = instruction_address[new_idx[0]][new_idx[1]]; ///ra에 다음 실행할 ins의 주소를 저장.
                        }
                        std::string jump_target_32bit = j->j_operation(instruction_address, instruction_memory_big_index, instruction_memory_small_index); ////28비트의 jump_target과 instruction_or_data_address[instruction_memory_index]의 상위 4비트랑 합침
                        std::bitset<32> bitset_jump_target_32bit = change_to_bitset(jump_target_32bit);
                        int jump_target = int(bitset_jump_target_32bit.to_ulong());                                  ////32비트 2진수를 int로 바꾸고
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
                    std::cout << "R" << n << ": 0x" << int_to_hex(int(my_register[n].to_ulong())) << std::endl;
                }
            }

            if (branch_switch == true)
            {
                goto Label;
            }

            instruction_memory_big_index++;
            instruction_memory_small_index = 0;
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
                    << " 0x" << int_to_hex(int(my_register[n].to_ulong())) << std::endl;
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
            std::cout << "Memory content [0x" << int_to_hex(d_first_address) << "..0x" << int_to_hex(d_second_address) << "]" << std::endl;
            std::cout << "Current register value: " << std::endl;
            std::cout << "---------------------------------------------------------------" << std::endl;
            for (int n = d_first_address; n < d_second_address + 1; n +=4)
            {   
                std::cout << "0x" << int_to_hex(n) << ":";
                if (n >= start_data_address && n <= end_data_address)
                {
                    std::vector<int> index = find_address_index_for_branch(data_address, n);
                    std::cout << data_memory[index[0]][index[1]] << std::endl;
                }
                if (n >= start_instruction_address && n <= end_instruction_address)
                {
                    std::vector<int> index = find_address_index_for_branch(instruction_address, n);
                    std::cout << instruction_memory[index[0]][index[1]] << std::endl;
                }
                else
                {
                    std::cout << "0x0" << std::endl;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}
