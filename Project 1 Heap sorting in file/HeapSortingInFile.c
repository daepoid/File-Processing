#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

#define MAX_SIZE 100
#define SWAP(a,b) {rectype tmp; tmp = a; a=b; b=tmp;} // ��ũ�η� ����
#define SWAP2(a,b){rectype temp; temp = *a; *a = *b; *b = temp;}

typedef struct arec *recptr;
typedef struct arec {
	char Company[MAX_SIZE];
	int monincome;
} rectype;

void step1();
void step2();
void step3();
void SwapInFile(FILE *fp, int a, int b);
void adjust_file(FILE *fp, int a, int b);
void adjust(rectype a[], int root, int n);
int isCountLineNum = 0;

int main() {

	clock_t beg = clock();
	step1(); // ù ��° ����
	printf("\n---------------------STEP1 �Ϸ�------------------------\n");
	step2(); // �� ��° ����
	printf("\n---------------------STEP2 �Ϸ�------------------------\n");
	step3(); // �� ��° ���� 
	printf("\n---------------------STEP3 �Ϸ�------------------------\n");
	clock_t end = clock();
	long msec = end - beg;
	printf("%d\n", msec);
	return 0;
}

void step1() {

	int len, monincome;
	char Text_File_Name[100], company[100];
	rectype comp;
	FILE *fp_r, *fp_w;

	srand((unsigned)time(NULL));
	printf("�Է� �ؽ�Ʈ ȭ�ϸ���?");
	scanf("%s", Text_File_Name);

	fp_r = fopen(Text_File_Name, "r");

	if (!fp_r) {
		printf("���� ���� ����.\n");
		return;
	}
	fp_w = fopen("Companies_binary.bin", "wb");

	while (1) {

		if (fgets(company, 100, fp_r) == NULL) { // ���� �����߿� company�� ������ �̿��Ͽ� �� �ܾ� �̻����� �̷���� �־� 
			break;
		}
		len = strlen(company);
		if (company[len - 1] == '\n') {
			company[len - 1] = '\0';
		}
		isCountLineNum++;
		strcpy(comp.Company, company);
		monincome = rand() % 20000001;
		comp.monincome = monincome;

		fwrite(&comp, sizeof(rectype), 1, fp_w);
	}
	fclose(fp_r);
	fclose(fp_w);
} // step1 �Ϸ�

void step2() {
	int i, j;
	FILE* fp, *fp_s;
	recptr Rec = (recptr)malloc((isCountLineNum + 1) * sizeof(rectype));

	fp = fopen(".\\Companies_binary.bin", "rb");
	fp_s = fopen("Companies_sort_mem.txt", "w");

	fread(Rec + 1, sizeof(rectype), isCountLineNum + 1, fp);

	for (i = (isCountLineNum / 2); i > 0; i--) {
		adjust(Rec, i, isCountLineNum);
	}
	// �� ���·� �����
	for (i = (isCountLineNum - 1); i > 0; i--) {
		SWAP2(&Rec[1], &Rec[i + 1]);
		adjust(Rec, 1, i);
	}
	// Heap Sorting
	for (j = 1; j < (isCountLineNum + 1); j++) {
		fprintf(fp_s, "%s %d\n", Rec[j].Company, Rec[j].monincome);
	}
	// ���Ͽ� ����ֱ�
	fclose(fp);
	fclose(fp_s);
}

void adjust(rectype a[], int root, int n) { //������ ppt �ڷ� ����
	rectype temp;
	int child;
	char key[100];

	temp = a[root];
	strcpy(key, a[root].Company);
	child = 2 * root;

	while (child <= n) {
		if ((child < n) && (strcmp(a[child].Company, a[child + 1].Company) < 0)) {
			child++;
		}
		if (strcmp(key, a[child].Company) > 0) {
			break;
		}
		else {
			a[child / 2] = a[child];
			child = child * 2;
		}
	}
	a[child / 2] = temp;
}

void step3() {

	FILE *fp, *fp_w;
	int pos, i;
	rectype temp2;

	fp = fopen("Companies_binary.bin", "rb+");
	fp_w = fopen("Companies_sorting_file.txt", "w");

	for (pos = (isCountLineNum / 2); pos > 0; pos--) {
		adjust_file(fp, pos, isCountLineNum);
	}
	// bin ���Ͽ��� ������ �� ���·� ����

	for (pos = (isCountLineNum - 1); pos > 0; pos--) {
		SwapInFile(fp, 1, pos + 1);
		adjust_file(fp, 1, pos);
	} // Heap Sorting in File

	fseek(fp, 0 * sizeof(rectype), SEEK_SET);

	for (i = 0; i < isCountLineNum; i++) {
		fread(&temp2, sizeof(rectype), 1, fp);
		fprintf(fp_w, "%s %d\n", temp2.Company, temp2.monincome);
	} // bin���Ͽ� ���ĵ� ������ txt ���Ͽ� ����

	fclose(fp);
	fclose(fp_w);
}

void adjust_file(FILE *fp, int k, int n) { // ������ ppt���� ����
	rectype tmp;
	int isChildPos, check;
	char key[100];
	rectype child1, child2;


	fseek(fp, (k - 1) * sizeof(rectype), SEEK_SET);
	fread(&tmp, sizeof(rectype), 1, fp); //root�� ���� ��������

	strcpy(key, tmp.Company);
	isChildPos = k * 2;

	while (isChildPos < n + 1) {
		check = 0;
		fseek(fp, (isChildPos - 1)*sizeof(rectype), SEEK_SET);
		fread(&child1, sizeof(rectype), 1, fp);
		if (isChildPos < n) {
			fread(&child2, sizeof(rectype), 1, fp); //child+1�� ���� ��������
			if (strcmp(child1.Company, child2.Company) < 0) {
				isChildPos++;
				check = 1;
			}
		}
		if (check == 0) {
			if (strcmp(key, child1.Company) > 0)
				break;
			else {
				fseek(fp, ((isChildPos / 2) - 1)*sizeof(rectype), SEEK_SET); //child/2�� �� ��ġ ã��
				fwrite(&child1, sizeof(rectype), 1, fp);
				isChildPos = isChildPos * 2;
			}
		}
		else {
			if (strcmp(key, child2.Company) > 0)
				break;
			else {
				fseek(fp, ((isChildPos / 2) - 1)*sizeof(rectype), SEEK_SET); //child/2�� �� ��ġ ã��
				fwrite(&child2, sizeof(rectype), 1, fp);
				isChildPos = isChildPos * 2;
			}
		}
	}
	fseek(fp, (isChildPos / 2 - 1)*sizeof(rectype), SEEK_SET); // while �� child/2�� �� ��ġ ã��
	fwrite(&tmp, sizeof(rectype), 1, fp);
}

void SwapInFile(FILE *fp, int a, int b) { // ���Ͽ��� ���� swap

	rectype temp, temp2;

	fseek(fp, (a - 1)*sizeof(rectype), SEEK_SET);
	fread(&temp, sizeof(rectype), 1, fp);
	fseek(fp, (b - 1)*sizeof(rectype), SEEK_SET);
	fread(&temp2, sizeof(rectype), 1, fp);
	fseek(fp, (a - 1)*sizeof(rectype), SEEK_SET);
	fwrite(&temp2, sizeof(rectype), 1, fp);
	fseek(fp, (b - 1)*sizeof(rectype), SEEK_SET);
	fwrite(&temp, sizeof(rectype), 1, fp);
}