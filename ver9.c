#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <asm/ioctls.h>
#include <math.h>
#include <sys/time.h>

#define KEY_NUM1	1
#define KEY_NUM2    2
#define KEY_NUM3    3
#define KEY_NUM4    4
#define KEY_NUM5    5
#define KEY_NUM6    6
#define KEY_NUM7    7
#define KEY_NUM8    8
#define KEY_NUM9    9
#define KEY_NUM10   10
#define KEY_NUM11   11
#define KEY_NUM12   12

#define MAXCHR	32
#define led_dev	"/dev/led"
#define dot "/dev/dot"
#define fnd_dev	"/dev/fnd"
#define tact_d "/dev/tactsw"
#define clcd "/dev/clcd"

#define dbg(x...)       printf(x)

#define CARDSIZE 12
int card_in[12]; //카드 내용을 담는 배열 
int check_card[2]; // 선택한 카드가 짝이 맞는지 다른지 확인하기 위한 배열 
int check=0;//플레이어가 선택한 카드 번호 
int card_select[2];//플레이어가 선택한 카드 번호 두개 담는 배열 
int ordernum=0; //지금 고르는 카드가 첫번째 인지 두번째인지 확인 
int answer=0;//맞춘횟수  
int num1;//카드 12개 다맞췄을 시 게임을 종료하도록 함 
int player1_score=0;//플레이어1 점수 
int player2_score=0; //플레이어2 점수 
int player;//두 플레이어 구분 하기 위한 변수 
char c1,c2; //맞췄을 시 화면에 카드 내용을 보여주기 위한 변수 
char qmap[12];//카드 뒷면 
int dot_d = 0;
bool bools=true;// while 함수 종료하기 위한 논리값 
static char tactswDev[] = "/dev/tactsw";
static char lcdDev[] = "/dev/clcd";
static int  lcdFd = (-1);
char pla1[3] = "";
char pla2[3] = "";
char playervs[16] ="    ";
char texttext[32]="";
char lcd_score1[16] = ""; 


void print_lcd(char clcd_text[]) {
	int clcd_d;

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0){
		printf("clcd error\n");
	}

	write(clcd_d, clcd_text, 32);
	close(clcd_d);

}


void led_player(int player){
	int dev,led_device, count;
	int a=player;
	unsigned char data;
	dev = open(led_dev, O_RDWR);
	if(a==0){
		for( count =0 ; count <16; count ++) {
			if( count%2){
				data = 0xF0;
				write(dev, &data, sizeof(unsigned char));
				usleep(1000);
			}
			else{
				data = 0xFF;
				write(dev, &data, sizeof(unsigned char));
				usleep(1000);
			}
		}		
	}
	else{
		for( count =0 ; count <16; count ++) {
			if( count%2){
				data = 0x0F;
				write(dev, &data, sizeof(unsigned char));
				usleep(1000);
			}
			else{
				data = 0xFF;
				write(dev, &data, sizeof(unsigned char));
				usleep(1000);
			}
		}		
	}
	close(dev);
}

void led_name(int count){
	int dev,led_device;
	int a=count;
	unsigned char data;
	dev = open(led_dev, O_RDWR);
	switch(a){
		case 1:{
			data = 0xFc;
			write(dev, &data, sizeof(unsigned char));
			usleep(100000);
			break;
		}
		case 2:{
			data=0xF0;
			write(dev, &data, sizeof(unsigned char));
			usleep(100000);		
			print_lcd("  please enter    player2 name  ");			
			break;
		}
		case 3:{
			data=0xc0;
			write(dev, &data, sizeof(unsigned char));
			usleep(100000);
			break;
		}
		case 4:{
			data=0x00;
			write(dev, &data, sizeof(unsigned char));
			usleep(100000);
			break;
		}
	}

	close(dev);
}

void lcd_score(){
	
	char s1[10];
	char s2[10];
	
	sprintf(s1, "%d", player1_score);
	sprintf(s2, "%d", player2_score);
	printf("%s\n",s1);
	strcat(lcd_score1,"     ");
	strcat(lcd_score1, s1);
	strcat(lcd_score1," vs ");
	strcat(lcd_score1, s2);
	strcat(lcd_score1,"     ");	
	printf("%s",lcd_score1);
}

void append_name(char* dst, char c) {
	char* p = dst;
	while (*p != '\0') p++; // 문자열 끝 탐색
	*p = c;
	*(p + 1) = '\0';
}

int intro_game() {
	int dot_d = 0;
	int tact = 0;
	int fnd_d = 0;
	unsigned char t = 0;
	unsigned char c;
	unsigned char d;
	struct timeval dotst1, dotend1, tactst1, tactend1;
	int i = 0;

	int count = 0;

	char n1[1];
	char n2[1];
	char alphP[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	unsigned char alph[26][8] = { {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42},	// A
								{0x3c, 0x22, 0x22, 0x3c, 0x22, 0x22, 0x22, 0x3c},	//B?
								{0x1C, 0x22, 0x20, 0x20, 0x20, 0x20, 0x22, 0x1C},	//C?
								{0x38, 0x44, 0x42, 0x42, 0x42, 0x42, 0x44, 0x38},	//D?
								{0x3E, 0x20, 0x20, 0x3E, 0x20, 0x20, 0x20, 0x3E},	//E?
								{0x3E, 0x20, 0x20, 0x3E, 0x20, 0x20, 0x20, 0x20},	//F?
								{0x1C, 0x22, 0x42, 0x40, 0x40, 0x47, 0x42, 0x3C},	//G?
								{0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42},	//H??
								{0x1C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C},	//I?
								{0x1C, 0x08, 0x08, 0x08, 0x08, 0x48, 0x48, 0x30},	//J?
								{0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x44},	//K?
								{0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3E},	//L?
								{0x81, 0xC3, 0xA5, 0x99, 0x81, 0x81, 0x81, 0x81},	//M??
								{0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42},	//N
								{0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C},	//O
								{0x7C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40},	//P
								{0x38, 0x44, 0x82, 0x82, 0x82, 0x8A, 0x44, 0x3A},	//Q
								{0x7C, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42},	//R
								{0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x3C},	//S?
								{0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08},	//T?
								{0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},	//U
								{0x81, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18},	//V
								{0x81, 0x99, 0x99, 0x99, 0x99, 0x99, 0x5A, 0x24},	//W
								{0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81},	//X
								{0x81, 0x81, 0x42, 0x42, 0x3C, 0x18, 0x18, 0x18},	//Y
								{0xFF, 0x03, 0x02, 0x04, 0x08, 0x10, 0x60, 0xFF},	//Z?
	};

	gettimeofday(&dotst1, NULL);

	while (1)
	{

		if(count==4){
			strcat(playervs,pla1);
			strcat(playervs," vs ");
			strcat(playervs,pla2);
			strcat(playervs,"    ");
			printf("%s\n",playervs);
			printf("%s\n",pla1);
			printf("%s\n",pla2);
			return 0;
		}
		if (dot_d == 0) {
			dot_d = open(dot, O_RDWR);
		}
		gettimeofday(&dotend1, NULL);

		write(dot_d, &alph[i], sizeof(alph[i]));

		if ((dotend1.tv_usec - dotst1.tv_usec > 200000) || (dotend1.tv_sec > dotst1.tv_sec && (dotend1.tv_usec + 1000000 - dotst1.tv_usec > 200000)))
		{
			dot_d = close(dot_d);
			if (tact == 0)     //tact switch에 접근하지 않은 경우만 open
			{
				tact = open(tact_d, O_RDWR);
			}
			gettimeofday(&tactst1, NULL);
			while (1) {
				gettimeofday(&tactend1, NULL);
				read(tact, &t, sizeof(t));
				switch (t) {

				case KEY_NUM4:
					i = i - 1;
					break;

				case KEY_NUM5: {
					count = count + 1;
					led_name(count);
					if (count < 3) {
						append_name(pla1, alphP[i]);
						printf("%s\n", pla1);
						
					}
					else {
						append_name(pla2, alphP[i]);
						printf("%s\n", pla2);
					}

					break;
				}

				case KEY_NUM6:
					i = i + 1;
					break;
				}
				if ((tactend1.tv_usec - tactst1.tv_usec > 200000) || (tactend1.tv_sec > tactst1.tv_sec && (tactend1.tv_usec + 1000000 - tactst1.tv_usec > 200000)) || t)
				{
					tact = close(tact);
					break;
				}
			}
			gettimeofday(&dotst1, NULL);
		}

	}


}



unsigned char rps[1][8] = {	// dot matrix
	{ 0x00,0x54,0x00,0x54,0x00,0x54,0x00,0x54 }, // 초기값 

};
unsigned char card_led[1][3] = {
	{0x40,0x10,0x04},
};
void card_off(int a) {
	int a1 = a;
	int back1, back2, back3;
	if (a1 < 4) {
		back1 = rps[0][1];
	}
	else if (a1 < 7) {
		back1 = rps[0][3];
	}
	else if (a1 < 10) {
		back1 = rps[0][5];
	}
	else {
		back1 = rps[0][7];
	}
	switch (a1) {
	case(1): {
		back2 = card_led[0][0];
		back3 = back1 - back2;
		rps[0][1] = back3;
		break;
	}
	case(2): {
		back2 = card_led[0][1];
		back3 = back1 - back2;
		rps[0][1] = back3;
		break;
	}
	case(3): {
		back2 = card_led[0][2];
		back3 = back1 - back2;
		rps[0][1] = back3;
		break;
	}
	case(4): {
		back2 = card_led[0][0];
		back3 = back1 - back2;
		rps[0][3] = back3;
		break;
	}
	case(5): {
		back2 = card_led[0][1];
		back3 = back1 - back2;
		rps[0][3] = back3;
		break;
	}
	case(6): {
		back2 = card_led[0][2];
		back3 = back1 - back2;
		rps[0][3] = back3;
		break;
	}
	case(7): {
		back2 = card_led[0][0];
		back3 = back1 - back2;
		rps[0][5] = back3;
		break;
	}
	case(8): {
		back2 = card_led[0][1];
		back3 = back1 - back2;
		rps[0][5] = back3;
		break;
	}
	case(9): {
		back2 = card_led[0][2];
		back3 = back1 - back2;
		rps[0][5] = back3;
		break;
	}
	case(10): {
		back2 = card_led[0][0];
		back3 = back1 - back2;
		rps[0][7] = back3;
		break;
	}
	case(11): {
		back2 = card_led[0][1];
		back3 = back1 - back2;
		rps[0][7] = back3;
		break;
	}
	case(12): {
		back2 = card_led[0][2];
		back3 = back1 - back2;
		rps[0][7] = back3;
		break;
	}
	}
}

int FND_Out(int a, int b, int c, int d) {

	unsigned char FND_DATA_TBL[] = {
			0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,
			0x83,0xC6,0xA1,0x86,0x8E,0xC0,0xF9,0xA4,0xB0,0x99,0x89
	};

	int fnd_fd = 0;

	unsigned char fnd_num[4];

	fnd_num[0] = FND_DATA_TBL[a];
	fnd_num[1] = FND_DATA_TBL[b];
	fnd_num[2] = FND_DATA_TBL[c];
	fnd_num[3] = FND_DATA_TBL[d];

	fnd_fd = open(fnd_dev, O_RDWR);

	if (fnd_fd < 0) {
		printf("Can't Open Device\n");
	}
	write(fnd_fd, &fnd_num, sizeof(fnd_num));
	sleep(1);
	close(fnd_fd);
}



void map1(void) {
	int i;
	for (i = 0; i < 12; i++) {
		qmap[i] = '?';
	}
}//카드 내용을 가리기 위해 카드 뒷면으로 사용할 '?' 배열 


void card_shuffle(void) {
	srand(time(NULL));//게임을 시작할때마다 다르게 섞이도록 하기 위한 srand()함수 
	int i, j, x;
	memset(card_in, 0, sizeof(card_in));
	for (i = 1; i < 7; i++) //1~6 숫자를  
	{
		for (j = 0; j < 2; j++)//두 개의 카드에 집어 넣음 
		{
			do
			{
				x = rand() % 12;
			} while (card_in[x] != 0);

			card_in[x] = i;

		}
	}
	sleep(2);
} //12개의 카드에 1~6의 숫자를 두번씩 총 12번 넣어 카드를 섞는 코드

void show_map(void) {
	printf("\n");
	int i;
	for (i = 0; i < 12; i++) {
		if (i % 3 == 0) {
			printf("\n");
		}
		printf("%c ", qmap[i]);
	}
	printf("\n");
}//플레이어에게 카드나열한걸 보여주는 맵. 처음 시작시에는 카드 뒷면 '?'를 보여줌 

void show_num(int a, int b) {
	c1 = card_in[a] + '0';
	c2 = card_in[b] + '0';
	qmap[a] = c1;
	qmap[b] = c2;
}//플레이어가 카드 짝을 맞췄을 시 '?" 를 지우고 카드 내용을 보여줌 

void reset_check(void) {
	check_card[0] = 0;
	check_card[1] = 0;
	card_select[0] = 0;
	card_select[1] = 0;
} //짝을 맞췄거나 틀렸을경우 선택한 카드를 초기화하는 함수 

void print_please(void) {
	if (ordernum == 1) {
		printf("\n");
		printf("두번째 카드를 골라주세요:  ");
	}
	else {
		printf("\n");
		printf("첫번째 카드를 골라주세요: ");
	}//지금 고르는 게 몇번째인지 보여주기 위한 함수 
}

void print_waiting(void) {
	printf("----------------\n");
	printf("  shuffling...\n");
	printf("----------------\n");
	print_lcd("   shuffling.   ");
}//게임 시작시 카드 섞는것처럼 보여줌 

void change_player(void) {
	if (player == 0) {
		player = 1;
	}
	else {
		player = 0;
	}//플레이어가 짝이 맞는 카드를 고르지 못했을 경우 다른 플레이어에게 차례를 넘기는 함수 
}

void dot_smile(int right) {
	int i;
	unsigned char c[2][8] = { {0x00,0x66,0x66,0x00,0x00,0x42,0x3c,0x00},
				{0x00,0x66,0x66,0x00,0x00,0x3c,0x42,0x00} };
	if ((dot_d = open(dot, O_RDWR)) < 0) {
		printf("Can't Open\n");
		exit(0);
	}
	write(dot_d, &c[right], sizeof(c[right]));
	sleep(2);
	close(dot_d);
} // dot_matrix에 웃음 표시

void checkcard(int a, int b) {
	a = a - 1;//첫번째 카드 
	b = b - 1;//두번째 카드  
	if (card_in[a] == card_in[b]) {
		num1++;
		if (player == 0) {
			player1_score++;
			answer = player1_score;
		}
		else {
			player2_score++;
			answer = player2_score;
		}
		printf("\n");
		printf("짝을 맞췄습니다!\n");
		printf("\n");
		dot_smile(0); // 웃음 표시
		printf("플레이어%d의 점수: %d", player + 1, answer);
		card_off(a + 1);
		card_off(b + 1);
		show_num(a, b);
		reset_check();//check_card[],card_select[] 초기화 
		card_in[a] = 0;//이미 맞춘 카드를 고르지 못하도록 카드내용을 0으로 설정 
		card_in[b] = 0;//이미 맞춘 카드를 고르지 못하도록 카드내용을 0으로 설정 
		show_map();//현재 남은 카드 배치를 보여줌
		texttext[0]='\0';
		lcd_score1[0]='\0';
		lcd_score();	
		strcat(texttext,playervs);
		strcat(texttext,lcd_score1);
		print_lcd(texttext);
	}//짝이 맞을경우 카드를 뒤집어 숫자를 보여주고 check_card[],card_select[] 초기화  
	else {
		printf("\n");
		printf("틀렸습니다 차례가 넘어갑니다.\n");
		printf("\n");
		dot_smile(1);
		change_player();//상대방에게 순서를 넘겨줌 
		reset_check();//check_card[],card_select[] 초기화
		led_player(player);
		printf("플레이어%d의 차례입니다.\n", player + 1);

	}
}//고른 두 개의 카드가 짝이 맞는지 체크하는 함수 

void sum_score(void) {
	char a[32]="";
	if (player1_score > player2_score) {
		printf("\n");
		printf("플레이어1이 승리하였습니다!");
		printf("\n");
		strcat(a,"congratulations!    ");
		strcat(a,pla1);
		strcat(a," win!!    ");
		print_lcd(a);
		led_player(0);
		
	}
	else if (player1_score == player2_score) {
		printf("\n");
		printf("아쉽습니다 비겼습니다...");
		printf("\n");
		strcat(a,"    too bad.    ");
		strcat(a,"      draw      ");
	}
	else {
		printf("\n");
		printf("플레이어2가 승리하였습니다!");
		printf("\n");
		strcat(a,"congratulations!    ");
		strcat(a,pla2);
		strcat(a," win!!    ");
		print_lcd(a);
		led_player(1);
	}

}//다 맞춘 후 점수 계산하는 함수 

void dot_num(int choice) {
	unsigned char c[7][8] = { {0x3c,0x42,0x42,0x42,0x42,0x42,0x3c,0x00},
								{0x18,0x28,0x08,0x08,0x08,0x08,0x3c,0x00},
								{0x18,0x24,0x24,0x04,0x08,0x10,0x3c,0x00},
								{0x18,0x24,0x04,0x18,0x04,0x24,0x18,0x00},
								{0x04,0x0C,0x14,0x24,0x7E,0x04,0x04,0x00},
								{0x3c,0x20,0x20,0x18,0x04,0x24,0x18,0x00},
								{0x18,0x24,0x20,0x38,0x24,0x24,0x18,0x00}, };
	dot_d = open(dot, O_RDWR);

	write(dot_d, &c[choice], sizeof(c[choice]));
	sleep(1);
	dot_d = close(dot_d);
}

void put_num(int check) {
	int x, y;
	if (card_in[check - 1] == 0) {
		printf("\n");
		printf("이미 맞춘 카드입니다.\n");
		if (card_select[0] == 0) {
			ordernum = 0;
		}
		else {
			ordernum = 1;
		}
	}//고른 카드가 이미 짝을 맞춘 카드인지를 체크하는 조건 문 
	else {
		if (card_select[0] == 0) {
			check_card[0] = card_in[check - 1];
			card_select[0] = check;
			x = check_card[0];
			printf("\n");
			printf("첫번째 카드 내용: %d", check_card[0]);
			printf("\n");
			ordernum = 1;
			dot_num(x);
		}//첫번째 카드를 고르지 않았을 경우 첫번째 카드를  card_select[0]에 카드번호를, check_card[0]에 카드 앞면을 저장 
		else {
			if (card_in[check - 1] == 0) {
				ordernum = 1;
				printf("\n");
				printf("이미 맞춘 카드입니다.\n");
			}//고른 카드가 이미 짝을 맞춘 카드인지를 체크하는 조건 문
			else {
				check_card[1] = card_in[check - 1];
				card_select[1] = check;
				y = check_card[1];
				printf("\n");
				printf("두번째 카드 내용: %d", check_card[1]);
				printf("\n");
				dot_num(y);

				if (card_select[0] == card_select[1]) {
					printf("\n");
					printf("중복된 카드를 골랐습니다 다시 고르세요.\n");
					printf("\n");
					check_card[1] = 0;
					card_select[1] = 0;
				}
			}//첫번째와 두번째 카드를 같은 카드를 골랐을 경우 두번째 카드 정보를 담은 check_card[1],card_select[1]을 초기화후 다시 고르도록 함 

			if (card_select[0] != 0 && card_select[1] != 0) {
				printf("\n");
				printf("두개의 카드를 골랐습니다\n");
				printf("\n");
				ordernum = 0;
				checkcard(card_select[0], card_select[1]);
			}//서로 다른 카드 두개를 골랐으면 두 카드를 비교 
		}
	}
}//카드를 고르는 함수 



int main(void) {
	struct timeval dotst, dotend, tactst, tactend, fndst, fndend, ledst, ledend;
	int dot_d = 0;
	int tact = 0;
	int fnd_d = 0;
	unsigned char t = 0;
	unsigned char c;
	unsigned char d;
	int dev;
	print_lcd("  please enter    player1 name  ");
	intro_game();
	lcd_score();
	print_waiting();
	card_shuffle();	
	strcat(texttext,playervs);
	strcat(texttext,lcd_score1);
	print_lcd(texttext);	
	map1();
	show_map();
	led_player(player);
	gettimeofday(&dotst, NULL);

	while (num1 < 6)
	{

		if (dot_d == 0) {
			dot_d = open(dot, O_RDWR);
		}
		gettimeofday(&dotend, NULL);

		write(dot_d, &rps, sizeof(rps));

		if ((dotend.tv_usec - dotst.tv_usec > 200000) || (dotend.tv_sec > dotst.tv_sec && (dotend.tv_usec + 1000000 - dotst.tv_usec > 200000)))
		{
			dot_d = close(dot_d);
			if (tact == 0)     //tact switch에 접근하지 않은 경우만 open
			{
				tact = open(tact_d, O_RDWR);
			}
			gettimeofday(&tactst, NULL);
			while (1) {
				gettimeofday(&tactend, NULL);
				read(tact, &t, sizeof(t));
				switch (t) {
				case KEY_NUM1:
					printf("%d", 1);
					put_num(1);
					break;
				case KEY_NUM2:
					printf("%d", 2);
					put_num(2);
					break;
				case KEY_NUM3:
					printf("%d", 3);
					put_num(3);
					break;
				case KEY_NUM4:
					printf("%d", 4);
					put_num(4);
					break;
				case KEY_NUM5:
					printf("%d", 5);
					put_num(5);
					break;
				case KEY_NUM6:
					printf("%d", 6);
					put_num(6);
					break;
				case KEY_NUM7:
					printf("%d", 7);
					put_num(7);
					break;
				case KEY_NUM8:
					printf("%d", 8);
					put_num(8);
					break;
				case KEY_NUM9:
					printf("%d", 9);
					put_num(9);
					break;
				case KEY_NUM10:
					printf("%d", 10);
					put_num(10);
					break;
				case KEY_NUM11:
					printf("%d", 11);
					put_num(11);
					break;
				case KEY_NUM12:
					printf("%d", 12);
					put_num(12);
					break;
				}
				if ((tactend.tv_usec - tactst.tv_usec > 200000) || (tactend.tv_sec > tactst.tv_sec && (tactend.tv_usec + 1000000 - tactst.tv_usec > 200000)) || t)
				{
					tact = close(tact);
					
					gettimeofday(&ledst, NULL);
					led_player(player);
					gettimeofday(&ledend, NULL);
					if ((ledend.tv_usec - ledst.tv_usec > 200000) || (ledend.tv_sec > ledst.tv_sec && (ledend.tv_usec + 1000000 - ledst.tv_usec > 200000))){
						
					
						break;
					}
					break;
				}
			}
			gettimeofday(&dotst, NULL);
		}

	}
	sum_score();

	return 0;
}
