class Cache_generic
{
  public:
  //initial considerations for direct mapped cache//
  int block_size, cache_size, associativity,no_of_sets, cache_index,tag_store_size, set_size;
  bitset<32> temp_writeback_address;
  bitset<32> *address_storage;
  int *tstore;
  int *tstore_valid;
  int *tstore_dirty;
  int *lru_counter;
  int *lru_counter_swap;
  int *tstore_temp;
  int *tstore_dirty_temp;
  int *tstore_valid_temp;
  int read_count, write_count, total_count;
  int set_offset_to_lru; 
  //Member function declaration
  //virtual int cache_read_operation(int block_offset, int index, int tag_values, int dummy_value, int dummy_value_2);
  virtual int cache_read_operation(int, int, int, int, int, bitset<32>);
  virtual int cache_write_operation(int, int, int, int, int, bitset<32>);
  //virtual int cache_tstore_update(int tag_values);
  virtual int cache_lru_update(int hit_or_miss, int set_offset_to_lru, int index, int tag_value);
  virtual int print_tag_lru();
  virtual int print_l2_metrics();
  
  //Cache_generic constructor
  Cache_generic()
  {
    //Default constructor written here to avoid constructor clash during inheritance
    //cout<<"\n Calling cache_l1_constructor";
  }


  Cache_generic(int b_size, int c_size, int assoc)
  {       
    //cout<<"Constructer of Cache Hierarchy called";
    block_size = b_size;
    cache_size = c_size; 
    associativity = assoc;
    no_of_sets = c_size / (b_size * assoc);
    set_size = assoc;
    address_storage = new bitset<32>[no_of_sets*assoc];
    tstore = new int[no_of_sets*assoc];
    tstore_valid = new int[no_of_sets*assoc];
    tstore_dirty = new int[no_of_sets*assoc];
    lru_counter = new int[no_of_sets*assoc];
    tstore_temp = new int[no_of_sets*assoc];
    tstore_dirty_temp = new int[no_of_sets*assoc];
    tstore_valid_temp = new int[no_of_sets*assoc];
    lru_counter_swap = new int[no_of_sets*assoc];
    int k =0;
    //cout<<"\n"<<"###########LRU COUNTER#######"<<"\n";
    for(int i = 0; i< no_of_sets; i++)
    {
      for(int j=0; j< associativity; j++)
      {
        lru_counter[k] = j;
        tstore_dirty[k] = 0;
        //cout<<lru_counter[k];
        k++;
      }
    //cout<<"\n";
    }
  } //Class Constructor


}; //end of class cache generic



int Cache_generic::cache_read_operation(int block_offset, int index, int tag_value, int dummy, int dummy_2, bitset<32> address)
{
  int read_hit_flag = 0;
  int write_back = 0;
  int replacement_index = 0;
  set_offset_to_lru= 0;
  for(int i = 0; i < associativity; i++)
  {
    if(tstore[index*associativity + i] == tag_value && tstore_valid[index*associativity + i] == 1)
    { 
      read_hit_flag = 1;
      //cout<<"\n"<<"CACHE READ HIT"<<"\n";
      set_offset_to_lru = i;
      cache_lru_update(1, set_offset_to_lru, index, tag_value);
      continue;
    }
  }


  //If there is a miss call LRU function//
  if(read_hit_flag != 1)
  { 
    ///Call LRU function///
    //cout<<"\n"<<"Read MISS at L1 cache, calling LRU"<<"\n";  
    replacement_index = cache_lru_update(0, 0, index, tag_value);
    if(tstore_dirty[replacement_index] == 1)
    {
      //cout<<"\n"<<"Writing dirty blocks to lower level"<<"\n";
      //Function call to write back dirty block should be made here
      tstore_dirty[replacement_index] = 0;
      temp_writeback_address = address_storage[replacement_index];
      write_back = 15;
    }
    tstore[replacement_index] = tag_value;
    address_storage[replacement_index] = address;
    tstore_valid[replacement_index] = 1;
  } 
  return read_hit_flag + write_back;
}

int Cache_generic::cache_write_operation(int block_offset, int index, int tag_value, int dummy_1, int dummy_2, bitset<32> address)
{
  int write_hit_flag = 0;
  int write_back = 0;
  int replacement_index = 0;
  set_offset_to_lru= 0;
  for(int i = 0; i < associativity; i++)
  {
    if(tstore[index*associativity + i] == tag_value && tstore_valid[index*associativity + i] == 1)
    { 
      write_hit_flag = 1;
      //cout<<"\n"<<"CACHE WRITE HIT"<<"\n";
      set_offset_to_lru = i;
      tstore_dirty[index*associativity + i] = 1;
      cache_lru_update(1, set_offset_to_lru, index, tag_value);
      continue;
    }
  }


  //If there is a miss call LRU function//
  if(write_hit_flag != 1)
  {
    ///Call LRU function///
    //cout<<"\n"<<"Write MISS at L1 cache, calling LRU"<<"\n";  
    replacement_index = cache_lru_update(0, 0, index, tag_value);
    if(tstore_dirty[replacement_index] == 1)
    {
      //cout<<"\n"<<"writing dirty block to lower hierarchy!!!"<<"\n";
      write_back = 15;
      temp_writeback_address = address_storage[replacement_index];
      ///////CALL LOGIC TO WRITE THE CACHE CONTENT TO NEXT LEVEL HIERARCHY/////////
    }
    tstore_dirty[replacement_index] = 1;
    tstore[replacement_index] = tag_value;
    address_storage[replacement_index] = address;
    tstore_valid[replacement_index] = 1; 
  } 
  return write_hit_flag + write_back;
}


int Cache_generic::cache_lru_update(int hit_or_miss, int set_offset_to_lru, int index, int tag_value)
{
  int largest = lru_counter[index*associativity];
  int large_index = 0;
  int hit_or_miss_ = hit_or_miss;
  if(hit_or_miss_)//Hit cases go here
  {
    for(int j = index*associativity; j < index*associativity + associativity; j++)
    {
      if(j != (index*associativity + set_offset_to_lru))
      {
        if(lru_counter[j] < lru_counter[index*associativity + set_offset_to_lru])
        { 
          lru_counter[j] += 1;
        }
      }
    }
    lru_counter[index*associativity + set_offset_to_lru] = 0;
  }
  
  else //Cache miss cases here
  {
    for(int i = index*associativity; i < index*associativity + associativity; i++)
    {
      if(lru_counter[i] >= largest)
      {
        largest = lru_counter[i];
        large_index = i;
      }
    }
    lru_counter[large_index] = 0;
    //update the lru counter
    for(int j = index*associativity; j < index*associativity + associativity; j++)
    {
      if(j!=large_index) lru_counter[j] = lru_counter[j] + 1;
    }   
  }
return large_index;
}


int Cache_generic::print_tag_lru()
{
  cout<<"\n"<<"===== L1 contents =====";
  for(int i =0; i<no_of_sets*associativity; i++)
  {
    lru_counter_swap[i] = lru_counter[i];
    tstore_temp[i] = tstore[i];
    tstore_dirty_temp[i] = tstore_dirty[i];
    tstore_valid_temp[i] = tstore_valid[i];
  }
  int temp_lru;
  int temp_tstore;
  int temp_tstore_dirty;
  int temp_tstore_valid;
  //sort the contents of LRU counter///
  for(int i =0; i< no_of_sets; i++)
  {
   /* for(int m=0; m< associativity; m++)
      {
      lru_counter_swap[i*associativity
     
      }
   */
    for(int k =0; k< associativity; k++)
    { 
      for(int l= 0; l< associativity - 1 ; l++)
      { 
        //cout<<"\n LRU index"<<i*associativity + l;
        if(lru_counter_swap[i*associativity + l] > lru_counter_swap[i*associativity + l + 1])
        {
          temp_lru = lru_counter_swap[i*associativity + l + 1];
          lru_counter_swap[i*associativity + l + 1] = lru_counter_swap[i*associativity + l];
          lru_counter_swap[i*associativity + l] = temp_lru;
          
          temp_tstore = tstore_temp[i*associativity + l + 1];
          tstore_temp[i*associativity + l + 1] = tstore_temp[i*associativity + l];
          tstore_temp[i*associativity + l] = temp_tstore;
          
          temp_tstore_dirty = tstore_dirty_temp[i*associativity + l + 1];
          tstore_dirty_temp[i*associativity + l + 1] = tstore_dirty_temp[i*associativity + l];
          tstore_dirty_temp[i*associativity + l] = temp_tstore_dirty;
          
          temp_tstore_valid = tstore_valid_temp[i*associativity + l + 1];
          tstore_valid_temp[i*associativity + l + 1] = tstore_valid_temp[i*associativity + l];
          tstore_valid_temp[i*associativity + l] = temp_tstore_valid;
       
        }       
      }
    }
  }
  for(int i =0; i< no_of_sets*associativity; i++)
  { 
    int set_index =  i / associativity;
    if(i % associativity == 0) cout<<"\nset\t"<<std::dec<<set_index<<":";
    cout<<std::hex<<"\t"<<tstore_temp[i];
    if(tstore_dirty_temp[i] == 1) cout<<" D\t||";
    else cout<<" N\t||";
    //cout<<"\t LRU"<<lru_counter[i];
    //cout<<" !!"<<lru_counter_swap[i];
  }

  //cout<<"\n"<<"####LRU COUNTER#########"<<"\n";
  for(int i=0; i< no_of_sets*associativity; i++)
  {
    //if(i % associativity == 0) cout<<"\n";
   // cout<<"\t"<<lru_counter[i];
  }


  //cout<<"\n"<<"####DIRTY BIT#########"<<"\n";
  for(int i=0; i< no_of_sets*associativity; i++)
  {
    //if(i % associativity == 0) cout<<"\n";
    //cout<<"\t"<<tstore_dirty[i];
  }
return 0;
}


int Cache_generic::print_l2_metrics()
{
  cout<<"Dummy class!!";
return 0;
}
