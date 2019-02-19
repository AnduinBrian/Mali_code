#include<stdio.h>
#include<Windows.h>
#include <time.h>

char random()
{
	srand(time(NULL));
	BYTE i = 0;
	BYTE num = (rand() % (0x7A - 0x40 + 1)) + 0x40;
	if (num == 0x5c || num == 0x5f || num == 0x7c) num = random();
	return num;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr,"usage: file_name", argv[0]);
		exit(1);
	}
	else
	{
		HANDLE v1, v2;
		v1 = NULL;
		v2 = NULL;
		DWORD file_size = 0;
		char *buffer;
		DWORD temp_length = strlen(argv[1]) + 1;
		char *temp = (char*)malloc(temp_length);
		strcpy(temp, argv[1]);

		temp_length = strlen(temp) - 1;
		while (temp[temp_length] != 0x5c) {
			temp[temp_length] = random();
			temp_length--;
		}


		v1 = CreateFileA(argv[1], GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
		if (v1 == INVALID_HANDLE_VALUE || v1 == NULL) {

			CloseHandle(v1);
			free(temp);
		}

		else {
			v2 = CreateFileMappingA(v1, NULL, 0x04, 0, 0, 0);

			if (v2 == INVALID_HANDLE_VALUE || !v2) {
				CloseHandle(v1);
				CloseHandle(v2);
			}

			else {

				file_size = GetFileSize(v1, 0);
				buffer = (char*)malloc(file_size);
				buffer = (char*)MapViewOfFile(v2, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);


				if (buffer == NULL) {
					CloseHandle(v1);
					CloseHandle(v2);
					UnmapViewOfFile(buffer);
					printf("Error in file :%s\nError code :0x%x", argv[1], GetLastError());
				}

				else {

					int i = 0;
					for (i; i < (int)file_size - 1; i++)
					{
						buffer[i] = (rand() << 2) + buffer[i + 1] ^ 3;
					}
					UnmapViewOfFile(buffer);
					CloseHandle(v1);
					CloseHandle(v2);
				}
				free(temp);

			}
		}
	}
	puts("DONE");
	system("PAUSE");
}