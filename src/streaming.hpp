
namespace winsock {
	
	DECLD WSADATA	wsa;
	
	void init () {
		
		{
			auto ret = WSAStartup(MAKEWORD(2,2), &wsa);
			assert(ret == 0);
		}
		
	}
	
	#define PROFILE_STREAMING_PORT	27015
	#define SERVER_IP				"127.0.0.1"
	
	void serv_test () {
		
		SOCKET listen_sock;
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
		
		for (;;) {
			{
				auto ret = listen(listen_sock, SOMAXCONN);
				assert(ret != SOCKET_ERROR);
			}
			
			print(">>> waiting for client\n");
			
			SOCKET client_sock;
			{
				client_sock = accept(listen_sock, NULL, NULL);
				assert(client_sock != INVALID_SOCKET);
			}
			
			print(">>> client connected, reading messages\n");
			
			for (;;) {
				char buf[512];
				
				auto ret = recv(client_sock, buf, arrlenof<int>(buf), 0);
				if (ret > 0) {
					print(">>> recieved % bytes: '%'\n", ret, escaped(lstr(buf, (u32)ret)));
					
				} else if (ret == 0) {
					break;
				} else {
					// failed
					auto err = WSAGetLastError();
					if (err == WSAECONNRESET) {
						warning("hard connection reset [WSAECONNRESET]");
					} else {
						warning("recv failed: % [%]", ret, err);
					}
					break;
				}
			}
			
			print(">>> connection closed\n");
		}
	}
	
	void client_test () {
		
		SOCKET serv_sock;
		{
			addrinfo hints = {}; // zero
			hints.ai_family =	AF_UNSPEC;
			hints.ai_socktype =	SOCK_STREAM;
			hints.ai_protocol =	IPPROTO_TCP;
			
			addrinfo* result;
			defer { freeaddrinfo(result); };
			{
				auto ret = getaddrinfo(SERVER_IP, TO_STR(PROFILE_STREAMING_PORT), &hints, &result);
				assert(ret == 0, "%", ret);
			}
			
			{
				serv_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
				assert(serv_sock != INVALID_SOCKET);
			}
			
			print(">>> trying to connect to server\n");
			
			{
				auto ret = connect(serv_sock, result->ai_addr, result->ai_addrlen);
				if (ret == SOCKET_ERROR) {
					warning("Cannot connect to server %", SERVER_IP ":" TO_STR(PROFILE_STREAMING_PORT));
					serv_sock = INVALID_SOCKET;
				}
			}
			
		}
		
		print(">>> sending initial message\n");
		
		if (serv_sock != INVALID_SOCKET) {
			
			for (u32 i=0; i<10; ++i) {
				DEFER_POP(&working_stk);
				lstr msg = print_working_stk("Hello World %\\0", i);
				
				auto ret = send(serv_sock, msg.str, safe_cast_assert(int, msg.len), 0);
				assert(ret != SOCKET_ERROR);
			}
		}
		
	}
	
}
