	
//// OpenGL
	#include "oglInterface.h"
	
	struct Mem_Block {
		byte*	ptr;
		uptr	size;
	};
	
	typedef u32 rd_flags_t;
	enum rd_flags_e : rd_flags_t {
		RD_FILENAME_ON_STK =		0x1,
		RD_NULL_TERMINATE_CHAR =	0x2,
	};
	
namespace platform {
	
	DECL err_e read_whole_file_onto_heap (char const* filename, rd_flags_t flags, Mem_Block* out_data) {
		
		HANDLE handle;
		auto err = win32::open_existing_file_rd(filename, &handle);
		if (err) { return err; }
		defer {
			win32::close_file(handle);
		};
		
		u64 file_size = win32::get_file_size(handle);
		
		// 
		bool null_terminate = (flags & RD_NULL_TERMINATE_CHAR) != 0;
		
		auto data = (char*)malloc(file_size +(null_terminate ? 1 : 0));
		
		err = win32::read_file(handle, data, file_size);
		if (err) { return err; }
		
		if (null_terminate) {
			data[file_size] = '\0';
		}
		
		*out_data = { (byte*)data, file_size };
		return OK;
	}
	
	
	DECLD constexpr u32 FAST_FILE_READ_ALIGN = 64;
	
	DECL err_e read_whole_file_onto (Stack* stk, char const* filename, rd_flags_t flags, Mem_Block* out_data) {
		
		assert((flags & ~(RD_FILENAME_ON_STK|RD_NULL_TERMINATE_CHAR)) == 0,
				"read_whole_file_onto_stack flags argument invalid!");
		
		HANDLE handle;
		auto err = win32::open_existing_file_rd(filename, &handle);
		if (err) { return err; }
		defer {
			win32::close_file(handle);
		};
		
		if (flags & RD_FILENAME_ON_STK) {
			stk->pop(filename);
		}
		
		u64 file_size = win32::get_file_size(handle);
		
		// 
		bool null_terminate = (flags & RD_NULL_TERMINATE_CHAR) != 0;
		
		auto data = stk->pushArrNoAlign<byte>(file_size +(null_terminate ? 1 : 0));
		
		err = win32::read_file(handle, data, file_size);
		if (err) { return err; }
		
		if (null_terminate) {
			reinterpret_cast<char*>(data)[file_size] = '\0';
		}
		
		*out_data = { data, file_size };
		return OK;
	}
	
	DECL err_e write_whole_file (char const* filename, void const* data, u64 size) {
		HANDLE handle;
		auto err = win32::overwrite_file_wr(filename, &handle);
		if (err) { return err; }
		
		defer {
			// Close file
			win32::close_file(handle);
		};
		
		err = win32::write_file(handle, data, size);
		if (err) { return err; }
		
		return OK;
	}
}
	
	enum img_color_channels_e : u32 {
		CH_RED =	1,
		CH_BGR =	3,
		CH_BGRA =	4,
	};
	
	#define THR_ENGINE 
	#define PROFILE_SCOPED(...) 
	#define PROFILE_BEGIN(...) 
	#define PROFILE_END(...) 
	
//// Time
namespace time {
	
	DECLD u64					cpu_qpc_process_begin;
	DECLD u64					cpu_qpc_prev;
	
	DECL void process_begin () {
		cpu_qpc_process_begin = QPC::get_time();
		cpu_qpc_prev = cpu_qpc_process_begin;
	}
	
	DECL void startup_event (char const* event_name) {
		u64 now = QPC::get_time();
		
		u64 since_begin = now -cpu_qpc_process_begin;
		u64 since_prev = now -cpu_qpc_prev;
		
		auto tsince_begin = units::Time(since_begin);
		auto tsince_prev = units::Time(since_prev);
		
		//print("Startup event at %% (dt: %%): %\n",
		//		tsince_begin.val,tsince_begin.unit, tsince_prev.val,tsince_prev.unit, event_name);
		
		cpu_qpc_prev = now;
	}
	
	DECL void startup_event_worker_thread (char const* thread_name, char const* event_name) {
		u64 now = QPC::get_time();
		
		u64 since_begin = now -cpu_qpc_process_begin;
		
		auto tsince_begin = units::Time(since_begin);
		
		//print("%: Startup event at %%: %\n",
		//		thread_name, tsince_begin.val,tsince_begin.unit, event_name);
		
	}
	
	DECLD u64			cpu_qpc_prev_frame_end;
	// timestep is last how long the last frame took to calculate and display (difference between last end of frame and current end of frame, measured after SwapBuffers())
	//	-> time is never missed, at no point does time elapse without it being measured, not even during the measurement itself
	//	-> the resulting timestep is the last frametime
	// first frame is calculated with zero timestep, so on the first frame everything should be in it's inital state
	//	-> is it safe to have a timestep of zero?
	//	-> what are the alternatives to this?
	DECLD u64			cpu_qpc_frame_period;
	
	DECLD u64			cpu_qpc_this_frame_work_end;
	
	DECLD f64			cpu_qpc_prev_frame_ms = 0.0;
	
	DECL u64 get_dt () {
		return cpu_qpc_frame_period;
	}
	
	DECL void inital_frame () {
		cpu_qpc_prev_frame_end = QPC::get_time();
		cpu_qpc_frame_period = 0;
		
	}
	DECL void inital_frame_after_sleep () {
		cpu_qpc_prev_frame_end = QPC::get_time();
		cpu_qpc_frame_period = 0;
		
	}
	
	DECL void cpu_work_done () {
		cpu_qpc_this_frame_work_end = QPC::get_time();
	}
	
	DECL f64 frame_end (u32 frame_number) {
		u64 this_frame_end = time::QPC::get_time();
		
		cpu_qpc_frame_period =				this_frame_end					-cpu_qpc_prev_frame_end;
		u64 cpu_qpc_this_frame_work_diff =	cpu_qpc_this_frame_work_end		-cpu_qpc_prev_frame_end;
		
		f64 msecs;
		{
			f64 diff =				(f64)cpu_qpc_frame_period;
			
			msecs =					(diff * QPC::inv_freq) * 1000;
			f64 fps =				QPC::freq / diff;
			
			cpu_qpc_prev_frame_ms = msecs;
			
			f64 cpu_work_msec =		((f64)cpu_qpc_this_frame_work_diff * QPC::inv_freq) * 1000;
			f64 cpu_work_percent =	(cpu_work_msec / msecs) * 100;
			
			#if 0
			print(
					"Frame#% % fps - % msec cpu_work: % ms (%\\%)\n", frame_number,
					fps, msecs, cpu_work_msec, cpu_work_percent);
			
			#endif
		}
		
		cpu_qpc_prev_frame_end =	this_frame_end;
		
		return msecs;
	}
	
}
	
////
	DECLD constexpr lstr	SHADERS_DIR =						"shaders/";
	#define					TEXTURES_DIR						"textures/"
	#define					WINDOW_PLACEMENT_SAVE_FILENAME		"saves/window_placement_%.save\\0" // % is some unique computer identifier
	#define					WINDOW_PLACEMENT_SAVE_FILENAME		"saves/window_placement_%.save\\0" // % is some unique computer identifier
	
	DECLD cstr				input_filename;
	
	typedef s32							resolution_t;
	typedef tv2<resolution_t>			resolution_v;
	
namespace platform {
	
	DECLD HANDLE	h_engine_thread;
	
	//// GL context
	DECLD HWND		hWnd =											NULL;
	DECLD HMODULE	hInstance;
	DECLD HDC		hDC;
	DECLD HGLRC		hGLContext;
	
}
	
	#include "input.h"
	
	namespace input {
		
		DECLD sync::Mutex				shared_state_mutex;
		
		bool						do_close_application = false;
		
		DECL void query (State* inp, Sync_Input* sinp) {
			
			{
				SYNC_SCOPED_MUTEX_LOCK(&shared_state_mutex);
				get_shared_state(inp);
			}
			
			{
				resolution_v res;
				{
					RECT rect;
					auto ret = GetClientRect(platform::hWnd, &rect);
					assert(ret != 0);
					res = safe_cast_assert_v2<resolution_v>(rect.right -rect.left, rect.bottom -rect.top);
				}
				res = MAX_v(res, resolution_v(1)); // make sure the engine does no div by zero
				
				sinp->window_res_changed = !identical( res, sinp->window_res );
				sinp->window_res = res;
			}
			{
				POINT point;
				{
					auto ret = GetCursorPos(&point);
					if (ret == 0) {
						warning("GetCursorPos() failed [].", GetLastError());
						point = {0,0};
					}
				}
				{
					auto ret = ScreenToClient(platform::hWnd, &point);
					if (ret == 0) {
						warning("ScreenToClient() failed [].", GetLastError());
						point = {0,0};
					}
				}
				
				point.y = (sinp->window_res.y -1) -point.y;
				
				sinp->mouse_cursor_pos = safe_cast_assert_v2<resolution_v>(point.x, point.y);
				
				sinp->mouse_cursor_in_window = (	sinp->mouse_cursor_pos >= resolution_v(0) &&
													sinp->mouse_cursor_pos < sinp->window_res ).all();
			}
		}
		
		DECLD resolution_v			mouse_confinement_point;
		
		// Platform side input handling
		DECLD bool	shiftState =	false;
		DECLD bool	altState =		false;
		DECLD bool	f1State =		false;
		DECLD bool	f4State =		false;
		DECLD bool	f10State =		false;
		DECLD bool	f11State =		false;
		
		DECLD bool											mouselook_cursor = false;
		DECLD resolution_v									gui_mouse_pos;
		DECLD BitArray<input::buttons::_NUM_BUTTONS>		gameButtonState = {}; // initialized to false
		
		DECL void calc_mouse_confinement_point (RECT clientRect) {
			auto w = clientRect.right -clientRect.left;
			auto h = clientRect.bottom -clientRect.top;
			mouse_confinement_point = safe_cast_assert_v2<resolution_v>(	clientRect.left +(w / 2),
																		clientRect.top +(h / 2) );
		}
		
		namespace mapVirtualKeyToGameKey_table {
			using namespace input;
			using namespace buttons;
			
			static_assert(_NUM_BUTTONS <= 0xff, "");
			DECLD constexpr button_e ALL_BITS = static_cast<button_e>(0xff);
			
			DECLD constexpr input::buttons::button_e _table[256] = {
				/* 0x00: NULL? */					ALL_BITS,
				/* 0x01: VK_LBUTTON */				ALL_BITS,
				/* 0x02: VK_RBUTTON */				ALL_BITS,
				/* 0x03: VK_CANCEL */				ALL_BITS,
				/* 0x04: VK_MBUTTON */				ALL_BITS,
				/* 0x05: VK_XBUTTON1 */				ALL_BITS,
				/* 0x06: VK_XBUTTON2 */				ALL_BITS,
				/* 0x07: Undefined */				ALL_BITS,
				/* 0x08: VK_BACK */				BACKSPACE,
				/* 0x09: VK_TAB */				TABULATOR,
				/* 0x0A: Reserved */				ALL_BITS,
				/* 0x0B: Reserved */				ALL_BITS,
				/* 0x0C: CLEAR */					ALL_BITS,
				/* 0x0D: VK_RETURN */			ENTER,
				/* 0x0E: Undefined */				ALL_BITS,
				/* 0x0F: Undefined */				ALL_BITS,
				/* 0x10: VK_SHIFT */			L_SHIFT,
				/* 0x11: VK_CONTROL */			L_CTRL,
				/* 0x12: VK_MENU */				L_ALT,
				/* 0x12: VK_PAUSE */				ALL_BITS,
				/* 0x14: VK_CAPITAL */			CAPSLOCK,
				/* 0x15: VK_HANGUL */				ALL_BITS,
				/* 0x16: Undefined */				ALL_BITS,
				/* 0x17: VK_JUNJA */				ALL_BITS,
				/* 0x18: VK_FINAL */				ALL_BITS,
				/* 0x19: VK_HANJA */				ALL_BITS,
				/* 0x1A: VK_KANJI */				ALL_BITS,
				/* 0x1B: VK_ESCAPE */				ALL_BITS, // ESC
				/* 0x1C: VK_CONVERT */				ALL_BITS,
				/* 0x1D: VK_NONCONVERT */			ALL_BITS,
				/* 0x1E: VK_ACCEPT */				ALL_BITS,
				/* 0x1F: VK_MODECHANGE */			ALL_BITS,
				/* 0x20: VK_SPACE */			SPACE,
				/* 0x21: VK_PRIOR */				ALL_BITS,
				/* 0x22: VK_NEXT */					ALL_BITS,
				/* 0x23: VK_END */					ALL_BITS,
				/* 0x24: VK_HOME */					ALL_BITS,
				/* 0x25: VK_LEFT */				LEFT,
				/* 0x26: VK_UP */				UP,
				/* 0x27: VK_RIGHT */			RIGHT,
				/* 0x28: VK_DOWN */				DOWN,
				/* 0x29: VK_SELECT */				ALL_BITS,
				/* 0x2A: VK_PRINT */				ALL_BITS,
				/* 0x2B: VK_EXECUTE */				ALL_BITS,
				/* 0x2C: VK_SNAPSHOT */				ALL_BITS,
				/* 0x2D: VK_INSERT */				ALL_BITS,
				/* 0x2E: VK_DELETE */				ALL_BITS,
				/* 0x2F: VK_HELP */					ALL_BITS,
				/* 0x30: 0 */					N_0,
				/* 0x31: 1 */					N_1,
				/* 0x32: 2 */					N_2,
				/* 0x33: 3 */					N_3,
				/* 0x34: 4 */					N_4,
				/* 0x35: 5 */					N_5,
				/* 0x36: 6 */					N_6,
				/* 0x37: 7 */					N_7,
				/* 0x38: 8 */					N_8,
				/* 0x39: 9 */					N_9,
				/* 0x3A: Undefined */				ALL_BITS,
				/* 0x3B: Undefined */				ALL_BITS,
				/* 0x3C: Undefined */				ALL_BITS,
				/* 0x3D: Undefined */				ALL_BITS,
				/* 0x3E: Undefined */				ALL_BITS,
				/* 0x3F: Undefined */				ALL_BITS,
				/* 0x40: Undefined */				ALL_BITS,
				/* 0x41: A */					A,
				/* 0x42: B */					B,
				/* 0x43: C */					C,
				/* 0x44: D */					D,
				/* 0x45: E */					E,
				/* 0x46: F */					F,
				/* 0x47: G */					G,
				/* 0x48: H */					H,
				/* 0x49: I */					I,
				/* 0x4A: J */					J,
				/* 0x4B: K */					K,
				/* 0x4C: L */					L,
				/* 0x4D: M */					M,
				/* 0x4E: N */					N,
				/* 0x4F: O */					O,
				/* 0x50: P */					P,
				/* 0x51: Q */					Q,
				/* 0x52: R */					R,
				/* 0x53: S */					S,
				/* 0x54: T */					T,
				/* 0x55: U */					U,
				/* 0x56: V */					V,
				/* 0x57: W */					W,
				/* 0x58: X */					X,
				/* 0x59: Y */					Y,
				/* 0x5A: Z */					Z,
				/* 0x5B: L_LOGO */				L_LOGO,
				/* 0x5C: VK_RWIN */				R_LOGO,
				/* 0x5D: VK_APPS */				APP,
				/* 0x5E: Reserved */				ALL_BITS,
				/* 0x5F: VK_SLEEP */				ALL_BITS,
				/* 0x60: VK_NUMPAD0 */			NP_0,
				/* 0x61: VK_NUMPAD1 */			NP_1,
				/* 0x62: VK_NUMPAD2 */			NP_2,
				/* 0x63: VK_NUMPAD3 */			NP_3,
				/* 0x64: VK_NUMPAD4 */			NP_4,
				/* 0x65: VK_NUMPAD5 */			NP_5,
				/* 0x66: VK_NUMPAD6 */			NP_6,
				/* 0x67: VK_NUMPAD7 */			NP_7,
				/* 0x68: VK_NUMPAD8 */			NP_8,
				/* 0x69: VK_NUMPAD9 */			NP_9,
				/* 0x6A: VK_MULTIPLY */				ALL_BITS,
				/* 0x6B: VK_ADD */					ALL_BITS,
				/* 0x6C: VK_SEPARATOR */			ALL_BITS,
				/* 0x6D: VK_SUBTRACT */				ALL_BITS,
				/* 0x6E: VK_DECIMAL */				ALL_BITS,
				/* 0x6F: VK_DIVIDE */				ALL_BITS,
				/* 0x70: VK_F1 */				F1,
				/* 0x71: VK_F2 */				F2,
				/* 0x72: VK_F3 */				F3,
				/* 0x73: VK_F4 */				F4,
				/* 0x74: VK_F5 */				F5,
				/* 0x75: VK_F6 */				F6,
				/* 0x76: VK_F7 */				F7,
				/* 0x77: VK_F8 */				F8,
				/* 0x78: VK_F9 */				F9,
				/* 0x79: VK_F10 */					ALL_BITS,
				/* 0x7A: VK_F11 */					ALL_BITS,
				/* 0x7B: VK_F12 */					ALL_BITS,
				/* 0x7C: VK_F13 */					ALL_BITS,
				/* 0x7D: VK_F14 */					ALL_BITS,
				/* 0x7E: VK_F15 */					ALL_BITS,
				/* 0x7F: VK_F16 */					ALL_BITS,
				/* 0x80: VK_F17 */					ALL_BITS,
				/* 0x81: VK_F18 */					ALL_BITS,
				/* 0x82: VK_F19 */					ALL_BITS,
				/* 0x83: VK_F20 */					ALL_BITS,
				/* 0x84: VK_F21 */					ALL_BITS,
				/* 0x85: VK_F22 */					ALL_BITS,
				/* 0x86: VK_F23 */					ALL_BITS,
				/* 0x87: VK_F24 */					ALL_BITS,
				/* 0x88: Unassigned */				ALL_BITS,
				/* 0x89: Unassigned */				ALL_BITS,
				/* 0x8A: Unassigned */				ALL_BITS,
				/* 0x8B: Unassigned */				ALL_BITS,
				/* 0x8C: Unassigned */				ALL_BITS,
				/* 0x8D: Unassigned */				ALL_BITS,
				/* 0x8E: Unassigned */				ALL_BITS,
				/* 0x8F: Unassigned */				ALL_BITS,
				/* 0x90: VK_NUMLOCK */				ALL_BITS,
				/* 0x91: VK_SCROLL */				ALL_BITS,
				/* 0x92: OEM specific */			ALL_BITS,
				/* 0x93: OEM specific */			ALL_BITS,
				/* 0x94: OEM specific */			ALL_BITS,
				/* 0x95: OEM specific */			ALL_BITS,
				/* 0x96: OEM specific */			ALL_BITS,
				/* 0x97: Unassigned */				ALL_BITS,
				/* 0x98: Unassigned */				ALL_BITS,
				/* 0x99: Unassigned */				ALL_BITS,
				/* 0x9A: Unassigned */				ALL_BITS,
				/* 0x9B: Unassigned */				ALL_BITS,
				/* 0x9C: Unassigned */				ALL_BITS,
				/* 0x9D: Unassigned */				ALL_BITS,
				/* 0x9E: Unassigned */				ALL_BITS,
				/* 0x9F: Unassigned */				ALL_BITS,
				/* 0xA0: VK_LSHIFT */				ALL_BITS,
				/* 0xA1: VK_RSHIFT */				ALL_BITS,
				/* 0xA2: VK_LCONTROL */				ALL_BITS,
				/* 0xA3: VK_RCONTROL */				ALL_BITS,
				/* 0xA4: VK_LMENU */				ALL_BITS,
				/* 0xA5: VK_RMENU */				ALL_BITS,
				/* 0xA6: VK_BROWSER_BACK */			ALL_BITS,
				/* 0xA7: VK_BROWSER_FORWARD */		ALL_BITS,
				/* 0xA8: VK_BROWSER_REFRESH */		ALL_BITS,
				/* 0xA9: VK_BROWSER_STOP */			ALL_BITS,
				/* 0xAA: VK_BROWSER_SEARCH */		ALL_BITS,
				/* 0xAB: VK_BROWSER_FAVORITES */	ALL_BITS,
				/* 0xAC: VK_BROWSER_HOME */			ALL_BITS,
				/* 0xAD: VK_VOLUME_MUTE */			ALL_BITS,
				/* 0xAE: VK_VOLUME_DOWN */			ALL_BITS,
				/* 0xAF: VK_VOLUME_UP */			ALL_BITS,
				/* 0xB0: VK_MEDIA_NEXT_TRACK */		ALL_BITS,
				/* 0xB1: VK_MEDIA_PREV_TRACK */		ALL_BITS,
				/* 0xB2: VK_MEDIA_STOP */			ALL_BITS,
				/* 0xB3: VK_MEDIA_PLAY_PAUSE */		ALL_BITS,
				/* 0xB4: VK_LAUNCH_MAIL */			ALL_BITS,
				/* 0xB5: VK_LAUNCH_MEDIA_SELECT */	ALL_BITS,
				/* 0xB6: VK_LAUNCH_APP1 */			ALL_BITS,
				/* 0xB7: VK_LAUNCH_APP2 */			ALL_BITS,
				/* 0xB8: Reserved */				ALL_BITS,
				/* 0xB9: Reserved */				ALL_BITS,
				/* 0xBA: VK_OEM_1 */				ALL_BITS,
				/* 0xBB: VK_OEM_PLUS */				ALL_BITS,
				/* 0xBC: VK_OEM_COMMA */			ALL_BITS,
				/* 0xBD: VK_OEM_MINUS */			ALL_BITS,
				/* 0xBE: VK_OEM_PERIOD */			ALL_BITS,
				/* 0xBF: VK_OEM_2 */				ALL_BITS,
				/* 0xC0: VK_OEM_3 */				ALL_BITS,
				/* 0xC1: Reserved */				ALL_BITS,
				/* 0xC2: Reserved */				ALL_BITS,
				/* 0xC3: Reserved */				ALL_BITS,
				/* 0xC4: Reserved */				ALL_BITS,
				/* 0xC5: Reserved */				ALL_BITS,
				/* 0xC6: Reserved */				ALL_BITS,
				/* 0xC7: Reserved */				ALL_BITS,
				/* 0xC8: Reserved */				ALL_BITS,
				/* 0xC9: Reserved */				ALL_BITS,
				/* 0xCA: Reserved */				ALL_BITS,
				/* 0xCB: Reserved */				ALL_BITS,
				/* 0xCC: Reserved */				ALL_BITS,
				/* 0xCD: Reserved */				ALL_BITS,
				/* 0xCE: Reserved */				ALL_BITS,
				/* 0xCF: Reserved */				ALL_BITS,
				/* 0xD0: Reserved */				ALL_BITS,
				/* 0xD1: Reserved */				ALL_BITS,
				/* 0xD2: Reserved */				ALL_BITS,
				/* 0xD3: Reserved */				ALL_BITS,
				/* 0xD4: Reserved */				ALL_BITS,
				/* 0xD5: Reserved */				ALL_BITS,
				/* 0xD6: Reserved */				ALL_BITS,
				/* 0xD7: Reserved */				ALL_BITS,
				/* 0xD8: Unassigned */				ALL_BITS,
				/* 0xD9: Unassigned */				ALL_BITS,
				/* 0xDA: Unassigned */				ALL_BITS,
				/* 0xDB: VK_OEM_4 */				ALL_BITS,
				/* 0xDC: VK_OEM_5 */				ALL_BITS,
				/* 0xDD: VK_OEM_6 */				ALL_BITS,
				/* 0xDE: VK_OEM_7 */				ALL_BITS,
				/* 0xDF: VK_OEM_8 */				ALL_BITS,
				/* 0xE0: Reserved */				ALL_BITS,
				/* 0xE1: OEM specific */			ALL_BITS,
				/* 0xE2: OEM VK_OEM_102 */			ALL_BITS,
				/* 0xE3: OEM specific */			ALL_BITS,
				/* 0xE4: OEM specific */			ALL_BITS,
				/* 0xE5: VK_PROCESSKEY */			ALL_BITS,
				/* 0xE6: OEM specific */			ALL_BITS,
				/* 0xE7: VK_PACKET */				ALL_BITS,
				/* 0xE8: Unassigned */				ALL_BITS,
				/* 0xE9: OEM specific */			ALL_BITS,
				/* 0xEA: OEM specific */			ALL_BITS,
				/* 0xEB: OEM specific */			ALL_BITS,
				/* 0xEC: OEM specific */			ALL_BITS,
				/* 0xED: OEM specific */			ALL_BITS,
				/* 0xEE: OEM specific */			ALL_BITS,
				/* 0xEF: OEM specific */			ALL_BITS,
				/* 0xF0: OEM specific */			ALL_BITS,
				/* 0xF1: OEM specific */			ALL_BITS,
				/* 0xF2: OEM specific */			ALL_BITS,
				/* 0xF3: OEM specific */			ALL_BITS,
				/* 0xF4: OEM specific */			ALL_BITS,
				/* 0xF5: OEM specific */			ALL_BITS,
				/* 0xF6: VK_ATTN */					ALL_BITS,
				/* 0xF7: VK_CRSEL */				ALL_BITS,
				/* 0xF8: VK_EXSEL */				ALL_BITS,
				/* 0xF9: VK_EREOF */				ALL_BITS,
				/* 0xFA: VK_PLAY */					ALL_BITS,
				/* 0xFB: VK_ZOOM */					ALL_BITS,
				/* 0xFC: VK_NONAME */				ALL_BITS,
				/* 0xFD: VK_PA1 */					ALL_BITS,
				/* 0xFE: VK_PLAY */					ALL_BITS,
				/* 0xFF: ? */						ALL_BITS,
			};
		}
		
		DECL bool mapVirtualKeyToGameKey (
				WPARAM vkeyCode, bool isExtendedKey, input::buttons::button_e* outMappedKey) {
			using namespace input;
			using namespace buttons;
			
			button_e ret;
			using namespace mapVirtualKeyToGameKey_table;
			
			ret = _table[vkeyCode];
			switch (ret) {
				case ENTER:
				case L_SHIFT:
				case L_CTRL:
				case L_ALT:
				{
					if (isExtendedKey) {
						ret = static_cast<button_e>( static_cast<u8>(ret) +1 );
					}
				} break;
				default: {
					if (ret == ALL_BITS) {
						return false;
					} else {
						// Do nothing
					}
				} break;
			}
			*outMappedKey = ret;
			return true;
		}
		
		DECL void keyboard_focus_lost () {
			// Act as if all keys are up when we lose keyboard focus
			{
				SYNC_SCOPED_MUTEX_LOCK(&shared_state_mutex);
				
				// Note that we have to enforce a key release order, or else we would have to write extra code to handle this case
				for (ui i=0; i<buttons::_NUM_BUTTONS; ++i) {
					bool state = gameButtonState.get(i);
					if (state == true) {
						
						input::shared_state.process_button_binding_input(
								static_cast<buttons::button_e>(i), false );
					}
				}
				
				if (mouselook_cursor) {
					mouselook_cursor = false;
					input::shared_state.process_mouselook_change(false);
				}
			}
			
			gameButtonState.clear_all();
		}
		
		DECL void toggle_mouselook_cursor () {
			using namespace input;
			
			mouselook_cursor = !mouselook_cursor;
			
			if (mouselook_cursor) {
				auto temp = safe_cast_assert_v2<tv2<int>>(mouse_confinement_point);
				auto ret = SetCursorPos(temp.x, temp.y);
				assert(ret != 0); // this can happen when ctrl-alt-del'ing ???
				// getlasterror and print instead of assert
			} else {
				auto temp = safe_cast_assert_v2<tv2<int>>(gui_mouse_pos);
				POINT pos = { temp.x, temp.y };
				{
					auto ret = ClientToScreen(platform::hWnd, &pos);
					assert(ret != 0);
				}
				auto ret = SetCursorPos(pos.x, pos.y);
				assert(ret != 0);
			}
			{
				SYNC_SCOPED_MUTEX_LOCK(&shared_state_mutex);
				input::shared_state.process_mouselook_change(mouselook_cursor);
			}
		}
	}
	
	#include "engine.h"
	
namespace platform {
	DECL void msg_thread_close_application () {
		{
			SYNC_SCOPED_MUTEX_LOCK(&input::shared_state_mutex);
			input::do_close_application = true;
		}
		WaitForSingleObject(h_engine_thread, INFINITE);
	}
	
	DECLA LRESULT CALLBACK dummy_windowProc (HWND hWnd, WIN_H_UINT message, WPARAM wParam, LPARAM lParam) {
		return DefWindowProcA(hWnd, message, wParam, lParam);
	}
	
	DECL void setup_renderer_context () {
		
		{
			hDC = GetDC(hWnd);
			assert(hDC != NULL);
		}
		time::startup_event("real window GetDC()");
		
		{
			WNDCLASSA wndClass = {}; // Initialize to zero
			wndClass.style =			CS_OWNDC;
			wndClass.lpfnWndProc =		dummy_windowProc;
			wndClass.hInstance =		hInstance;
			wndClass.lpszClassName =	"space_rocks dummy gl window";
			
			auto wndClassAtom = RegisterClassA(&wndClass);
			assert(wndClassAtom != 0);
			
			HWND dummy_hwnd = CreateWindowExA(
					0, reinterpret_cast<LPCTSTR>(wndClassAtom), "", 0,
					CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
					NULL, NULL, hInstance, NULL);
			assert(dummy_hwnd != NULL);
			
			time::startup_event("dummy window created");
			
			HDC dummy_hdc;
			{
				dummy_hdc= GetDC(dummy_hwnd);
				assert(dummy_hdc != NULL);
			}
			time::startup_event("dummy window GetDC()");
			
			PIXELFORMATDESCRIPTOR pfd = {}; // Initialize to zero
			{
				pfd.nSize =			sizeof pfd;
				pfd.nVersion =		1;
				pfd.dwFlags =		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
				pfd.iPixelType =	PFD_TYPE_RGBA;
				pfd.cColorBits =	32;
				pfd.cDepthBits =	24;
				pfd.cStencilBits =	8;
				pfd.cAuxBuffers =	0;
				pfd.iLayerType =	PFD_MAIN_PLANE;
				
				auto pf = ChoosePixelFormat(dummy_hdc, &pfd);
				assert(pf != 0);
				
				auto ret = SetPixelFormat(dummy_hdc, pf, &pfd);
				assert(ret == TRUE);
			}
			time::startup_event("dummy window ChoosePixelFormat() & SetPixelFormat()");
			
			HGLRC dummy_hglrc;
			{
				dummy_hglrc = wglCreateContext(dummy_hdc);
				assert(dummy_hglrc != NULL);
			}
			time::startup_event("dummy window wglCreateContext()");
			
			{
				auto ret = wglMakeCurrent(dummy_hdc, dummy_hglrc);
				assert(ret == TRUE);
			}
			time::startup_event("dummy window wglMakeCurrent()");
			
			if (0) {
				auto* gl_vers_cstr = reinterpret_cast<char const*>(glGetString(GL_VERSION));
				assert(gl_vers_cstr != nullptr);
				print("GL_VERSION of dummy window: %\n", gl_vers_cstr);
			}
			
			int wgl_pixel_format;
			{ // WGL_ARB_pixel_format; requires WGL_ARB_extensions_string
				auto wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(
						wglGetProcAddress("wglChoosePixelFormatARB") );
				assert(wglChoosePixelFormatARB != nullptr);
				//auto wglGetPixelFormatAttribivARB = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBIVARBPROC>(
				//		wglGetProcAddress("wglGetPixelFormatAttribivARB") );
				//assert(wglGetPixelFormatAttribivARB != nullptr);
				
				static constexpr int attribs[] = {
					WGL_DRAW_TO_WINDOW_ARB,				GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB,				GL_TRUE,
					WGL_ACCELERATION_ARB,				WGL_FULL_ACCELERATION_ARB,
					WGL_DOUBLE_BUFFER_ARB,				GL_TRUE,
					WGL_PIXEL_TYPE_ARB,					WGL_TYPE_RGBA_ARB,
					WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT,	GL_TRUE,
					WGL_RED_BITS_ARB,					8,
					WGL_GREEN_BITS_ARB,					8,
					WGL_BLUE_BITS_ARB,					8,
					WGL_ALPHA_BITS_ARB,					8,
					WGL_DEPTH_BITS_ARB,					24,
					WGL_STENCIL_BITS_ARB,				8,
					0
				};
				
				int			format;
				WIN_H_UINT	formats_count;
				{
					auto ret = wglChoosePixelFormatARB(dummy_hdc, attribs, NULL,
							(WIN_H_UINT)1, &format, &formats_count);
					assert(ret != FALSE);
					assert(format > 0);
				}
				
				#if 0
				static constexpr int query_attribs[] = {
					WGL_DRAW_TO_WINDOW_ARB,
					WGL_SUPPORT_OPENGL_ARB,
					WGL_ACCELERATION_ARB,
					WGL_DOUBLE_BUFFER_ARB,
					WGL_PIXEL_TYPE_ARB,
					WGL_COLOR_BITS_ARB,
					WGL_RED_BITS_ARB,
					WGL_GREEN_BITS_ARB,
					WGL_BLUE_BITS_ARB,
					WGL_ALPHA_BITS_ARB,
					WGL_DEPTH_BITS_ARB,
					WGL_STENCIL_BITS_ARB,
				};
				int values[arrlenof(query_attribs)];
				
				{
					auto wglGetPixelFormatAttribivARB = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBIVARBPROC>(
							wglGetProcAddress("wglGetPixelFormatAttribivARB") );
					assert(wglGetPixelFormatAttribivARB);
					
					auto ret = wglGetPixelFormatAttribivARB(hDC, format, (int)0,
							arrlenof<WIN_H_UINT>(query_attribs), query_attribs, values);
					assert(ret != FALSE);
					
					print("formats_count:  %\n",				formats_count);
					print("Chosen pixel format:	 %\n",		format);
					print("	 WGL_DRAW_TO_WINDOW_ARB:  %\n", values[ 0]);
					print("	 WGL_SUPPORT_OPENGL_ARB:  %\n", values[ 1]);
					print("	 WGL_ACCELERATION_ARB: ");
					switch (values[2]) {
						case WGL_NO_ACCELERATION_ARB:		print("WGL_NO_ACCELERATION_ARB\n");		break;
						case WGL_GENERIC_ACCELERATION_ARB:	print("WGL_GENERIC_ACCELERATION_ARB\n");	break;
						case WGL_FULL_ACCELERATION_ARB:		print("WGL_FULL_ACCELERATION_ARB\n");	break;
						default:							print("%\n", values[2]);					break;
					}
					print("	 WGL_DOUBLE_BUFFER_ARB: %\n",	values[ 3]);
					print("	 WGL_PIXEL_TYPE_ARB: ");
					switch (values[4]) {
						case WGL_TYPE_RGBA_ARB:				print("WGL_TYPE_RGBA_ARB\n");			break;
						case WGL_TYPE_COLORINDEX_ARB:		print("WGL_TYPE_COLORINDEX_ARB\n");		break;
						default:							print("%\n", values[4]);					break;
					}
					print("	 WGL_COLOR_BITS_ARB: %\n",		values[ 5]);
					print("	 WGL_RED_BITS_ARB: %\n",			values[ 6]);
					print("	 WGL_GREEN_BITS_ARB: %\n",		values[ 7]);
					print("	 WGL_BLUE_BITS_ARB: %\n",		values[ 8]);
					print("	 WGL_ALPHA_BITS_ARB: %\n",		values[ 9]);
					print("	 WGL_DEPTH_BITS_ARB: %\n",		values[10]);
					print("	 WGL_STENCIL_BITS_ARB: %\n",		values[11]);
				}
				#endif
				
				wgl_pixel_format = format;
			}
			time::startup_event("dummy window wglChoosePixelFormatARB()");
			
			{
				auto ret = SetPixelFormat(hDC, wgl_pixel_format, &pfd);
				assert(ret == TRUE);
			}
			time::startup_event("real window SetPixelFormat()");
			
			{
				int attribs[] = {
					WGL_CONTEXT_MAJOR_VERSION_ARB,		3,
					WGL_CONTEXT_MINOR_VERSION_ARB,		3,
					WGL_CONTEXT_PROFILE_MASK_ARB,		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
					WGL_CONTEXT_LAYER_PLANE_ARB,		0,
					WGL_CONTEXT_FLAGS_ARB,				WGL_CONTEXT_DEBUG_BIT_ARB,
					0
				};
				
				auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
							wglGetProcAddress("wglCreateContextAttribsARB") );
				assert(wglCreateContextAttribsARB != nullptr);
				
				hGLContext = wglCreateContextAttribsARB(hDC, NULL, attribs);
				if (hGLContext == NULL) {
					warning("wglCreateContextAttribsARB failed");
				}
			}
			time::startup_event("real window wglCreateContextAttribsARB()");
			
			if (hGLContext != NULL) {
				{
					auto ret = wglMakeCurrent(hDC, NULL);
					assert(ret == TRUE);
				}
				{
					auto ret = wglDeleteContext(dummy_hglrc);
					assert(ret == TRUE);
				}
				{
					auto ret = ReleaseDC(dummy_hwnd, dummy_hdc);
					assert(ret == TRUE);
				}
				#if 0 // oversight, right?
				{
					auto ret = ReleaseDC(dummy_hwnd, dummy_hdc);
					assert(ret == 1);
				}
				#endif
				{
					auto ret = DestroyWindow(dummy_hwnd);
					assert(ret != 0);
				}
				{
					auto ret = UnregisterClass(reinterpret_cast<LPCTSTR>(wndClassAtom), hInstance);
					assert(ret != 0);
				}
				time::startup_event("dummy window destroyed");
				
				{
					auto ret = wglMakeCurrent(hDC, hGLContext);
					assert(ret == TRUE);
				}
				time::startup_event("real window wglMakeCurrent()");
				
			} else {
				// experimental fallback
				hDC = dummy_hdc;
				hGLContext = dummy_hglrc;
			}
		}
		
		if (0) {
			auto* gl_vers_cstr = reinterpret_cast<char const*>(glGetString(GL_VERSION));
			assert(gl_vers_cstr != nullptr);
			print("GL_VERSION of real window: %\n", gl_vers_cstr);
		}
		
		//// Loading OpenGL functions
		{
			// We would do extension checking here
			gl_tbl.load_all();
		}
		time::startup_event("OGL functions loaded");
		
		{
			glDebugMessageCallbackARB(reinterpret_cast<GLDEBUGPROCARB>(gl_debug_proc), NULL);
		}
	}
	
	//// Engine
	DECLA DWORD WINAPI engine_main (LPVOID lpParam) {
		
		time::startup_event("engine thread start");
		
		setup_renderer_context();
		
		engine::init();
		
		time::inital_frame();
		
		for (engine::frame_number=0;; ++engine::frame_number) {
			
			{
				SYNC_SCOPED_MUTEX_LOCK(&input::shared_state_mutex);
				if (input::do_close_application) break;
			}
			
			bool minimized = IsIconic(hWnd) != 0;
			if (minimized) {
				
				print("Game minimized, engine sleeping.\n");
				
				{
					PROFILE_SCOPED(THR_ENGINE, minimized_sleep);
					do {
						Sleep(16);
						minimized = IsIconic(hWnd) != 0;
					} while (minimized);
				}
				
				time::inital_frame_after_sleep();
				
				print("Game un-minimized, engine waking up.\n");
			}
			
			engine::dt =		(f32)( (f64)time::get_dt() * time::QPC::inv_freq );
			
			engine::frame();
			
			time::cpu_work_done();
			
			{
				PROFILE_SCOPED(THR_ENGINE, SwapBuffers);
				SwapBuffers(hDC);
			}
			
			time::frame_end(engine::frame_number);
			
		}
		
		//time::application_close_timing();
		
		return 0;
	}
	
	//// msg/input thread
	
	DECL void print_rect (RECT rect) {
		assert(rect.left <= rect.right);
		assert(rect.top <= rect.bottom);
		int w = rect.right -rect.left;
		int h = rect.bottom -rect.top;
		
		print("{%, %, %, % (%, %)}", rect.left, rect.top, rect.right, rect.bottom, w, h);
	}
	DECL void print_point (POINT rect) {
		
		print("{%, %}", rect.x, rect.y);
	}
	
	DECLD constexpr WIN_H_LONG WINDOWED_STYLE =					WS_VISIBLE|WS_OVERLAPPEDWINDOW;
	DECLD constexpr WIN_H_LONG WINDOWED_EX_STYLE =				WS_EX_APPWINDOW|WS_EX_WINDOWEDGE;
	
	DECLD constexpr WIN_H_LONG BORDERLESS_FULLSCREEN_STYLE =		WS_VISIBLE;
	DECLD constexpr WIN_H_LONG BORDERLESS_FULLSCREEN_EX_STYLE = WS_EX_APPWINDOW;
	
	DECLD constexpr WIN_H_LONG MINIMUM_WINDOW_CLIENT_W = 16 * 8;
	DECLD constexpr WIN_H_LONG MINIMUM_WINDOW_CLIENT_H = 9 * 8;
	
	DECLD HCURSOR			cursor_handle;
	
#if !DBG_NOTE
	//resolution_v		client_size =			resolution_v(1920, 1080);
	DECLD resolution_v		client_size =			resolution_v(1680, 1050);
#else
	DECLD resolution_v		client_size =			resolution_v(4 * MINIMUM_WINDOW_CLIENT_W, 4 * MINIMUM_WINDOW_CLIENT_H);
#endif
	
	DECLD bool				fullscreen =			false;
	DECLD RECT				fullscreenWndRect;
	
	DECLD RECT				windowBorderSizes;
	
	DECLD WINDOWPLACEMENT*	window_placement;
	
	DECL bool init_window_placement_save () {
		DEFER_POP(&working_stk);
		
		cstr filename;
		{
			cstr pc_name;
			char buf[MAX_COMPUTERNAME_LENGTH +1];
			{
				DWORD len = arrlenof<DWORD>(buf);
				auto ret = GetComputerNameA(buf, &len);
				if (ret == 0) {
					DBGBREAK_IF_DEBUGGER_PRESENT;
					pc_name = "<GetComputerNameA failed>";
				} else {
					pc_name = buf;
				}
				
			}
			
			filename = print_working_stk(WINDOW_PLACEMENT_SAVE_FILENAME, pc_name).str;
		}
		
		HANDLE		file_h;
		
		bool		valid = true;
		char const* invalid_str;
		{
			file_h = CreateFile(filename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
					NULL, OPEN_ALWAYS, 0, NULL);
			auto err = GetLastError();
			
			valid = file_h != INVALID_HANDLE_VALUE;
			if (!valid) {
				warning("CreateFile() for % failed (err: %), window placement won't be loaded or saved!",
						filename, err);
			} else {
				
				valid = err == ERROR_ALREADY_EXISTS;
				if (!valid) {
					invalid_str = "does not exist";
				} else {
					
					LARGE_INTEGER size;
					auto ret = GetFileSizeEx(file_h, &size);
					assert(ret != 0, "GetFileSizeEx failed for '%'", filename);
					
					valid = size.QuadPart == sizeof(WINDOWPLACEMENT);
					if (!valid) {
						invalid_str = "exists but is not valid";
					}
				}
			}
		}
		if (file_h != INVALID_HANDLE_VALUE) {
			if (!valid) {
				win32::set_filepointer_and_eof(file_h, sizeof(WINDOWPLACEMENT));
			}
			
			HANDLE map_h = CreateFileMapping(file_h, NULL, PAGE_READWRITE, 0,(DWORD)sizeof(WINDOWPLACEMENT), NULL);
			
			window_placement = (WINDOWPLACEMENT*)MapViewOfFile(map_h, FILE_MAP_WRITE, 0,0, (DWORD)sizeof(WINDOWPLACEMENT));
			assert(window_placement);
		} else {
			window_placement = (WINDOWPLACEMENT*)malloc(sizeof(WINDOWPLACEMENT));
		}
		
		if (valid) {
			valid = window_placement->length == sizeof(WINDOWPLACEMENT);
			if (!valid) {
				invalid_str = "exists but is not valid";
			}
			
			window_placement->showCmd = window_placement->showCmd == SW_MAXIMIZE ? SW_MAXIMIZE : SW_RESTORE;
		}
		if (!valid) {
			window_placement->length = sizeof(WINDOWPLACEMENT);
		}
		
		if (!valid && file_h != INVALID_HANDLE_VALUE) {
			print("% %, using default window placement.\n", filename, invalid_str);
		}
		return valid;
	}
	DECL void windowed_placement_changed () {
		auto ret = GetWindowPlacement(hWnd, window_placement);
		auto err = GetLastError();
		assert(ret != 0);
	}
	
	DECL void toggleFullscreen (HWND hWnd) {
		// SetForegroundWindow ????
		fullscreen = !fullscreen;
		if (fullscreen) {
			// Switch to fullscreen
			
			auto monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			{
				MONITORINFO mi;
				mi.cbSize = sizeof mi;
				auto ret = GetMonitorInfoA(monitor, &mi);
				assert(ret != 0);
				fullscreenWndRect = mi.rcMonitor;
			}
			{
				SetLastError(0);
				auto prevStyle = SetWindowLongA(hWnd, GWL_STYLE, BORDERLESS_FULLSCREEN_STYLE);
				assert(GetLastError() == 0);
			}
			{
				SetLastError(0);
				auto prevStyle = SetWindowLongA(hWnd, GWL_EXSTYLE, BORDERLESS_FULLSCREEN_EX_STYLE);
				assert(GetLastError() == 0);
			}
			{
				int x = safe_cast_assert(int, fullscreenWndRect.left);
				int y = safe_cast_assert(int, fullscreenWndRect.top);
				int w = safe_cast_assert(int, fullscreenWndRect.right -fullscreenWndRect.left);
				int h = safe_cast_assert(int, fullscreenWndRect.bottom -fullscreenWndRect.top);
				assert(w >= 0);
				assert(h >= 0);
				
				auto ret = SetWindowPos(hWnd, HWND_TOP, x, y, w, h, SWP_FRAMECHANGED|
						SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOREDRAW);
				assert(ret != 0);
			}
			
			// Don't do this for now, it does not seem to do anything for us, but it interferes with our window rect saving
			//ShowWindow(hWnd, SW_SHOWMAXIMIZED); // This does not prevent us from dragging the maximized window when we were dragging the window when we pressed F11
		} else {
			// Switch to windowed
			{
				SetLastError(0);
				auto prevStyle = SetWindowLongA(hWnd, GWL_STYLE, WINDOWED_STYLE);
				assert(GetLastError() == 0);
			}
			{
				SetLastError(0);
				auto prevStyle = SetWindowLongA(hWnd, GWL_EXSTYLE, WINDOWED_EX_STYLE);
				assert(GetLastError() == 0);
			}
			{
				auto ret = SetWindowPlacement(hWnd, window_placement);
				assert(ret != 0);
			}
			{
				auto ret = SetWindowPos(hWnd, 0, 0, 0, 0, 0,
						SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
				assert(ret != 0);
			}
			
			//ShowWindow(hWnd, SW_RESTORE);
		}
	}
	
	DECL RECT clientRectFromWindowRect (RECT windowScreenRect) {
		RECT ret;
		ret.left =		windowScreenRect.left	+windowBorderSizes.left;
		ret.top =		windowScreenRect.top	+windowBorderSizes.top;
		ret.right =		windowScreenRect.right	-windowBorderSizes.right;
		ret.bottom =	windowScreenRect.bottom -windowBorderSizes.bottom;
		return ret;
	}
	
	namespace disable_wnd_anim {
		typedef enum _DWMWINDOWATTRIBUTE {
			DWMWA_NCRENDERING_ENABLED=1,
			DWMWA_NCRENDERING_POLICY,
			DWMWA_TRANSITIONS_FORCEDISABLED,
			DWMWA_ALLOW_NCPAINT,
			DWMWA_CAPTION_BUTTON_BOUNDS,
			DWMWA_NONCLIENT_RTL_LAYOUT,
			DWMWA_FORCE_ICONIC_REPRESENTATION,
			DWMWA_FLIP3D_POLICY,
			DWMWA_EXTENDED_FRAME_BOUNDS,
			DWMWA_HAS_ICONIC_BITMAP,
			DWMWA_DISALLOW_PEEK,
			DWMWA_EXCLUDED_FROM_PEEK,
			DWMWA_CLOAK,
			DWMWA_CLOAKED,
			DWMWA_FREEZE_REPRESENTATION,
			DWMWA_LAST
		} DWMWINDOWATTRIBUTE;
		
		typedef HRESULT (WINAPI* DwmSetWindowAttribute_fp) (HWND, DWORD, LPCVOID, DWORD);
		
		DECLD HMODULE						dwmapi_module;
		DECLD DwmSetWindowAttribute_fp	DwmSetWindowAttribute;
		
	}
	
	DECLA LRESULT CALLBACK windowProc (HWND hWnd, WIN_H_UINT message, WPARAM wParam, LPARAM lParam) {
		if (0) {
			cstr str;
			switch (message) {
				case WM_CLOSE					:	str = "WM_CLOSE";					break;
				case WM_SIZE					:	str = "WM_SIZE";					break;
				case WM_SIZING					:	str = "WM_SIZING";					break;
				case WM_MOVING					:	str = "WM_MOVING";					break;
				case WM_MOVE					:	str = "WM_MOVE";					break;
				case WM_MOUSEMOVE				:	str = "WM_MOUSEMOVE";				break;
				case WM_SETCURSOR				:	str = "WM_SETCURSOR";				break;
				case WM_INPUT_DEVICE_CHANGE		:	str = "WM_INPUT_DEVICE_CHANGE";		break;
				case WM_ACTIVATE				:	str = "WM_ACTIVATE";				break;
				case WM_ACTIVATEAPP				:	str = "WM_ACTIVATEAPP";				break;
				case WM_MOUSEACTIVATE			:	str = "WM_MOUSEACTIVATE";			break;
				case WM_INPUT					:	str = "WM_INPUT";					break;
				default							:	str = "DefWindowProcA";				break;
			}
			print(">> windowProc: %\n", str);
		}
		
		switch (message) {
			case WM_CLOSE: {
				msg_thread_close_application();
				PostQuitMessage(0);
				return 0;
			}
			
			case WM_SIZE: {
				auto w = LOWORD(lParam);
				auto h = HIWORD(lParam);
				
				#if 0
				char const* wParamStr;
				switch (wParam) {
					case SIZE_MAXHIDE:		wParamStr = "SIZE_MAXHIDE";		break;
					case SIZE_MAXIMIZED:	wParamStr = "SIZE_MAXIMIZED";	break;
					case SIZE_MAXSHOW:		wParamStr = "SIZE_MAXSHOW";		break;
					case SIZE_MINIMIZED:	wParamStr = "SIZE_MINIMIZED";	break;
					case SIZE_RESTORED:		wParamStr = "SIZE_RESTORED";		break;
					default: assert(false);
				}
				print("WM_SIZE: % % %\n" wParamStr, w, h);
				#endif
				
				RECT rect;
				{
					auto ret = GetClientRect(hWnd, &rect);
					assert(ret != 0, "%", ret);
				}
				
				if (wParam != SIZE_MINIMIZED) {
					if (w <= 0 || h <= 0) {
						warning("windowProc::WM_SIZE: w <= 0 || h <= 0");
						return 0;
					}
				}
				
				assert((rect.right -rect.left) == w, "% %", (rect.right -rect.left), w);
				assert((rect.bottom -rect.top) == h, "% %", (rect.bottom -rect.top), h);
				
				auto newClientSize = safe_cast_assert_v2<resolution_v>(w, h);
				client_size = newClientSize;
				
				if (!fullscreen) {
					windowed_placement_changed();
				}
				
				return 0;
			}
			case WM_SIZING: {
				auto rectPtr = reinterpret_cast<RECT*>(lParam);
				auto rect = *rectPtr;
				
				#if 0
				print("WM_SIZING: %x% at % %\n",
						safe_cast_assert(ui, rect.right -rect.left),
						safe_cast_assert(ui, rect.bottom -rect.top),
						rect.left, rect.top);
				#endif
				
				if (fullscreen) {
					// Override window rect if the user was resizing the window while switching to fullscreen
					rect = fullscreenWndRect;
				} else {
					// Enforce a minimum window size
					auto borderX = windowBorderSizes.left +windowBorderSizes.right;
					auto borderY = windowBorderSizes.top +windowBorderSizes.bottom;
					
					auto w = MAX<WIN_H_LONG>(rect.right -rect.left, (16 * 8) +borderX);
					auto h = MAX<WIN_H_LONG>(rect.bottom -rect.top, (9 * 8) +borderY);
					
					switch (wParam) {
						case WMSZ_BOTTOM:
						case WMSZ_BOTTOMRIGHT:
						case WMSZ_RIGHT:
						case WMSZ_TOPRIGHT:
						{
							rect.right = rect.left +w;
						} break;
						case WMSZ_TOP:
						case WMSZ_TOPLEFT:
						case WMSZ_LEFT:
						case WMSZ_BOTTOMLEFT:
						{
							rect.left = rect.right -w;
						} break;
						case 9: { // This happens when dragging a window away from the border of the screen after it was snapped to it
							// Do nothing since the window should not be small enough for our min window size to apply anyway
						} break;
						default: assert(false, "%", wParam); // TODO: should probably use a different assert here
					}
					switch (wParam) {
						case WMSZ_BOTTOMLEFT:
						case WMSZ_BOTTOM:
						case WMSZ_BOTTOMRIGHT:
						case WMSZ_RIGHT:
						{
							rect.bottom = rect.top +h;
						} break;
						case WMSZ_TOPRIGHT:
						case WMSZ_TOP:
						case WMSZ_TOPLEFT:
						case WMSZ_LEFT:
						{
							rect.top = rect.bottom -h;
						} break;
						case 9: {
							
						} break;
						default: assert(false, "%", wParam);
					}
					input::calc_mouse_confinement_point(rect);
				}
				
				*rectPtr = rect;
				return TRUE;
			}
			case WM_MOVING: {
				auto rectPtr = reinterpret_cast<RECT*>(lParam);
				
				if (fullscreen) {
					// Override window rect if the user was moving the window while switching to fullscreen
					*rectPtr = fullscreenWndRect;
				} else {
					auto rect = *rectPtr;
					
					input::calc_mouse_confinement_point(rect);
				}
				return TRUE;
			}
			case WM_MOVE: {
				
				if (!fullscreen) {
					windowed_placement_changed();
				}
				return 0;
			}
			
			case WM_MOUSEMOVE: {
				using namespace input;
				
				auto x = static_cast<short>( lParam >> (0 * (sizeof(short) * 8)) );
				auto y = static_cast<short>( lParam >> (1 * (sizeof(short) * 8)) );
				
				// It seems like we consistently get WM_MOUSEMOVE messages with mouse coords
				//	on the window border in the + dir (outside of our client area) when using window styles other than WS_OVERLAPPEDWINDOW
				if (x < 0 || (decltype(client_size.x))x > client_size.x) {
					warning("!? % %\n", x, client_size.x);
				}
				if (y < 0 || (decltype(client_size.y))y > client_size.y) {
					warning("!? % %\n", y, client_size.y);
				}
				auto pos = safe_cast_assert_v2<resolution_v>(x, y);
				
				//print("WM_MOUSEMOVE ", "v2{", pos.x(), ", ", pos.y(), "}", nl);
				
				if (mouselook_cursor) {
					resolution_v screenPos;
					{
						POINT point;
						{
							auto temp = safe_cast_assert_v2<tv2<int>>(x, y);
							point.x = temp.x;
							point.y = temp.y;
						}
						{
							auto ret = ClientToScreen(hWnd, &point);
							assert(ret != 0);
						}
						screenPos = safe_cast_assert_v2<resolution_v>(point.x, point.y);
					}
					if (!identical(screenPos, mouse_confinement_point)) {
						// TODO: We could also additionally react to WM_MOUSELEAVE to reset the mouse to the mouseConfinementPoint
						// -this should probably not ever be necessary,
						auto temp = safe_cast_assert_v2<tv2<int>>(mouse_confinement_point);
						auto ret = SetCursorPos(temp.x, temp.y);
						assert(ret != 0); // this can happen when ctrl-alt-del'ing ???
					}
				} else {
					gui_mouse_pos = pos;
				}
				return 0;
			}
			case WM_SETCURSOR: {
				using namespace input;
				
				if (LOWORD(lParam) == HTCLIENT) {
					if (mouselook_cursor) {
						SetCursor(NULL);
					} else {
						SetCursor(cursor_handle);
					}
					return TRUE;
				} else {
					return DefWindowProcA(hWnd, message, wParam, lParam);
				}
			}
			
			case WM_INPUT_DEVICE_CHANGE: {
				//print("windowProc message: WM_INPUT_DEVICE_CHANGE", nl);
				return 0;
			}
			
			case WM_ACTIVATE: {
				bool wasMinimized = HIWORD(wParam) != 0;
				#if 0
				char const* wParamStr;
				switch (LOWORD(wParam)) {
					case WA_INACTIVE:		wParamStr = "WA_INACTIVE";		break;
					case WA_ACTIVE:			wParamStr = "WA_ACTIVE";		break;
					case WA_CLICKACTIVE:	wParamStr = "WA_CLICKACTIVE";	break;
					default: assert(false);
				}
				print("WM_ACTIVATE: % wasMinimized: %\n", wParamStr, wasMinimized);
				#endif
				
				switch (LOWORD(wParam)) {
					case WA_INACTIVE: {
						//print("keyboard focus lost", nl);
						input::keyboard_focus_lost();
					} break;
					case WA_ACTIVE:
					case WA_CLICKACTIVE: {
						//print("keyboard focus gained", nl);
					} break;
					default: assert(false);
				}
				return 0;
			}
			case WM_ACTIVATEAPP: {
				#if 0
				print("WM_ACTIVATEAPP: %\n", wParam == TRUE ? "activated" : "deactivated");
				#endif
				return 0;
			}
			
			case WM_MOUSEACTIVATE: {
				auto mouseMsg = HIWORD(lParam);
				#if 0
				cCntStr mouseMsgStr;
				switch (mouseMsg) {
					case WM_LBUTTONDOWN:	mouseMsgStr = litCntStr("WM_LBUTTONDOWN");		break;
					case WM_MBUTTONDOWN:	mouseMsgStr = litCntStr("WM_MBUTTONDOWN");		break;
					case WM_RBUTTONDOWN:	mouseMsgStr = litCntStr("WM_RBUTTONDOWN");		break;
					default:				mouseMsgStr = litCntStr("other");				break;
				}
				print("WM_MOUSEACTIVATE: % (%)\n", mouseMsgStr, mouseMsg);
				#endif
				
				// When activating the window via mouse click we do not get a button down message via Raw Input (we do get a button up message)
				// normally you would return MA_ACTIVATE from WM_MOUSEACTIVATE so that you get the WM_xBUTTONDOWN messages,
				//	but we simply handle the button down directly in here and return MA_ACTIVATEANDEAT
				{
					using namespace input;
					buttons::button_e button;
					switch (mouseMsg) {
						using namespace buttons;
						case WM_LBUTTONDOWN:	button = LMB;	break;
						case WM_MBUTTONDOWN:	button = MMB;	break;
						case WM_RBUTTONDOWN:	button = RMB;	break;
						case WM_XBUTTONDOWN:	return MA_ACTIVATE; // TODO: what do in this case?, seems to work fine, activating the window with MB4 or MB5 works, but the click is eaten, which is probably OK
						default: assert(false, "%", mouseMsg);
							button = (buttons::button_e)0; // get rid of warning
					}
					bool newState = true;
					
					auto prevState = gameButtonState.get(button);
					if (prevState != newState) {
						gameButtonState.toggle(button);
						
						{
							SYNC_SCOPED_MUTEX_LOCK(&shared_state_mutex);
							input::shared_state.process_button_binding_input(button, newState);
						}
					}
				}
				return MA_ACTIVATE;
			}
			
			case WM_INPUT: {
				RAWINPUT rawInput;
				{
					auto pcbSize = safe_cast_assert(WIN_H_UINT, sizeof rawInput);
					auto ret = GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
							&rawInput, &pcbSize, sizeof(RAWINPUTHEADER));
					assert(ret <= sizeof rawInput);
				}
				
				using namespace input;
				
				switch (rawInput.header.dwType) {
					case RIM_TYPEMOUSE: {
						assert(rawInput.header.dwSize == (sizeof(RAWINPUTHEADER) +sizeof(RAWMOUSE)));
						auto m = rawInput.data.mouse;
						if (m.usFlags & MOUSE_ATTRIBUTES_CHANGED) {
							print("MOUSE_ATTRIBUTES_CHANGED\n");
						}
						auto flags =		m.usFlags;
						auto buttonFlags =	m.usButtonFlags;
						auto buttonData =	m.usButtonData;
						auto rawButtons =	m.ulRawButtons;
						WIN_H_LONG x =		m.lLastX;
						WIN_H_LONG y =		m.lLastY;
						
						#if !DBG_VM_DEBUG
						//assert((flags & MOUSE_VIRTUAL_DESKTOP) == 0); // This is set when running in VM
						// Can happen when using windows magnify application switched from full screen to non-fullscreen
						// maybe happened once while i was moving the window to the second monitor
						if (flags & MOUSE_VIRTUAL_DESKTOP) {
							warning("RawInput: MOUSE_VIRTUAL_DESKTOP set! x: % y: %", x, y);
						}
						#endif
						
						using namespace buttons;
						
						auto v = safe_cast_assert_v2<resolution_v>(x, y);
						bool doPushMouselook = !identical(v, resolution_v(0));
						
						#if !DBG_VM_DEBUG
						if (flags & MOUSE_MOVE_ABSOLUTE) {
							warning("RawInput: Absolute mouse input! x: % y: %", x, y);
							doPushMouselook = false;
						}
						#endif
						{
							SYNC_SCOPED_MUTEX_LOCK(&shared_state_mutex);
							
							if (doPushMouselook) {
								input::shared_state.process_mouse_rel(v);
							}
							
							STATIC_ASSERT(RI_MOUSE_LEFT_BUTTON_DOWN ==		RI_MOUSE_BUTTON_1_DOWN);
							STATIC_ASSERT(RI_MOUSE_LEFT_BUTTON_UP ==		RI_MOUSE_BUTTON_1_UP);
							STATIC_ASSERT(RI_MOUSE_RIGHT_BUTTON_DOWN ==		RI_MOUSE_BUTTON_2_DOWN);
							STATIC_ASSERT(RI_MOUSE_RIGHT_BUTTON_UP ==		RI_MOUSE_BUTTON_2_UP);
							STATIC_ASSERT(RI_MOUSE_MIDDLE_BUTTON_DOWN ==	RI_MOUSE_BUTTON_3_DOWN);
							STATIC_ASSERT(RI_MOUSE_MIDDLE_BUTTON_UP ==		RI_MOUSE_BUTTON_3_UP);
							static constexpr WIN_H_USHORT buttonFlagsArr[_NUM_MOUSE_BUTTON * 2] = {
								RI_MOUSE_LEFT_BUTTON_DOWN,
								RI_MOUSE_LEFT_BUTTON_UP,
								
								RI_MOUSE_MIDDLE_BUTTON_DOWN,
								RI_MOUSE_MIDDLE_BUTTON_UP,
								
								RI_MOUSE_RIGHT_BUTTON_DOWN,
								RI_MOUSE_RIGHT_BUTTON_UP,
								
								RI_MOUSE_BUTTON_4_DOWN,
								RI_MOUSE_BUTTON_4_UP,
								
								RI_MOUSE_BUTTON_5_DOWN,
								RI_MOUSE_BUTTON_5_UP,
							};
							for (button_e button=_FIRST_MOUSE_BUTTON; button<_MAX_MOUSE_BUTTON; ++button) {
								auto indxDown =	((button -_FIRST_MOUSE_BUTTON) * 2) +0;
								auto indxUp =	((button -_FIRST_MOUSE_BUTTON) * 2) +1;
								bool down =		(buttonFlags & buttonFlagsArr[indxDown]) != 0;
								bool up =		(buttonFlags & buttonFlagsArr[indxUp]) != 0;
								
								if (down && up) {
									assert(false);
								} else if (down || up) {
									bool newState;
									if (down) {
										newState = true;
									} else {
										newState = false;
									}
									#if 0
									print("WM_INPUT mouse % %\n", buttonCStrs[button], newState ? "down" : "up");
									#endif
									
									auto prevState = gameButtonState.get(button);
									if (prevState != newState) {
										
										gameButtonState.toggle(button);
										input::shared_state.process_button_binding_input(button, newState);
									}
								}
							}
							
							if (buttonFlags & RI_MOUSE_WHEEL) {
								
								auto temp = static_cast<SHORT>(buttonData);
								auto tempDiv = temp / WHEEL_DELTA;
								
								assert((temp % WHEEL_DELTA) == 0);
								
								input::shared_state.process_mouse_wheel_rel_input(safe_cast_assert(s32, tempDiv));
							}
						}
						return 0;
					}
					case RIM_TYPEKEYBOARD: {
						assert(rawInput.header.dwSize == (sizeof(RAWINPUTHEADER) +sizeof(RAWKEYBOARD)));
						auto kb = rawInput.data.keyboard;
						
						auto flags =	kb.Flags;
						auto message =	kb.Message;
						auto vkey =		kb.VKey;
						
						bool state = // TODO: Is this reliable? see the failed assert on mismatched	 state	and	 message
								(flags & 1) == 0 ?
								true : // RI_KEY_MAKE
								false; // RI_KEY_BREAK
						
						// Make sure kb.Message matches kb.Flags
						switch (message) {
							case WM_SYSKEYDOWN:
							case WM_KEYDOWN: {
								//assert(state == true); // TODO: This failed while moving camera & pressing multiple keys at the same time
								assert(state == true, "Mismatched raw input key message (%) and flags (%)", message, flags);
							} break;
							case WM_SYSKEYUP:
							case WM_KEYUP: {
								//assert(state == false); // TODO: This failed while moving camera & pressing multiple keys at the same time
								assert(state == false, "Mismatched raw input key message (%) and flags (%)", message, flags);
							} break;
							default: assert(false);
						}
						
						bool isExtendedKey;
						{
							bool e0 = (flags & RI_KEY_E0) != 0;
							bool e1 = (flags & RI_KEY_E1) != 0;
							
							// Docs say e0 -> left version of the key, e1 -> right version of the key
							//	but this seems to be wrong
							
							isExtendedKey = e0;
						}
						
						bool changed;
						bool down =		state == true;
						bool up =		state == false;
						
						switch (vkey) {
							
							case VK_SHIFT: {
								if (!isExtendedKey) {
									// left SHIFT
								} else {
									// right SHIFT
								}
								changed = shiftState != state;
								shiftState = state;
								// DO pass SHIFT to game input
							} break;
							
							case VK_MENU: {
								if (!isExtendedKey) {
									// left ALT
								} else {
									// right ALT
								}
								changed = altState != state;
								altState = state;
								return 0; // Don't pass ALT to game input
							} break;
							
							case VK_F1: {
								changed = f1State != state;
								f1State = state;
								if (changed && down) {
									input::toggle_mouselook_cursor();
								}
								return 0; // Don't pass F1 to game input
							} break;
							
							case VK_F4: {
								changed = f4State != state;
								f4State = state;
								if (altState && changed && down) {
									msg_thread_close_application();
								}
								return 0; // Don't pass F4 to game input
							} break;
							
							case VK_F11: {
								changed = f11State != state;
								f11State = state;
								if (changed && down) {
									toggleFullscreen(hWnd);
								}
								return 0; // Don't pass F11 to game input
							} break;
							
							default: {
								
							}
						}
						
						buttons::button_e button;
						auto isKnownKey = mapVirtualKeyToGameKey(vkey, isExtendedKey, &button);
						
						if (isKnownKey) {
							bool prevState = gameButtonState.get(button +0);
							
							if (state != prevState) {
								
								gameButtonState.toggle(button +0);
								{
									SYNC_SCOPED_MUTEX_LOCK(&shared_state_mutex);
									input::shared_state.process_button_binding_input(button, state);
								}
							}
						}
						return 0;
					}
					default: assert(false); return 0;
				}
			}
			
			default: {
				return DefWindowProcA(hWnd, message, wParam, lParam);
			}
		}
	}
	
	DECL void do_setup_work () {
		
		input::shared_state_mutex.init();
		
		{
			hInstance = GetModuleHandleA(NULL);
			assert(hInstance != NULL);
			
			auto hIcon = LoadIconA(NULL, IDI_WINLOGO);
			assert(hIcon != NULL);
			
			auto hCursor = LoadCursorA(NULL, IDC_ARROW);
			assert(hCursor != NULL);
			
			WNDCLASSA wndClass = {}; // Initialize to zero
			wndClass.style =			CS_OWNDC;
			wndClass.lpfnWndProc =		windowProc;
			wndClass.hInstance =		hInstance;
			wndClass.hIcon =			hIcon;
			wndClass.hCursor =			hCursor;
			wndClass.lpszClassName =	"space_rocks gl window";
			
			cursor_handle = hCursor;
			
			auto wndClassAtom = RegisterClassA(&wndClass);
			assert(wndClassAtom != 0);
			
			DWORD windowStyle = static_cast<DWORD>(WINDOWED_STYLE);
			DWORD windowExStyle = static_cast<DWORD>(WINDOWED_EX_STYLE);
			
			struct Int_Rect {
				int x, y, w, h;
			} rect;
			
			{ // Get window border sizes
				windowBorderSizes = { 0, 0, 0, 0 };
				auto ret = AdjustWindowRectEx(&windowBorderSizes, windowStyle, FALSE, windowExStyle);
				assert(ret != 0);
				
				assert(windowBorderSizes.left <= 0);
				assert(windowBorderSizes.top <= 0);
				assert(windowBorderSizes.right >= 0);
				assert(windowBorderSizes.bottom >= 0);
				
				windowBorderSizes.left = -windowBorderSizes.left;
				windowBorderSizes.top = -windowBorderSizes.top;
				
				rect.x =	CW_USEDEFAULT;
				rect.y =	CW_USEDEFAULT;
				rect.w =	 safe_cast_assert(int, windowBorderSizes.left)
							+safe_cast_assert(int, windowBorderSizes.right)
							+safe_cast_assert(int, client_size.x);
				rect.h =	 safe_cast_assert(int, windowBorderSizes.top)
							+safe_cast_assert(int, windowBorderSizes.bottom)
							+safe_cast_assert(int, client_size.y);
			}
			
			bool save_loaded = init_window_placement_save();
			
			hWnd = CreateWindowExA(
					windowExStyle, reinterpret_cast<LPCTSTR>(wndClassAtom), "OpenGL Tutorial",
					windowStyle & ~WS_VISIBLE,
					rect.x, rect.y, rect.w, rect.h,
					NULL, NULL, hInstance, NULL);
			assert(hWnd != NULL);
			
			if (save_loaded) {
				auto ret = SetWindowPlacement(hWnd, window_placement);
				assert(ret != 0);
			}
			
			{
				ShowWindow(hWnd, SW_SHOW);
			}
		}
		
		{
			using namespace disable_wnd_anim;
			
			dwmapi_module = LoadLibraryA("Dwmapi.dll");
			assert(dwmapi_module != NULL);
			
			DwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_fp>(
					GetProcAddress(dwmapi_module, "DwmSetWindowAttribute") );
			
			BOOL val = TRUE;
			auto ret = DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &val, sizeof val);
			assert(ret == S_OK);
		}
		
		{
			RECT rect;
			{
				auto ret = GetWindowRect(hWnd, &rect);
				assert(ret != 0);
			}
			input::calc_mouse_confinement_point(clientRectFromWindowRect(rect));
			{
				auto ret = GetClientRect(hWnd, &rect);
				assert(ret != 0);
			}
			
			#if 0
			assert(rect.left < rect.right); // hit sometimes, think there is a bug with calc_mouse_confinement_point and fullscreen, not sure if this causes this
			assert(rect.top < rect.bottom);
			#else
			if (rect.left >= rect.right || rect.top >= rect.bottom) {
				rect.right = rect.left +500;
				rect.bottom = rect.top +300;
				warning("calc_mouse_confinement_point out of range! r% l% t% b%", rect.left, rect.right, rect.top, rect.bottom);
			}
			#endif
			
			client_size = safe_cast_assert_v2<decltype(client_size)>(rect.right -rect.left, rect.bottom -rect.top);
		}
		
		{
			RAWINPUTDEVICE	rid[2];
			// Mouse
			rid[0].usUsagePage =	1;
			rid[0].usUsage =		2;
			rid[0].dwFlags =		RIDEV_DEVNOTIFY;
			rid[0].hwndTarget =		hWnd;
			// Keyboard
			rid[1].usUsagePage =	1;
			rid[1].usUsage =		6;
			rid[1].dwFlags =		RIDEV_DEVNOTIFY;
			rid[1].hwndTarget =		hWnd;
			
			// RIDEV_NOLEGACY, RIDEV_NOHOTKEYS ???
			
			auto ret = RegisterRawInputDevices(rid,
					arrlenof<WIN_H_UINT>(rid), safe_cast_assert(WIN_H_UINT, sizeof(RAWINPUTDEVICE)));
			assert(ret != FALSE);
		}
		time::startup_event("real window created");
		
	}
	
	DECL int msg_thread_main () {
		
		{
			if (cmd_line.argc != 2) {
				print("Usage: graph <filename>\n");
				return 1;
			}
			input_filename = cmd_line.argv[1];
		}
		
		time::process_begin();
		
		do_setup_work();
		{
			h_engine_thread = CreateThread(NULL, 0, engine_main, NULL, 0, NULL);
			assert(h_engine_thread != NULL);
		}
		
		MSG msg;
		for (;;) {
			auto ret = GetMessageA(&msg, NULL, 0, 0);
			if (ret == -1) {
				// Error
				assert(false);
			} else if (ret) {
				// Valid message
				//TranslateMessage(&msg); this would be used for translating
				//	keystrokes to char messages for typing input
				DispatchMessageA(&msg);
			} else {
				// WM_QUIT
				break;
			}
		}
		
		return 0;
	}
}
