
#define DBG_DO_MEMSET DBG_MEMORY

struct Stack {
	byte*	top;
	byte*	end;
	uptr	reserveSize;
	byte*	base;
	
	DECLM NOINLINE void resize ();
	
	DECLM void purge ();
	
	DECLM void decommit_all ();
	
	DECLM byte* _push (uptr size) {
		auto ret = top;
		
		top += size;
		if (UNLIKELY(top > end)) {
			resize();
		}
		
		#if DBG_DO_MEMSET
		cmemset(ret, DBG_MEM_UNINITIALIZED_BYTE, size);
		#endif
		
		return ret;
	}
	template <uptr ALIGN>
	DECLM byte * _pushAlign (uptr size) {
		
		#if DBG_DO_MEMSET
		byte* old_top = top;
		#endif
		
		top = align_up(top, ALIGN);
		
		#if DBG_DO_MEMSET
		cmemset(old_top, DBG_MEM_ALIGN_PADDING_BYTE, ptr_sub(old_top, top));
		#endif
		
		return _push(size);
	}
	
	// DEFAULT
	template <typename T>	DECLM								T * push () {
		return reinterpret_cast<T*>( _pushAlign<alignof(T)>(sizeof(T)) );
	}
	template <typename T>	DECLM								T * push (T val) {
		auto ret = push<T>();
		*ret = val;
		return ret;
	}
	template <typename T>	DECLM								T * pushArr (uptr count) {
		return reinterpret_cast<T*>( _pushAlign<alignof(T)>(sizeof(T) * count) );
	}
	
	// NO_ALIGN
	template <typename T>	DECLM								T * pushNoAlign () {
		return reinterpret_cast<T*>( _push(sizeof(T)) );
	}
	template <typename T>	DECLM								T * pushNoAlign (T val) {
		auto ret = pushNoAlign<T>();
		*ret = val;
		return ret;
	}
	template <typename T>	DECLM								T * pushArrNoAlign (uptr count) {
		return reinterpret_cast<T*>( _push(sizeof(T) * count) );
	}
	
	// ALIGN
	template <uptr ALIGN, typename T>	DECLM					T * pushAlign () {
		return reinterpret_cast<T*>( _pushAlign<ALIGN>(sizeof(T)) );
	}
	template <uptr ALIGN, typename T>	DECLM					T * pushAlign (T val) {
		auto ret = pushAlign<ALIGN, T>();
		*ret = val;
		return ret;
	}
	template <uptr ALIGN, typename T>	DECLM					T * pushArrAlign (uptr count) {
		return reinterpret_cast<T*>( _pushAlign<ALIGN>(sizeof(T) * count) );
	}
	
	#if 0
	DECLM byte* pushDynamic (uptr size) {
		return _push(size);
	}
	#endif
	
	//
	template <typename T=byte>
	DECLM T * getTop () const {
		return reinterpret_cast<T*>(align_up(top, alignof(T)));
	}
	template <typename T=byte>
	DECLM T * getTopNoAlign () const {
		return reinterpret_cast<T*>(top);
	}
	template <uptr ALIGN, typename T=byte>
	DECLM T * getTopAlign () const {
		return reinterpret_cast<T*>(align_up(top, ALIGN));
	}
	
	#if 0 // use pop instead
	template <typename T>
	DECLM T* setTop (T* ptr) {
		top = reinterpret_cast<byte*>(ptr);
		return ptr;
	}
	#endif
	
	template <uptr ALIGN, typename T=byte>
	DECLM T* alignTop () {
		return top = getTopAlign<ALIGN, T>();
		
	}
	
	DECLM void alignTopDynamic (uptr align) {
		top = align_up(top, align);
	}
	
	DECLM uptr getTopBytes () const {
		return ptr_sub(top, end);
	}
	
	template <uptr ALIGN, typename T=byte> DECLM T* zeropad_to_align () {
		u8* cur = getTop<byte>();
		u8* end = align_up(cur, ALIGN);
		pushArrZero<byte>(ptr_sub(cur, end));
		return reinterpret_cast<T*>(cur);
	}
	
	template <typename T>
	DECLM void pop (T* ptr_) {
		byte* ptr = const_cast<byte*>(reinterpret_cast<byte const*>(ptr_)); // we want to be able to pop the stack to a const char*
		
		#if DBG_DO_MEMSET
		uptr pop_size = ptr_sub(ptr, top);
		#endif
		
		top = reinterpret_cast<byte*>(ptr);
		
		#if DBG_DO_MEMSET
		cmemset(top, DBG_MEM_FREED_BYTE, pop_size);
		#endif
	}
	
	DECLM void pop_all () {
		pop(base);
	}
	
	template <typename T>	DECLM	T* append (T val) {
		return push<T>(val);
	}
	template <typename T>	DECLM	T* append (T const* arr, uptr count) {
		T* ret = pushArr<T>(count);
		cmemcpy(ret, arr, sizeof(T)*count);
		return ret;
	}
};
	
	template <uptr ALIGN=kibi(64)>
	DECL Stack makeStack (uptr commitSize, uptr reserveSize);

#define _DEFER_POP(stack_ptr, counter) auto CONCAT(stack_top_, counter) = stack_ptr->getTop(); _defer(counter) { stack_ptr->pop(CONCAT(stack_top_, counter)); }
#define DEFER_POP(stack_ptr) _DEFER_POP((stack_ptr), __COUNTER__)
