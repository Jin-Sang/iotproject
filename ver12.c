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

int timer = 5;
int card_in[12]; //ī�� ������ ��� �迭 
int check_card[2]; // ������ ī�尡 ¦�� �´��� �ٸ��� Ȯ���ϱ� ���� �迭 
int check=0;//�÷��̾ ������ ī�� ��ȣ 
int card_select[2];//�÷��̾ ������ ī�� ��ȣ �ΰ� ��� �迭 
int ordernum=0; //���� ������ ī�尡 ù��° ���� �ι�°���� Ȯ�� 
int answer=0;//����Ƚ��  
int num1;//ī�� 12�� �ٸ����� �� ������ �����ϵ��� �� 
int player1_score=0;//�÷��̾�1 ���� 
int player2_score=0; //�÷��̾�2 ���� 
int player;//�� �÷��̾� ���� �ϱ� ���� ���� 
char c1,c2; //������ �� ȭ�鿡 ī�� ������ �����ֱ� ���� ���� 
char qmap[12];//ī�� �޸� 
int dot_d = 0;
int fnd_fd = 0;
int dev=0;
bool bools=true;// while �Լ� �����ϱ� ���� ������ 
static char tactswDev[] = "/dev/tactsw";
static char lcdDev[] = "/dev/clcd";
static int  lcdFd = (-1);
char pla1[3] = "";
char pla2[3] = "";
char playervs[16] ="    ";
char texttext[32]="";
char lcd_score1[16] = ""; 


void change_player(void) {
	if (player == 0) {
		player = 1;
	}
	else {
		player = 0;
	}//�÷��̾ ¦�� �´� ī�带 ������ ������ ��� �ٸ� �÷��̾�� ���ʸ� �ѱ�� �Լ� 
}

int FND_Out(int a, int b, int c, int d) {
	int i;
	unsigned char FND_DATA_TBL[] = {
			0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,
			0x83,0xC6,0xA1,0x86,0x8E,0xC0,0xF9,0xA4,0xB0,0x99,0x89
	};



	unsigned char fnd_num[4];

	fnd_num[0] = FND_DATA_TBL[a];
	fnd_num[1] = FND_DATA_TBL[b];
	fnd_num[2] = FND_DATA_TBL[c];
	fnd_num[3] = FND_DATA_TBL[d];

	fnd_fd = open(fnd_dev, O_RDWR);
	write(fnd_fd, &fnd_num, sizeof(fnd_num));
	sleep(1);
	if (fnd_fd < 0) {
		printf("Can't Open Device\n");
	}
	for(i=5; i>0; i--){
		d=d--;
		fnd_num[3]=FND_DATA_TBL[d];
		write(fnd_fd, &fnd_num, sizeof(fnd_num));
		sleep(1);		
	}
	change_player();
	close(fnd_fd);
}



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
	int count;
	int a=player;
	unsigned char data;
	if(a==0){
		for( count =0 ; count <16; count ++) {
			if( count%2){
				data = 0xF0;
				write(dev, &data, sizeof(unsigned char));
				usleep(10000);
			}
			else{
				data = 0xFF;
				write(dev, &data, sizeof(unsigned char));
				usleep(10000);
			}
		}		
	}
	else{
		for( count =0 ; count <16; count ++) {
			if( count%2){
				data = 0x0F;
				write(dev, &data, sizeof(unsigned char));
				usleep(10000);
			}
			else{
				data = 0xFF;
				write(dev, &data, sizeof(unsigned char));
				usleep(10000);
			}
		}		
	}
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
	while (*p != '\0') p++; // ���ڿ� �� Ž��
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
			if (tact == 0)     //tact switch�� �������� ���� ��츸 open
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
	{ 0x00,0x54,0x00,0x54,0x00,0x54,0x00,0x54 }, // �ʱⰪ 

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


void map1(void) {
	int i;
	for (i = 0; i < 12; i++) {
		qmap[i] = '?';
	}
}//ī�� ������ ������ ���� ī�� �޸����� ����� '?' �迭 


void card_shuffle(void) {
	srand(time(NULL));//������ �����Ҷ����� �ٸ��� ���̵��� �ϱ� ���� srand()�Լ� 
	int i, j, x;
	memset(card_in, 0, sizeof(card_in));
	for (i = 1; i < 7; i++) //1~6 ���ڸ�  
	{
		for (j = 0; j < 2; j++)//�� ���� ī�忡 ���� ���� 
		{
			do
			{
				x = rand() % 12;
			} while (card_in[x] != 0);

			card_in[x] = i;

		}
	}
	sleep(2);
} //12���� ī�忡 1~6�� ���ڸ� �ι��� �� 12�� �־� ī�带 ���� �ڵ�

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
}//�÷��̾�� ī�峪���Ѱ� �����ִ� ��. ó�� ���۽ÿ��� ī�� �޸� '?'�� ������ 

void show_num(int a, int b) {
	c1 = card_in[a] + '0';
	c2 = card_in[b] + '0';
	qmap[a] = c1;
	qmap[b] = c2;
}//�÷��̾ ī�� ¦�� ������ �� '?" �� ����� ī�� ������ ������ 

void reset_check(void) {
	check_card[0] = 0;
	check_card[1] = 0;
	card_select[0] = 0;
	card_select[1] = 0;
} //¦�� ����ų� Ʋ������� ������ ī�带 �ʱ�ȭ�ϴ� �Լ� 

void print_please(void) {
	if (ordernum == 1) {
		printf("\n");
		printf("�ι�° ī�带 ����ּ���:  ");
	}
	else {
		printf("\n");
		printf("ù��° ī�带 ����ּ���: ");
	}//���� ������ �� ���°���� �����ֱ� ���� �Լ� 
}

void print_waiting(void) {
	printf("\n----------------\n");
	printf("  shuffling...");
	printf("\n----------------\n");
	print_lcd("   shuffling.   ");
}//���� ���۽� ī�� ���°�ó�� ������ 



void dot_smile(int right) {
	int i;
	unsigned char c[2][8] = { {0x00,0x66,0x66,0x00,0x00,0x42,0x3c,0x00},
				{0x00,0x66,0x66,0x00,0x00,0x3c,0x42,0x00} };
	dot_d = open(dot, O_RDWR);
	write(dot_d, &c[right], sizeof(c[right]));
	sleep(2);
	dot_d = close(dot_d);
} // dot_matrix�� ���� ǥ��

void checkcard(int a, int b) {
	a = a - 1;//ù��° ī�� 
	b = b - 1;//�ι�° ī��  
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
		printf("¦�� ������ϴ�!\n");
		printf("\n");
		dot_smile(0); // ���� ǥ��
		printf("�÷��̾�%d�� ����: %d", player + 1, answer);
		card_off(a + 1);
		card_off(b + 1);
		show_num(a, b);
		reset_check();//check_card[],card_select[] �ʱ�ȭ 
		card_in[a] = 0;//�̹� ���� ī�带 ������ ���ϵ��� ī�峻���� 0���� ���� 
		card_in[b] = 0;//�̹� ���� ī�带 ������ ���ϵ��� ī�峻���� 0���� ���� 
		show_map();//���� ���� ī�� ��ġ�� ������
		texttext[0]='\0';
		lcd_score1[0]='\0';
		timer=5;
		lcd_score();	
		strcat(texttext,playervs);
		strcat(texttext,lcd_score1);
		print_lcd(texttext);
	}//¦�� ������� ī�带 ������ ���ڸ� �����ְ� check_card[],card_select[] �ʱ�ȭ  
	else {
		printf("\n");
		printf("Ʋ�Ƚ��ϴ� ���ʰ� �Ѿ�ϴ�.\n");
		printf("\n");
		dot_smile(1);
		change_player();//���濡�� ������ �Ѱ��� 
		reset_check();//check_card[],card_select[] �ʱ�ȭ
		led_player(player);
		timer=5;
		printf("�÷��̾�%d�� �����Դϴ�.\n", player + 1);

	}
}//���� �� ���� ī�尡 ¦�� �´��� üũ�ϴ� �Լ� 

void sum_score(void) {
	char a[32]="";
	if (player1_score > player2_score) {
		printf("\n");
		printf("�÷��̾�1�� �¸��Ͽ����ϴ�!");
		printf("\n");
		strcat(a,"congratulations!    ");
		strcat(a,pla1);
		strcat(a," win!!    ");
		print_lcd(a);
		dev=open(led_dev, O_RDWR);
		led_player(0);
		dev=close(dev);
		
	}
	else if (player1_score == player2_score) {
		printf("\n");
		printf("�ƽ����ϴ� �����ϴ�...");
		printf("\n");
		strcat(a,"    too bad.    ");
		strcat(a,"      draw      ");
	}
	else {
		printf("\n");
		printf("�÷��̾�2�� �¸��Ͽ����ϴ�!");
		printf("\n");
		strcat(a,"congratulations!    ");
		strcat(a,pla2);
		strcat(a," win!!    ");
		print_lcd(a);
		dev = open(led_dev, O_RDWR);
		led_player(1);
		dev=close(dev);
	}

}//�� ���� �� ���� ����ϴ� �Լ� 

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
		printf("�̹� ���� ī���Դϴ�.\n");
		if (card_select[0] == 0) {
			ordernum = 0;
		}
		else {
			ordernum = 1;
		}
	}//���� ī�尡 �̹� ¦�� ���� ī�������� üũ�ϴ� ���� �� 
	else {
		if (card_select[0] == 0) {
			check_card[0] = card_in[check - 1];
			card_select[0] = check;
			x = check_card[0];
			printf("\n");
			printf("ù��° ī�� ����: %d", check_card[0]);
			printf("\n");
			ordernum = 1;
			dot_num(x);
		}//ù��° ī�带 ������ �ʾ��� ��� ù��° ī�带  card_select[0]�� ī���ȣ��, check_card[0]�� ī�� �ո��� ���� 
		else {
			if (card_in[check - 1] == 0) {
				ordernum = 1;
				printf("\n");
				printf("�̹� ���� ī���Դϴ�.\n");
			}//���� ī�尡 �̹� ¦�� ���� ī�������� üũ�ϴ� ���� ��
			else {
				check_card[1] = card_in[check - 1];
				card_select[1] = check;
				y = check_card[1];
				printf("\n");
				printf("�ι�° ī�� ����: %d", check_card[1]);
				printf("\n");
				dot_num(y);

				if (card_select[0] == card_select[1]) {
					printf("\n");
					printf("�ߺ��� ī�带 ������ϴ� �ٽ� ��������.\n");
					printf("\n");
					check_card[1] = 0;
					card_select[1] = 0;
				}
			}//ù��°�� �ι�° ī�带 ���� ī�带 ����� ��� �ι�° ī�� ������ ���� check_card[1],card_select[1]�� �ʱ�ȭ�� �ٽ� �������� �� 

			if (card_select[0] != 0 && card_select[1] != 0) {
				printf("\n");
				printf("�ΰ��� ī�带 ������ϴ�\n");
				printf("\n");
				ordernum = 0;
				checkcard(card_select[0], card_select[1]);
			}//���� �ٸ� ī�� �ΰ��� ������� �� ī�带 �� 
		}
	}
}//ī�带 ������ �Լ� 



int main(void) {
	struct timeval dotst, dotend, tactst, tactend, fndst, fndend, ledst, ledend, timest, timeend;
	int tact = 0;
	int i=0;
	unsigned char t = 0;
	unsigned char c;
	unsigned char d;
	
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
	gettimeofday(&timest, NULL);
		
	while (num1 < 6)
	{

		if (dot_d == 0) {
			dot_d = open(dot, O_RDWR);
		}
		gettimeofday(&dotend, NULL);

		write(dot_d, &rps, sizeof(rps));

		if ((dotend.tv_usec - dotst.tv_usec > 100000) || (dotend.tv_sec > dotst.tv_sec && (dotend.tv_usec + 1000000 - dotst.tv_usec > 100000)))
		{
			dot_d = close(dot_d);
			if (tact == 0)     //tact switch�� �������� ���� ��츸 open
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
				if ((tactend.tv_usec - tactst.tv_usec > 100000) || (tactend.tv_sec > tactst.tv_sec && (tactend.tv_usec + 1000000 - tactst.tv_usec > 100000)) || t)
				{
					tact = close(tact);
					
					gettimeofday(&ledst, NULL);
					if(dev==0){
						dev = open(led_dev, O_RDWR);
					}
					while(1){
						led_player(player);
					gettimeofday(&ledend, NULL);
					
					if ((ledend.tv_usec - ledst.tv_usec > 100000) || (ledend.tv_sec > ledst.tv_sec && (ledend.tv_usec + 1000000 - ledst.tv_usec > 100000))){
						dev=close(dev);
						gettimeofday(&fndst, NULL);
						fnd_fd = open(fnd_dev, O_RDWR);
						gettimeofday(&timeend, NULL);
						
						if ((timeend.tv_usec - timest.tv_usec > 1000000)|| (ledend.tv_sec > ledst.tv_sec && (ledend.tv_usec + 1000000 - ledst.tv_usec > 100000))){
							gettimeofday(&timest, NULL);
							timer--;
							printf("%d\n", timer);
							
							if (timer == 0){
								timer = 5;
								change_player();
							}
						}
						
						while(1){
							unsigned char FND_DATA_TBL[] = {
								0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,
								0x83,0xC6,0xA1,0x86,0x8E,0xC0,0xF9,0xA4,0xB0,0x99,0x89
									};
							unsigned char fnd_num[4];

							fnd_num[0] = FND_DATA_TBL[0];
							fnd_num[1] = FND_DATA_TBL[0];
							fnd_num[2] = FND_DATA_TBL[0];
							fnd_num[3] = FND_DATA_TBL[timer];
							write(fnd_fd, &fnd_num, sizeof(fnd_num));
							gettimeofday(&fndend, NULL);
							if ((fndend.tv_usec - fndst.tv_usec > 100000) || (fndend.tv_sec > fndst.tv_sec && (fndend.tv_usec + 1000000 - fndst.tv_usec > 100000))) {
                    					fnd_fd = close(fnd_fd);
                   					 break;
                					}							
						}

           				break;
					}

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
