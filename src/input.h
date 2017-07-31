
namespace input {
	
	// button states are c++ bools, ie. false=0 and true=1
	// false:	button up
	// true:	button down
	
	//typedef s32						freecamDir_t;
	//typedef tv3<freecamDir_t>		freecamDir_v;
	
	namespace buttons {
		enum button_e : u8 {
			_FIRST_BUTTON=0,
			
			// Mouse buttons
			LMB=0, MMB, RMB, MB4, MB5,
			// Keyboard buttons
			L_CTRL, R_CTRL,
			L_SHIFT, R_SHIFT,
			L_ALT, R_ALT,
			L_LOGO, R_LOGO,
			ENTER, NP_ENTER,
			APP, SPACE, CAPSLOCK, TABULATOR,
			BACKSPACE,
			LEFT, DOWN, RIGHT, UP,
			A, B, C, D, E, F, G, H, I, J, K, L, M,
			N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			N_1, N_2, N_3, N_4, N_5, N_6, N_7, N_8, N_9, N_0,
			F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			NP_0, NP_1, NP_2, NP_3, NP_4, NP_5, NP_6, NP_7, NP_8, NP_9,
			
			_NUM_BUTTONS
		};
		DEFINE_ENUM_ITER_OPS(button_e, u8)
		
		constexpr const char * buttonCStrs[_NUM_BUTTONS] = {
			// Mouse buttons
			"LMB", "MMB", "RMB", "MB4", "MB5",
			// Keyboard buttons
			"L_CTRL", "R_CTRL", "L_SHIFT", "R_SHIFT",
			"L_ALT", "R_ALT", "L_LOGO", "R_LOGO", "APP",
			"SPACE", "CAPSLOCK", "TABULATOR", "ENTER", "NP_ENTER", "BACKSPACE",
			"LEFT", "DOWN", "RIGHT", "UP",
			"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
			"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
			"N_1", "N_2", "N_3", "N_4", "N_5", "N_6", "N_7", "N_8", "N_9", "N_0",
			"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
			"NP_0", "NP_1", "NP_2", "NP_3", "NP_4", "NP_5", "NP_6", "NP_7", "NP_8", "NP_9"
		};
		
		DECLD constexpr button_e	_FIRST_MOUSE_BUTTON =	LMB;
		DECLD constexpr button_e	_MAX_MOUSE_BUTTON =		(button_e)(MB5 +1);
		DECLD constexpr u8			_NUM_MOUSE_BUTTON =		_MAX_MOUSE_BUTTON -_FIRST_MOUSE_BUTTON;
	}
	
	namespace button_bindings {
		enum group_e : u8 {
			UNBOUND=0,
			MOUSE,
			KEYB,
		};
		enum function_e : u8 {
			NONE=0,
			MOUSE_LMB=0,
			MOUSE_RMB,
			KEYB_CTRL=0,
		};
		struct Binding {
			group_e			group;
			function_e		function;
		};
		
		#define BUTTON_BINDINGS \
				X( LMB,		MOUSE,		LMB ) \
				X( RMB,		MOUSE,		RMB ) \
				X( L_CTRL,	KEYB,		CTRL )
		
		Binding getButtonBinding (buttons::button_e button) {
			using namespace buttons;
			switch (button) {
				
				#define X(key, binding_group, binding_func) \
						case key:	return { binding_group, binding_group##_##binding_func };
				
				BUTTON_BINDINGS
				
				#undef X
				
				default:		return { UNBOUND, NONE };
			}
		}
		
	}
	
	namespace print_button_bindings {
		
		#define X(key, binding_group, binding_func) buttons::key,
		constexpr u8 bound_key_arr[] = {
			BUTTON_BINDINGS
		};
		constexpr uptr count = arrlenof(bound_key_arr);
		#undef X
		
		#define X(key, binding_group, binding_func) TO_STR(binding_group) "." TO_STR(binding_func),
		constexpr char const* str_arr[] = {
			BUTTON_BINDINGS
		};
		static_assert(count == arrlenof(str_arr), "");
		#undef X
		
		void print () {
			::print("Button bindings:\n");
			for (uptr i=0; i<count; ++i) {
				::print("  bound % to %\n", buttons::buttonCStrs[bound_key_arr[i]], str_arr[i]);
			}
		}
	}
	
	#undef BUTTON_BINDINGS
	
	struct State {
		
		bool			lmb_down;
		bool			rmb_down;
		bool			ctrl_down;
		
		ui				lmb_count;
		ui				rmb_count;
		
		s32				mouse_wheel_accum;
		
		void clear_accums () {
			
			lmb_count =			0;
			rmb_count =			0;
			
			mouse_wheel_accum = 0;
		}
		
		void process_button_binding_input (buttons::button_e button, bool down) {
			using namespace button_bindings;
			
			auto binding = getButtonBinding(button);
			if (binding.group == UNBOUND) {
				return;
			}
			
			switch (binding.group) {
				
				case MOUSE: {
					
					switch (binding.function) {
						
						case MOUSE_LMB: {
							lmb_down = down;
							++lmb_count;
						} break;
						case MOUSE_RMB: {
							rmb_down = down;
							++rmb_count;
						} break;
						
						default: assert(false);
					}
					
				} break;
				
				case KEYB: {
					
					switch (binding.function) {
						
						case KEYB_CTRL: {
							ctrl_down = down;
						} break;
						
						default: assert(false);
					}
					
				} break;
				
				case UNBOUND:	assert(false);
				default:		assert(false);
			}
		}
		
		DECLM void process_mouse_rel (resolution_v diff) {
			#if 0
			if (mouselook) {
				mouselook_accum += diff;
			}
			#endif
		}
		
		DECLM void process_mouselook_change (bool mouselook_enabled) {
			#if 0
			mouselook &= ~FPS_MOUSELOOK;
			mouselook |= mouselook_enabled ? FPS_MOUSELOOK : 0;
			#endif
		}
		
		DECLM void process_mouse_wheel_rel_input (s32 diff) {
			mouse_wheel_accum += diff;
		}
	};
	
	State		shared_state = {};
	
	void get_shared_state (State* state) {
		
		*state = shared_state;
		
		shared_state.clear_accums();
	}
	
	// Input that comes that is queried directly in game loop
	struct Sync_Input {
		
		resolution_v		window_res;
		resolution_v		mouse_cursor_pos;
		bool				window_res_changed;
		bool				mouse_cursor_in_window;
		
	};
}
