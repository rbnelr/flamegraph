
namespace winsock {
	
	DECLD WSADATA	wsa;
	
	void init () {
		auto ret = WSAStartup(MAKEWORD(2,2), &wsa);
		assert(ret == 0);
	}
	
	void cleanup () {
		auto ret = WSACleanup();
		if (ret != 0) warning("WSACleanup() failed, % [%]", ret, WSAGetLastError());
	}
	
	#define PROFILE_STREAMING_PORT	27015
	
}
namespace streaming {
	
	struct IP_Addr {
		u16		port;
		union {
			u8	raw[4];
			u32	i;
		} ip;
		
		void set (sockaddr const* addr) {
			assert(addr->sa_family == AF_INET);
			auto* a = (sockaddr_in*)addr;
			
			port = ntohs(a->sin_port);
			ip.i = ntohl(a->sin_addr.S_un.S_addr);
		}
		
		// for use with '%.%.%.%:%'
		#define _IP_PRINT(a) (a).ip.raw[3], (a).ip.raw[2], (a).ip.raw[1], (a).ip.raw[0], (a).port
	};
	
	struct Server {
		SOCKET	listen_sock;
		SOCKET	client_sock;
		IP_Addr	client_ip;
		
		bool	connected;
		
		void start () {
			connected = false;
			
			{
				addrinfo hints = {}; // zero
				hints.ai_family =	AF_INET;
				hints.ai_socktype =	SOCK_STREAM;
				hints.ai_protocol =	IPPROTO_TCP;
				hints.ai_flags =	AI_PASSIVE;
				
				addrinfo* result;
				defer { freeaddrinfo(result); };
				
				print("Server on port %\n", PROFILE_STREAMING_PORT);
				
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
		
		bool client_pending () {
			
			WSAPOLLFD fd = {};
			fd.fd =		listen_sock;
			fd.events =	POLLRDNORM;
			
			auto ret = WSAPoll(&fd, 1, 0);
			if (		ret == 0 ) {
				
				assert(fd.revents == 0);
				return false;
				
			} else if (	ret == 1 ) {
				
				assert(fd.revents == POLLRDNORM, "%", hex(fd.revents));
				return true;
				
			} else {
				assert(false, "%", ret);
				connected = false;
				return false;
			}
			
		}
		
		void connect_to_client () {
			
			//print(">>> waiting for client\n");
			
			{
				client_sock = accept(listen_sock, NULL, NULL);
				assert(client_sock != INVALID_SOCKET);
			}
			
			{
				sockaddr addr;
				int addr_sz = sizeof(addr);
				auto ret = getpeername(client_sock, &addr, &addr_sz);
				assert(ret == 0 && addr_sz == (int)sizeof(addr));
				
				client_ip.set(&addr);
			}
			
			// TODO: is it valid to change buffer size after accept() (since there might already be data in the buffer)
			//        how would i do it before accept() ? does setting the SO_RCVBUF of the listen_sock set the default of all created sockets?
			{ // Set recieve buffer to large value for good measure
				int size = mebi<int>(1);
				{
					int size_sz = sizeof(size);
					auto ret = setsockopt(client_sock, SOL_SOCKET, SO_RCVBUF, (char*)&size, size_sz);
					assert(ret == 0, "% [%]", ret, WSAGetLastError());
				}
				{
					int size_sz = sizeof(size);
					auto ret = getsockopt(client_sock, SOL_SOCKET, SO_RCVBUF, (char*)&size, &size_sz);
					assert(ret == 0, "% [%]", ret, WSAGetLastError());
				}
				//print(">>> SO_RCVBUF: %\n", size);
			}
			
			connected = true;
			print(">>> Connected to client %.%.%.%:%.\n", _IP_PRINT(client_ip));
			
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
		
		#if 0
		int pollerr =		POLLERR;
		int pollhup =		POLLHUP;
		int pollnval =		POLLNVAL;
		int pollpri =		POLLPRI;
		int pollrdband =	POLLRDBAND;
		int pollrdnorm =	POLLRDNORM;
		int pollwrnorm =	POLLWRNORM;
		#endif
		
		bool poll_read_avail () {
			
			WSAPOLLFD fd = {};
			fd.fd =		client_sock;
			fd.events =	POLLRDNORM;
			
			auto ret = WSAPoll(&fd, 1, 0);
			if (		ret == 0 ) {
				
				assert(fd.revents == 0);
				return false;
				
			} else if (	ret == 1 ) {
				// POLLRDNORM			-> data available
				// POLLRDNORM|POLLHUP	-> data available (also disconnected, but lets read the last data chunk first)
				// POLLHUP				-> disconnected, let's do enter disconnected state
				
				if (fd.revents == POLLHUP) {
					print(">>> Client %.%.%.%:% disconnected gracefully.\n", _IP_PRINT(client_ip));
					
					connected = false;
					return false;
				}
				
				if (!(fd.revents & POLLRDNORM)) warning("%", hex(fd.revents));
				
				return fd.revents & POLLRDNORM;
				
			} else {
				assert(false, "%", ret);
				connected = false;
				return false;
			}
		}
		
	};
	
	struct Client {
		SOCKET	serv_sock;
		bool	connected;
		
		void connect_to_server () {
			cstr server_ip;
			{
				if (cmd_line.argc >= 2) {
					server_ip = cmd_line.argv[1];
				} else {
					server_ip = "127.0.0.1";
				}
			}
			connected = false;
			
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
					
					{ // Set send buffer size to a large value to avoid stalls if the flamegraph app is not frequently enough (slow framerate)
						int size = mebi<int>(1);
						{
							int size_sz = sizeof(size);
							auto ret = setsockopt(serv_sock, SOL_SOCKET, SO_SNDBUF, (char*)&size, size_sz);
							assert(ret == 0, "% [%]", ret, WSAGetLastError());
						}
						{
							int size_sz = sizeof(size);
							auto ret = getsockopt(serv_sock, SOL_SOCKET, SO_SNDBUF, (char*)&size, &size_sz);
							assert(ret == 0, "% [%]", ret, WSAGetLastError());
						}
						//print(">>> SO_SNDBUF: %\n", size);
					}
					
					//print(">>> trying to connect to server\n");
					
					{
						auto ret = connect(serv_sock, result->ai_addr, result->ai_addrlen);
						if (ret == SOCKET_ERROR) {
							warning("Cannot connect to server %:% [%]",
									server_ip, TO_STR(PROFILE_STREAMING_PORT), WSAGetLastError());
							serv_sock = INVALID_SOCKET;
						}
					}
					
				}
				
			}
			
			if (serv_sock != INVALID_SOCKET) {
				connected = true;
				print("Connected to server %:%.\n", server_ip, TO_STR(PROFILE_STREAMING_PORT));
			}
		}
		
		void disconnect_from_server () {
			
			auto ret = closesocket(serv_sock);
			if (ret != 0) warning("closesocket() failed % [%]", ret, WSAGetLastError());
			
			connected = false;
			print("Disconnected from server.\n");
			
		}
		
		void write (void* data, u64 size) {
			assert(connected);
			
			auto ret = send(serv_sock, (char*)data, safe_cast_assert(int, size), 0);
			assert(ret != SOCKET_ERROR);
			assert(ret == safe_cast_assert(int, size));
			
		}
		
		bool poll_write_avail () {
			assert(connected);
			
			WSAPOLLFD fd = {};
			fd.fd =		serv_sock;
			fd.events =	POLLWRNORM;
			
			auto ret = WSAPoll(&fd, 1, 0);
			if (		ret == 0 ) {
				
				assert(fd.revents == 0);
				return false;
				
			} else if (	ret == 1 ) {
				
				if (fd.revents & POLLHUP) {
					connected = false;
					return false;
				}
				
				assert(fd.revents == POLLWRNORM, "%", hex(ret));
				return true;
				
			} else {
				assert(false, "%", ret);
				return false;
			}
		}
		
	};
	
}
