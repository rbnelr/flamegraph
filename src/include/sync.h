
namespace sync {
	// We had strong evidence that 2 threads were held the lock at the same time (sse texture_load_rewrite.txt :  BUG #00!!!)
	//	ratchet_freak also said this:
	//	 <ratchet_freak> there is a race between the decrement/increment and the mutex itself
	//	 <ratchet_freak> A increments and yields before the wait then if B decrement it will release before A is waiting
	#if 0 // So for now use the critical section
	struct Mutex {
	private:
		volatile WIN_H_LONG		contentionCounter;
		HANDLE					semaphore;
		
	public:
		void init (bool startLocked=false) {
			contentionCounter = startLocked ? 1 : 0;
			{
				auto ret = CreateSemaphoreW(NULL, 0, 1, NULL);
				assert(ret != NULL);
				semaphore = ret;
			}
		}
		
		void wait_for_semaphore () {
			auto ret = WaitForSingleObject(semaphore, INFINITE);
			//assert(ret != WAIT_ABANDONED);
			//assert(ret != WAIT_TIMEOUT);
			//assert(ret != WAIT_FAILED);
			assert(ret == WAIT_OBJECT_0, ret);
		}
		
		void release_semaphore () {
			auto ret = ReleaseSemaphore(semaphore, 1, NULL);
			assert(ret != 0);
		}
		
		bool lock () {
			auto newCounter = InterlockedIncrement(&contentionCounter);
			if (newCounter > 1) {
				wait_for_semaphore();
				return true;
			}
			return false;
		}
		
		bool unlock () {
			auto newCounter = InterlockedDecrement(&contentionCounter);
			if (newCounter > 0) {
				release_semaphore();
				return true;
			}
			return false;
		}
	};
	#else
	struct Mutex {
	private:
		CRITICAL_SECTION		cs;
		
	public:
		void init () {
			auto ret = InitializeCriticalSectionAndSpinCount(&cs, 128);
			assert(ret != 0);
		}
		void init (DWORD spin_count) {
			auto ret = InitializeCriticalSectionAndSpinCount(&cs, spin_count);
			assert(ret != 0);
		}
		
		bool lock () {
			EnterCriticalSection(&cs);
			return false;
		}
		
		bool unlock () {
			LeaveCriticalSection(&cs);
			return false;
		}
	};
	#endif
	
	#define SYNC_SCOPED_MUTEX_LOCK(mutex_ptr) \
			(mutex_ptr)->lock(); \
			defer { (mutex_ptr)->unlock(); }
	
}
