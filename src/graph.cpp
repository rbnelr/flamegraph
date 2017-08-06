	
	#include "global_includes.hpp"
	
	void global_init () {
		
		assert_page_size();
		
		print_n::init(); // TODE: DEPENDS on print_working_stk, so do threadlocal_init() before doing global_init() in main thread
		
		time::QPC::init();
		
	}
	
	struct Cmd_Line {
		u32		argc;
		cstr*	argv;
	};
	DECLD Cmd_Line cmd_line;
	
////
	#include "platform.hpp"
	
	s32 main (s32 argc, cstr* argv) {
		
		cmd_line.argc = MAX(argc, 0);
		cmd_line.argv = argv;
		
		global_init();
		
		return platform::msg_thread_main();
	}
	