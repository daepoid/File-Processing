#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma warning (disable: 4996)

#define capacity_order_d 2
#define capacity_order_2d capacity_order_d * 2
#define MAX 100

typedef struct _rec {
	char name[51]; // ���� ���Ͽ� ���ڿ��� ��ü ���̸� 51�� �����Ͽ��µ� �� ���⸸ 50??
	int leng;
} type_rec;

typedef struct node* nodeptr;
typedef struct node {
	nodeptr ptr[capacity_order_2d + 1];
	type_rec rec[capacity_order_2d];
	int fill_cnt;
}node;

typedef struct b_node {
	nodeptr ptr[capacity_order_2d + 2];
	type_rec rec[capacity_order_2d + 1];
}big_node;

typedef struct two_Bn {
	nodeptr ptr[capacity_order_2d * 2 + 1];
	type_rec rec[2 * capacity_order_2d];
}two_Bnode;

void push(nodeptr data);
nodeptr pop();
void B_tree_Insertion(char CompName[51]); // ������ *Root���� int�� ������ ��� �ٲ�
void B_tree_Search(); //
void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char R, int j);
void B_tree_Deletion(); //������ nodeptr�� �־��µ� ���������� ����ϴ°� �� ȥ���� ���� �� �־ �ٲ�
void B_tree_Sequential_print(); // ���忡�� �տ� char sname[51]�� �߰��� �ִµ� �ִ� ������ �𸣰��� ���� ���� int���ε� ������ �ʿ䰡 ���� void�� �ٲ�
void B_tree_Insert();
void SmallPartOfSearch(int level, char sname[51]);
void SmallPartOfDeletion(char dname[51], nodeptr curr);
nodeptr stack[MAX], Root;
int top = -1;

int split = 0;

FILE *fp_w;

/*
void FileToMemory(FILE * fp){

	fp = fopen("Com_names1.txt", "r");
	if (!fp) {
		printf("file open error\n");
		return 0;
	}
	while (1) { // �ߺ��� �����Ͱ� �ְ�, ��緹�ڵ尡 �� �ܾ�� �̷���������� �ʴ�.
		res = fgets(sname, 51, fp);
		if (!res) {
			break;
		}
		clength = strlen(sname);
		if (sname[clength - 1] == '\n') {
			sname[clength - 1] = '\0';
		} // \n�� \0���� �ٲپ��ش�
		B_tree_Insertion(sname, root); // �ߺ����� �ȵ�
									   // ���� leng���� ����Ͽ� �̵���ų �ʿ����. �׳� strlen���� ó��
		top = -1; // ���� ��ġ �ʱ�ȭ
	}
	prinf("file to memory insert clear\n");

}
*/

void main() {

	FILE *fp, *fp2; //  �������� �������ؼ� �Լ��� ���ڸ� ���̴°� �������� ������ ����
	char *res, sname[51], command[2];
	int clength;
	Root = NULL;

	fp = fopen("Com_names1.txt", "r");
	if (!fp) {
		printf("file open error\n");
		return;
	}
	while (1) { // �ߺ��� �����Ͱ� �ְ�, ��緹�ڵ尡 �� �ܾ�� �̷���������� �ʴ�.
		res = fgets(sname, 51, fp);
		if (!res) {
			break;
		}
		clength = strlen(sname);
		if (sname[clength - 1] == '\n') {
			sname[clength - 1] = '\0';
		} // \n�� \0���� �ٲٱ�
		B_tree_Insertion(sname); // �ߺ����� �ȵ�
		// ���� leng���� ����Ͽ� �̵���ų �ʿ����. �׳� strlen���� ó��
		top = -1; // ���� ��ġ �ʱ�ȭ
	}
	printf("file to memory insert clear\n");
	fclose(fp);
	
	fp2 = fopen("Com_names2.txt", "r");
	if (!fp2) {
		printf("file open error\n");
		return;
	}
	while (1) { // �ߺ��� �����Ͱ� �ְ�, ��緹�ڵ尡 �� �ܾ�� �̷���������� �ʴ�.
		res = fgets(sname, 51, fp2);
		if (!res) {
			break;
		}
		clength = strlen(sname);
		if (sname[clength - 1] == '\n') {
			sname[clength - 1] = '\0';
		} // \n�� \0���� �ٲپ��ش�
		B_tree_Insertion(sname); // �ߺ����� �ȵ�
									   // ���� leng���� ����Ͽ� �̵���ų �ʿ����. �׳� strlen���� ó��
		top = -1; // ���� ��ġ �ʱ�ȭ
	}
	printf("file to memory insert clear\n");
	fclose(fp2);

	while (1) {
		printf("Command? ");
		scanf("%s", &command);
		if (strcmp(command, "r") == 0) {
			B_tree_Search();
		}
		else if (strcmp(command, "d") == 0) {
			top = -1;
			B_tree_Deletion();
		}
		else if (strcmp(command, "sp") == 0) {
			fp_w = fopen("SeqData.txt", "w");
			B_tree_Sequential_print(Root);
			fclose(fp_w);
		}
		else if (strcmp(command, "i") == 0) {
			B_tree_Insert();
		}
		else if (strcmp(command, "e") == 0) {
			printf("�����մϴ�.\n");
			exit(1);
		}
		else{
			printf("��ɾ �ٽ� �Է��ϼ���.\n");
		}
	}
	return;
}

void push(nodeptr data) {
	if (top >= MAX) {
		printf("Stack is full.\n");
		return;
	}
	stack[++top] = data;
	return;
}

nodeptr pop() {
	if (top == -1) {
		printf("Stack is empty.\n");
		return NULL;
	}
	return stack[top--];
}

void B_tree_Insertion(char CompName[51]) {

	char in_name[51];
	nodeptr curr, P, tptr, new_Root;
	big_node bnode;
	int i, j, found, finished; // 0�̸� flase�� 1�̸� true��

	strcpy(in_name, CompName);
	if (!Root) {
		Root = (nodeptr)malloc(sizeof(node));
		Root->ptr[0] = Root->ptr[1] = NULL;
		strcpy(Root->rec[0].name, in_name);
		Root->rec[0].leng = strlen(in_name);		
		Root->fill_cnt = 1;
		return;
	}
	found = 0;
	curr = Root;
	do {
		for (i = 0; i < curr->fill_cnt; i++) {
			if (strcmp(in_name, curr->rec[i].name) == 0) {
				found = 1;
				break;
			}
			else if (strcmp(in_name, curr->rec[i].name) < 0) {
				break;
			}
		}
		P = curr->ptr[i];
		if (P) {
			push(curr);
			curr = P;
		}
	} while ((found == 0) && P);
	if (found == 1) {
		printf("%s already exists.\n", in_name);
		return;
	}
	P = NULL;
	finished = 0;
	do {
		// ���ڸ��� ������ ppt������ �� ������ ����
		if (curr->fill_cnt < capacity_order_2d) {
			for (i = 0; i < curr->fill_cnt; i++) {
				if (strcmp(in_name, curr->rec[i].name) < 0) {
					break;
				}
			}
			for (j = curr->fill_cnt; j > i; j--) {
				curr->rec[j] = curr->rec[j - 1];
				curr->ptr[j + 1] = curr->ptr[j];
			}
			strcpy(curr->rec[i].name, in_name);
			curr->rec[i].leng = strlen(in_name);
			curr->ptr[i + 1] = P;
			curr->fill_cnt++;
			finished = 1;
		}
		else {
			split++;
			for (i = 0; i < capacity_order_2d; i++) {
				if (strcmp(in_name, curr->rec[i].name) < 0) {
					break;
				}
			}
			bnode.ptr[0] = curr->ptr[0];
			for (j = 0; j < i; j++) {
				bnode.rec[j] = curr->rec[j];
				strcpy(curr->rec[j].name,""); //curr ��� ���� �����
				curr->rec[j].leng = 0; // curr ��� ���� �����
				bnode.ptr[j + 1] = curr->ptr[j + 1];
			}
			strcpy(bnode.rec[j].name, in_name);
			bnode.rec[j].leng = strlen(in_name);
			bnode.ptr[j + 1] = P;
			j++;
			for (; i < capacity_order_2d;i++, j++) {
				bnode.rec[j] = curr->rec[i];
				strcpy(curr->rec[i].name, ""); //curr ��� ���� �����
				curr->rec[i].leng = 0; // curr ��� ���� �����
				bnode.ptr[j + 1] = curr->ptr[i + 1];
			}
			for (i = 0; i < capacity_order_d; i++) {
				curr->ptr[i] = bnode.ptr[i];
				curr->rec[i] = bnode.rec[i];
			}
			curr->ptr[i] = bnode.ptr[i];
			curr->fill_cnt = capacity_order_d;
			tptr = (nodeptr)malloc(sizeof(node));
			for (i = 0; i < capacity_order_d; i++) {
				tptr->ptr[i] = bnode.ptr[i + 1 + capacity_order_d];
				tptr->rec[i] = bnode.rec[i + 1 + capacity_order_d];
			}
			tptr->ptr[i] = bnode.ptr[capacity_order_2d + 1];
			tptr->fill_cnt = capacity_order_d;
			strcpy(in_name, bnode.rec[capacity_order_d].name);
			P = tptr;
			if (top >= 0) {
				curr = pop();
			}
			else { // curr �� root �� ����. ���� �� root node �� ����.
				new_Root = (nodeptr)malloc(sizeof(node));
				new_Root->ptr[0] = Root;
				new_Root->ptr[1] = P;
				strcpy(new_Root->rec[0].name, in_name);
				new_Root->rec[0].leng = strlen(in_name); // �̺κ� Ȯ��
				new_Root->fill_cnt = 1;
				Root = new_Root;
				return;
			}
		} // end of else
	} while (finished == 0);
}//end of function

void B_tree_Search() {

	// �ۼ� sp�� �̿��Ͽ� ��������� �ۼ�
	int length, level, i;
	char sname[51];

	gets(sname);
	length = strlen(sname);
	for (i = 0; i < length; i++) { // �տ� ���� ����
		sname[i] = sname[i + 1];
	}

	level = 0;
	SmallPartOfSearch(level, sname, Root);
}

void SmallPartOfSearch(int level, char sname[51], nodeptr root) {

	nodeptr curr = root, P, father;
	int i, j, found = 0;

	level++;
	for (i = 0; i < curr->fill_cnt; i++) {
		if (strcmp(sname, curr->rec[i].name) < 0) {
			break;
		}
		else if (strcmp(sname, curr->rec[i].name) == 0) {
			found = 1;
			break;
		}
	}
	if (found == 1) {
		;	
	}
	else {
		P = curr->ptr[i];
		if (P) {
			push(curr);
			curr = P;
			SmallPartOfSearch(level, sname, curr);
			return;
		}
	}
	if (found == 0) {
		printf("%s�� Ʈ���� �������� �ʽ��ϴ�.\n", sname);
		return;
	}

	if (curr == Root) {
		printf("%s�� %dlevel, %d��°(1~4) record.\n", sname, level, i + 1);
	}
	else {
		father = pop();
		for (j = 0; j <= father->fill_cnt; j++) {
			if (father->ptr[j] == curr) {
				break;
			}
		}
		if (j == father->fill_cnt) {
			printf("%s�� %s�� �������ڽ�, %dlevel, %d��°(1~4) record.\n", sname, father->rec[j - 1].name, level, i + 1);
		}
		else {
			printf("%s�� %s�� �����ڽ�, %dlevel, %d��°(1~4) record.\n", sname, father->rec[j].name, level, i + 1);
		}
	}
}

void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char side, int j) {

	int i, k, h, n, m; // k : twoB�� curr�� �ֱ� ���� ����, h :  �߰���ġ, n, m : ��й踦 ���� ����
	two_Bnode twoB;
	int c_father; // ������

	if (side == 'L') { // curr�� ������ ��� �踦 �ϴ����� ���� j�� ���� �ʿ� <-Ȯ��
		c_father = j - 1;
	}
	else {
		c_father = j;
	}
	for (i = 0; i < l_sibling->fill_cnt; i++) {
		twoB.ptr[i] = l_sibling->ptr[i];
		twoB.rec[i] = l_sibling->rec[i];		
	}
	twoB.ptr[i] = l_sibling->ptr[i];
	twoB.rec[i] = father->rec[c_father];
	//twoB.rec[i] = father->rec[j];
	i++;
	for (k = 0; k < r_sibling->fill_cnt; k++, i++) {
		twoB.ptr[i] = r_sibling->ptr[k];
		twoB.rec[i] = r_sibling->rec[k];
	}
	twoB.ptr[i] = r_sibling->ptr[k];
	h = i / 2;  //h is the position of middle.

	for (n = 0; n < h; n++) {
		l_sibling->ptr[n] = twoB.ptr[n];
		l_sibling->rec[n] = twoB.rec[n];		
	}
	l_sibling->ptr[n] = twoB.ptr[n];
	l_sibling->fill_cnt = h;
	n++;
	for (m = 0; m < (i - h - 1); m++, n++) {
		r_sibling->ptr[m] = twoB.ptr[n];
		r_sibling->rec[m] = twoB.rec[n];		
	}
	r_sibling->ptr[m] = twoB.ptr[n];
	r_sibling->fill_cnt = i - h - 1;
	father->rec[c_father] = twoB.rec[h];
}

void B_tree_Deletion() {

	int length, d_space, i, j;
	int l_OK, r_OK, found, finished;
	int c_redistribution, c_merging, c_father;
	int tmp_1, tmp_2;
	char out_record[51], dname[51];
	nodeptr curr, P, Pt, father;
	nodeptr l_sibling, r_sibling, leftptr, rightptr;

	gets(dname);
	length = strlen(dname);
	for (d_space = 0; d_space < length; d_space++) { // �տ� ���� ����
		dname[d_space] = dname[d_space + 1];
	}

	strcpy(out_record, dname);
	curr = Root;
	found = 0;

	do { // �̺κ� ��������� �Ϸ��� ���ο� �Լ��� ����� �����ؾ��ؼ� �� ���������Ű���
		for (i = 0; i < curr->fill_cnt; i++) {
			if (strcmp(out_record, curr->rec[i].name) == 0) {
				found = 1;
				break;
			}
			else if (strcmp(out_record, curr->rec[i].name) < 0) {
				break;
			}
		}
		if (found == 1) {
			break;
		}
		else {
			P = curr->ptr[i];
			if (P) {
				push(curr);
				curr = P;
			}
			else {
				break;
			}
		}
	} while (found == 0);

	if (found == 0) {
		printf("%s to delete does not exist.\n", dname);
		return;
	}

	if (curr->ptr[0]) {
		Pt = curr->ptr[i + 1];
		push(curr);
		while (Pt->ptr[0]) {
			push(Pt);
			Pt = Pt->ptr[0];
		}
		curr->rec[i] = Pt->rec[0];
		curr = Pt;
		strcpy(out_record, Pt->rec[0].name);
		i = 0;
	}

	//balancing
	finished = 0;
	c_redistribution = 0;
	c_merging = 0;
	do {
		for (j = i + 1; j < curr->fill_cnt; j++) {
			curr->rec[j - 1] = curr->rec[j];
			curr->ptr[j] = curr->ptr[j + 1];
		}
		curr->fill_cnt = curr->fill_cnt - 1;
		if (curr == Root) {
			if (curr->fill_cnt == 0) {
				Root = Root->ptr[0];
			}
			finished = 1;
			break;
		}
		if (curr->fill_cnt >= capacity_order_d) {
			finished = 1;
			break;
		}
		father = pop();
		for (j = 0; j <= father->fill_cnt; j++) {
			if (father->ptr[j] == curr) {
				break;
			}
		}
		if (j >= 1) {
			l_sibling = father->ptr[j - 1];
		}
		else {
			l_sibling = NULL;
		}
		if (j < father->fill_cnt) {
			r_sibling = father->ptr[j + 1];
		}
		else {
			r_sibling = NULL;
		}
		l_OK = r_OK = 0;
		if (r_sibling && r_sibling->fill_cnt > capacity_order_d) {
			r_OK = 1;
		}
		else if (l_sibling && l_sibling->fill_cnt > capacity_order_d) {
			l_OK = 1;
		}
		if (r_OK || l_OK) { // ��й谡 ������
			c_redistribution++;
			if (r_OK) {
				// ��й� �Լ� ���				
				redistribution(father, curr, r_sibling, 'R', j);
			}
			else {
				redistribution(father, l_sibling, curr, 'L', j);
			}
			finished = 1;
			break;
		}
		else {//merging
			c_merging++;
			if (r_sibling) {
				c_father = j;
				leftptr = curr;
				rightptr = r_sibling;
			}
			else if (l_sibling) {
				c_father = j - 1;
				leftptr = l_sibling;
				rightptr = curr;
			}
		}
		//copy(append) intermediate key in father to leftptr's node;
		tmp_1 = leftptr->fill_cnt;
		leftptr->rec[tmp_1++] = father->rec[c_father];
		//copy content of rightptr's node into leftptr's node;
		for (tmp_2 = 0; tmp_2 < rightptr->fill_cnt; tmp_1++, tmp_2++) {
			leftptr->ptr[tmp_1] = rightptr->ptr[tmp_2];
			leftptr->rec[tmp_1] = rightptr->rec[tmp_2];			
		}
		leftptr->ptr[tmp_1] = rightptr->ptr[tmp_2];
		leftptr->fill_cnt = tmp_1;
		//disallocate rightptr's node;
		strcpy(out_record, father->rec[c_father].name);
		curr = father;
		i = c_father;

	} while (finished == 0);
	printf("%s �����Ϸ�. redistribution�� : %d merging�� : %d\n", dname, c_redistribution, c_merging);
}

void B_tree_Sequential_print(nodeptr root) { // ������ �̿����� �ʰ� ��������� ����� �ϴ� ���
	int i;

	if (root) {
		for (i = 0; i < root->fill_cnt; i++) {
			B_tree_Sequential_print(root->ptr[i]);
			fprintf(fp_w, "%s\n", root->rec[i].name);
		}
		B_tree_Sequential_print(root->ptr[i]);
	}
	return;
}

void B_tree_Insert() {

	char iname[51]; // �Է��� �̸�
	int length;
	int i;

	gets(iname);
	//gets_s(iname, sizeof(iname));
	length = strlen(iname);
	for (i = 0; i < length; i++) {
		iname[i] = iname[i + 1];
	}

	split = 0;
	B_tree_Insertion(iname);
	printf("split�� �� : %d\n", split);
}