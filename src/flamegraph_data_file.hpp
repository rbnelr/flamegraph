
#pragma pack (push, 1)

enum block_type_flags_e : u32 {
	BT_NONE =	0x0,
	BT_WAIT =	0x1,
	
};
DEFINE_ENUM_FLAG_OPS(block_type_flags_e, u32)

namespace flamegraph_data_file {
	union file_id {
		char	str[8];
		u64		i;
	};
	
	DECLD constexpr file_id FILE_ID = {{'R','A','Z','_','F','G','E','V'}};
	
	struct File_Header {
		file_id id;
		u64		file_size; // file size SHOULD never be even close to 4 GiB, but could in theory be (block_count::max * sizeof(Block))
		u32		total_event_count;
		u32		thr_name_str_tbl_size;
		u32		thread_count;
		u32		chunks_count;
	};
	struct Thread {
		f32		sec_per_unit;
		u32		name_tbl_offs; // index into name_str_tbl
		u32		event_count;
	};
	
	struct Chunk { // Chunks can get streamed into the file
		u64		offs_to_next; // bytes to next Chunk struct, to allow linked list style traversal
		u32		event_count;
		u32		index;
		u64		ts_begin;
		u64		ts_length;
		//char	name[]; // null terminated string
		//Event events[event_count];
	};
	struct Event {
		u64		ts;
		u32		index;
		u8		thread_indx;
		//char name[]; // null terminated string
	};
	
	#if 0
	struct File {
		File_Header		header;
		Thread			threads[thread_count];
		char			thr_name_str_tbl[header.str_tbl_size]; // a bunch of null-terminated strings representing names of blocks
		
		Chunk			chunks[header.chunks_count];
		
		// ... and so on
	};
	#endif
	
	DECL mlstr get_thread_name (u32 thread_i, File_Header* header, Thread* threads, char* thr_str_tbl) {
		assert(thread_i < header->thr_name_str_tbl_size);
		assert(threads[thread_i].name_tbl_offs < header->thr_name_str_tbl_size);
		return mlstr::count_cstr( thr_str_tbl +threads[thread_i].name_tbl_offs );
	}
	
	DECL mlstr get_chunk_name (Chunk* chunk) {
		return mlstr::count_cstr( (char*)(chunk +1) );
	}
	
	DECL Event* get_chunk_events (mlstr chunk_name) {
		return (Event*)(chunk_name.str +chunk_name.len +1);
	}
	
	DECL mlstr get_event_name (Event* event) {
		return mlstr::count_cstr( (char*)(event +1) );
	}
	
	DECL Event* get_next_event (mlstr event_name) {
		return (Event*)(event_name.str +event_name.len +1);
	}
	
}
#pragma pack (pop)
