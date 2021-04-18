//
// SE379 Introduction to Computer Architecture, Prof. Daehoon Kim, Department of ICE, DGIST
//
// Project 2: Simple MIPS Emulator
//

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <bitset>
#include <string>
#include <iomanip>

#define FALSE 0
#define TRUE 1
#define _REG 32

using namespace std;

short SLL = 0; 
short SRL = 2; 
short J = 2;
short JAL = 3; 
short BEQ = 4; 
short BNE = 5;
short JR = 8; 
short ADDIU = 9; 
short ANDI = 12;
short SLTIU = 11; 
short ORI = 13; 
short LUI = 15;
short ADDU = 33; 
short LW = 35;
short SUBU = 35;
short AND = 36; 
short OR = 37; 
short NOR = 39;
short SLTU = 43;
short SW = 43; 

uint32_t MEM_TEXT_START = 0x00400000; uint32_t MEM_TEXT_SIZE = 0x00100000;
uint32_t MEM_DATA_START = 0x10000000; uint32_t MEM_DATA_SIZE = 0x00100000;

typedef struct CPU_STATE 
{
    uint32_t PC_VALUE; uint32_t REGISTER[_REG];
} CPU_STATE;

typedef struct INSTRUTION_SET 
{
    short _OPcode; short _FunctionCode;

    union 
    {
        struct 
        {
            unsigned char _RScode; unsigned char _RTcode;

            union 
            {
                short _Immediatecode;

                struct 
                {
                    unsigned char _RDcode; unsigned char _Shiftcode;

                } Rformat;

            } R_Iformat;

        } R_Iformat;

        uint32_t _TargetAddress;

    } R_Tformat;

    uint32_t Value;

} Instruction;

typedef struct 
{
    uint32_t START_POINT, SIZE; uint8_t *MEMORY;

} MEMORY_STATE;

CPU_STATE CURRENT_STATE;

Instruction *INST_INFO;

int NUM_INST; int RUN_BIT;

int INSTRUCTION_COUNT;

int TEXT_SIZE; int DATA_SIZE;

MEMORY_STATE MEM_REGIONS[] = 
{
    { MEM_TEXT_START, MEM_TEXT_SIZE, NULL },
    { MEM_DATA_START, MEM_DATA_SIZE, NULL },
};

void INSTRUCTION_PROCESSING(); void INITIALIZATION(char *FILENAME);

void LOAD_EMULATOR(char *FILENAME); uint32_t READ(uint32_t Address); void WRITE(uint32_t Address, uint32_t Value);

void EXECUTE(); void SIMULATE(int RecursionNumber);

char** ARGUMENT_PARSER(char *ARGUMENT, const char DELIMITER);

void DUMP_MEMORY(int START_POINT, int STOP_POINT); void DUMP_REGISTER();

Instruction INSTRUTION_PARSER(const char *BUFFER, const int INDEX); void DATA_PARSER(const char *BUFFER, const int INDEX);

int HEX2BIN(string HEX); int BIN2INT(char *BIN);

int RUN_DEFAULT_VALUE = 100;
int ZERO_DEFAULT_VALUE = 0;

int TRUE_BOOL = 1;

int DumpMemorySet = 0;
int D_NUMBER = 0;
int InstructionSetNumber = 0;
int MEM_NREGIONS_ = (sizeof(MEM_REGIONS) / sizeof(MEMORY_STATE));

int main(int argc, char *argv[]) 
{
    char **TOKENS;
    int i = RUN_DEFAULT_VALUE;

    if (argc < 2) 
    {
        cout << "Error: Can't open program file " << endl;
        exit(1);
    }
    int FirstAddress = ZERO_DEFAULT_VALUE;
    int SecondAddress = ZERO_DEFAULT_VALUE;

    INITIALIZATION(argv[argc - 1]);

    int CountNumber = 1;
    int ListNumber = ZERO_DEFAULT_VALUE;
    while (CountNumber != argc - 1) 
    {
        if (strcmp(argv[CountNumber], "-m") == 0) 
        {
            TOKENS = ARGUMENT_PARSER(argv[++CountNumber], ':');

            FirstAddress 
            = (int) strtol(*(TOKENS), NULL, 16);

            SecondAddress 
            = (int) strtol(*(TOKENS + 1), NULL, 16);
            
            if (TRUE_BOOL)
            {
                DumpMemorySet = 1;
            }
        } 
        else if (strcmp(argv[CountNumber], "-d") == 0) 
        {
            if (TRUE_BOOL)
            {
                D_NUMBER = 1;
            }
        } 
        else if (strcmp(argv[CountNumber], "-n") == 0) 
        {
            ListNumber = (int) strtol(argv[++CountNumber], NULL, 10);
            if (TRUE_BOOL)
            {
                InstructionSetNumber = 1;
            }
        } 
        else 
        {
            if(TRUE_BOOL)
            {
                cout << "Error: Can't open program file " << endl;
                exit(1);
            }
        }

        if(TRUE_BOOL)
        {
            CountNumber++;
        }
    }

    if (InstructionSetNumber) 
    {
        i = ListNumber;
    }

    if (D_NUMBER) 
    {
        for(; i > 0; i--) 
        {
            if (RUN_BIT == FALSE) 
            {
                break;
            }
            EXECUTE();
            DUMP_REGISTER();

            if (DumpMemorySet) 
            {
                DUMP_MEMORY(FirstAddress, SecondAddress);
            }
        }
    } 
    else 
    {
        SIMULATE(i);
        DUMP_REGISTER();

        if (DumpMemorySet) 
        {
            DUMP_MEMORY(FirstAddress, SecondAddress);
        }
    }
    return 0;
}


void INITIALIZATION(char *FILENAME) 
{
    for (int i = 0; i < MEM_NREGIONS_; i++) 
    {
        MEM_REGIONS[i].MEMORY 
        = new u_int8_t[MEM_REGIONS[i].SIZE];
        
        memset(MEM_REGIONS[i].MEMORY, 0, MEM_REGIONS[i].SIZE);
    }
    LOAD_EMULATOR(FILENAME);
    RUN_BIT = TRUE;
}

char **ARGUMENT_PARSER(char *ARGUMENT, const char DELIMITER) 
{
    char *Suffix = 0;
    char *Temporary = ARGUMENT;
    size_t CountNumber = 0;
    while (*Temporary) 
    {
        if (DELIMITER == *Temporary) 
        {
            CountNumber++;
            Suffix = Temporary;
        }
        Temporary++;
    }
    char **Result = 0;
    Result = new char*[CountNumber];
    
    CountNumber += Suffix < (ARGUMENT + strlen(ARGUMENT) - 1);
    CountNumber++;
    
    char _DELIMITER[2];
    _DELIMITER[0] = DELIMITER;
    _DELIMITER[1] = 0;

    if (Result) 
    {
        size_t idx = 0;
        char *TOKEN = strtok(ARGUMENT, _DELIMITER);

        while (TOKEN) 
        {
            assert(idx < CountNumber);
            *(Result + idx++) = strdup(TOKEN);
            TOKEN = strtok(0, _DELIMITER);
        }
        assert(idx == CountNumber - 1);
        *(Result + idx) = 0;
    }

    return Result;
}

void DATA_PARSER(const char *BUFFER, const int INDEX) 
{
    WRITE(MEM_DATA_START + INDEX, BIN2INT((char*) BUFFER));
}

void WRITE(uint32_t Address, uint32_t Value) 
{
    for (int i = 0; i < MEM_NREGIONS_; i++) 
    {
        if (Address >= MEM_REGIONS[i].START_POINT && Address < (MEM_REGIONS[i].START_POINT + MEM_REGIONS[i].SIZE)) 
        {
            uint32_t Offset_Value 
            = Address - MEM_REGIONS[i].START_POINT;

            MEM_REGIONS[i].MEMORY[Offset_Value + 3] 
            = (Value >> 24) & 0x000000FF;
            
            MEM_REGIONS[i].MEMORY[Offset_Value + 2] 
            = (Value >> 16) & 0x000000FF;
            
            MEM_REGIONS[i].MEMORY[Offset_Value + 1] 
            = (Value >> 8) & 0x000000FF;
            
            MEM_REGIONS[i].MEMORY[Offset_Value + 0] 
            = (Value >> 0) & 0x000000FF;
            
            return;
        }
    }
    cout << "ERROR : Programe can't write memory" << endl;
    exit(1);
}


uint32_t READ(uint32_t Address) 
{
    for (int i = 0; i < MEM_NREGIONS_; i++) 
    {
        if (Address >= MEM_REGIONS[i].START_POINT && Address < (MEM_REGIONS[i].START_POINT + MEM_REGIONS[i].SIZE)) 
        {
            uint32_t Offset_Value = Address - MEM_REGIONS[i].START_POINT;

            return (MEM_REGIONS[i].MEMORY[Offset_Value + 3] << 24) | (MEM_REGIONS[i].MEMORY[Offset_Value + 2] << 16) | (MEM_REGIONS[i].MEMORY[Offset_Value + 1] << 8) | (MEM_REGIONS[i].MEMORY[Offset_Value + 0] << 0);
        }
    }
    cout << "ERROR : Programe can't read memory" << endl;
    exit(1);
}

void SIMULATE(int RecursionNumber) 
{
    if (RUN_BIT == FALSE) 
    {
        if (TRUE_BOOL)
        {
            return;
        }
    }
    for (int i = 0; i < RecursionNumber; i++) 
    {
        if (RUN_BIT == FALSE) 
        {
            break;
        }

        if (TRUE_BOOL)
        {
            EXECUTE();
        }
    }
}

void EXECUTE() 
{
    INSTRUCTION_PROCESSING();
    INSTRUCTION_COUNT++;
}

void INSTRUCTION_PROCESSING() 
{
    Instruction INSTRUCTION = INST_INFO[(CURRENT_STATE.PC_VALUE - MEM_TEXT_START) / 4];
    unsigned char _RScode; unsigned char _RTcode; unsigned char _RDcode; unsigned char _Shiftcode; short _Immediatecode; uint32_t _TargetAddress; int instr_index;

    if (TRUE_BOOL)
    {
        CURRENT_STATE.PC_VALUE += 4;
    }
    
    if (INSTRUCTION._OPcode == 0)
    {
        _RScode = INSTRUCTION.R_Tformat.R_Iformat._RScode;
        _RTcode = INSTRUCTION.R_Tformat.R_Iformat._RTcode;
        _RDcode = INSTRUCTION.R_Tformat.R_Iformat.R_Iformat.Rformat._RDcode;
        _Shiftcode = INSTRUCTION.R_Tformat.R_Iformat.R_Iformat.Rformat._Shiftcode;

        if (INSTRUCTION._FunctionCode == ADDU)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = CURRENT_STATE.REGISTER[_RScode] 
            + CURRENT_STATE.REGISTER[_RTcode];
        }
        else if(INSTRUCTION._FunctionCode == AND)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = CURRENT_STATE.REGISTER[_RScode] 
            & CURRENT_STATE.REGISTER[_RTcode];
        }
        else if(INSTRUCTION._FunctionCode == JR)
        {
            CURRENT_STATE.PC_VALUE 
            = CURRENT_STATE.REGISTER[_RScode];
        }
        else if(INSTRUCTION._FunctionCode == NOR)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = ~(CURRENT_STATE.REGISTER[_RScode] 
            | CURRENT_STATE.REGISTER[_RTcode]);
        }
        else if(INSTRUCTION._FunctionCode == OR)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = CURRENT_STATE.REGISTER[_RScode] 
            | CURRENT_STATE.REGISTER[_RTcode];
        }
        else if(INSTRUCTION._FunctionCode == SLTU)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = (CURRENT_STATE.REGISTER[_RScode]
             < CURRENT_STATE.REGISTER[_RTcode]) ? 1 : 0;
        }
        else if(INSTRUCTION._FunctionCode == SLL)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = CURRENT_STATE.REGISTER[_RTcode] << _Shiftcode;
        }
        else if(INSTRUCTION._FunctionCode == SRL)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = CURRENT_STATE.REGISTER[_RTcode] >> _Shiftcode;
        }
        else if(INSTRUCTION._FunctionCode == SUBU)
        {
            CURRENT_STATE.REGISTER[_RDcode] 
            = CURRENT_STATE.REGISTER[_RScode] - CURRENT_STATE.REGISTER[_RTcode];
        }
        else
        {
            cout << "ERROR : Programe can't find a instruction" << endl;
        }
    }
    else if (INSTRUCTION._OPcode == J)
    {
        _TargetAddress 
        = INSTRUCTION.R_Tformat._TargetAddress;

        CURRENT_STATE.PC_VALUE 
        = (CURRENT_STATE.PC_VALUE & 0xF0000000) 
        | (_TargetAddress << 2);
    }
    else if (INSTRUCTION._OPcode == JAL)
    {
        _TargetAddress 
        = INSTRUCTION.R_Tformat._TargetAddress;

        CURRENT_STATE.REGISTER[31] 
        = CURRENT_STATE.PC_VALUE;

        CURRENT_STATE.PC_VALUE 
        = (CURRENT_STATE.PC_VALUE & 0xF0000000) 
        | (_TargetAddress << 2);
    }
    else
    {
        _RScode
         = INSTRUCTION.R_Tformat.R_Iformat._RScode;

        _RTcode 
        = INSTRUCTION.R_Tformat.R_Iformat._RTcode;

        _Immediatecode 
        = INSTRUCTION.R_Tformat.R_Iformat.R_Iformat._Immediatecode;

        if(INSTRUCTION._OPcode == ADDIU)
        {
            CURRENT_STATE.REGISTER[_RTcode] 
            = CURRENT_STATE.REGISTER[_RScode] 
            + (int32_t) _Immediatecode;
        }
        else if(INSTRUCTION._OPcode == ANDI)
        {
            CURRENT_STATE.REGISTER[_RTcode] 
            = CURRENT_STATE.REGISTER[_RScode] 
            & (uint32_t) (unsigned short) _Immediatecode;
        }
        else if(INSTRUCTION._OPcode == BEQ)
        {
            if (CURRENT_STATE.REGISTER[_RScode] == CURRENT_STATE.REGISTER[_RTcode]) 
            {
                CURRENT_STATE.PC_VALUE 
                += (int32_t) _Immediatecode << 2;
            }
        }
        else if(INSTRUCTION._OPcode == BNE)
        {
            if (CURRENT_STATE.REGISTER[_RScode] != CURRENT_STATE.REGISTER[_RTcode]) 
            {
                CURRENT_STATE.PC_VALUE 
                += (int32_t) _Immediatecode << 2;
            }
        }
        else if(INSTRUCTION._OPcode == LUI)
        {
            CURRENT_STATE.REGISTER[_RTcode] 
            = _Immediatecode << 16;
        }
        else if(INSTRUCTION._OPcode == LW)
        {
            CURRENT_STATE.REGISTER[_RTcode] 
            = READ(CURRENT_STATE.REGISTER[_RScode] + (int32_t) _Immediatecode);
        }
        else if(INSTRUCTION._OPcode == ORI)
        {
            CURRENT_STATE.REGISTER[_RTcode] 
            = CURRENT_STATE.REGISTER[_RScode] 
            | (uint32_t) (unsigned short) _Immediatecode;
        }
        else if(INSTRUCTION._OPcode == SLTIU)
        {
            CURRENT_STATE.REGISTER[_RTcode] 
            = (CURRENT_STATE.REGISTER[_RScode] 
            < (uint32_t) (int32_t) _Immediatecode) ? 1 : 0;
        }
        else if(INSTRUCTION._OPcode == SW)
        {
            WRITE( CURRENT_STATE.REGISTER[_RScode] + (int32_t) _Immediatecode, CURRENT_STATE.REGISTER[_RTcode]);
        }
        else
        {
            cout << "ERROR : Programe can't find a instruction" << endl;
        }
    }
    if (TRUE_BOOL)
    {
        instr_index = (CURRENT_STATE.PC_VALUE - MEM_TEXT_START) / 4;
    }

    if (instr_index >= NUM_INST) 
    {
        RUN_BIT = FALSE;
    }
}

Instruction INSTRUTION_PARSER(const char *BUFFER, const int INDEX) 
{
    Instruction INSTRUCTION = INST_INFO[INDEX];
    uint32_t inst_binary = BIN2INT((char *) BUFFER);

    WRITE(MEM_TEXT_START + INDEX, inst_binary);

    INSTRUCTION.Value 
    = inst_binary;

    INSTRUCTION._OPcode 
    = inst_binary >> 26;

    if(INSTRUCTION._OPcode == 0)
    {
        INSTRUCTION._FunctionCode = inst_binary & 0x0000003F;
        if(INSTRUCTION._FunctionCode == JR)
        {
            INSTRUCTION.R_Tformat.R_Iformat._RScode 
            = (inst_binary >> 21) & 0x0000001F;
        }
        else if(INSTRUCTION._FunctionCode == SLL || INSTRUCTION._FunctionCode == SRL)
        {
            INSTRUCTION.R_Tformat.R_Iformat._RTcode 
            = (inst_binary >> 16) & 0x0000001F;
            
            INSTRUCTION.R_Tformat.R_Iformat.R_Iformat.Rformat._RDcode 
            = (inst_binary >> 11) & 0x0000001F;
            
            INSTRUCTION.R_Tformat.R_Iformat.R_Iformat.Rformat._Shiftcode 
            = (inst_binary >> 6) & 0x0000001F;
        }
        else
        {
            INSTRUCTION.R_Tformat.R_Iformat._RScode 
            = (inst_binary >> 21) & 0x0000001F;
            
            INSTRUCTION.R_Tformat.R_Iformat._RTcode 
            = (inst_binary >> 16) & 0x0000001F;
            
            INSTRUCTION.R_Tformat.R_Iformat.R_Iformat.Rformat._RDcode 
            = (inst_binary >> 11) & 0x0000001F;  
        }
    }
    else if(INSTRUCTION._OPcode == J || INSTRUCTION._OPcode == JAL)
    {
        INSTRUCTION.R_Tformat._TargetAddress 
        = inst_binary & 0x03FFFFFF;
    }
    else
    {
        INSTRUCTION.R_Tformat.R_Iformat._RTcode 
        = (inst_binary >> 16) & 0x0000001F;
        
        INSTRUCTION.R_Tformat.R_Iformat.R_Iformat._Immediatecode 
        = inst_binary & 0x0000FFFF;

        if(INSTRUCTION._OPcode != LUI)
        {
            INSTRUCTION.R_Tformat.R_Iformat._RScode 
            = (inst_binary >> 21) & 0x0000001F;
        }
    }
    return INSTRUCTION;
}

int LE_RepetitionNumber = 0;
int LE_INDEX_TEXT = 0;

void LOAD_EMULATOR(char *FILENAME) 
{
    FILE *ProgramFile;
    int j = 0;
    char BUFFER[14];
    char binary_buffer[33];
    

    ProgramFile = fopen(FILENAME, "r");
    if (ProgramFile == NULL) 
    {
        cout << "Error: Can't open program file " << FILENAME << endl;
        exit(-1);
    }

    while (fgets(BUFFER, 14, ProgramFile) != NULL) 
    {
        string STR_BUFFER(BUFFER);
        int BINARY = HEX2BIN(STR_BUFFER);
        string binary = std::bitset<32>(BINARY).to_string();
        strcpy(binary_buffer, binary.c_str());

        if (LE_RepetitionNumber == 0) 
        {
            if (TRUE_BOOL)
            {
                TEXT_SIZE = BIN2INT(binary_buffer);
            
                NUM_INST = TEXT_SIZE / 4;
                INST_INFO = new Instruction[sizeof(Instruction) * (TEXT_SIZE / 4)];

                for (int i = 0; i < NUM_INST; i++) 
                {
                    INST_INFO[i]._FunctionCode = 0; INST_INFO[i].R_Tformat.R_Iformat._RScode = 0;
                    INST_INFO[i].R_Tformat.R_Iformat._RTcode = 0; INST_INFO[i].R_Tformat.R_Iformat.R_Iformat.Rformat._RDcode = 0;
                    INST_INFO[i].R_Tformat.R_Iformat.R_Iformat._Immediatecode = 0; INST_INFO[i].R_Tformat._TargetAddress = 0;
                    INST_INFO[i].R_Tformat.R_Iformat.R_Iformat.Rformat._Shiftcode = 0; INST_INFO[i].Value = 0; INST_INFO[i]._OPcode = 0;
                }
            }

        } 
        else if (LE_RepetitionNumber == 1) 
        {
            if (TRUE_BOOL)
            {
                DATA_SIZE = BIN2INT(binary_buffer);
            }
        } 
        else 
        {
            if (j < TEXT_SIZE) 
            {
                INST_INFO[LE_INDEX_TEXT++] = INSTRUTION_PARSER(binary_buffer, j);
            } 
            else if (j < TEXT_SIZE + DATA_SIZE) 
            {
                DATA_PARSER(binary_buffer, j - TEXT_SIZE);
            } 
            j += 4;
        }
        LE_RepetitionNumber++;
    }
    if (TRUE_BOOL)
    {
        CURRENT_STATE.PC_VALUE = MEM_TEXT_START;
    }
}

void DUMP_MEMORY(int START_POINT, int STOP_POINT) 
{
    int Address;
    
    cout << "Memory content [0x" << hex << START_POINT << "..0x" << hex <<STOP_POINT << "]:" << endl;
    cout << "------------------------------------" << endl;
    for (Address = START_POINT; Address <= STOP_POINT; Address += 4) 
    {
        cout << "0x" << hex << Address << ": 0x" << hex <<  READ(Address) << endl;
    }
    cout << endl;
}

void DUMP_REGISTER() 
{
    if (TRUE_BOOL)
    {
        int REGISTER_NUMBER;
        cout << "Current register values:" << endl;
        cout << "------------------------------------" << endl;
        cout << "PC: 0x" << hex << CURRENT_STATE.PC_VALUE << endl;
        cout <<"Registers:" << endl;

        for (REGISTER_NUMBER = 0; REGISTER_NUMBER < _REG; REGISTER_NUMBER++) 
        {
            cout << "R" << dec << REGISTER_NUMBER;
            cout << ": 0x" << hex << CURRENT_STATE.REGISTER[REGISTER_NUMBER] << endl;
        }
        cout << endl;
    }    
}

int BIN2INT(char *BIN) 
{
    return (int) strtol(BIN, NULL, 2);
}

int HEX2BIN(string HEX) 
{
    return (int)strtol(HEX.c_str(), 0, 16);
}
