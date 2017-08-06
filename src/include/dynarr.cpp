
template <typename T, typename LEN_T> DECLM void dynarr<T, LEN_T>:: init (LEN_T init_len, LEN_T init_cap) {
	
	this->len = init_len;
	assert(init_cap >= init_len);
	cap = init_cap;
	
	cap = alignup_power_of_two(cap);
	
	this->arr = (T*)malloc(cap*sizeof(T));
	
	#if DBG_MEMORY
	cmemset(arr, DBG_MEM_UNALLOCATED_BYTE, cap*sizeof(T));
	#endif
}
template <typename T, typename LEN_T> DECLM void dynarr<T, LEN_T>:: init (LEN_T init_cap) {
	init(0, init_cap);
}

template <typename T, typename LEN_T> DECLM void dynarr<T, LEN_T>:: free () {
	assert(this->arr != nullptr);
	
	::free(this->arr);
	#if DBG_INTERNAL
	arr = nullptr;
	#endif
}

template <typename T, typename LEN_T> DECLM void dynarr<T, LEN_T>:: _resize_cap (LEN_T new_cap) {
	
	if (new_cap != cap) {
		assert(this->arr != nullptr);
		this->arr = (T*)realloc((void*)this->arr, new_cap*sizeof(T));
		
		#if DBG_MEMORY
		if (new_cap > cap) {
			cmemset(arr +cap, DBG_MEM_UNALLOCATED_BYTE, (new_cap -cap)*sizeof(T));
		}
		#endif
		
		cap = new_cap;
	}
}

DECL void _dyn_array_generic__init (dynarr<byte>* arr, u32 init_len, u32 init_cap, uptr stride) {
	
	arr->len = init_len;
	assert(init_cap >= init_len);
	arr->cap = init_cap;
	
	arr->cap = alignup_power_of_two(arr->cap);
	
	arr->arr = (byte*)malloc(arr->cap*stride);
	
	#if DBG_MEMORY
	cmemset(arr->arr, DBG_MEM_UNALLOCATED_BYTE, arr->cap*stride);
	#endif
	
}

DECL byte* _dyn_array_generic__index (dynarr<byte>* arr, u32 indx, uptr stride) {
	return arr->arr +(indx*stride);
}

DECL void _dyn_array_generic__resize_cap (dynarr<byte>* arr, u32 new_cap, uptr stride) {
	
	if (new_cap != arr->cap) {
		arr->arr = (byte*)realloc((void*)arr->arr, new_cap*stride);
		
		#if DBG_MEMORY
		if (new_cap > arr->cap) {
			cmemset(arr->arr +(arr->cap * stride), DBG_MEM_UNALLOCATED_BYTE, (new_cap -arr->cap)*stride);
		}
		#endif
		
		arr->cap = new_cap;
	}
}

DECL void _dyn_array_generic__fit_cap (dynarr<byte>* arr, uptr stride) {
	_dyn_array_generic__resize_cap( arr, alignup_power_of_two(arr->len), stride );
}
DECL byte* _dyn_array_generic__grow_to (dynarr<byte>* arr, u32 new_len, uptr stride) {
	u32 old_len = arr->len;
	arr->len = new_len;
	if (arr->len > arr->cap) {
		_dyn_array_generic__fit_cap(arr, stride);
	}
	#if DBG_MEMORY
	if (arr->len > old_len) {
		cmemset(&arr->arr[old_len*stride], DBG_MEM_UNINITIALIZED_BYTE, (arr->len -old_len)*stride);
	}
	#endif
	return _dyn_array_generic__index(arr, old_len, stride);
}

DECL byte* _dyn_array_generic__grow_by (dynarr<byte>* arr, u32 diff, uptr stride) {
	return _dyn_array_generic__grow_to(arr, arr->len +diff, stride);
}
DECL byte* _dyn_array_generic__append (dynarr<byte>* arr, uptr stride) {
	return _dyn_array_generic__grow_by(arr, 1, stride);
}
