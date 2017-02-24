#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ����
#define INSERT			0			// type ���� 0  => ���μ��� ���� (ť�� ���μ��� ����)
#define SCHEDULE		1			// type ���� 1  => �����ٸ�
#define END				-1			// type ���� -1 => ����


// ����ü ����
typedef struct priority_queue *queue_pointer;
typedef struct queue_head *head_pointer;

// ����ü ����
struct priority_queue	{
	int				process_id;			// ���μ��� ID
	int				priority;			// �켱����
	int				computing_time;		// ó���Ǵµ� �ɸ��� �ð�
	int				start_time;			// ���α׷��� queue�� �� ����
	int				end_time;			// ���α׷��� �����ٸ� �Ϸ�� ����
	queue_pointer	left_link;			
	queue_pointer	right_link;
};
struct queue_head	{
	queue_pointer	left_link;
	queue_pointer	right_link;
};

// �Լ� ����
void initialize_queue(void);											// ť �ʱ�ȭ
int create_queue(int priority, int process_id ,int computing_time);		// ť�� ���μ��� ����
int schedule_queue(void);												// ���μ��� �����ٸ�
int end_queue(void);													// �Է¿Ϸ�� �� ������ ���μ��� �����ٸ��� ���
void print_queue(queue_pointer queue);									// ť ���
void PrintError(char *msg);												// �����޼��� ���

// ���� ���� 
head_pointer	queue;				// ť
int				process_time;		// ���μ����� ���� ����
int				cnt;				// normalized average turn_around_time�� average turn_around_time�� ���� ī��Ʈ ����
double			plus_trn_time;		// average turn_around_time�� ���� turn_around_time ��� �����ִ� ����
double			plus_n_trn_time;	// normalized average turn_around_time�� ���� turn_around_time ��� �����ִ� ����


int main(void)	{
	FILE	*fp;			// ���� ������
	int ret;				// ���ϰ�
	int type = 0, process_id = 0, priority = 0, computing_time = 0;
		
	fp = fopen("input_file.txt","rt");
	if(fp == NULL)	{
		PrintError("fopen ����");
		exit(0);
	}

	// ť �ʱ�ȭ ����
	initialize_queue();

	printf(" --------------------------------------------------------------------------\n");
	printf(" id \t priority \t computing_time \t trn_time \t n_trn_time \n");
	printf(" --------------------------------------------------------------------------\n");
	while(1)	{
		// input_file.txt�κ��� type, priority, computing_time�� �޾ƿ�
		fscanf(fp,"%d	%d	%d	%d", &type, &process_id, &priority, &computing_time);
		
		// type ���� ���� INSERT , SCHEDULE, END ����
		switch(type)	{
			case INSERT:	// ���μ��� ����	( 0 )
				create_queue(priority, process_id, computing_time);
				break;
			case SCHEDULE:	// ������			( 1 )
				schedule_queue();
				break;
			case END:		// �Է¿Ϸ�			( -1 )
				end_queue();
				break;
		}
		if(type == END)		break;
	}	 
	ret = fclose(fp);
	if(ret != 0)
		PrintError("fclose ����");

	return 0;
}


void initialize_queue(void)	{
	// ť �ʱ�ȭ
	queue = (head_pointer)malloc(sizeof(struct queue_head));
	queue->left_link = queue->right_link = NULL;
}

int create_queue(int priority, int process_id,int computing_time)	{
	// ť�� ���μ��� ����
	queue_pointer new_node = (queue_pointer)malloc(sizeof(struct priority_queue));	// ���� �߰��� ��� && �޸��Ҵ�
	if(new_node == NULL)
		PrintError("�޸� �Ҵ� ����");
	new_node->left_link = new_node->right_link = NULL;
	new_node->process_id = process_id;
	new_node->priority = priority;
	new_node->computing_time = computing_time;
	new_node->start_time = process_time;

	if( queue->right_link == NULL )	{	//  ù ����϶�
		queue->right_link = new_node;
		return 0;
	}

	queue_pointer temp;					// ��ġã��� 
	temp = queue->right_link;

	while(temp != NULL)		{
	// ù��尡 �ƴҶ�
		if(temp->computing_time <= computing_time && temp->right_link == NULL)	{
			// temp->computing_time < new_node->computing_time �׸��� temp->right_link�� NULL 
			temp->right_link = new_node;
			new_node->left_link = temp;
			break;
		}
		else if(temp->computing_time > computing_time && temp->left_link == NULL)	{
			// temp->computing_time > new_node->computing_time �׸��� temp->left_link�� NULL 
			queue->right_link = new_node;
			new_node->right_link = temp;
			temp->left_link = new_node;
			break;
		}
		else if(temp->computing_time <= computing_time)
			temp = temp->right_link;
		else
		{
			// �� ���� �߰�������
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
	// �����층
	queue_pointer temp;

	temp = queue->right_link;

	process_time += temp->computing_time;
	temp->end_time = process_time;
	print_queue(temp);

	//--------- �����ٸ� �� ��� ����----------
	if(temp->right_link == NULL)
		//�� ó��or������ ����϶�
		queue->right_link = NULL;
	else	{
		// �׿� ���
		queue->right_link = temp->right_link;
		queue->right_link->left_link = NULL;
	}
	//----------------------------------------

	return 0;
}
int end_queue(void)	{
	// �Է� �� ����Ǿ����� ������ ���μ����� �����ٸ�
	queue_pointer temp;
	temp = queue->right_link;

	while( temp != NULL )	{
		// ��� ���μ����� ó���ɶ����� �۾����
		process_time += temp->computing_time;
		temp->end_time = process_time;
		print_queue( temp );		
	
		//--------- �����ٸ� �� ��� ����----------
		if(temp->right_link == NULL)
			//�� ó��or������ ����϶�
			queue->right_link = NULL;
		else	{
			// �׿� ���
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
	printf(" > Average turn around time            : %0.2lf \n", plus_trn_time/cnt );				// Average Turn a round Time ���
	printf(" > Normalized average turn around time : %0.2lf \n", plus_n_trn_time/cnt );				// Normalized Average Turn a round Time ���

	printf("\n\n");
	return 0;
}
void print_queue( queue_pointer temp )	{
	// ť ���
	int trn_time = 0;		// �Ͼ���� Ÿ��
	double n_trn_time = 0;	//�븻������� �Ͼ���� Ÿ��

	trn_time = temp->end_time - temp->start_time;
	n_trn_time = (double)trn_time / temp->computing_time;
	printf(" %d \t %d \t\t %d \t\t\t %d \t\t %0.2lf \n", temp->process_id, temp->priority, temp->computing_time, trn_time, n_trn_time);
	
	cnt++;
	plus_trn_time += trn_time;
	plus_n_trn_time += n_trn_time;
}
void PrintError(char *msg)	{
	// �����޼��� ���
	printf("> [ERROR] %s \n",msg);
	exit(0);
}