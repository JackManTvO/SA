#include<iostream>
#include<ctime>
using namespace std;

enum goodsstate { ship, crane, buffer, shuttle, warehouse };
enum shipstate { pre, ing, post };
enum poptype { pmax, pmin };

class Cgoods
{
public:
	int time;
	int shipno;
	int goodsno;
	int lifeline[3];							//��ۡ����������ʱ���
	int warehouseno;
	goodsstate state;//����״̬

};

class Cship
{
public:
	int shipno;
	Cgoods sgoods[6];
	int intime, outtime;
	int goodsqtt;
	int lifeline[2];							//��ۡ�����ʱ���
	shipstate state;//����״̬
};

template<class T>
struct cmp_fgoods
{
	bool operator() (T x, T y)
	{
		return x.time > y.time;					//С�����ȼ���
	}
};

int shuttletime[5];
template<class T>
struct cmp_bgoods
{
	bool operator() (T x, T y)
	{
		return shuttletime[x.warehouseno] > shuttletime[y.warehouseno];		//С�����ȼ���
	}
};

template<class T>
struct cmp_ship
{
	bool operator() (T x, T y)
	{
		return x.intime + x.outtime > y.intime + y.outtime;			//С�����ȼ���
	}
};


template<class T>
struct Node //���
{		
	T data;
	Node* next;
};

template<class T, typename fcmp>
class Cqueue				//�Զ�������
{
private:
	Node<T>* head;			//ͷָ�룬dataΪ��
	int size;
public:
	Cqueue ();
	~Cqueue ();
	void push (T a);
	T min ();
	T max ();
	bool empty ();
	void pop (poptype t);
	void swap (int i, int j);
};

template<class T, typename fcmp>
Cqueue<T, fcmp>::Cqueue ()
{
	head = new Node<T>;
	head->next = NULL;              //������ͷ��������
	size = 0;
}

//ɾ����㣬�ͷ��ڴ�
template<class T, typename fcmp>
Cqueue<T, fcmp>::~Cqueue ()
{
	Node<T>* p;		//��ʱ���ָ��,����Ҫ�ͷŵĽ��
	size = 0;
	while (head)
	{
		p = head;
		head = head->next;        //ͨ����p���浱ǰҪ�ͷŵĽ��,Ȼ����head�����ƶ������ͷ�pָ��Ŀռ�,ѭ����˴ﵽ��ȫ�ͷ�
		delete p;
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::push (T a)
{
	Node<T>* p = head;
	Node<T>* s = new Node<T>;
	fcmp cmp;
	s->data = a;
	s->next = NULL;
	if (size == 0)
		head->next = s;
	else
	{
		while (p->next != NULL)
		{
			bool go = cmp (a, p->next->data);
			if (go)
				p = p->next;		//��λ��i-1��λ��
			else
				break;
		}
		s->next = p->next;
		p->next = s;
	}
	size++;
}

template<class T, typename fcmp>
T Cqueue<T, fcmp>::min ()//���п�ʼ
{
	T s ;
	s = head->next->data;
	return s;
}

template<class T, typename fcmp>
T Cqueue<T, fcmp>::max ()//����ĩβ
{
	Node<T>* p = head;
	while (p->next!=NULL)
	{
		p = p->next;		
	}
	T s;
	s = p->data;
	return s;
}

template<class T, typename fcmp>
bool Cqueue<T, fcmp>::empty ()
{
	if (size == 0)
		return true;
	else
		return false;
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::pop (poptype t)
{
	if (t == pmax)
	{
		Node<T>* p = head;
		while (p->next->next != NULL)
		{
			p = p->next;
		}
		Node<T>* s = p->next;     //����Ҫ�ͷŵĽ��
		p->next = NULL;
		delete s;
		size--;
	}
	else
	{
		Node<T>* p = head->next;
		head->next = p->next;
		delete p;
		size--;
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::swap (int i, int j)
{
	//�ֱ��ҵ�����������ǰ��
	Node<T>* node1, * node2;
	Node<T>* prenode1, * prenode2;
	Node<T>* p = head;
	int i0 = 0;
	while (i0 < i || i0 < j)
	{
		if (i0 == i - 1)
		{
			prenode1 = p;
			node1 = p->next;
		}
		if (i0 == j - 1)
		{
			prenode2 = p;
			node2 = p->next;
		}
		i0++;
		p = p->next;
	}
	//�����������
	if (i + 1 == j)		//���1�ڽ��2��ǰ��
	{
		Node<T>* postnode2 = node2->next;
		prenode1->next = node2;
		node2->next = node1;
		node1->next = postnode2;
		return;
	}
	if (j + 1 == i)
	{
		Node<T>* postnode1 = node2->next;
		prenode2->next = node1;
		node1->next = node2;
		node2->next = postnode1;
		return;
	}
	Node<T>* postnode1 = node2->next;
	Node<T>* postnode2 = node2->next;
	//һ�����ֱ�ӽ����������
	prenode1->next = node2;
	node2->next = postnode1;
	prenode2->next = node1;
	node1->next = postnode2;
}

int totaltime;
int shipqtt, berthqtt, craneqtt, shuttleqtt, warehouseqtt;		//���֡���λ��������С�����ֿ�����
Cgoods craninggoods[5], shuttlinggoods[5];
Cship parkingship[5];
int freeberth, freecrane, freeshuttle;
Cqueue<Cgoods,  cmp_fgoods<Cgoods>> freegoods;					//���������
Cqueue<Cgoods,  cmp_bgoods<Cgoods>> buffergoods;				//����������
Cqueue<Cship,  cmp_ship<Cship>> freeship;						//��������

int main ()
{
	clock_t start, finish;
	double runtime;
	cout << "̰���㷨2:" << endl;
	cout << "��λ����С�����������С�������ͣ��ɴ�С" << endl;
	cout << "���֡���λ��������С�����ֿ�����" << endl;
	cin >> shipqtt >> berthqtt >> craneqtt >> shuttleqtt >> warehouseqtt;
	memset (shuttletime, 0, 5);
	for (int i = 0; i < warehouseqtt; i++)
	{
		cout << "�ֿ�" << i << "ʱ��" << endl;
		cin >> shuttletime[i];
	}
	for (int i = 0; i < shipqtt; i++)							//��������
	{
		Cship temp;
		cout << "����" << i + 1 << "������ʱ��" << endl;
		temp.shipno = i;
		cin >> temp.intime >> temp.outtime;						//ʻ�롢ʻ��ʱ��
		int goodsqtt;
		cout << "������" << endl;
		cin >> goodsqtt;										//��������
		temp.goodsqtt = goodsqtt;
		temp.state = pre;
		memset (temp.lifeline, 0, 2);
		cout << goodsqtt << "�����ʱ��Ͳֿ���" << endl;
		for (int j = 0; j < goodsqtt; j++)
		{
			cin >> temp.sgoods[j].time >> temp.sgoods[j].warehouseno;	//ÿ����������ʱ��
			temp.sgoods[j].shipno = i;
			temp.sgoods[j].goodsno = j;
			temp.sgoods[j].state = ship;
			memset (temp.sgoods[j].lifeline, 0, 3);
		}
		freeship.push (temp);
	}
	start = clock ();//��ʼʱ��
	freeberth = berthqtt;
	freecrane = craneqtt;
	freeshuttle = shuttleqtt;
	while (freeberth < berthqtt || freecrane < craneqtt || freeshuttle < shuttleqtt || !buffergoods.empty () || !freegoods.empty () || !freeship.empty ())		//���޻���ʱֹͣ
	{
		/*��λ*/
		/*�ڲ�*/
	OUT:
		for (int i = 0; i < berthqtt - freeberth; i++)
		{
			if (parkingship[i].lifeline[0] + parkingship[i].intime == totaltime)					//�벴���
			{
				parkingship[i].state = ing;//��parkingship����freegoods
				for (int j = 0; j < parkingship[i].goodsqtt; j++)
					freegoods.push (parkingship[i].sgoods[j]);
			}
			if (!parkingship[i].goodsqtt && parkingship[i].state == ing)							//װж���
			{
				parkingship[i].state = post;
				parkingship[i].lifeline[1] = totaltime;
			}
			if (parkingship[i].lifeline[1] + parkingship[i].outtime == totaltime)					//�������
			{
				freeberth++;//��parkingship�Ƴ�
				for (int j = i; j < berthqtt - freeberth; j++)
					parkingship[j] = parkingship[j + 1];
				i--;
			}
		}
		/*�벴*/
		while (freeberth && !freeship.empty ())
		{
			Cship temp = freeship.min ();//��freeship��������parkingship
			temp.lifeline[0] = totaltime;
			for (int i = 0; i < temp.goodsqtt; i++)
				temp.sgoods[i].lifeline[0] = totaltime;
			parkingship[berthqtt - freeberth] = temp;
			freeship.pop (pmin);
			freeberth--;
		}
		/*�����*/
		/*�ڵ�*/
		for (int i = 0; i < craneqtt - freecrane; i++)
		{
			if (craninggoods[i].lifeline[1] + craninggoods[i].time == totaltime)		//������
			{
				craninggoods[i].state = buffer;//��craninggoods�Ƴ�
				buffergoods.push (craninggoods[i]);
				int shipno = craninggoods[i].shipno;
				int goodsno = craninggoods[i].goodsno;
				freecrane++;
				for (int j = i; j < craneqtt - freecrane; j++)
					craninggoods[j] = craninggoods[j + 1];
				bool found = false;									
				for (int j = 0; j < berthqtt - freeberth && !found; j++)//ɾ�������ϻ���
				{
					if (parkingship[j].shipno == shipno)
					{
						for (int k = 0; k < parkingship[j].goodsqtt && !found; k++)
						{
							if (parkingship[j].sgoods[k].goodsno == goodsno)
							{
								found = true;
								parkingship[j].goodsqtt--;
								for (int l = k; l < parkingship[j].goodsqtt; l++)
									parkingship[j].sgoods[l] = parkingship[j].sgoods[l + 1];
								if (parkingship[j].goodsqtt == 0)
									goto OUT;
							}
						}
					}
				}
				i--;
			}
		}
		/*���*/
		while (freecrane && !freegoods.empty ())//��freegoods��������craninggoods
		{
			Cgoods temp = freegoods.min ();
			temp.lifeline[1] = totaltime;
			temp.state = crane;
			craninggoods[craneqtt - freecrane] = temp;
			freegoods.pop (pmin);
			freecrane--;
		}
		/*С��*/
		/*����*/
		for (int i = 0; i < shuttleqtt - freeshuttle; i++)
		{
			if (shuttlinggoods[i].lifeline[2] + 2 * shuttletime[shuttlinggoods[i].warehouseno] == totaltime)	//�������
			{
				shuttlinggoods[i].state = warehouse;//��shuttlinggoods�Ƴ�
				freeshuttle++;
				for (int j = i; j < shuttleqtt - freeshuttle; j++)
					shuttlinggoods[j] = shuttlinggoods[j + 1];
				i--;
			}
		}
		/*����*/
		while (freeshuttle && !buffergoods.empty ())//��buffergoods��������shuttlinggoods
		{
			Cgoods temp = buffergoods.max ();
			temp.lifeline[2] = totaltime;
			temp.state = shuttle;
			shuttlinggoods[shuttleqtt - freeshuttle] = temp;
			buffergoods.pop (pmax);
			freeshuttle--;
		}
		totaltime++;
	}
	cout << totaltime - 1;
	finish = clock ();//������ʱ
	runtime = (double)(finish - start);
	cout << "����ʱ��Ϊ" << runtime << "ms" << endl;//����ʱ��
	return 0;
}