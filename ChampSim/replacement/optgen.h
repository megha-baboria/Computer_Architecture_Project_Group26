//Hawkeye Cache Replacement Tool v2.0
//UT AUSTIN RESEARCH LICENSE (SOURCE CODE)
//The University of Texas at Austin has developed certain software and documentation that it desires to
//make available without charge to anyone for academic, research, experimental or personal use.
//This license is designed to guarantee freedom to use the software for these purposes. If you wish to
//distribute or make other use of the software, you may purchase a license to do so from the University of
//Texas.
///////////////////////////////////////////////
//                                            //
//     Hawkeye [Jain and Lin, ISCA' 16]       //
//     Akanksha Jain, akanksha@cs.utexas.edu  //
//                                            //
///////////////////////////////////////////////


#ifndef OPTGEN_H
#define OPTGEN_H
#define MAX_CONFIDENCE 7

using namespace std;

#include <iostream>

#include <math.h>
#include <set>
#include <vector>

struct ADDR_INFO
{
    uint64_t addr;
    uint32_t last_quanta;
    uint64_t PC; 
    bool prefetched;
    uint32_t lru;
    int confidence;

    void init(unsigned int curr_quanta)
    {
        last_quanta = 0;
        PC = 0;
        prefetched = false;
        confidence = 0;
        // cout<<confidence<<endl;
        lru = 0;
        // cout << "*************************************************" <<confidence<<"**************************"<<endl;

    }

    void update(unsigned int curr_quanta, uint64_t _pc, bool prediction)
    {
        last_quanta = curr_quanta;
        PC = _pc;
    }

    void mark_prefetch()
    {
        prefetched = true;
    }
};

struct OPTgen
{
    vector<unsigned int> liveness_history;

    uint64_t num_cache;
    uint64_t num_dont_cache;
    uint64_t access;

    uint64_t CACHE_SIZE;

    void init(uint64_t size)
    {
        num_cache = 0;
        num_dont_cache = 0;
        access = 0;
        CACHE_SIZE = size;
        liveness_history.resize(OPTGEN_VECTOR_SIZE, 0);
    }

    void add_access(uint64_t curr_quanta)
    {
        access++;
        liveness_history[curr_quanta] = 0;
    }

    void add_prefetch(uint64_t curr_quanta)
    {
        liveness_history[curr_quanta] = 0;
    }

    vector<int> should_cache(uint64_t curr_quanta, uint64_t last_quanta, int confidence)
    {
        bool is_cache = true;
        vector<int> vec;

        unsigned int i = last_quanta;
        while (i != curr_quanta)
        {
            if(liveness_history[i] >= CACHE_SIZE)
            {
                is_cache = false;
                break;
            }

            i = (i+1) % liveness_history.size();
        }

        //if ((is_cache) && (last_quanta != curr_quanta))
        if ((is_cache))
        {
            i = last_quanta;
            while (i != curr_quanta)
            {
                liveness_history[i]++;
                i = (i+1) % liveness_history.size();
            }
            assert(i == curr_quanta);
        }

        if(confidence == 0 || confidence == MAX_CONFIDENCE){
            confidence = 0;
        }

        if (is_cache) num_cache++;
        else num_dont_cache++;

        if(is_cache){
            confidence ++;
        }
        else confidence--;

        vec.push_back(confidence);

        if(confidence >= (MAX_CONFIDENCE+1)/2){
            vec.push_back(1);
        }  

        else
        vec.push_back(0);
        // cout << "*****************************megha********************" <<confidence<<" "<<vec[1]<<""<<is_cache<<"**************************"<<endl;
        
        return vec;
    }

    uint64_t get_num_opt_hits()
    {
        return num_cache;

        uint64_t num_opt_misses = access - num_cache;
        return num_opt_misses;
    }
};

#endif
