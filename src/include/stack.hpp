
#define DBG_DO_MEMSET DBG_MEMORY

DECL void assert_page_size () {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	
	if (info.dwPageSize != 4096) {
		DBGBREAK;
	}
}

DECLM NOINLINE void Stack::resize () {
	uptr new_size = ptr_sub(base, top);
	
	uptr cur_cap = ptr_sub(base, end);
	while (new_size > cur_cap) {
		cur_cap = cur_cap ? cur_cap * 2 : 4096;
	}
	{
		auto cur = units::Bytes(cur_cap);
		auto new_ = units::Bytes(new_size);
		auto reserve = units::Bytes(reserveSize);
		
		assert(cur_cap <= reserveSize,
				"stack expanded to: %% (requested size: %%) reserved address space: %%\n",
				cur.val,cur.unit, new_.val,new_.unit, reserve.val,reserve.unit);
		auto ret = VirtualAlloc(base, cur_cap, MEM_COMMIT, PAGE_READWRITE);
		assert(ret != nullptr);
	}
	
	#if DBG_DO_MEMSET
	byte* old_end = end;
	#endif
	
	end = base +cur_cap;
	
	#if DBG_DO_MEMSET
	cmemset(old_end, DBG_MEM_UNALLOCATED_BYTE, ptr_sub(old_end, end));
	#endif
}

DECLM void Stack::purge () {
	uptr cur_size = ptr_sub(base, top);
	uptr cur_cap = ptr_sub(base, end);
	
	assert(is_aligned<uptr>(cur_cap, 4096));
	
	uptr new_cap = 0;
	while (cur_size > new_cap) {
		new_cap = new_cap ? new_cap * 2 : 4096;
	}
	
	end = base +new_cap;
	
	byte* addr = base +new_cap;
	uptr size = cur_cap -new_cap;
	
	if (size > 0) {
		auto ret = VirtualFree(addr, size, MEM_DECOMMIT);
		if (ret == 0) {
			auto err = GetLastError();
			assert(false, "%", err);
		}
	}
	
	
}

DECLM void Stack::decommit_all () {
	uptr cur_cap = ptr_sub(base, end);
	
	assert(is_aligned<uptr>(cur_cap, 4096));
	
	end = base;
	
	if (cur_cap > 0) {
		auto ret = VirtualFree(base, cur_cap, MEM_DECOMMIT);
		if (ret == 0) {
			auto err = GetLastError();
			assert(false, "%", err);
		}
	}
}


template <uptr ALIGN>
DECL Stack makeStack (uptr commitSize, uptr reserveSize) {
	static_assert(ALIGN <= kibi(64), "");
	
	Stack stack;
	stack.reserveSize = reserveSize;
	{ // Reserve address space for stack
		auto ret = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE);
		assert(ret != nullptr, "%", reserveSize);
		stack.base = reinterpret_cast<byte*>(ret);
	}
	if (commitSize != 0) {
		auto ret = VirtualAlloc(stack.base, commitSize, MEM_COMMIT, PAGE_READWRITE);
		assert(ret != nullptr);
	}
	stack.top = stack.base;
	stack.end = stack.base +commitSize;
	return stack;
}
DECL void freeStack (Stack* stack) {
	auto ret = VirtualFree(stack->base, 0, MEM_RELEASE);
	assert(ret != 0);
	
	#if 1
	stack->top = nullptr;
	stack->end = nullptr;
	stack->reserveSize = 0;
	stack->base = nullptr;
	#endif
}
