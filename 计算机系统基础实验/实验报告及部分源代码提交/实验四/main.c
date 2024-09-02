#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>

void cal(void);

char username[20] = "mayong";
char password[20] = "123456";

int main() {
	char username_input[20];
	char password_input[20];
	int times = 0;

	while (strcmp(username, username_input)!=0 || strcmp(password, password_input)!=0) {
		if (times == 3) {
			printf("Failed too many times, the program will exit!\n");
			return 0;
		}
		if (times) {
			printf("User name or password is incorrect, please input again!\n");
		}
		printf("Please input user name and password!\n");
		printf("username: ");
		scanf("%s", username_input);
		printf("password: ");
		scanf("%s", password_input);
		times++;
	}

	printf("Login successfully!\n");
	
	printf("MIDF:\n");
	cal();
	return 0;
}
