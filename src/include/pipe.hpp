
{
			PROFILE_SCOPED(THR_ENGINE, "var_file_cmd_pipe_setup");
			{
				auto ret = CreateNamedPipe(CMD_PIPE_NAME, PIPE_ACCESS_INBOUND|FILE_FLAG_OVERLAPPED,
						PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT, 1, kibi(1), kibi(1), 0, NULL);
				if (ret == INVALID_HANDLE_VALUE) {
					warning("CreateNamedPipe(\"%\", ...) failed [%], "
							"possibly because another instance is already running, pipe commands will not work.\n",
							CMD_PIPE_NAME, GetLastError());
				}
				cmd_pipe_handle = ret;
			}
			
			{
				auto ret = CreateEventW(NULL, FALSE, TRUE, NULL);
				assert(ret != NULL);
				cmd_pipe_ov.hEvent = ret;
			}
			
			if (cmd_pipe_handle != INVALID_HANDLE_VALUE) {
				auto ret = ConnectNamedPipe(cmd_pipe_handle, &cmd_pipe_ov);
				
				if (ret != 0) {
					print("inital ConnectNamedPipe() completed instantly\n");
				} else {
					auto err = GetLastError();
					assert(err == ERROR_IO_PENDING, "%", err);
					//print("inital ConnectNamedPipe() pending\n");
				}
			}
		}
		
		DECLD HANDLE			cmd_pipe_handle;
	DECLD OVERLAPPED		cmd_pipe_ov =					{}; // initialize to zero
	
	DECL bool pipe_test () {
		
		if (cmd_pipe_handle == INVALID_HANDLE_VALUE) {
			return false;
		}
		
		{
			auto ret = HasOverlappedIoCompleted(&cmd_pipe_ov);
			if (ret == FALSE) {
				// still pending
				return false;
			} else {
				//print("ConnectNamedPipe() completed\n");
			}
		}
		
		bool cmd_read = false;
		
		do {
			constexpr DWORD SIZE = 64;
			char buf[SIZE +1];
			
			DWORD bytes_read;
			{
				auto ret = ReadFile(cmd_pipe_handle, buf, SIZE, &bytes_read, &cmd_pipe_ov);
				
				if (ret != 0) {
					//print("ReadFile() completed instantly\n");
					assert(bytes_read <= SIZE);
				} else {
					auto err = GetLastError();
					if (err == ERROR_MORE_DATA) {
						warning("Message bigger than buffer recieved over %!, ignoring.\n", CMD_PIPE_NAME);
						break;
					} else {
						
						assert(err == ERROR_IO_PENDING, "%", err);
						assert(bytes_read == 0, "%", bytes_read);
						
						{
							auto ret = GetOverlappedResult(cmd_pipe_handle, &cmd_pipe_ov, &bytes_read, TRUE);
							assert(ret != 0);
						}
						print("ReadFile() completed after waiting\n");
						assert(bytes_read <= SIZE);
						
					}
				}
			}
			buf[bytes_read] = '\0';
			
			lstr iden = { "", 0 };
			{
				char* cur = buf;
				char* res;
				
				res = parse_n::whitespace(cur);
				if (res) { cur = res; }
				
				if (parse_n::is_identifier_start_c(*cur)) {
					
					cur = var::identifier(cur, &iden);
					
					res = parse_n::whitespace(cur);
					if (res) { cur = res; }
				}
				
				res = parse_n::newline(cur);
				if (res) { cur = res; }
			}
			
			if (	str::comp(iden, "update") ) {
				//print("update cmd recieved.\n");
				cmd_read = true;
			} else {
				print("unknown cmd(-format) recieved (cmd: '%')\n",
						escaped( lstr{buf, bytes_read} ) );
			}
			//bool cmd = str::comp(buf, bytes_read, "var_write_save");
			//bool cmd = str::comp(buf, bytes_read, "var_read");
			
		} while (0);
		
		{
			auto ret = DisconnectNamedPipe(cmd_pipe_handle);
			assert(ret != 0, "%", GetLastError());
		}
		
		{
			auto ret = ConnectNamedPipe(cmd_pipe_handle, &cmd_pipe_ov);
			
			if (ret != 0) {
				print("ConnectNamedPipe() completed instantly\n");
			} else {
				auto err = GetLastError();
				assert(err == ERROR_IO_PENDING, "%", err);
				//print("ConnectNamedPipe() pending\n");
			}
		}
		
		return cmd_read;
	}