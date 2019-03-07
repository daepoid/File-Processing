#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<time.h>
#define	Tbl_Size 37533
#pragma warning(disable:4996)

typedef struct str_ty_record* str_ty_record_ptr;
typedef struct str_ty_record { //링크가 하나라서 일반으로
	char name[100];
	int monincome;
	int link;
} type_record; 
//문제에서 5개의 필드를 가지는 레코드로 작성이라고 되어있는데 필드는 3개밖에 없음

type_record table[Tbl_Size];

//hash_table 구현
int hash(char recname[]);
void make_hash_table(); // hash table 만들기
int find_empty_loc(char name[]); //빈자리 찾기
int R = Tbl_Size; // Tbl_Size 인지 Tbl_Size + 1인지 확인 해보기

//hash_table 기능  - ppt참고
int insert_hash_table(char recname[]);//삽입
void retrieval(char name[]); //검색
void del_hash(char name[]); // 삭제
void del_start(int delete_record_position);
void del_middle(int s, int before_s);
int moves = 0; //전역으로 선언하여 함수에서 확인 하지 않고 밖에서 확인
void del_hash_enbloc(); // 일괄 삭제
void average_probe(); //성능측정 - 평균 probe 수

int main() {
	int i, address;
	char command, name[100];

	/*
	삽입작업: uns.txt 화일 에서 이름을 하나씩을 읽어서 해시테이블에 삽입한다.
	이름의 중간에 blank 가 들어 있을 수 있으므로 fgets 를 이용하여 화일에서 이름을 읽어야 한다.
	이 이름를 레코드의 name 필드에 넣는다 (그리고 레코드의 monincome 필드도 랜덤 넘버로 채운다). 
	그리고 이 레코드를 해시테이블에 삽입한다.	
	*/

	for (i = 0; i < Tbl_Size; i++) {
		strcpy(table[i].name, "");
	}
	make_hash_table();
	while (1) {
		printf("명령은? ");
		scanf("%s", &command);
		switch (command) {
		case 'i':
			/*
			상호명을 하나 입력받아서 이를 hash table 에 삽입한다. 삽입위치를 출력한다.
			동일한 키가 이미 존재하면 삽입실패를 출력한다.
			*/
			gets(name);
			for (i = 0; i < strlen(name); i++) {
				name[i] = name[i + 1];
			}
			strtok(name, "\n");
			R = Tbl_Size;
			address = insert_hash_table(name);
			if (address == -1) {
				; 
			}
			else {
				printf("%d\n", address);
			}
			break;
		case 'r':
			/*
			상호명을 받아서 이를 탐색한다. 저장위치 및 탐색과정에서의 probe 수 를 출력한다;
			(주: Hash table 에서 하나의 위치를 조사하는 행위를 하면 한번의 probe 를 한 것이 된다.)
			*/
			gets(name);
			for (i = 0; i < strlen(name); i++) {
				name[i] = name[i + 1];
			}
			strtok(name, "\n");
			retrieval(name);
			break;
		case 'd':
			/*
			상호명을 받아서 이를 찾아서 삭제한다. 삭제 중에 발생한 record move 횟수를 출력한다.
			*/
			gets(name);
			for (i = 0; i < strlen(name); i++) {
				name[i] = name[i + 1];
			}
			strtok(name, "\n");
			moves = 0;
			del_hash(name);
			printf("%d\n", moves);
			break;
		case 't':
			/*
			일괄삭제. 화일 uns.txt 의 처음 30,000 개의 상호명을 차례로 읽어서 이를 삭제하는 작업을
			수행한다.이 과정에서 일어난 총 move 수를 누적하여 출력한다.
			*/
			moves = 0;
			del_hash_enbloc();
			printf("총 move 수 = %d\n", moves);
			break;
		case 'v':
			/*
			화일 uns.txt 의 처음부터 하나씩 이름을 읽어서 탐색을 수행한다.
			탐색과정에서 발생한 모든 probe 의 수의 총합을 구한다(total_probe).
			그리고 total_probe 를 탐색한 모든 이름의 수(num_names)로 나누어 구한
			탐색당 평균 프로브수 ( average_probe) 을 출력한다.
			주: 실수가 나오게 해야 함. 즉 average_probe = double(total_probe) / num_names ;
			*/
			average_probe();
			break;
		case 'e':
			return 0;
		default:
			printf("명령어를 다시 입력하세요.\n");
			break;
		}
	}
}
//hash_table 구현
void make_hash_table() {

	FILE *fp, *fw;
	char *res, name[100];
	int line_count = 0;

	fp = fopen("uns.txt", "r");
	if (!fp) {
		printf("File open error\n");
		return;
	}
	fw = fopen("uns_30000.txt", "w");
	/*
	30000개를 일괄삭제하는 부분에서 터지는 경우가 있어 30000개 까지의 레코드를 따로 저장 해 놓음
	*/
	srand(time(NULL));

	while (1) {
		res = fgets(name, 100, fp);
		if (!res) {
			break;
		}
		if (line_count < 30000) {
			fputs(name, fw);
			line_count++;
		}
		strtok(name, "\n");		
		insert_hash_table(name);
	}
	fclose(fw);
	fclose(fp);
}

int insert_hash_table(char name[]) {
	int monincome, HA, address;
	monincome = (rand() % 10000000) + (rand() % 1000);
	HA = hash(name);
	if (strcmp(table[HA].name, "") == 0) {
		//해쉬 테이블 안에 내용이 비어있는 경우
		strcpy(table[HA].name, name);
		table[HA].monincome = monincome;
		table[HA].link = -1;
		return HA;
	}
	else {
		if (strcmp(table[HA].name, name) == 0) {
			address = -2;
		}
		else {
			address = find_empty_loc(name);
		}
		if (address == -1) {
			printf("Hash table full\n");
			return -1;
		}
		else if (address == -2) {
			printf("already saved record\n");
			return -1;
		}
		strcpy(table[address].name, name);
		table[address].monincome = monincome;
		table[address].link = -1;
		while (table[HA].link != -1) {
			HA = table[HA].link;
		}
		table[HA].link = address;
		return address;
	}
}
// ppt 자료
int find_empty_loc(char name[]) {
	do {
		if (R < 0) {
			return -1;
		}
		if (strcmp(table[R].name, name) == 0) {
			return -2;
		}
		R--;
		if (strcmp(table[R].name, "") == 0) { // 확인/////////////////////////////////////
			return R;
		}
	} while (1);
}
// 문제에서 제공된 hash 함수
int hash(char recname[]) {
	unsigned char u; 
	int HA, j, leng, halfleng;
	long sum = 0;
	int A[100];
	// name 의 글자 j 에 대하여 다음을 수행한다
	leng = strlen(recname);
	for (j = 0; j < leng; j++) {
		u = recname[j];
		A[j] = u;
	}
	halfleng = leng / 2;
	for (j = 0; j < halfleng; j++)
		sum = sum + (A[j] * A[leng - 1 - j] * A[(leng - 1) / 2]);
	if (leng % 2 == 1)
		sum = sum + A[halfleng] * A[halfleng + 1] * A[(leng - 1) / 2];
	HA = sum % Tbl_Size;  // HA is a home address given by the hash function.
	return HA;
}

//hash_table 기능
void retrieval(char name[]) { // 확인 요망 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	상호명을 받아서 이를 탐색한다.저장위치 및 탐색과정에서의 probe 수 를 출력한다;
	(주: Hash table 에서 하나의 위치를 조사하는 행위를 하면 한번의 probe 를 한 것이 된다.)
	*/
	int HA, probe = 1;

	HA = hash(name);
	while (HA != -1) {
		if (strcmp(table[HA].name, name) == 0) {
			printf("저장된 주소 : %d, probe수 : %d\n", HA, probe);
			return;
		}
		else {
			HA = table[HA].link;
			probe++;
		}
	}
	printf("찾을 수 없음\n");
}

void del_hash(char name[]) {
	/*
	상호명을 받아서 이를 찾아서 삭제한다. 삭제 중에 발생한 record move 횟수를 출력한다. 
	*/
	int HA, curr, next;
	HA = hash(name);
	curr = HA;
	next = table[HA].link;
	if (strcmp(table[HA].name, name) == 0) {
		del_start(HA);
	}
	else {
		while (next != -1) {
			if (strcmp(table[next].name, name) == 0) {
				break;
			}
			else {
				curr = next;
				next = table[next].link;
			}
		}
		if (next == -1) {
			//printf("존재하지 않는 레코드\n");
			return;
		}
		else {
			del_middle(next, curr);
		}
	}
}

void del_start(int s) {

	int next, curr; 
	/*
	 - next
	s위치의 link가 가리키는 노드에서부터 그 이하로 
	체인에서 HA가 s인 맨 나중노드를 찾아야함

	 - curr
	 next의 선행자의 위치 저장용
	*/
	int found = -1, before_found; 
	/*
	 - found
	HA가 s인 맨 나중의 노드

	 - before_found
	 found의 선행자의 위치 저장용	
	 dummy_record를 넣으면 삭제에서 에러가 발생
	*/


	curr = s;
	next = table[s].link;
	while (next != -1) {
		if (s == hash(table[next].name)) {
			found = next;
			before_found = curr;
		}
		curr = next;
		next = table[next].link;
	}

	if (found == -1) {
		/*
		체인의 아래에 위치 s를 이용하는 노드가 없다.
		따라서 체인에서 위치 s룰 제거해도 무방하다.	
		위치 s를 체인에서 제거한다.
		방법: 위치 s에 dummy record와 포인터 NULL을 넣음
		*/
		strcpy(table[s].name, "");
		table[s].monincome = -1;
		table[s].link = -1;
	}
	else {
		/*
		발견된 위치가 found, 그 위치의 선행자의 위치가 before_found라 하자
		*/
		strcpy(table[s].name, table[found].name);
		table[s].monincome = table[found].monincome;
		moves++;
		del_middle(found, before_found);
	}
}

void del_middle(int s, int before_s) {
	int next, curr, HA, i, j;
	/*
	- next
	s위치의 link가 가리키는 노드에서부터 그 이하로
	체인에서 HA가 s인 맨 나중노드를 찾아야함

	- curr
	next의 선행자의 위치 저장용
	*/
	int found = -1, before_found;
	/*
	- found
	HA가 s인 맨 나중의 노드

	- before_found
	found의 선행자의 위치 저장용
	*/	
	int D[Tbl_Size], location, except_D, before_except_D,
		last_except_D = -1, last_before_except_D;
	/*
	 - D[Tbl_Size]
	 각 위치를 저장하기 위한 집합

	 - location
	위치 s부터 시작하여 체인의 각 노드를 따라 가면서
	각 위치에 대한 정보를 D에 저장하기 전에 임시로 저장하는 변수

	 - except_D
	D에 속하지 않는 노드의 위치를 저장하는 용도

	 - before_except_D
	except_D의 선행자의 위치 저장용

	 - last_except_D
	 D에 속하지 않는 마지막 노드의 위치

	 - last_before_except_D
	 D에 속하지 않는 마지막 노드의 선행자
	*/
	//dummy_record로 처리하면 에러 발생
	curr = s;
	next = table[s].link;
	while (next != -1) {
		if (s == hash(table[next].name)) {
			found = next;
			before_found = curr;
		}
		curr = next;
		next = table[next].link;
	} //del_start와 같음

	if (found == -1) {
		/*
		위치 s를 체인으로부터 제거한다.
		위치 s가 가진 link의 값을 위치 p의 link에 넣는다.
		위치 s에 dummy_record와 NULL을 집어넣는다.
		*/
		table[before_s].link = table[s].link;
		strcpy(table[s].name, "");
		table[s].monincome = -1;
		table[s].link = -1;
	}
	else {
		/*
		위치 s를 제거하면 아래에 검새기 불가능해지는 레코드가
		있으므로 이 위치를 제거하는 것은 안된다.

		위치 s부터 시작하여 체인의 각 노드를 따라 가면서
		각 위치를 집합 D(s)에 수집한다. (주의 : s도 포함시킴)
		*/
		location = s;
		i = 0;
		/*
		for (i = 0; location != -1; i++) {//집합 D에 내용 채우기
			D[i] = location;
			location = table[location].link;
			i++;
		}
		*/
		while (location != -1) { //집합 D에 내용 채우기
			D[i++] = location;
			location = table[location].link;
		}
		/*
		위치 s 의 다음 노드부터 체인의 모든 노드에 대하여 그것의
		HA 가 "D(s) 에 속하지 않는(즉 D'(s)에 속하는)"
		가장 마지막 노드를 찾는다
		(만약, 발견되면 그 노드의 위치를 except_D, 선행자 위치를 before_except_D 라 함)
		*/
		except_D = table[s].link;
		before_except_D = s;
		while (except_D != -1) {
			HA = hash(table[except_D].name);
			for (j = 0; j < i; j++) {
				if (D[j] == HA)
					break;
			}
			if (j == i) {
				last_except_D = except_D;
				last_before_except_D = before_except_D;
			}
			before_except_D = except_D;
			except_D = table[except_D].link;
		}
		if (last_except_D != -1) {
			/*
			위치 r 의 레코드를 위치 s 로 올려줌.
			위치 r 의 레코드의 HA 는 체인에서 위치 s 보다 앞의
			노드들의 위치 중 하나이다. 따라서 이 레코드를 위치
			s 로 옮겨도 전혀 문제가 되지 않는다.
			위치 r 에 저장된 레코드를 위치 s 로 옮긴다
			위치 r 을 중간에서 제거하는 작업을 하도록 함수 호출.
			*/
			strcpy(table[s].name, table[last_except_D].name);
			table[s].monincome = table[last_except_D].monincome;
			moves++;
			del_middle(last_except_D, last_before_except_D);
		}
		else {
			table[before_s].link = -1;
			del_start(s);
		}
	}
}

void del_hash_enbloc() {
	/*
	일괄삭제. 화일 uns.txt 의 처음 30,000 개의 상호명을 차례로 읽어서 이를 삭제하는 작업을 수행한다. 이 과정에서 일어난 총 move 수를 누적하여 출력한다.
	*/
	FILE *fp2;
	char *res2, name[100];
	fp2 = fopen("uns_30000.txt", "r");
	while (1) {
		res2 = fgets(name, 100, fp2);
		if (!res2) {
			break;
		}
		strtok(name, "\n");
		del_hash(name);
	}
	fclose(fp2);
}

void average_probe() {
	/*
	화일 uns.txt 의 처음부터 하나씩 이름을 읽어서 탐색을 수행한다.
	탐색과정에서 발생한 모든 probe 의 수의 총합을 구한다(total_probe).
	그리고 total_probe 를 탐색한 모든 이름의 수(num_names)로 나누어 구한
	탐색당 평균 프로브수 (average_probe) 을 출력한다.
	주: 실수가 나오게 해야 함. 즉 average_probe = double(total_probe) / num_names ;
	*/
	FILE *fp;
	char *res, name[100];
	int HA, num_names = 0, probe = -1, total_probe = 0;
	double average_probe;
	fp = fopen("uns.txt", "r");
	if (!fp) {
		printf("error\n");
		return;
	}
	while (1) {
		res = fgets(name, 100, fp);
		if (!res) {
			break;
		}
		strtok(name, "\n");
		HA = hash(name);
		while (HA != -1) {
			if (strcmp(table[HA].name, name) == 0) {
				num_names++;
				total_probe += probe;
				break;
			}
			else {
				HA = table[HA].link;
				probe++;
			}
		}
		if (HA == -1) {
			printf("삭제된 레코드 %s\n", name);
		}
		probe = 1;
	}
	fclose(fp);
	average_probe = (double)total_probe / num_names;
	printf("average_prove : %lf\n", average_probe);
}