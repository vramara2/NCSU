#include<iostream>
#include<cstdlib>
#include<string>
#include<fstream>
#include<sstream>
#include<bits/stdc++.h>
#include<bitset>
using namespace std;

#include "cache_top.cpp"
#include "cache_l2_top.cpp"
//Function declaration


int main(int argc, char *argv[])
{
  //cout<<"Enjoy time designing cache!!";
  //variable declarartions//
  
  fstream input_file;
  string trace_input;
  
  //CACHE parameters//
  int l1_cache_block_size = atoi(argv[1]);
  int l1_cache_associativity = atoi(argv[3]);
  int l1_cache_capacity = atoi(argv[2]);
  int l2_cache_capacity = atoi(argv[4]);
  int l2_cache_associativity = atoi(argv[5]);
  int l2_cache_data_block_size = atoi(argv[6]);
  int l2_cache_addr_tag_sel = atoi(argv[7]);
  int l2_cache_block_size = l1_cache_block_size;
  int l2_no_of_sets = 0; 
  //Flag bits
  int cache_hit;
  int l2_cache_hit;
 
  int instruction_count = 0; 
  //Performance calculation parameters
  float read_count = 0, write_count = 0, write_hit_count = 0, write_miss_count = 0, read_hit_count = 0, read_miss_count = 0, write_back_count = 0, cache_miss_rate=0;
  
  float l2_read_count = 0, l2_write_count = 0, l2_write_hit_count = 0, l2_write_miss_count = 0, l2_read_hit_count = 0, l2_read_miss_count = 0, l2_write_back_count = 0;
  //L1 cache parameters
  int l1_index_, l1_block_offset_, l1_tag_store_;
  
  //cout<<"Cache parameters input : cache_block_size ="<<l1_cache_block_size<<" cache_associativity ="<<l1_cache_associativity<<" cache_capacity ="<<l1_cache_capacity;
  
  
  char read_or_write;
  //string address;
  int address = 0x00;
  //for initial debug purpose
  
  
  
  //Bitset//
  bitset<32> address_set;
  bitset<32> writeback_address;



  //bitset<32> l2_index = 0;
  //bitset<32> l2_block_offset = 0;
  //bitset<32> l2_data_block = 0;
  //bitset<32> l2_tag_store = 0;
  //bitset<32> l2_addr_tag_sel = 0;
  
  
  int l2_block_offset_ = 0;
  int l2_index_ = 0;
  int l2_data_block_ = 0;
  int l2_addr_tag_sel_ = 0;
  int l2_tag_store_ = 0;
  int wb_l2_block_offset_ = 0;
  int wb_l2_index_ = 0;
  int wb_l2_data_block_ = 0;
  int wb_l2_addr_tag_sel_ = 0;
  int wb_l2_tag_store_ = 0;



  Cache_generic cache_l1(l1_cache_block_size, l1_cache_capacity, l1_cache_associativity);
  //cache_l2_decouple_sectored cache_l2(l2_cache_block_size, l2_cache_capacity, l2_cache_data_block_size, l2_cache_associativity, l2_cache_addr_tag_sel); 
  Cache_generic *cache_l2_pointer;
  if(l2_cache_capacity != 0) cache_l2_pointer = new cache_l2_decouple_sectored(l2_cache_block_size, l2_cache_capacity, l2_cache_data_block_size, l2_cache_associativity, l2_cache_addr_tag_sel);
  //Calculate L1 parameters//
  int l1_no_of_sets = l1_cache_capacity/(l1_cache_block_size * l1_cache_associativity);
  //cout<<"\n L1 No of sets = "<<l1_no_of_sets;
  
  //Calculate L2 parameters
  if(l2_cache_capacity !=0) l2_no_of_sets = l2_cache_capacity/(l2_cache_block_size * l2_cache_data_block_size * l2_cache_associativity); 

  //Read file//
  input_file.open(argv[8]);
  
  //functions - file opening, address decoding, passing cache_operations//
  int iteration = 0;
  int parse_input_file();
  {
    if(input_file.is_open())
      {
      while(getline(input_file, trace_input))
        {
        instruction_count += 1;
        //getline(input_file, trace_input);
        //cout<<"\n"<<trace_input;
        istringstream ss(trace_input);
        ss >> read_or_write;
        ss >> std::hex >> address;
        address_set = address;
        //cout<<"\t"<<std::hex<<"DEBUG: Address="<<address<<"\n"<<"Read or Write = "<<read_or_write<<"\n";
        //cout<<"\tDEBUG:  Address in binary"<<address_set;
        
        //ADDRESS DECODING LOGIC FOR L1 CACHE// 
        bitset<32> l1_index = 0;
        bitset<32> l1_block_offset = 0;
        bitset<32> l1_tag_store = 0;
        int k = 0;
        int p = 0;

        for(int n=0;n<log2(l1_cache_block_size); n++)
          {
          l1_block_offset[n] = address_set[n];
          }
        //cout<<"\n";
        for(int l=log2(l1_cache_block_size); l <(log2(l1_cache_block_size) + log2(l1_no_of_sets)); l++)
          {
          l1_index[k] = address_set[l];
          k++;
          }
  
        ///l1 block offset and index calculations
        l1_block_offset_   = l1_block_offset.to_ulong();
        l1_index_          = l1_index.to_ulong();
   
        for( int m=(log2(l1_cache_block_size) + log2(l1_no_of_sets)); m<32 ; m++)
          {
          l1_tag_store[p] = address_set[m];
          //cout<<"\t"<<l1_tag_store[p]<<"\t";
          p++;
          }
         
        l1_tag_store_  = l1_tag_store.to_ulong();

        //cout<<"\t L1 BLOCK_OFFSET = "<<l1_block_offset_;
        //cout<<"\t L1 INDEX = "<<l1_index_;
        //cout<<"\t L1 TAG STORE BITS="<<l1_tag_store_;
        
        //ADDRESS DECODING FOR L2 CACHE//
        if(l2_cache_capacity !=0)
        {
          bitset<32> l2_index = 0;
          bitset<32> l2_block_offset = 0;
          bitset<32> l2_data_block = 0;
          bitset<32> l2_tag_store = 0;
          bitset<32> l2_addr_tag_sel = 0;

          for(int n=0;n<log2(l2_cache_block_size); n++)
          {
            l2_block_offset[n] = address_set[n];
          }
          //cout<<"\n";
          int l2_dsize = 0;
          for(int n=log2(l2_cache_block_size);n<(log2(l2_cache_block_size) + log2(l2_cache_data_block_size));n++)
          {
            l2_data_block[l2_dsize] = address_set[n];
            l2_dsize++; 
          } 

          int l2_index_size = 0;
          for(int l=(log2(l2_cache_block_size) + log2(l2_cache_data_block_size));l<(log2(l2_cache_block_size) + log2(l2_cache_data_block_size)+log2(l2_no_of_sets));l++)
          {
            l2_index[l2_index_size] = address_set[l];
            l2_index_size++; 
          } 
       
          int l2_addr_tag_sel_size = 0;
          int l2_addr_tag_sel_pos = log2(l2_cache_block_size) + log2(l2_cache_data_block_size)+log2(l2_no_of_sets);
          for(int m=l2_addr_tag_sel_pos;m< (l2_addr_tag_sel_pos + log2(l2_cache_addr_tag_sel));m++)
          {
            l2_addr_tag_sel[l2_addr_tag_sel_size] = address_set[m];
            l2_addr_tag_sel_size++;
          }

          int l2_tag_store_bits=0;
          int l2_tag_store_pos = l2_addr_tag_sel_pos + log2(l2_cache_addr_tag_sel);
          for(int lm=l2_tag_store_pos;lm<32;lm++)
          {
            l2_tag_store[l2_tag_store_bits] = address_set[lm];
            l2_tag_store_bits++;
          }
          
          l2_block_offset_ = l2_block_offset.to_ulong();
          l2_index_ = l2_index.to_ulong();
          l2_data_block_ = l2_data_block.to_ulong();
          l2_addr_tag_sel_ = l2_addr_tag_sel.to_ulong();
          l2_tag_store_ = l2_tag_store.to_ulong();

        }
        

        //Start to process the incoming requests!!!
        if(read_or_write == 'r') 
        {
          //cout<<"\n # "<<instruction_count<<" read "<<std::hex<<address_set;
          read_count += 1;
          cache_hit = cache_l1.cache_read_operation(l1_block_offset_, l1_index_, l1_tag_store_, 0, 0, address_set);
          //cout<<"\n Called cache_read !! with "<<cache_hit;
          //cout<<"\n Cache hit read:"<<cache_hit;
          if(cache_hit == 1) 
          { 
            read_hit_count += 1;
          }
          else if(cache_hit == 15) 
          { 
            read_miss_count += 1; write_back_count += 1;
            //CALL WRITEBACK FUNCTION OF L2 LEVEL
            if(l2_cache_capacity != 0)
            { 
              writeback_address = cache_l1.temp_writeback_address;
              bitset<32> writeback_l2_index = 0;
              bitset<32> writeback_l2_block_offset = 0;
              bitset<32> writeback_l2_data_block = 0;
              bitset<32> writeback_l2_tag_store = 0;
              bitset<32> writeback_l2_addr_tag_sel = 0;
    
              for(int n=0;n<log2(l2_cache_block_size); n++)
              {
                writeback_l2_block_offset[n] = writeback_address[n];
              }
              //cout<<"\n";
              int wb_l2_dsize = 0;
              for(int n=log2(l2_cache_block_size);n<(log2(l2_cache_block_size) + log2(l2_cache_data_block_size));n++)
              {
                writeback_l2_data_block[wb_l2_dsize] = writeback_address[n];
                wb_l2_dsize++; 
              } 
    
              int wb_l2_index_size = 0;
              for(int l=(log2(l2_cache_block_size) + log2(l2_cache_data_block_size));l<(log2(l2_cache_block_size) + log2(l2_cache_data_block_size)+log2(l2_no_of_sets));l++)
              {
                writeback_l2_index[wb_l2_index_size] = writeback_address[l];
                wb_l2_index_size++; 
              } 
           
              int wb_l2_addr_tag_sel_size = 0;
              int wb_l2_addr_tag_sel_pos = log2(l2_cache_block_size) + log2(l2_cache_data_block_size)+log2(l2_no_of_sets);
              for(int m=wb_l2_addr_tag_sel_pos;m< (wb_l2_addr_tag_sel_pos + log2(l2_cache_addr_tag_sel));m++)
              {
                writeback_l2_addr_tag_sel[wb_l2_addr_tag_sel_size] = writeback_address[m];
                wb_l2_addr_tag_sel_size++;
              }
    
              int wb_l2_tag_store_bits=0;
              int wb_l2_tag_store_pos = wb_l2_addr_tag_sel_pos + log2(l2_cache_addr_tag_sel);
              for(int lm=wb_l2_tag_store_pos;lm<32;lm++)
              {
                writeback_l2_tag_store[wb_l2_tag_store_bits] = writeback_address[lm];
                wb_l2_tag_store_bits++;
              }
              
              wb_l2_block_offset_ = writeback_l2_block_offset.to_ulong();
              wb_l2_index_ = writeback_l2_index.to_ulong();
              wb_l2_data_block_ = writeback_l2_data_block.to_ulong();
              wb_l2_addr_tag_sel_ = writeback_l2_addr_tag_sel.to_ulong();
              wb_l2_tag_store_ = writeback_l2_tag_store.to_ulong();
            }


            if(l2_cache_capacity != 0)
            {
              l2_cache_hit = cache_l2_pointer->cache_write_operation(wb_l2_block_offset_, wb_l2_data_block_, wb_l2_index_, wb_l2_addr_tag_sel_, wb_l2_tag_store_, 0);
              l2_write_count += 1; 
              l2_cache_hit = cache_l2_pointer->cache_read_operation(l2_block_offset_,l2_data_block_,l2_index_,l2_addr_tag_sel_, l2_tag_store_, 0); 
              l2_read_count += 1;
            }
          } 
          else 
          { 
            read_miss_count += 1;
            if(l2_cache_capacity != 0)
            {
              l2_cache_hit = cache_l2_pointer->cache_read_operation(l2_block_offset_,l2_data_block_,l2_index_,l2_addr_tag_sel_, l2_tag_store_, 0);
              l2_read_count += 1;
            }
          }
          
          ///L2 post read and write, miss hit writeback calculations
          if(l2_cache_capacity != 0)
          {
            if(l2_cache_hit == 1)
            {
              l2_read_hit_count += 1;          
            }
            else if(l2_cache_hit == 15)
            {
              l2_read_miss_count += 1;  
              l2_write_back_count += 1;
            }
            else
            {
              l2_read_miss_count += 1;
            }
          }
 
        }
  
        else if(read_or_write == 'w')
        {
           //cout<<"\n # "<<instruction_count<<" write "<<std::hex<<address_set;
           write_count += 1;
           cache_hit = cache_l1.cache_write_operation(l1_block_offset_, l1_index_, l1_tag_store_, 0, 0, address_set);
           //cout<<"\n Called Cache write!! with "<<cache_hit;
           //cout<<"\n cache_hit = "<<cache_hit; 
           if(cache_hit == 1) 
           { 
             write_hit_count += 1;
           }
           else if(cache_hit == 15) 
           { 
             write_miss_count += 1; write_back_count += 1;
             ///CALL FUNCTION TO WRITEBACK TO THE L2 CACHE..
             //CALL L2 write operation
            if(l2_cache_capacity != 0)
            {
              writeback_address = cache_l1.temp_writeback_address;
              bitset<32> writeback_l2_index = 0;
              bitset<32> writeback_l2_block_offset = 0;
              bitset<32> writeback_l2_data_block = 0;
              bitset<32> writeback_l2_tag_store = 0;
              bitset<32> writeback_l2_addr_tag_sel = 0;
    
              for(int n=0;n<log2(l2_cache_block_size); n++)
              {
                writeback_l2_block_offset[n] = writeback_address[n];
              }
              //cout<<"\n";
              int wb_l2_dsize = 0;
              for(int n=log2(l2_cache_block_size);n<(log2(l2_cache_block_size) + log2(l2_cache_data_block_size));n++)
              {
                writeback_l2_data_block[wb_l2_dsize] = writeback_address[n];
                wb_l2_dsize++; 
              } 
    
              int wb_l2_index_size = 0;
              for(int l=(log2(l2_cache_block_size) + log2(l2_cache_data_block_size));l<(log2(l2_cache_block_size) + log2(l2_cache_data_block_size)+log2(l2_no_of_sets));l++)
              {
                writeback_l2_index[wb_l2_index_size] = writeback_address[l];
                wb_l2_index_size++; 
              } 
           
              int wb_l2_addr_tag_sel_size = 0;
              int l2_addr_tag_sel_pos = log2(l2_cache_block_size) + log2(l2_cache_data_block_size)+log2(l2_no_of_sets);
              for(int m=l2_addr_tag_sel_pos;m< (l2_addr_tag_sel_pos + log2(l2_cache_addr_tag_sel));m++)
              {
                writeback_l2_addr_tag_sel[wb_l2_addr_tag_sel_size] = writeback_address[m];
                wb_l2_addr_tag_sel_size++;
              }
    
              int wb_l2_tag_store_bits=0;
              int l2_tag_store_pos = l2_addr_tag_sel_pos + log2(l2_cache_addr_tag_sel);
              for(int lm=l2_tag_store_pos;lm<32;lm++)
              {
                writeback_l2_tag_store[wb_l2_tag_store_bits] = writeback_address[lm];
                wb_l2_tag_store_bits++;
              }
              
              wb_l2_block_offset_ = writeback_l2_block_offset.to_ulong();
              wb_l2_index_ = writeback_l2_index.to_ulong();
              wb_l2_data_block_ = writeback_l2_data_block.to_ulong();
              wb_l2_addr_tag_sel_ = writeback_l2_addr_tag_sel.to_ulong();
              wb_l2_tag_store_ = writeback_l2_tag_store.to_ulong();



              l2_cache_hit = cache_l2_pointer->cache_write_operation(wb_l2_block_offset_, wb_l2_data_block_, wb_l2_index_, wb_l2_addr_tag_sel_, wb_l2_tag_store_, 0);
              l2_write_count += 1; 
              l2_cache_hit = cache_l2_pointer->cache_read_operation(l2_block_offset_,l2_data_block_,l2_index_,l2_addr_tag_sel_, l2_tag_store_, 0);
              l2_read_count += 1;
            } 
           } 
           else 
           { 
             write_miss_count += 1;
             if(l2_cache_capacity != 0)
             { 
              l2_cache_hit = cache_l2_pointer->cache_read_operation(l2_block_offset_,l2_data_block_,l2_index_,l2_addr_tag_sel_, l2_tag_store_, 0); 
              l2_read_count += 1;
             }
           }
           
           //L2 post write, update the miss hit and writeback counters 
           if(l2_cache_capacity != 0)
           {
             if(l2_cache_hit == 1) 
             {
               l2_write_hit_count += 1;
             }
             else if(l2_cache_hit == 15)
             {
               l2_write_back_count += 1;
               l2_write_miss_count += 1;
             }
             else
             {
               l2_write_miss_count += 1;
             }
           }
         }
        iteration++; 
        }
      }
     
  
     cout<<"===== Simulator Configuration ====="<<"\n";
     cout<<"BLOCKSIZE:"<<"\t"<<l1_cache_block_size<<"\n";
     cout<<"L1_SIZE:"<<"\t"<<l1_cache_capacity<<"\n";
     cout<<"L1_ASSOC:"<<"\t"<<l1_cache_associativity<<"\n";
     cout<<"L2_SIZE:"<<"\t"<<l2_cache_capacity<<"\n";
     cout<<"L2_ASSOC:"<<"\t"<<l2_cache_associativity<<"\n";
     cout<<"L2_DATA_BLOCKS:"<<"\t"<<l2_cache_data_block_size<<"\n";
     cout<<"L2_ADDRESS_TAGS:"<<"\t"<<l2_cache_addr_tag_sel<<"\n";
     cout<<"trace_file:"<<"\t"<<argv[8]<<"\n";
     cache_l1.print_tag_lru();
     if(l2_cache_capacity != 0)
     {
       cache_l2_pointer->print_tag_lru();
     }
     cout<<"\n";
     cout<<"\n===== Simulation Results =====";
     cout<<"\na. number of L1 reads:                       "<<read_count;
     cout<<"\nb. number of L1 read misses:                 "<<read_miss_count;
     cout<<"\nc. number of L1 writes:                      "<<write_count;
     cout<<"\nd. number of L1 write misses:                "<<write_miss_count;
     cache_miss_rate = (read_miss_count + write_miss_count)/(read_count + write_count);
     cout<<"\ne. L1 miss rate:                         "<<std::fixed<<setprecision(4)<<cache_miss_rate;
     cout<<"\nf. number of write backs from L1 memory: "<<std::fixed<<setprecision(0)<<write_back_count;
     
     //cout<<"\ng. number of L2 reads:                       "<<l2_read_count;
     //cout<<"\nh. number of L2 read misses:                 "<<cache_l2_pointer->l2_read_miss_count;
     //cout<<"\ni. number of L2 writes:                      "<<l2_write_count;
     //cout<<"\nj. number of L2 write misses:                "<<cache_l2_pointer->l2_write_miss_count;
     //cout<<"\nk. L2 miss rate:";
     //cout<<"\nl. number of write backs from L2 memory:     "<<cache_l2_pointer->l2_write_back_count;
     if(l2_cache_capacity == 0)
     {
       double memory_bandwidth = read_miss_count + write_miss_count + write_back_count;
       cout<<"\ng. total memory traffic:                 "<<std::fixed<<setprecision(0)<<memory_bandwidth;
     }
  
     if(l2_cache_capacity != 0)
     {     
       cache_l2_pointer->print_l2_metrics();
     }
  return 0;
  }  
  
  
  //call PARSE function//
  parse_input_file();
  
  
  //cout<<"Success reading file";
  
  
return 0;
}

