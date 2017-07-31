
//// Time
namespace time {
	
	namespace QPC {
		DECL u64 get_freq () {
			LARGE_INTEGER ret;
			QueryPerformanceFrequency(&ret);
			return static_cast<u64>(ret.QuadPart);
		//	return mega<u64>(4100);
		}
		
		DECL u64 get_time () {
			LARGE_INTEGER ret;
			QueryPerformanceCounter(&ret);
			return static_cast<u64>(ret.QuadPart);
		//	return __rdtsc();
		}
		
		DECLD f64 freq;
		DECLD f64 inv_freq;
		
		DECL void init () {
			freq =		(f64)get_freq();
			inv_freq =	1.0 / freq;
		}
	}
	
}
