class cache_l2_decouple_sectored: public Cache_generic
{
  public:
  //int l2_block_size, l2_data_blocks, l2_no_of_sets, l2_associativity, l2_addr_tag_sel, l2_tag_store;
  int data_block_size, tag_store_sel_size;

  //performance metrics calculations
  int l2_write_back_count;
  int l2_write_count;
  int l2_write_miss_count;
  float l2_read_count;
  float l2_read_miss_count;
  int l2_cache_block_miss_count;
  int l2_sector_miss_count;
  int misc_read_count;
  
  //Tagstore and LRU declarations
  int *l2_lru_counter;
  int **tstore_contents;
  int **tstore_valid_contents;
  int **tstore_dirty_contents;
  int **tstore_select_contents;

  //Temporary stores

  int *l2_lru_counter_temp;
  int **tstore_contents_temp;
  int **tstore_valid_contents_temp;
  int **tstore_dirty_contents_temp;
  int **tstore_select_contents_temp;

  //function declarations
  int cache_read_operation(int, int, int, int, int, bitset<32>);
  int cache_write_operation(int, int, int, int, int, bitset<32>);
  int cache_lru_update(int, int, int, int);
  int print_tag_lru();
  int cache_write_back(int, int, int, int, int);
  int print_l2_metrics();


  cache_l2_decouple_sectored(int b_size, int capacity, int data_block_size_, int assoc, int addr_tag_sel)
  {
    //cout<<"\n Calling cache l2 constructor!";
    l2_write_back_count = 0;
    l2_write_miss_count = 0;
    l2_write_count = 0;
    l2_read_count = 0;
    l2_read_miss_count = 0;
    misc_read_count = 0;
    l2_cache_block_miss_count = 0;
    l2_sector_miss_count = 0;

    //values passed from object 
    block_size = b_size;
    cache_size = capacity;
    associativity = assoc;
    data_block_size = data_block_size_;
    tag_store_sel_size = addr_tag_sel;
    //cout<<"tag store selection size:"<<tag_store_sel_size<<"\n";
    no_of_sets = cache_size/(block_size * data_block_size * associativity);
    

    //Declare LRU counters
    l2_lru_counter = new int[no_of_sets*assoc];
    l2_lru_counter_temp = new int[no_of_sets*assoc];
    //Create Tagstore 
    tstore_contents = new int*[no_of_sets*assoc];
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_contents[i] = new int[tag_store_sel_size];
    }
    
    tstore_contents_temp = new int*[no_of_sets*assoc];
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_contents_temp[i] = new int[tag_store_sel_size];
    }
    //Create Tagstore valid    
    tstore_valid_contents = new int*[no_of_sets*assoc];
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_valid_contents[i] = new int[data_block_size];
    }

    tstore_valid_contents_temp = new int*[no_of_sets*assoc];
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_valid_contents_temp[i] = new int[data_block_size];
    }

    //Create Tagstore dirty
    tstore_dirty_contents = new int*[no_of_sets*assoc]; 
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_dirty_contents[i] = new int[data_block_size];
    } 
    
    tstore_dirty_contents_temp = new int*[no_of_sets*assoc]; 
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_dirty_contents_temp[i] = new int[data_block_size];
    } 
    //Create Tagstore selection
    tstore_select_contents = new int*[no_of_sets*assoc]; 
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_select_contents[i] = new int[data_block_size];
    }
    
    tstore_select_contents_temp = new int*[no_of_sets*assoc]; 
    for(int i=0;i<(no_of_sets*assoc);i++)
    {
      tstore_select_contents_temp[i] = new int[data_block_size];
    }
    int k=0;
    //cout<<"\n########LRU COUNTER#######";
    for(int i = 0; i< no_of_sets; i++)
    {
      for(int j=0; j< associativity; j++)
      {
        l2_lru_counter[k] = j;
        //tstore_dirty[k] = 0;
        //cout<<l2_lru_counter[k];
        k++;
      }
    //cout<<"\n";
    }
  }
};


int cache_l2_decouple_sectored::cache_read_operation(int block_offset, int data_block, int index, int tag_sel, int tag_store, bitset<32> address_dummy)
{
  l2_read_count += 1;
  int l2_cache_read_hit_flag = 0;
  int l2_replacement_index = 0;
  int l2_set_offset_to_lru = 0;
  int perfect_read_hit=0;
  for(int i=0;i<associativity;i++)
  {
    if(tstore_contents[index*associativity + i][tag_sel] == tag_store) 
    {
      //cout<<"\nRead Tag hit! Yet to check dirty bit";
      if((tstore_select_contents[index*associativity + i][data_block] == tag_sel) && (tstore_valid_contents[index*associativity + i][data_block] == 1))
      {
        //cout<<"\nTag and Block Read hit at L2 cache!";
          perfect_read_hit = 1;
      }
      else if((tstore_select_contents[index*associativity + i][data_block] == tag_sel) && (tstore_valid_contents[index*associativity + i][data_block]==0))
      {
        //cout<<"\n Intended tag sel matches, but tag entry not valid, hence replacing the element";
        tstore_valid_contents[index*associativity + i][data_block] = 1;
        tstore_dirty_contents[index*associativity + i][data_block] = 0;
        l2_read_miss_count += 1;
      }
      else if((tstore_select_contents[index*associativity + i][data_block] != tag_sel) && (tstore_valid_contents[index*associativity + i][data_block] == 1))
      { 
        //cout<<"\nTag hit but corresponding block not found";
        if(tstore_dirty_contents[index*associativity + i][data_block] == 0)
        {
          //cout<<"\nIntended read location not dirty, hence replacing without writeback!";
        }
        else
        {
          //write back to lower level and set the dirty bit to 0
          l2_write_back_count += 1;
          //cout<<"\nIntended read location dirty, hence writing back to lower level and replacing the block, setting the dirty bit to 0";
          tstore_dirty_contents[index*associativity + i][data_block] = 0;
        }
        //cout<<"\n After bringing the corresponding block, we are updating the tag sel associated with the block";
        tstore_select_contents[index*associativity + i][data_block] = tag_sel;
        l2_read_miss_count += 1;
      }
      else if((tstore_select_contents[index*associativity + i][data_block] != tag_sel) && (tstore_valid_contents[index*associativity + i][data_block] == 0))
      {
        //cout<<"\nIntended read location not valid, just replace the block and update the valid bit";
        tstore_valid_contents[index*associativity + i][data_block] = 1;
        tstore_dirty_contents[index*associativity + i][data_block] = 0;
        tstore_select_contents[index*associativity + i][data_block] = tag_sel;
        l2_read_miss_count += 1;
      }
      else 
      {
        cout<<"\n WARNING!! case in read tag hit that is not considered!!";
        misc_read_count += 1;
      }
      //CALL POST READ LRU HERE!!!!
      //cout<<"\n POST READ LRU";
      l2_cache_read_hit_flag = 1;
      tstore_select_contents[index*associativity + i][data_block] = tag_sel;
      l2_set_offset_to_lru = i;
      cache_lru_update(1, l2_set_offset_to_lru, index, tag_store);

      //logic to count the cache block misses and sector misses
      if(perfect_read_hit!=1)
      {
        int l2_sector_miss_type_flag = 0;
        for(int j =0;j <data_block_size; j++)
        {
            //if(j != data_block)
            //{
              if(tstore_valid_contents[index*associativity + i][j] == 1) l2_sector_miss_type_flag = 1;
            //}
        }
        
        if(l2_sector_miss_type_flag == 1) l2_cache_block_miss_count += 1;
        else l2_sector_miss_count += 1;
      }

      break;
    }
  }
  if(l2_cache_read_hit_flag != 1)
  {
    //cout<<"\nRead Miss at L2 cache!";
    l2_read_miss_count += 1;
    //call LRU here
    l2_replacement_index = cache_lru_update(0, 0, index, tag_store);
    //cout<<"\n called LRU at read miss!"; 
    
    int miss_type_flag_ = 0;
    for(int i =0;i< data_block_size; i++)
    {
        if(tstore_valid_contents[l2_replacement_index][i] == 1)  miss_type_flag_ = 1; 
    }
    if(miss_type_flag_ == 1) l2_cache_block_miss_count += 1;
    else l2_sector_miss_count += 1;
    

    for(int i=0; i<data_block_size; i++)
    {
        
      if(i!= data_block)
      {
        if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 1 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          l2_write_back_count += 1;
          //cout<<"\nwriting back all the dirty data_blocks matching the selection tags in case of read miss";
          tstore_valid_contents[l2_replacement_index][i] = 0;
          tstore_dirty_contents[l2_replacement_index][i] = 0;
          tstore_select_contents[l2_replacement_index][i] = 0;
        }
          
        else if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of read miss";
          tstore_valid_contents[l2_replacement_index][i] = 0;
          tstore_select_contents[l2_replacement_index][i] = 0;
        }
        
        else if(tstore_valid_contents[l2_replacement_index][i] == 0 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of read miss";
          tstore_select_contents[l2_replacement_index][i] = 0;
        } 
        else if(tstore_valid_contents[l2_replacement_index][i] == 0 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] != tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of read miss";
          tstore_select_contents[l2_replacement_index][i] = 0;
        }
      }
      else if(i == data_block)
      {
        if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 1 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          l2_write_back_count += 1;
          //cout<<"\nwriting back all the dirty data_blocks matching the selection tags in case of read miss";
          tstore_valid_contents[l2_replacement_index][i] = 1;
          tstore_dirty_contents[l2_replacement_index][i] = 0;
        }
          
        else if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of read miss";
          tstore_valid_contents[l2_replacement_index][i] = 1;
        }

        else if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 1 && tstore_select_contents[l2_replacement_index][i] != tag_sel)
        {
          l2_write_back_count += 1;
          tstore_valid_contents[l2_replacement_index][i] = 1;
        }
      }

    }
      
    //cout<<"\nUpdating tag store in case of Read miss";
    tstore_contents[l2_replacement_index][tag_sel] = tag_store;
    tstore_valid_contents[l2_replacement_index][data_block] = 1;
    tstore_select_contents[l2_replacement_index][data_block] = tag_sel;
    tstore_dirty_contents[l2_replacement_index][data_block] = 0; 

  }
 
return 0;
}


int cache_l2_decouple_sectored::cache_write_operation(int block_offset, int data_block, int index, int tag_sel, int tag_store, bitset<32> address_dummy)
{
  
  l2_write_count += 1;
  int l2_cache_write_hit_flag = 0;
  int l2_replacement_index = 0;
  int l2_set_offset_to_lru = 0;
  int perfect_write_hit = 0;
  for(int i=0;i<associativity;i++)
  {
    if(tstore_contents[index*associativity + i][tag_sel] == tag_store) 
    {
        //cout<<"\nWrite Tag hit! Yet to check dirty bit";
        if((tstore_select_contents[index*associativity + i][data_block] == tag_sel) && (tstore_valid_contents[index*associativity + i][data_block] == 1))
        {
          tstore_dirty_contents[index*associativity + i][data_block] = 1;
          perfect_write_hit = 1;
          //cout<<"\nTag and Block Write hit at L2 cache!";
        }

        else if((tstore_select_contents[index*associativity + i][data_block] == tag_sel) && (tstore_valid_contents[index*associativity + i][data_block]==0))
        {
          //cout<<"\n Intended tag sel matches, but tag entry not valid, hence replacing the element";
          tstore_valid_contents[index*associativity + i][data_block] = 1;
          tstore_dirty_contents[index*associativity + i][data_block] = 1;
          l2_write_miss_count += 1;
        }
        else if((tstore_select_contents[index*associativity + i][data_block] != tag_sel) && (tstore_valid_contents[index*associativity + i][data_block] == 1))
        { 
          //cout<<"\nTag hit but corresponding block not found";
          if(tstore_dirty_contents[index*associativity + i][data_block] == 0)
          {
            //cout<<"\nIntended write location not dirty, hence replacing without writeback!";
            tstore_dirty_contents[index*associativity + i][data_block] = 1;
          }
          else
          {
            //write back to lower level and set the dirty bit to 0
            l2_write_back_count += 1;
            //cout<<"\nIntended read location dirty, hence writing back to lower level and replacing the block, setting the dirty bit to 0";
            tstore_dirty_contents[index*associativity + i][data_block] = 1;
          }
          l2_write_miss_count += 1;
        }
        else if((tstore_select_contents[index*associativity + i][data_block] != tag_sel) && (tstore_valid_contents[index*associativity + i][data_block] == 0))
        {
          //cout<<"\nIntended write location not valid, just replace the block and update the valid bit";
          tstore_valid_contents[index*associativity + i][data_block] = 1;
          tstore_dirty_contents[index*associativity + i][data_block] = 1;
          l2_write_miss_count += 1;
          //l2_cache_block_miss_count += 1;
        }
      
        else 
        {
        //cout<<"\n WARNING!! case in write tag hit that is not considered!!";
        }
        
        //CALL POST WRITE LRU HERE!!!!
        //cout<<"\n After bringing the corresponding block, we are updating the tag sel associated with the block";
        tstore_select_contents[index*associativity + i][data_block] = tag_sel;
        //cout<<"\n POST WRITE LRU";
        l2_set_offset_to_lru = i;
        cache_lru_update(1, l2_set_offset_to_lru, index, tag_store);
        l2_cache_write_hit_flag = 1;
   
        if(perfect_write_hit!=1)
        {
        int l2_sector_miss_type_flag_ = 0;
        for(int j =0;j <data_block_size; j++)
        {
            //if(j != data_block) 
            //{
              if(tstore_valid_contents[index*associativity + i][j] == 1) l2_sector_miss_type_flag_ = 1;
            //}
        }
        
        if(l2_sector_miss_type_flag_ == 1) l2_cache_block_miss_count += 1;
        else l2_sector_miss_count += 1;
        }

        break;
    }
  } 
  if(l2_cache_write_hit_flag!=1)
  {
    //cout<<"\nWrite Miss at L2 cache!";
    //call LRU here
    l2_write_miss_count += 1;
    //l2_sector_miss_count += 1;
    l2_replacement_index = cache_lru_update(0, 0, index, tag_store);

    int miss_type_flag = 0;
    for(int i =0;i< data_block_size; i++)
    {
      if(tstore_valid_contents[l2_replacement_index][i] == 1)  miss_type_flag = 1; 
    }
    if(miss_type_flag == 1) l2_cache_block_miss_count += 1;
    else l2_sector_miss_count += 1;
    
    //cout<<"\n called LRU at l2 write miss!"; 
    for(int i=0; i<data_block_size; i++)
    {
     
      if(i != data_block)
      {   
        if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 1 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          l2_write_back_count += 1;
          //cout<<"\nwriting back all the dirty data_blocks matching the selection tags in case of write miss";
          tstore_valid_contents[l2_replacement_index][i] = 0;
          tstore_select_contents[l2_replacement_index][i] = 0;
          tstore_dirty_contents[l2_replacement_index][i] = 0;
        }
          
        else if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of write miss";
          tstore_valid_contents[l2_replacement_index][i] = 0;
          tstore_select_contents[l2_replacement_index][i] = 0;
        } 
        
        else if(tstore_valid_contents[l2_replacement_index][i] == 0 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of read miss";
          tstore_select_contents[l2_replacement_index][i] = 0;
        } 
        else if(tstore_valid_contents[l2_replacement_index][i] == 0 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] != tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of read miss";
          tstore_select_contents[l2_replacement_index][i] = 0;
        }
      }


      else if(i == data_block)
      {
        if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 1 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          l2_write_back_count += 1;
          //cout<<"\nwriting back all the dirty data_blocks matching the selection tags in case of read miss";
          tstore_valid_contents[l2_replacement_index][i] = 1;
          tstore_dirty_contents[l2_replacement_index][i] = 0;
        }
          
        else if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 0 && tstore_select_contents[l2_replacement_index][i] == tag_sel)
        {
          //cout<<"\nInvalidating all other non-dirty data_blocks matching the selection tags in case of read miss";
          tstore_valid_contents[l2_replacement_index][i] = 1;
        }

        else if(tstore_valid_contents[l2_replacement_index][i] == 1 && tstore_dirty_contents[l2_replacement_index][i] == 1 && tstore_select_contents[l2_replacement_index][i] != tag_sel)
        {
          l2_write_back_count += 1;
          tstore_valid_contents[l2_replacement_index][i] = 1;
        }
      }

    }
      
    //cout<<"\nUpdating tag store in case of write miss";
    tstore_contents[l2_replacement_index][tag_sel] = tag_store;
    tstore_valid_contents[l2_replacement_index][data_block] = 1;
    tstore_select_contents[l2_replacement_index][data_block] = tag_sel;
    tstore_dirty_contents[l2_replacement_index][data_block] = 1; 
    
  }
 
return 0;
}


int cache_l2_decouple_sectored::cache_lru_update(int hit_or_miss, int set_offset_to_lru, int index, int tag_value)
{
  int largest = l2_lru_counter[index*associativity];
  int large_index = 0;
  int hit_or_miss_ = hit_or_miss;
  if(hit_or_miss_)//Hit cases go here
  {
    for(int j = index*associativity; j < index*associativity + associativity; j++)
    {
      if(j != (index*associativity + set_offset_to_lru))
      {
        if(l2_lru_counter[j] < l2_lru_counter[index*associativity + set_offset_to_lru])
        { 
          l2_lru_counter[j] += 1;
        }
      }
    }
    l2_lru_counter[index*associativity + set_offset_to_lru] = 0;
  }
  
  else //Cache miss cases here
  {
    for(int i = index*associativity; i < index*associativity + associativity; i++)
    {
      if(l2_lru_counter[i] >= largest)
      {
        largest = l2_lru_counter[i];
        large_index = i;
      }
    }
    l2_lru_counter[large_index] = 0;
    //update the lru counter
    for(int j = index*associativity; j < index*associativity + associativity; j++)
    {
      if(j!=large_index) l2_lru_counter[j] = l2_lru_counter[j] + 1;
    }   
  }
return large_index;
}


int cache_l2_decouple_sectored::print_tag_lru()
{

  for(int i =0; i<no_of_sets*associativity; i++)
  {
    
    l2_lru_counter_temp[i] = l2_lru_counter[i];
    
    for(int l=0;l<tag_store_sel_size;l++)
    {
      tstore_contents_temp[i][l] = tstore_contents[i][l];
    }
    
    for(int m=0;m<data_block_size;m++)
    {
      tstore_dirty_contents_temp[i][m] = tstore_dirty_contents[i][m];
      tstore_valid_contents_temp[i][m] = tstore_valid_contents[i][m];
      tstore_select_contents_temp[i][m] = tstore_select_contents[i][m];
    }
  }


  int temp_lru;
  int temp_tstore;
  int temp_tstore_dirty;
  int temp_tstore_valid;
  int temp_tstore_select;

  for(int i=0; i<no_of_sets;i++)
  {
    for(int k =0; k< associativity; k++)
    { 
      for(int l= 0; l< associativity - 1 ; l++)
      { 
        //cout<<"\n LRU index"<<i*associativity + l;
        if(l2_lru_counter_temp[i*associativity + l] > l2_lru_counter_temp[i*associativity + l + 1])
        {
          temp_lru = l2_lru_counter_temp[i*associativity + l + 1];
          l2_lru_counter_temp[i*associativity + l + 1] = l2_lru_counter_temp[i*associativity + l];
          l2_lru_counter_temp[i*associativity + l] = temp_lru;
          
          for(int tsel=0;tsel<tag_store_sel_size;tsel++)
          {
            temp_tstore = tstore_contents_temp[i*associativity + l + 1][tsel];
            tstore_contents_temp[i*associativity + l + 1][tsel] = tstore_contents_temp[i*associativity + l][tsel];
            tstore_contents_temp[i*associativity + l][tsel] = temp_tstore;
          }
          
          for(int bsel=0;bsel<data_block_size;bsel++)
          {
            temp_tstore_dirty = tstore_dirty_contents_temp[i*associativity + l + 1][bsel];
            tstore_dirty_contents_temp[i*associativity + l + 1][bsel] = tstore_dirty_contents_temp[i*associativity + l][bsel];
            tstore_dirty_contents_temp[i*associativity + l][bsel] = temp_tstore_dirty;
   
          
            temp_tstore_valid = tstore_valid_contents_temp[i*associativity + l + 1][bsel];
            tstore_valid_contents_temp[i*associativity + l + 1][bsel] = tstore_valid_contents_temp[i*associativity + l][bsel];
            tstore_valid_contents_temp[i*associativity + l][bsel] = temp_tstore_valid;
          
            temp_tstore_select = tstore_select_contents_temp[i*associativity + l + 1][bsel];
            tstore_select_contents_temp[i*associativity + l + 1][bsel] = tstore_select_contents_temp[i*associativity + l][bsel];
            tstore_select_contents_temp[i*associativity + l][bsel] = temp_tstore_select;
          }
       
        }       
      }
    }
  }



  //PRINTING THE RESULTS

  if(tag_store_sel_size == 1)
  {
    cout<<"\n";
    cout<<"\n"<<"===== L2 contents =====";

    for(int i=0;i<no_of_sets*associativity;i++)
    {
      if(i%associativity == 0) cout<<"\nset\t"<<std::dec<<i/associativity<<":";
      for(int j=0;j<tag_store_sel_size;j++)
      {
        cout<<"\t"<<std::hex<<tstore_contents_temp[i][j];
        for(int k=0;k<data_block_size;k++)
        {
          //cout<<"\t";
          //if((tstore_valid_contents_temp[i][k]) == 1) cout<<"V";
          //else cout<<"I";
          if((tstore_dirty_contents_temp[i][k]) == 1) cout<<" D ||";
          else cout<<" N ||";
          //cout<<tstore_select_contents_temp[j][k];
        }
      }
    }
  }

  else
  {
    cout<<"\n";
    cout<<"\n===== L2 Address Array contents =====";

    for(int i=0;i<no_of_sets*associativity;i++)
    {
      if(i%associativity == 0) cout<<"\nset\t"<<std::dec<<i/associativity<<":";
      for(int j=0;j<tag_store_sel_size;j++)
      {
        cout<<"\t"<<std::hex<<tstore_contents_temp[i][j];
        if(j == tag_store_sel_size - 1) cout<<"\t||";
      }
    }
    cout<<"\n"; 
    cout<<"\n===== L2 Data Array contents =====";

    for(int i=0;i<no_of_sets*associativity;i++)
    {
      if(i%associativity == 0) cout<<"\nset\t"<<std::dec<<i/associativity<<":";
      for(int j=0;j<data_block_size;j++)
      {
        //cout<<"\t"<<std::hex<<tstore_contents_temp[i][j];
        cout<<"\t";
        cout<<tstore_select_contents_temp[i][j];
        if((tstore_valid_contents_temp[i][j]) == 1) cout<<",V,";
        else cout<<",I,";
        if((tstore_dirty_contents_temp[i][j]) == 1) cout<<"D";
        else cout<<"N";
        if(j == data_block_size - 1) cout<<"\t||";
      }
    } 
  }

  
  /*cout<<"\n #########################################";
  for(int j=0;j<no_of_sets*associativity;j++)
  {
    if(j%associativity == 0) cout<<"\n";
    for(int k=0;k<data_block_size;k++)
    {
      cout<<"\t";
      if((tstore_valid_contents_temp[j][k]) == 1) cout<<"V";
      else cout<<"I";
      if((tstore_dirty_contents_temp[j][k]) == 1) cout<<"D";
      else cout<<"N";
      //cout<<tstore_select_contents_temp[j][k];
    }
  }*/

  /*for(int i=0;i<no_of_sets*associativity;i++)
  {
    if(i%associativity == 0) cout<<"\n";
    cout<<l2_lru_counter[i]<<"\t";
  }*/


  //cout<<"\nNumber of L2 Read misses ="<<std::dec<<l2_read_miss_count;
  //cout<<"\nNumber of L2 Write misses ="<<std::dec<<l2_write_miss_count;
  //cout<<"\nNumber of L2 Writebacks ="<<std::dec<<l2_write_back_count;
  //cout<<"\nNumber of L2 Misc reads ="<<std::dec<<misc_read_count;
  //cout<<"\nNumber of L2 cache block misses ="<<std::dec<<l2_cache_block_miss_count;
  //cout<<"\nNumber of L2 sector misses ="<<std::dec<<l2_sector_miss_count;
return 0;
}


int cache_l2_decouple_sectored::print_l2_metrics()
{ 
  if(tag_store_sel_size != 1)
  {
    cout<<"\ng. number of L2 reads:                       "<<std::dec<<l2_read_count;
    cout<<"\nh. number of L2 read misses:                 "<<std::dec<<l2_read_miss_count;
    cout<<"\ni. number of L2 writes:                      "<<std::dec<<l2_write_count;
    cout<<"\nj. number of L2 write misses:                "<<std::dec<<l2_write_miss_count;
    float l2_miss_rate = l2_read_miss_count/l2_read_count;
    cout<<"\nk. number of L2 sector misses:               "<<std::dec<<l2_sector_miss_count;
    cout<<"\nl. number of L2 cache block misses:          "<<std::dec<<l2_cache_block_miss_count;
    cout<<"\nm. L2 miss rate:                             "<<std::fixed<<setprecision(4)<<l2_miss_rate;
    cout<<"\nn. number of writebacks from L2 memory:      "<<std::dec<<l2_write_back_count;
    double l2_memory_bandwidth = l2_read_miss_count + l2_write_miss_count + l2_write_back_count; 
    cout<<"\no. total memory traffic:                     "<<std::fixed<<setprecision(0)<<l2_memory_bandwidth;
  }
  else
  {
    cout<<"\ng. number of L2 reads:                       "<<std::dec<<l2_read_count;
    cout<<"\nh. number of L2 read misses:                 "<<std::dec<<l2_read_miss_count;
    cout<<"\ni. number of L2 writes:                      "<<std::dec<<l2_write_count;
    cout<<"\nj. number of L2 write misses:                "<<std::dec<<l2_write_miss_count;
    float l2_miss_rate = l2_read_miss_count/l2_read_count;
    cout<<"\nk. L2 miss rate:                             "<<std::fixed<<setprecision(4)<<l2_miss_rate;
    cout<<"\nl. number of writebacks from L2 memory:      "<<std::dec<<l2_write_back_count;
    double l2_memory_bandwidth = l2_read_miss_count + l2_write_miss_count + l2_write_back_count; 
    cout<<"\nm. total memory traffic:                     "<<std::fixed<<setprecision(0)<<l2_memory_bandwidth;
  }
return 0;
}
