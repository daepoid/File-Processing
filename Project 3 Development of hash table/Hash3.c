#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<time.h>
#define	Tbl_Size 37533
#pragma warning(disable:4996)

typedef struct str_ty_record* str_ty_record_ptr;
typedef struct str_ty_record { //��ũ�� �ϳ��� �Ϲ�����
	char name[100];
	int monincome;
	int link;
} type_record; 
//�������� 5���� �ʵ带 ������ ���ڵ�� �ۼ��̶�� �Ǿ��ִµ� �ʵ�� 3���ۿ� ����

type_record table[Tbl_Size];

//hash_table ����
int hash(char recname[]);
void make_hash_table(); // hash table �����
int find_empty_loc(char name[]); //���ڸ� ã��
int R = Tbl_Size; // Tbl_Size ���� Tbl_Size + 1���� Ȯ�� �غ���

//hash_table ���  - ppt����
int insert_hash_table(char recname[]);//����
void retrieval(char name[]); //�˻�
void del_hash(char name[]); // ����
void del_start(int delete_record_position);
void del_middle(int s, int before_s);
int moves = 0; //�������� �����Ͽ� �Լ����� Ȯ�� ���� �ʰ� �ۿ��� Ȯ��
void del_hash_enbloc(); // �ϰ� ����
void average_probe(); //�������� - ��� probe ��

int main() {
	int i, address;
	char command, name[100];

	/*
	�����۾�: uns.txt ȭ�� ���� �̸��� �ϳ����� �о �ؽ����̺� �����Ѵ�.
	�̸��� �߰��� blank �� ��� ���� �� �����Ƿ� fgets �� �̿��Ͽ� ȭ�Ͽ��� �̸��� �о�� �Ѵ�.
	�� �̸��� ���ڵ��� name �ʵ忡 �ִ´� (�׸��� ���ڵ��� monincome �ʵ嵵 ���� �ѹ��� ä���). 
	�׸��� �� ���ڵ带 �ؽ����̺� �����Ѵ�.	
	*/

	for (i = 0; i < Tbl_Size; i++) {
		strcpy(table[i].name, "");
	}
	make_hash_table();
	while (1) {
		printf("�����? ");
		scanf("%s", &command);
		switch (command) {
		case 'i':
			/*
			��ȣ���� �ϳ� �Է¹޾Ƽ� �̸� hash table �� �����Ѵ�. ������ġ�� ����Ѵ�.
			������ Ű�� �̹� �����ϸ� ���Խ��и� ����Ѵ�.
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
			��ȣ���� �޾Ƽ� �̸� Ž���Ѵ�. ������ġ �� Ž������������ probe �� �� ����Ѵ�;
			(��: Hash table ���� �ϳ��� ��ġ�� �����ϴ� ������ �ϸ� �ѹ��� probe �� �� ���� �ȴ�.)
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
			��ȣ���� �޾Ƽ� �̸� ã�Ƽ� �����Ѵ�. ���� �߿� �߻��� record move Ƚ���� ����Ѵ�.
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
			�ϰ�����. ȭ�� uns.txt �� ó�� 30,000 ���� ��ȣ���� ���ʷ� �о �̸� �����ϴ� �۾���
			�����Ѵ�.�� �������� �Ͼ �� move ���� �����Ͽ� ����Ѵ�.
			*/
			moves = 0;
			del_hash_enbloc();
			printf("�� move �� = %d\n", moves);
			break;
		case 'v':
			/*
			ȭ�� uns.txt �� ó������ �ϳ��� �̸��� �о Ž���� �����Ѵ�.
			Ž���������� �߻��� ��� probe �� ���� ������ ���Ѵ�(total_probe).
			�׸��� total_probe �� Ž���� ��� �̸��� ��(num_names)�� ������ ����
			Ž���� ��� ���κ�� ( average_probe) �� ����Ѵ�.
			��: �Ǽ��� ������ �ؾ� ��. �� average_probe = double(total_probe) / num_names ;
			*/
			average_probe();
			break;
		case 'e':
			return 0;
		default:
			printf("��ɾ �ٽ� �Է��ϼ���.\n");
			break;
		}
	}
}
//hash_table ����
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
	30000���� �ϰ������ϴ� �κп��� ������ ��찡 �־� 30000�� ������ ���ڵ带 ���� ���� �� ����
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
		//�ؽ� ���̺� �ȿ� ������ ����ִ� ���
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
// ppt �ڷ�
int find_empty_loc(char name[]) {
	do {
		if (R < 0) {
			return -1;
		}
		if (strcmp(table[R].name, name) == 0) {
			return -2;
		}
		R--;
		if (strcmp(table[R].name, "") == 0) { // Ȯ��/////////////////////////////////////
			return R;
		}
	} while (1);
}
// �������� ������ hash �Լ�
int hash(char recname[]) {
	unsigned char u; 
	int HA, j, leng, halfleng;
	long sum = 0;
	int A[100];
	// name �� ���� j �� ���Ͽ� ������ �����Ѵ�
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

//hash_table ���
void retrieval(char name[]) { // Ȯ�� ��� ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	��ȣ���� �޾Ƽ� �̸� Ž���Ѵ�.������ġ �� Ž������������ probe �� �� ����Ѵ�;
	(��: Hash table ���� �ϳ��� ��ġ�� �����ϴ� ������ �ϸ� �ѹ��� probe �� �� ���� �ȴ�.)
	*/
	int HA, probe = 1;

	HA = hash(name);
	while (HA != -1) {
		if (strcmp(table[HA].name, name) == 0) {
			printf("����� �ּ� : %d, probe�� : %d\n", HA, probe);
			return;
		}
		else {
			HA = table[HA].link;
			probe++;
		}
	}
	printf("ã�� �� ����\n");
}

void del_hash(char name[]) {
	/*
	��ȣ���� �޾Ƽ� �̸� ã�Ƽ� �����Ѵ�. ���� �߿� �߻��� record move Ƚ���� ����Ѵ�. 
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
			//printf("�������� �ʴ� ���ڵ�\n");
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
	s��ġ�� link�� ����Ű�� ��忡������ �� ���Ϸ� 
	ü�ο��� HA�� s�� �� ���߳�带 ã�ƾ���

	 - curr
	 next�� �������� ��ġ �����
	*/
	int found = -1, before_found; 
	/*
	 - found
	HA�� s�� �� ������ ���

	 - before_found
	 found�� �������� ��ġ �����	
	 dummy_record�� ������ �������� ������ �߻�
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
		ü���� �Ʒ��� ��ġ s�� �̿��ϴ� ��尡 ����.
		���� ü�ο��� ��ġ s�� �����ص� �����ϴ�.	
		��ġ s�� ü�ο��� �����Ѵ�.
		���: ��ġ s�� dummy record�� ������ NULL�� ����
		*/
		strcpy(table[s].name, "");
		table[s].monincome = -1;
		table[s].link = -1;
	}
	else {
		/*
		�߰ߵ� ��ġ�� found, �� ��ġ�� �������� ��ġ�� before_found�� ����
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
	s��ġ�� link�� ����Ű�� ��忡������ �� ���Ϸ�
	ü�ο��� HA�� s�� �� ���߳�带 ã�ƾ���

	- curr
	next�� �������� ��ġ �����
	*/
	int found = -1, before_found;
	/*
	- found
	HA�� s�� �� ������ ���

	- before_found
	found�� �������� ��ġ �����
	*/	
	int D[Tbl_Size], location, except_D, before_except_D,
		last_except_D = -1, last_before_except_D;
	/*
	 - D[Tbl_Size]
	 �� ��ġ�� �����ϱ� ���� ����

	 - location
	��ġ s���� �����Ͽ� ü���� �� ��带 ���� ���鼭
	�� ��ġ�� ���� ������ D�� �����ϱ� ���� �ӽ÷� �����ϴ� ����

	 - except_D
	D�� ������ �ʴ� ����� ��ġ�� �����ϴ� �뵵

	 - before_except_D
	except_D�� �������� ��ġ �����

	 - last_except_D
	 D�� ������ �ʴ� ������ ����� ��ġ

	 - last_before_except_D
	 D�� ������ �ʴ� ������ ����� ������
	*/
	//dummy_record�� ó���ϸ� ���� �߻�
	curr = s;
	next = table[s].link;
	while (next != -1) {
		if (s == hash(table[next].name)) {
			found = next;
			before_found = curr;
		}
		curr = next;
		next = table[next].link;
	} //del_start�� ����

	if (found == -1) {
		/*
		��ġ s�� ü�����κ��� �����Ѵ�.
		��ġ s�� ���� link�� ���� ��ġ p�� link�� �ִ´�.
		��ġ s�� dummy_record�� NULL�� ����ִ´�.
		*/
		table[before_s].link = table[s].link;
		strcpy(table[s].name, "");
		table[s].monincome = -1;
		table[s].link = -1;
	}
	else {
		/*
		��ġ s�� �����ϸ� �Ʒ��� �˻��� �Ұ��������� ���ڵ尡
		�����Ƿ� �� ��ġ�� �����ϴ� ���� �ȵȴ�.

		��ġ s���� �����Ͽ� ü���� �� ��带 ���� ���鼭
		�� ��ġ�� ���� D(s)�� �����Ѵ�. (���� : s�� ���Խ�Ŵ)
		*/
		location = s;
		i = 0;
		/*
		for (i = 0; location != -1; i++) {//���� D�� ���� ä���
			D[i] = location;
			location = table[location].link;
			i++;
		}
		*/
		while (location != -1) { //���� D�� ���� ä���
			D[i++] = location;
			location = table[location].link;
		}
		/*
		��ġ s �� ���� ������ ü���� ��� ��忡 ���Ͽ� �װ���
		HA �� "D(s) �� ������ �ʴ�(�� D'(s)�� ���ϴ�)"
		���� ������ ��带 ã�´�
		(����, �߰ߵǸ� �� ����� ��ġ�� except_D, ������ ��ġ�� before_except_D �� ��)
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
			��ġ r �� ���ڵ带 ��ġ s �� �÷���.
			��ġ r �� ���ڵ��� HA �� ü�ο��� ��ġ s ���� ����
			������ ��ġ �� �ϳ��̴�. ���� �� ���ڵ带 ��ġ
			s �� �Űܵ� ���� ������ ���� �ʴ´�.
			��ġ r �� ����� ���ڵ带 ��ġ s �� �ű��
			��ġ r �� �߰����� �����ϴ� �۾��� �ϵ��� �Լ� ȣ��.
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
	�ϰ�����. ȭ�� uns.txt �� ó�� 30,000 ���� ��ȣ���� ���ʷ� �о �̸� �����ϴ� �۾��� �����Ѵ�. �� �������� �Ͼ �� move ���� �����Ͽ� ����Ѵ�.
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
	ȭ�� uns.txt �� ó������ �ϳ��� �̸��� �о Ž���� �����Ѵ�.
	Ž���������� �߻��� ��� probe �� ���� ������ ���Ѵ�(total_probe).
	�׸��� total_probe �� Ž���� ��� �̸��� ��(num_names)�� ������ ����
	Ž���� ��� ���κ�� (average_probe) �� ����Ѵ�.
	��: �Ǽ��� ������ �ؾ� ��. �� average_probe = double(total_probe) / num_names ;
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
			printf("������ ���ڵ� %s\n", name);
		}
		probe = 1;
	}
	fclose(fp);
	average_probe = (double)total_probe / num_names;
	printf("average_prove : %lf\n", average_probe);
}