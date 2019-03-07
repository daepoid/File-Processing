#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma warning (disable: 4996)

#define capacity_order_d 2
#define capacity_order_2d capacity_order_d * 2
#define MAX 100

typedef struct _rec {
	char name[51]; // 워드 파일에 문자열의 전체 길이를 51로 규정하였는데 왜 여기만 50??
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
void B_tree_Insertion(char CompName[51]); // 원래는 *Root였음 int일 이유가 없어서 바꿈
void B_tree_Search(); //
void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char R, int j);
void B_tree_Deletion(); //원래는 nodeptr가 있었는데 전역변수로 사용하는게 더 혼돈을 줄일 수 있어서 바꿈
void B_tree_Sequential_print(); // 워드에는 앞에 char sname[51]이 추가로 있는데 있는 이유를 모르겠음 리턴 값도 int형인데 리턴할 필요가 없어 void로 바꿈
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
	while (1) { // 중복된 데이터가 있고, 모든레코드가 한 단어로 이루어져있지는 않다.
		res = fgets(sname, 51, fp);
		if (!res) {
			break;
		}
		clength = strlen(sname);
		if (sname[clength - 1] == '\n') {
			sname[clength - 1] = '\0';
		} // \n을 \0으로 바꾸어준다
		B_tree_Insertion(sname, root); // 중복저장 안됨
									   // 굳이 leng값을 계속하여 이동시킬 필요없음. 그냥 strlen으로 처리
		top = -1; // 스택 위치 초기화
	}
	prinf("file to memory insert clear\n");

}
*/

void main() {

	FILE *fp, *fp2; //  전역으로 선언하해서 함수의 인자를 줄이는게 나을수도 있을거 같음
	char *res, sname[51], command[2];
	int clength;
	Root = NULL;

	fp = fopen("Com_names1.txt", "r");
	if (!fp) {
		printf("file open error\n");
		return;
	}
	while (1) { // 중복된 데이터가 있고, 모든레코드가 한 단어로 이루어져있지는 않다.
		res = fgets(sname, 51, fp);
		if (!res) {
			break;
		}
		clength = strlen(sname);
		if (sname[clength - 1] == '\n') {
			sname[clength - 1] = '\0';
		} // \n을 \0으로 바꾸기
		B_tree_Insertion(sname); // 중복저장 안됨
		// 굳이 leng값을 계속하여 이동시킬 필요없음. 그냥 strlen으로 처리
		top = -1; // 스택 위치 초기화
	}
	printf("file to memory insert clear\n");
	fclose(fp);
	
	fp2 = fopen("Com_names2.txt", "r");
	if (!fp2) {
		printf("file open error\n");
		return;
	}
	while (1) { // 중복된 데이터가 있고, 모든레코드가 한 단어로 이루어져있지는 않다.
		res = fgets(sname, 51, fp2);
		if (!res) {
			break;
		}
		clength = strlen(sname);
		if (sname[clength - 1] == '\n') {
			sname[clength - 1] = '\0';
		} // \n을 \0으로 바꾸어준다
		B_tree_Insertion(sname); // 중복저장 안됨
									   // 굳이 leng값을 계속하여 이동시킬 필요없음. 그냥 strlen으로 처리
		top = -1; // 스택 위치 초기화
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
			printf("종료합니다.\n");
			exit(1);
		}
		else{
			printf("명령어를 다시 입력하세요.\n");
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
	int i, j, found, finished; // 0이면 flase고 1이면 true임

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
		// 이자리에 교수님 ppt에서는 한 문장이 있음
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
				strcpy(curr->rec[j].name,""); //curr 노드 내용 지우기
				curr->rec[j].leng = 0; // curr 노드 내용 지우기
				bnode.ptr[j + 1] = curr->ptr[j + 1];
			}
			strcpy(bnode.rec[j].name, in_name);
			bnode.rec[j].leng = strlen(in_name);
			bnode.ptr[j + 1] = P;
			j++;
			for (; i < capacity_order_2d;i++, j++) {
				bnode.rec[j] = curr->rec[i];
				strcpy(curr->rec[i].name, ""); //curr 노드 내용 지우기
				curr->rec[i].leng = 0; // curr 노드 내용 지우기
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
			else { // curr 은 root 와 같음. 따라서 새 root node 를 만듬.
				new_Root = (nodeptr)malloc(sizeof(node));
				new_Root->ptr[0] = Root;
				new_Root->ptr[1] = P;
				strcpy(new_Root->rec[0].name, in_name);
				new_Root->rec[0].leng = strlen(in_name); // 이부분 확인
				new_Root->fill_cnt = 1;
				Root = new_Root;
				return;
			}
		} // end of else
	} while (finished == 0);
}//end of function

void B_tree_Search() {

	// 작성 sp를 이용하여 재귀적으로 작성
	int length, level, i;
	char sname[51];

	gets(sname);
	length = strlen(sname);
	for (i = 0; i < length; i++) { // 앞에 공백 제거
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
		printf("%s가 트리에 존재하지 않습니다.\n", sname);
		return;
	}

	if (curr == Root) {
		printf("%s는 %dlevel, %d번째(1~4) record.\n", sname, level, i + 1);
	}
	else {
		father = pop();
		for (j = 0; j <= father->fill_cnt; j++) {
			if (father->ptr[j] == curr) {
				break;
			}
		}
		if (j == father->fill_cnt) {
			printf("%s는 %s의 오른쪽자식, %dlevel, %d번째(1~4) record.\n", sname, father->rec[j - 1].name, level, i + 1);
		}
		else {
			printf("%s는 %s의 왼쪽자식, %dlevel, %d번째(1~4) record.\n", sname, father->rec[j].name, level, i + 1);
		}
	}
}

void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char side, int j) {

	int i, k, h, n, m; // k : twoB에 curr을 넣기 위한 변수, h :  중간위치, n, m : 재분배를 위한 변수
	two_Bnode twoB;
	int c_father; // 조정용

	if (side == 'L') { // curr가 누구와 재분 배를 하는지에 따라 j의 조정 필요 <-확인
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
	for (d_space = 0; d_space < length; d_space++) { // 앞에 공백 제거
		dname[d_space] = dname[d_space + 1];
	}

	strcpy(out_record, dname);
	curr = Root;
	found = 0;

	do { // 이부분 재귀적으로 하려면 새로운 함수를 만들고 리턴해야해서 더 복잡해질거같음
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
		if (r_OK || l_OK) { // 재분배가 가능함
			c_redistribution++;
			if (r_OK) {
				// 재분배 함수 사용				
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
	printf("%s 삭제완료. redistribution수 : %d merging수 : %d\n", dname, c_redistribution, c_merging);
}

void B_tree_Sequential_print(nodeptr root) { // 스택을 이용하지 않고 재귀적으로 출력을 하는 방법
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

	char iname[51]; // 입력할 이름
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
	printf("split의 수 : %d\n", split);
}