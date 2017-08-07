
namespace winsock {
	
	DECLD WSADATA	wsa;
	
	void init () {
		
		{
			auto ret = WSAStartup(MAKEWORD(2,2), &wsa);
			assert(ret == 0);
		}
		
	}
	
	#define PROFILE_STREAMING_PORT	27015
	
}
namespace streaming {
	struct Server {
		SOCKET listen_sock;
		SOCKET client_sock;
		
		void start () {
			{
				addrinfo hints = {}; // zero
				hints.ai_family =	AF_INET;
				hints.ai_socktype =	SOCK_STREAM;
				hints.ai_protocol =	IPPROTO_TCP;
				hints.ai_flags =	AI_PASSIVE;
				
				addrinfo* result;
				defer { freeaddrinfo(result); };
				
				{
					auto ret = getaddrinfo(NULL, TO_STR(PROFILE_STREAMING_PORT), &hints, &result);
					assert(ret == 0, "%", ret);
				}
				
				{
					listen_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
					assert(listen_sock != INVALID_SOCKET);
				}
				
				{
					auto ret = bind(listen_sock, result->ai_addr, (int)result->ai_addrlen);
					assert(ret != SOCKET_ERROR);
				}
			}
			
			{
				auto ret = listen(listen_sock, SOMAXCONN);
				assert(ret != SOCKET_ERROR);
			}
			
		}
		
		void connect_to_client () {
			
			//print(">>> waiting for client\n");
			
			{
				client_sock = accept(listen_sock, NULL, NULL);
				assert(client_sock != INVALID_SOCKET);
			}
			
			//print(">>> client connected\n");
			
		}
		
		void read (void* buf, u64 buf_size) {
			char* cur = (byte*)buf;
			char* end = cur +safe_cast_assert(int, buf_size);
			
			while (cur != end) {
				auto ret = recv(client_sock, cur, (int)ptr_sub(cur, end), 0);
				if (ret > 0) {			// success
					
					//print(">>> recieved % bytes\n", ret);
					cur += ret;
					
				} else if (ret == 0) {	// graceful connection close
					
				} else {				// failed
					
					auto err = WSAGetLastError();
					if (err == WSAECONNRESET) {
						assert(false, "hard connection close [WSAECONNRESET]");
					} else {
						assert(false, "recv failed: % [%]", ret, err);
					}
				}
			}
			
		}
		
	};
	
	struct Client {
		SOCKET serv_sock;
		
		void connect_to_server () {
			cstr server_ip;
			{
				if (cmd_line.argc >= 2) {
					server_ip = cmd_line.argv[1];
				} else {
					server_ip = "127.0.0.1";
				}
			}
			
			{
				addrinfo hints = {}; // zero
				hints.ai_family =	AF_UNSPEC;
				hints.ai_socktype =	SOCK_STREAM;
				hints.ai_protocol =	IPPROTO_TCP;
				
				addrinfo* result;
				defer { freeaddrinfo(result); };
				{
					auto ret = getaddrinfo(server_ip, TO_STR(PROFILE_STREAMING_PORT), &hints, &result);
					if (ret != 0) {
						if (ret == WSAHOST_NOT_FOUND) {
							warning("getaddrinfo for '%':'%' failed [WSAHOST_NOT_FOUND]",
									server_ip, TO_STR(PROFILE_STREAMING_PORT));
						} else {
							assert(false, "getaddrinfo for '%':'%' failed [%]",
									server_ip, TO_STR(PROFILE_STREAMING_PORT), ret);
						}
					}
				}
				
				serv_sock = INVALID_SOCKET;
				
				if (result) {
					{
						serv_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
						assert(serv_sock != INVALID_SOCKET);
					}
					
					//print(">>> trying to connect to server\n");
					
					{
						auto ret = connect(serv_sock, result->ai_addr, result->ai_addrlen);
						if (ret == SOCKET_ERROR) {
							warning("Cannot connect to server %:%", server_ip, TO_STR(PROFILE_STREAMING_PORT));
							serv_sock = INVALID_SOCKET;
						}
					}
				}
				
			}
			
		}
		
		void write (void* data, u64 size) {
			if (serv_sock == INVALID_SOCKET) return;
			
			auto ret = send(serv_sock, (char*)data, safe_cast_assert(int, size), 0);
			assert(ret != SOCKET_ERROR);
			
		}
		
	};
	
}
