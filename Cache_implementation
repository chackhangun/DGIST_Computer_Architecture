#include <iostream>

#include <fstream>

#include <string>

#include <cstring>

#include <vector>

#include <utility>

#include <cmath>

#include <bitset>

#include <stdio.h>

#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Cache_component
{

public:
    int block_off;

    int index;

    int tag;

    int valid_bit = 0;

    int dirty_bit = 0;

    int num = 0;

    Cache_component()
    {

        block_off = 0;

        index = 0;

        tag = 0;
    }

    Cache_component(int block, int idx, int t)
    {

        block_off = block;

        index = idx;

        tag = t;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void access_setting(int &total, int &read, int &write, std::string readwrite)
{

    total++;

    if (readwrite == "R")
    {

        read++;
    }

    else
    {

        write++;
    }
}

std::vector<std::vector<Cache_component>> make_cache(int asso, int set)
{

    std::vector<std::vector<Cache_component>> Cache;

    for (int i = 0; i < set; i++)
    {

        std::vector<Cache_component> cache_set(asso);

        Cache.push_back(cache_set);
    }

    return Cache;
}

std::vector<int> L1_L2_setting_value(int capacity, int associativity, int block_size)
{

    std::vector<int> container;

    int L2_set = (capacity / block_size) / associativity;

    int L1_capacity = (capacity / 4);

    int L1_asso = 0;

    if (associativity <= 3)

    {

        L1_asso = associativity;
    }

    else

    {

        L1_asso = associativity / 4;
    }

    int L1_set = (L1_capacity / block_size) / L1_asso;

    container.push_back(L1_capacity);

    container.push_back(L1_asso);

    container.push_back(L1_set);

    container.push_back(L2_set);

    std::cout << "----------------L2 cache---------------" << std::endl;

    std::cout << "L2 cache capacity: " << capacity << std::endl;

    std::cout << "L2 cache set: " << L2_set << std::endl;

    std::cout << "L2 cache associativity: " << associativity << std::endl;

    std::cout << "L2 cache block_size: " << block_size << std::endl;

    std::cout << "----------------L1 cache---------------" << std::endl;

    std::cout << "L1 cache capacity: " << L1_capacity << std::endl;

    std::cout << "L1 cache set: " << L1_set << std::endl;

    std::cout << "L1 cache associativity: " << L1_asso << std::endl;

    std::cout << "L1 cache block_size: " << block_size << std::endl;

    return container;
}

int cal_bit(int offset)
{

    std::bitset<32> num(offset);

    for (int i = 0; i < 32; i++)
    {

        if (num[i] == 1)
        {

            return i;
        }
    }
}

std::vector<long> cal_idx_tag(std::bitset<36> bit, int tagbit, int idxbit, int offsetbit)
{

    //offsetbit가 4라면 0~3까지만 본다.

    std::bitset<36> index;

    std::bitset<36> tag;

    ///idx부터

    /// idxbit가 6이라면 4~9까지

    for (int i = offsetbit; i != offsetbit + idxbit; i++)
    {

        index[i - offsetbit] = bit[i];
    }

    for (int i = offsetbit + idxbit; i != bit.size(); i++)
    {

        tag[i - (offsetbit + idxbit)] = bit[i];
    }

    long l_index = index.to_ulong();

    long l_tag = tag.to_ulong();

    std::vector<long> value = {l_index, l_tag};

    return value;
}

int find_way(std::vector<Cache_component> &set, int associativity, int tag)
{

    for (int i = 0; i < set.size(); i++)
    {

        if (set[i].tag == tag)
        {

            return i;
        }
    }

    return -1;
}

void add_block(std::vector<Cache_component> &set, int block_size, int idx, int tag)
{

    Cache_component new_block(block_size, idx, tag);

    new_block.valid_bit = 1;

    set.push_back(new_block);

    return;
}

int find_valid(std::vector<Cache_component> &set)
{

    int num = 0;

    for (int i = 0; i < set.size(); i++)
    {

        if (set[i].valid_bit == 1)
        {

            num++;
        }
    }

    return num;
}

int find_min(std::vector<Cache_component> &set)
{

    int num = 0;

    for (int i = 0; i < set.size(); i++)
    {

        if (set[i].valid_bit == 1)
        {

            num = i;

            break;
        }
    }

    return num;
}

void check_eviction(Cache_component &block, int &dirty, int &clean)
{

    if (block.dirty_bit == 1)
    {

        dirty++;
    }

    else
    {

        clean++;
    }
}

void eviction_dirty_clean_judge(std::vector<Cache_component> &set, int asso, int &dirty, int &clean, std::string type)
{

    /*

    if(set.size() != 0){

        check_eviction(set[0],dirty, clean);

        set.erase(set.begin());

    }*/

    if (type == "lru")
    {

        if (find_valid(set) < asso)
        {

            return;
        }

        if (find_valid(set) == asso)
        {

            int min = find_min(set);

            check_eviction(set[min], dirty, clean);

            set.erase(set.begin() + min);
        }
    }

    if (type == "random")
    {

        int valid = find_valid(set);

        if (valid < asso)
        {

            return;
        }

        if (valid == asso)
        {

            int random_num = rand();

            random_num = random_num % valid;

            int first = find_min(set);

            check_eviction(set[first + random_num], dirty, clean);

            set.erase(set.begin() + first + random_num);
        }
    }
}

std::string addr_recovery(std::vector<Cache_component> &set, int idx_bit, int tag_bit, int block_bit)
{

    std::bitset<36> idx(set[0].index);

    std::bitset<36> tag(set[0].tag);

    std::string str_idx = idx.to_string();

    std::string str_tag = tag.to_string();

    for (int i = 36; i != idx_bit; i--)
    {

        str_idx.erase(str_idx.begin());
    }

    for (int i = 36; i != tag_bit; i--)
    {

        str_tag.erase(str_tag.begin());
    }

    std::string recover = str_tag + str_idx;

    for (int i = 0; i < block_bit; i++)
    {

        recover.push_back('0');
    }

    //std::cout << recover << std::endl;

    return recover;
}

void dirty_bit_setting(std::vector<Cache_component> &set, int set_index, std::string readwrite)
{

    if (readwrite == "W")
    {

        set[set_index].dirty_bit = 1;
    }
}

void setting_miss_number(std::vector<int> &container, std::string readwrite)
{

    if (readwrite == "R")
    {

        container[0]++;
    }

    else
    {

        container[1]++;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

    int L2_capacity;

    int L2_associativity;

    int block_size;

    std::string replacement_policy;

    //std::cout << "argc == " << argc << std::endl;

    try
    {

        if (argc != 9)
        {

            throw 0;
        }
    }

    catch (int exception)
    {

        std::cout << "Error::You need more command" << std::endl;
    }

    std::string filename;

    for (int num = 0; num < argc; num++)
    {

        std::string command = std::string(argv[num]);

        if (command == "-c")
        {

            L2_capacity = atoi(argv[num + 1]) * pow(2, 10); //KB 단위로 저장

        }

        if (command == "-a")
        {

            L2_associativity = atoi(argv[num + 1]);

        }

        if (command == "-b")
        {

            block_size = atoi(argv[num + 1]);

        }

        if (command == "-lru" || command == "-random")
        {

            filename = argv[num + 1];

            if (command == "-lru")
            {

                replacement_policy = "lru";
            }

            else
            {

                replacement_policy = "random";
            }
        }
    }

    //// cache 구성 요소: set, way -> 2차원 배열 사용 ---- index, valid bit, dirty bit, tag, data(block)

    //// cache는 level1 / level2 cache로 구성

    //// level1은 data만

    //// level2는 inclusive--- L1의 모든 데이터는 L2에도 포함되어있어야한다.   (R -> instruction or data  W-> data)

    std::vector<int> setting = L1_L2_setting_value(L2_capacity, L2_associativity, block_size);

    int L2_set = setting[3];

    int L1_capacity = setting[0];

    int L1_associativity = setting[1];

    int L1_set = setting[2];

    //// 2의몇제곱인가는 bitset을 보고 판단할수있다.

    //// 맨뒤 block offset은 block size을 보고판단.

    int block_offset_bit = cal_bit(block_size);

    //// index비트는 cache의 set개수를 보고 판단

    int L2_index_bit = cal_bit(L2_set);

    int L1_index_bit = cal_bit(L1_set);

    //// tag비트는 남은 비트

    int L2_tag_bit = 36 - (block_offset_bit + L2_index_bit);

    int L1_tag_bit = 36 - (block_offset_bit + L1_index_bit);

    printf("-------------------------------------------------\n");

    printf("L1_index_bit : %d\n", L1_index_bit);

    printf("L1_tag_bit : %d\n", L1_tag_bit);

    printf("L2_index_bit : %d\n", L2_index_bit);

    printf("L2_tag_bit : %d\n", L2_tag_bit);

    printf("block_offset_bit : %d\n", block_offset_bit);

    printf("---------------------read start------------------\n");

    std::vector<std::vector<Cache_component>> L2_cache = make_cache(L2_associativity, L2_set);

    std::vector<std::vector<Cache_component>> L1_cache = make_cache(L1_associativity, L1_set);

    std::ifstream readfile;

    readfile.open(filename);

    std::string str;

    int line_num = 1;

    int total_access = 0;

    int read_access = 0;

    int write_access = 0;

    int L1_read_miss = 0;

    int L1_write_miss = 0;

    int L2_read_miss = 0;

    int L2_write_miss = 0;

    std::vector<int> L1_readwrite_miss = {L1_read_miss, L1_write_miss};

    std::vector<int> L2_readwrite_miss = {L2_read_miss, L2_write_miss};

    int L1_clean_eviction = 0;

    int L1_dirty_eviction = 0;

    int L2_clean_eviction = 0;

    int L2_dirty_eviction = 0;

    int L1_hit_num = 0;

    int L1_miss_num = 0;

    int L2_hit_num = 0;

    int L2_miss_num = 0;

    while (!readfile.eof())
    {

        //printf("%d\n", line_num);

        ////확인을 위해서 넣어줌.

        /*

        if(line_num % 1000000== 0){

            break;

            //std::cout << line_num << std::endl;

        }*/

        getline(readfile, str);

        ////맨 마지막 라인은 empty-> 실행 x

        if (str == "")
        {

            break;
        }

        std::string readwrite = str.substr(0, 1);

        std::string addr = str.substr(2, str.size());

        //// 주소를 보고 판단 -> 36비트로 바꿈

        std::bitset<36> bit(strtol(addr.c_str(), NULL, 16));

        //// index 구하기

        //// tag 구하기

        std::vector<long> L1_index_tag = cal_idx_tag(bit, L1_tag_bit, L1_index_bit, block_offset_bit);

        int L1_idx = int(L1_index_tag[0]);

        int L1_tag = int(L1_index_tag[1]);

        std::vector<long> L2_index_tag = cal_idx_tag(bit, L2_tag_bit, L2_index_bit, block_offset_bit);

        int L2_idx = int(L2_index_tag[0]);

        int L2_tag = int(L2_index_tag[1]);

        ////L1 hit miss 판단

        ////hit이면 해당 인덱스 가져옴

        ////miss라면 check = -1

        std::vector<Cache_component> &the_L1set = L1_cache[L1_idx];

        std::vector<Cache_component> &the_L2set = L2_cache[L2_idx];

        access_setting(total_access, read_access, write_access, readwrite);

        int L1_check = find_way(the_L1set, L1_associativity, L1_tag);

        //// cache replaement 정책

        //// 1. LRU

        //// 블락마다 LRU를 위한 숫자 필요

        //// 숫자 가장 작은 cache 블락 없앤다

        //// L1이 가득 찼을 때 -> 숫자 가장 작은 블락을 모두 빈 블락으로 만들어준다.

        //// L2가 가득 찼을 때 -> 숫자 가장 작은 블락을 빈 블락 + L1에서도 동작 수행

        //// 2. Random

        //// 아무 블락 삭제 한다

        //// L1이 가득 찼을 때 -> 아무 블락 삭제

        //// L2가 가득 찼을 때 -> 아무 블락 삭제 + L1에서 해당 블락 삭제

        ////L1 miss

        if (L1_check == -1)
        {

            L1_miss_num++;

            setting_miss_number(L1_readwrite_miss, readwrite);

            //L2를 체크해야한다.

            int L2_check = find_way(the_L2set, L2_associativity, L2_tag);

            ///L2 MISS인 경우

            if (L2_check == -1)
            {

                L2_miss_num++;

                setting_miss_number(L2_readwrite_miss, readwrite);

                if (the_L2set.size() != 0)
                {

                    ///////////////////////L2의 첫번째 블록이 L1에 있는지 검사///////////////

                    ///L2의 첫번째 블록의 주소를 다시 만들어준다.

                    std::string make_address = addr_recovery(the_L2set, L2_index_bit, L2_tag_bit, block_offset_bit);

                    std::bitset<36> recover_address(make_address);

                    std::vector<long> L1_idx_tag = cal_idx_tag(recover_address, L1_tag_bit, L1_index_bit, block_offset_bit);

                    int temp_idx = L1_idx_tag[0];

                    int temp_tag = L1_idx_tag[1];

                    std::vector<Cache_component> &temp_set = L1_cache[temp_idx];

                    int result = find_way(temp_set, L1_associativity, temp_tag);

                    /// L2의 첫번째 블록이 L1에 있는경우

                    if (result != -1)
                    {

                        Cache_component &L1_block = temp_set[result];

                        ///L1에서 지워질 블락의 dirty bit확인

                        ///L1에서 지워질 블락의 dirty bit가 1이면 L1 dirty eviction 증가

                        check_eviction(L1_block, L1_dirty_eviction, L1_clean_eviction);

                        ///L1에서 evict 시켜준다.

                        temp_set.erase(temp_set.begin() + result);

                        /////////////////////////////L2의 첫번째 블록 - L1에서 삭제 완료///////////////////////////
                    }

                    /// L2에서 첫번째 블락 지우기

                    /// L2에서 첫번째 블락의 dirty bit가 1이면, L2 dirty eviction 증가

                    eviction_dirty_clean_judge(the_L2set, L2_associativity, L2_dirty_eviction, L2_clean_eviction, replacement_policy);
                }

                add_block(the_L2set, block_size, L2_idx, L2_tag);

                dirty_bit_setting(the_L2set, the_L2set.size() - 1, readwrite);

                ////////////////////////////L1에 블락 넣기/////////////////////////

                ///L1의 첫번째 블락 지우기

                eviction_dirty_clean_judge(the_L1set, L1_associativity, L1_dirty_eviction, L1_clean_eviction, replacement_policy);

                add_block(the_L1set, block_size, L1_idx, L1_tag);

                dirty_bit_setting(the_L1set, the_L1set.size() - 1, readwrite);
            }

            ///L2 HIT인 경우

            else

            {

                L2_hit_num++;

                eviction_dirty_clean_judge(the_L1set, L1_associativity, L1_dirty_eviction, L1_clean_eviction, replacement_policy);

                ///L2의 해당블락을 카피해서 L1의 set과 index에 맞게 가져온다.

                ///L2_check으로 몇번째 인덱스에 블락이 있는지 알고있음.

                the_L1set.push_back(the_L2set[L2_check]);

                dirty_bit_setting(the_L1set, the_L1set.size() - 1, readwrite);
            }
        }

        //// hit인 경우

        else
        {

            //// read는 그냥 읽기

            //// write는 dirty bit 추가해주기.(evict될때 업데이트)

            L1_hit_num++;

            dirty_bit_setting(the_L1set, L1_check, readwrite);
        }

        line_num++;
    }

    L1_read_miss = L1_readwrite_miss[0];

    L1_write_miss = L1_readwrite_miss[1];

    L2_read_miss = L2_readwrite_miss[0];

    L2_write_miss = L2_readwrite_miss[1];

    double L1_read_missrate = (double(L1_read_miss) / double(read_access)) * 100;

    double L2_read_missrate = (double(L2_read_miss) / double(L1_read_miss)) * 100;

    double L1_write_missrate = (double(L1_write_miss) / double(write_access)) * 100;

    double L2_write_missrate = (double(L2_write_miss) / double(L1_write_miss)) * 100;

    for (int i = 0; i < 4; i++)
    {

        filename.pop_back();
    }

    std::string output_name = filename + "_" + std::to_string(int(L2_capacity / pow(2, 10))) + "_" + std::to_string(L2_associativity) + "_" + std::to_string(block_size) + ".out";

    std::ofstream file(output_name);

    file << "-- General Stats --" << std::endl;

    file << "L1 Capacity : " << L1_capacity / pow(2, 10) << std::endl;

    file << "L1 Way : " << L1_associativity << std::endl;

    file << "L2 Capacity : " << L2_capacity / pow(2, 10) << std::endl;

    file << "L2 Way : " << L2_associativity << std::endl;

    file << "Block size : " << block_size << std::endl;

    file << "Total accesses : " << total_access << std::endl;

    file << "Read accesses : " << read_access << std::endl;

    file << "Write accesses : " << write_access << std::endl;

    file << "L1 Read misses : " << L1_read_miss << std::endl;

    file << "L2 Read misses : " << L2_read_miss << std::endl;

    file << "L1 Write misses : " << L1_write_miss << std::endl;

    file << "L2 Write misses : " << L2_write_miss << std::endl;

    file << "L1 Read miss rate : " << L1_read_missrate << "%" << std::endl;

    file << "L2 Read miss rate : " << L2_read_missrate << "%" << std::endl;

    file << "L1 Write miss rate : " << L1_write_missrate << "%" << std::endl;

    file << "L2 Write miss rate : " << L2_write_missrate << "%" << std::endl;

    file << "L1 Clean eviction : " << L1_clean_eviction << std::endl;

    file << "L2 Clean eviction : " << L2_clean_eviction << std::endl;

    file << "L1 Dirty eviction : " << L1_dirty_eviction << std::endl;

    file << "L2 Dirty eviction : " << L2_dirty_eviction << std::endl;

    /*
    std::cout << "L1 HIT NUM : " << L1_hit_num << std::endl;

    std::cout << "L1 MISS NUM : " << L1_miss_num << std::endl;

    std::cout << "L2 HIT NUM : " << L2_hit_num << std::endl;

    std::cout << "L2 MISS NUM : " << L2_miss_num << std::endl;
    */
}
