#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 정의
#define INSERT			0			// type 설정 0  => 프로세스 생성 (큐에 프로세스 삽입)
#define SCHEDULE		1			// type 설정 1  => 스케줄링
#define END				-1			// type 설정 -1 => 종료


// 구조체 정의
typedef struct priority_queue *queue_pointer;
typedef struct queue_head *head_pointer;

// 구조체 선언
struct priority_queue	{
	int				process_id;			// 프로세스 ID
	int				priority;			// 우선순위
	int				computing_time;		// 처리되는데 걸리는 시간
	int				start_time;			// 프로그램이 queue에 들어간 시점
	int				end_time;			// 프로그램이 스케줄링 완료된 시점
	queue_pointer	left_link;			
	queue_pointer	right_link;
};
struct queue_head	{
	queue_pointer	left_link;
	queue_pointer	right_link;
};

// 함수 정의
void initialize_queue(void);											// 큐 초기화
int create_queue(int priority, int process_id ,int computing_time);		// 큐에 프로세스 삽입
int schedule_queue(void);												// 프로세스 스케줄링
int end_queue(void);													// 입력완료된 후 나머지 프로세스 스케줄링후 출력
void print_queue(queue_pointer queue);									// 큐 출력
void PrintError(char *msg);												// 에러메세지 출력

// 전역 변수 
head_pointer	queue;				// 큐
int				process_time;		// 프로세스의 현재 시점
int				cnt;				// normalized average turn_around_time과 average turn_around_time을 위한 카운트 변수
double			plus_trn_time;		// average turn_around_time을 위한 turn_around_time 계속 더해주는 변수
double			plus_n_trn_time;	// normalized average turn_around_time을 위한 turn_around_time 계속 더해주는 변수


int main(void)	{
	FILE	*fp;			// 파일 포인터
	int ret;				// 리턴값
	int type = 0, process_id = 0, priority = 0, computing_time = 0;
		
	fp = fopen("input_file.txt","rt");
	if(fp == NULL)	{
		PrintError("fopen 실패");
		exit(0);
	}

	// 큐 초기화 실행
	initialize_queue();

	printf(" --------------------------------------------------------------------------\n");
	printf(" id \t priority \t computing_time \t trn_time \t n_trn_time \n");
	printf(" --------------------------------------------------------------------------\n");
	while(1)	{
		// input_file.txt로부터 type, priority, computing_time을 받아옴
		fscanf(fp,"%d	%d	%d	%d", &type, &process_id, &priority, &computing_time);
		
		// type 값을 통해 INSERT , SCHEDULE, END 진행
		switch(type)	{
			case INSERT:	// 프로세스 삽입	( 0 )
				create_queue(priority, process_id, computing_time);
				break;
			case SCHEDULE:	// 스케쥴			( 1 )
				schedule_queue();
				break;
			case END:		// 입력완료			( -1 )
				end_queue();
				break;
		}
		if(type == END)		break;
	}	 
	ret = fclose(fp);
	if(ret != 0)
		PrintError("fclose 실패");

	return 0;
}


void initialize_queue(void)	{
	// 큐 초기화
	queue = (head_pointer)malloc(sizeof(struct queue_head));
	queue->left_link = queue->right_link = NULL;
}

int create_queue(int priority, int process_id,int computing_time)	{
	// 큐에 프로세스 삽입
	queue_pointer new_node = (queue_pointer)malloc(sizeof(struct priority_queue));	// 새로 추가될 노드 && 메모리할당
	if(new_node == NULL)
		PrintError("메모리 할당 에러");
	new_node->left_link = new_node->right_link = NULL;
	new_node->process_id = process_id;
	new_node->priority = priority;
	new_node->computing_time = computing_time;
	new_node->start_time = process_time;

	if( queue->right_link == NULL )	{	//  첫 노드일때
		queue->right_link = new_node;
		return 0;
	}

	queue_pointer temp;					// 위치찾기용 
	temp = queue->right_link;

	while(temp != NULL)		{
	// 첫노드가 아닐때
		if(temp->computing_time <= computing_time && temp->right_link == NULL)	{
			// temp->computing_time < new_node->computing_time 그리고 temp->right_link가 NULL 
			temp->right_link = new_node;
			new_node->left_link = temp;
			break;
		}
		else if(temp->computing_time > computing_time && temp->left_link == NULL)	{
			// temp->computing_time > new_node->computing_time 그리고 temp->left_link가 NULL 
			queue->right_link = new_node;
			new_node->right_link = temp;
			temp->left_link = new_node;
			break;
		}
		else if(temp->computing_time <= computing_time)
			temp = temp->right_link;
		else
		{
			// 그 외의 중간노드삽입
			temp = temp->left_link;
			new_node->right_link = temp->right_link;
			new_node->left_link = temp; 
			temp->right_link->left_link = new_node;
			temp->right_link = new_node;
			break;
		}
	}
	

	return 0;
}
int schedule_queue(void)	{
	// 스케쥴링
	queue_pointer temp;

	temp = queue->right_link;

	process_time += temp->computing_time;
	temp->end_time = process_time;
	print_queue(temp);

	//--------- 스케줄링 된 노드 삭제----------
	if(temp->right_link == NULL)
		//맨 처음or마지막 노드일때
		queue->right_link = NULL;
	else	{
		// 그외 노드
		queue->right_link = temp->right_link;
		queue->right_link->left_link = NULL;
	}
	//----------------------------------------

	return 0;
}
int end_queue(void)	{
	// 입력 다 종료되었을때 나머지 프로세스들 스케줄링
	queue_pointer temp;
	temp = queue->right_link;

	while( temp != NULL )	{
		// 모든 프로세스가 처리될때까지 작업계속
		process_time += temp->computing_time;
		temp->end_time = process_time;
		print_queue( temp );		
	
		//--------- 스케줄링 된 노드 삭제----------
		if(temp->right_link == NULL)
			//맨 처음or마지막 노드일때
			queue->right_link = NULL;
		else	{
			// 그외 노드
			queue->right_link = temp->right_link;
			queue->right_link->left_link = NULL;
		}
		//----------------------------------------
		temp=queue->right_link;
	}
	printf(" --------------------------------------------------------------------------\n");
	printf("  cf) \n");
	printf("  - trn_time   : turn_around_time \n");
	printf("  - n_trn_time : normalized_turn_around_time \n");
	printf(" --------------------------------------------------------------------------\n");
	printf(" > Average turn around time            : %0.2lf \n", plus_trn_time/cnt );				// Average Turn a round Time 출력
	printf(" > Normalized average turn around time : %0.2lf \n", plus_n_trn_time/cnt );				// Normalized Average Turn a round Time 출력

	printf("\n\n");
	return 0;
}
void print_queue( queue_pointer temp )	{
	// 큐 출력
	int trn_time = 0;		// 턴어라운드 타임
	double n_trn_time = 0;	//노말라이즈드 턴어라운드 타임

	trn_time = temp->end_time - temp->start_time;
	n_trn_time = (double)trn_time / temp->computing_time;
	printf(" %d \t %d \t\t %d \t\t\t %d \t\t %0.2lf \n", temp->process_id, temp->priority, temp->computing_time, trn_time, n_trn_time);
	
	cnt++;
	plus_trn_time += trn_time;
	plus_n_trn_time += n_trn_time;
}
void PrintError(char *msg)	{
	// 에러메세지 출력
	printf("> [ERROR] %s \n",msg);
	exit(0);
}