#include<iostream>
#include<queue>
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
	int cw[2];//������ȼ�����ǰ���ȼ��ͽ������ȼ�
	int sw[2];//�������ȼ�����ǰ���ȼ��ͽ������ȼ�
	goodsstate state;//����״̬

};

class Cship
{
public:
	int shipno;
	Cgoods sgoods[6];
	int intime, outtime;
	int goodsqtt;
	int bw[2];			//�벴���ȼ�����ǰ���ȼ��ͽ������ȼ�					
	int lifeline[2];							//��ۡ�����ʱ���
	shipstate state;//����״̬
};

template<class T>
struct cmp_fgoods
{
	bool operator() (T x, T y, int i)
	{//Ĭ����less����
		return x.cw[i] >= y.cw[i];					//С�����ȼ���
	}
};

int shuttletime[5];
template<class T>
struct cmp_bgoods
{
	bool operator() (T x, T y, int i)
	{//Ĭ����less����
		return x.sw[i] >= y.sw[i];		//С�����ȼ���
	}
};

template<class T>
struct cmp_ship
{
	bool operator() (T x, T y, int i)
	{//Ĭ����less����
		return x.bw[i] >= y.bw[i];			//С�����ȼ���
	}
};

template<class T>
struct Node //���
{
	T data;
	Node* next;
};

template<class T, typename fcmp>
class Cqueue				//��С����
{
private:
	Node<T>* head;			//ͷָ��
	int size;
public:
	Cqueue ();	
	~Cqueue ();
	void push (T a, int i);
	T min ();
	T max ();
	bool empty ();
	void pop (poptype p);
	void copy (Cqueue& Q,int i);
	void setbw (int shipno, int bw);
	void setcw (int shipno, int goodsno, int cw);
	void setsw (int shipno, int goodsno, int sw);
	void bswap (int m, int n, int i);//�����벴���ȼ�
	void cswap (int m, int n, int i);//����������ȼ�
	void sswap (int m, int n, int i);//�����������ȼ�
	void save (int i);
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
void Cqueue<T, fcmp>::push (T a, int i)
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
			bool go = cmp (a, p->next->data, i);
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
	T s;
	s = head->next->data;
	return s;
}

template<class T, typename fcmp>
T Cqueue<T, fcmp>::max ()//����ĩβ
{
	Node<T>* p = head;
	while (p->next != NULL)
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
void Cqueue<T, fcmp>::copy (Cqueue& Q,int i)
{
	while (!this->empty ())
		this->pop (pmin);
	Node<T>* p = Q.head;
	while (p->next != NULL)
	{
		p = p->next;
		T s = p->data;
		this->push (s, i);
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::setbw (int shipno, int bw)//��ʼ���벴���ȼ�
{
	Node<T>* p = this->head;
	while (p->next != NULL)
	{
		p = p->next;
		if (p->data.shipno == shipno)
		{
			p->data.bw[0] = bw;
			p->data.bw[1] = bw;
		}
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::setcw (int shipno, int goodsno, int cw)//��ʼ��������ȼ�
{
	Node<T>* p = this->head;
	while (p->next != NULL)
	{
		p = p->next;
		if (p->data.shipno == shipno)
		{
			for (int j = 0; j < p->data.goodsqtt; j++)
			{
				if (p->data.sgoods[j].goodsno == goodsno)
				{
					p->data.sgoods[j].cw[0] = cw;
					p->data.sgoods[j].cw[1] = cw;
				}
			}
		}
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::setsw (int shipno, int goodsno, int sw)//��ʼ���������ȼ�
{
	Node<T>* p = this->head;
	while (p->next != NULL)
	{
		p = p->next;
		if (p->data.shipno == shipno)
		{
			for (int j = 0; j < p->data.goodsqtt; j++)
			{
				if (p->data.sgoods[j].goodsno == goodsno)
				{
					p->data.sgoods[j].sw[0] = sw;
					p->data.sgoods[j].sw[1] = sw;
				}
			}
		}
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::bswap (int m, int n, int i)//�����벴���ȼ�
{
	Node<T>* p = this->head;
	while (p->next != NULL)
	{
		p = p->next;
		if (p->data.bw[i] == m)
			p->data.bw[i] = n;
		else if (p->data.bw[i] == n)
			p->data.bw[i] = m;
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::cswap (int m, int n, int i)//����������ȼ�
{
	Node<T>* p = this->head;
	while (p->next != NULL)
	{
		p = p->next;
		for (int j = 0; j < p->data.goodsqtt; j++)
		{
			if (p->data.sgoods[j].cw[i] == m)
				p->data.sgoods[j].cw[i] = n;
			else if (p->data.sgoods[j].cw[i] == n)
				p->data.sgoods[j].cw[i] = m;
		}
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::sswap (int m, int n, int i)//�����������ȼ�
{
	Node<T>* p = this->head;
	while (p->next != NULL)
	{
		p = p->next;
		for (int j = 0; j < p->data.goodsqtt; j++)
		{
			if (p->data.sgoods[j].sw[i] == m)
				p->data.sgoods[j].sw[i] = n;
			else if (p->data.sgoods[j].sw[i] == n)
				p->data.sgoods[j].sw[i] = m;
		}
	}
}

template<class T, typename fcmp>
void Cqueue<T, fcmp>::save (int i)
{
	Node<T>* p = this->head;
	while (p->next != NULL)
	{
		p = p->next;
		p->data.bw[1 - i] = p->data.bw[i];
		for (int j = 0; j < p->data.goodsqtt; j++)
		{
			p->data.sgoods[j].cw[1 - i] = p->data.sgoods[j].cw[i];
			p->data.sgoods[j].sw[1 - i] = p->data.sgoods[j].sw[i];
		}
	}
}

int totaltime;
int shipqtt, berthqtt, craneqtt, shuttleqtt, warehouseqtt;		//���֡���λ��������С�����ֿ�����
int totalgoods;
Cgoods craninggoods[5], shuttlinggoods[5];
Cship parkingship[5];
int freeberth, freecrane, freeshuttle;
int F0;
int ans;
Cqueue<Cgoods, cmp_fgoods<Cgoods>> freegoods[5];				//���������
Cqueue<Cship, cmp_ship<Cship>> freeship0;						//��������
Cqueue<Cship, cmp_ship<Cship>> freeship;						//��������
Cqueue<Cgoods, cmp_bgoods<Cgoods>> buffergoods;					//����������

int main ()
{
	clock_t start, finish;
	double runtime;
	F0 = 0;
	ans = 0;
	totalgoods = 0;
	cout << "SAģ���˻�" << endl;
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
		temp.bw[0] = temp.intime + temp.outtime;
		int goodsqtt;
		cout << "������" << endl;
		cin >> goodsqtt;										//��������
		totalgoods += goodsqtt;
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
			temp.sgoods[j].cw[0] = temp.sgoods[j].time;
			temp.sgoods[j].sw[0] = 0;
			memset (temp.sgoods[j].lifeline, 0, 3);
		}
		freeship0.push (temp, 0);
	}
	start = clock ();
	int iw = 0;
	int bw0 = 0, cw0 = 18, sw0 = 0;
	double t = 10000;																						// �¶� 
	while (t > 0.0000000000000001)
	{
		//����
		freeship.copy (freeship0, iw);																		//��ʼ��
		int inship = 0, outship = 0;
		totaltime = 0;
		freeberth = berthqtt;
		freecrane = craneqtt;
		freeshuttle = shuttleqtt;
		while (freeberth < berthqtt || freeshuttle < shuttleqtt || !buffergoods.empty () || !freeship.empty ())		//���޻���ʱֹͣ
		{
			/*��λ*/
			/*�ڲ�*/
		OUT:
			for (int i = 0; i < berthqtt - freeberth; i++)
			{
				if (parkingship[i].lifeline[0] + parkingship[i].intime == totaltime)					//�벴���
				{
					parkingship[i].state = ing;
					for (int j = 0; j < parkingship[i].goodsqtt; j++)
						freegoods[inship].push (parkingship[i].sgoods[j], iw);
					inship++;
				}
				if (!parkingship[i].goodsqtt && parkingship[i].state == ing)							//װж���
				{
					parkingship[i].state = post;
					parkingship[i].lifeline[1] = totaltime;
				}
				if (parkingship[i].lifeline[1] + parkingship[i].outtime == totaltime)					//�������
				{
					freeberth++;
					for (int j = i; j < berthqtt - freeberth; j++)
						parkingship[j] = parkingship[j + 1];
					i--;
				}
			}
			/*�벴*/
			while (freeberth && !freeship.empty ())
			{
				Cship temp = freeship.min ();
				if (10000 - t < 0.000001)
					freeship0.setbw (temp.shipno, bw0++);
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
					craninggoods[i].state = buffer;
					buffergoods.push (craninggoods[i], iw);
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
			while (freecrane)
			{
				if (!freegoods[outship].empty ())
				{
					Cgoods temp = freegoods[outship].max ();
					if (10000 - t < 0.000001)
						freeship0.setcw (temp.shipno, temp.goodsno, cw0--);
					temp.lifeline[1] = totaltime;
					temp.state = crane;
					craninggoods[craneqtt - freecrane] = temp;
					freegoods[outship].pop (pmax);
					freecrane--;
				}
				else if (outship < inship - 1)
					outship++;
				else
					break;
			}
			/*С��*/
			/*����*/
			for (int i = 0; i < shuttleqtt - freeshuttle; i++)
			{
				if (shuttlinggoods[i].lifeline[2] + 2 * shuttletime[shuttlinggoods[i].warehouseno] == totaltime)	//�������
				{
					shuttlinggoods[i].state = warehouse;
					freeshuttle++;
					for (int j = i; j < shuttleqtt - freeshuttle; j++)
						shuttlinggoods[j] = shuttlinggoods[j + 1];
					i--;
				}
			}
			/*����*/
			while (freeshuttle && !buffergoods.empty ())
			{
				Cgoods temp = buffergoods.min ();
				if (10000 - t < 0.000001)
					freeship0.setsw (temp.shipno, temp.goodsno, sw0++);
				temp.lifeline[2] = totaltime;
				temp.state = shuttle;
				shuttlinggoods[shuttleqtt - freeshuttle] = temp;
				buffergoods.pop (pmin);
				freeshuttle--;
			}
			totaltime++;
		}
		totaltime--;						
		//iwΪ�½⣬1-iwΪ��ǰ�� 
		if (10000 - t < 0.000001)
		{
			F0 = totaltime;
			ans = F0;
		}
		int delta = F0 - totaltime;
		if (delta >= 0)
		{
			freeship0.save (iw);
			iw = 1 - iw;				//�����½�
			F0 = totaltime;
		}
		else
		{
			double p = (double)rand () / RAND_MAX;
			if (exp (delta / t) > p)
			{
				freeship0.save (iw);
				iw = 1 - iw;
				F0 = totaltime;
			}
			else
				freeship0.save (1 - iw);
		}
		int m = 0, n = 0;
		switch(rand () % 3)					//��iw��Ϊ��ǰ�⣬1-iw��Ϊ�½�Ѱ��
		{
		case 0:
			while (m == n)
			{
				m = rand () % shipqtt;    // ���ѡȡ���Ҵ������벴���ȼ�
				n = rand () % shipqtt;
			}
			freeship0.bswap (m, n, iw);
			break;
		case 1:
			while (m == n)
			{
				m = rand () % totalgoods;    // ���ѡȡ�����ｻ��������ȼ�
				n = rand () % totalgoods;
			}
			freeship0.cswap (m, n, iw);
			break;
		case 2:
			while (m == n)
			{
				m = rand () % totalgoods;    // ���ѡȡ�����ｻ���������ȼ�
				n = rand () % totalgoods;
			}
			freeship0.sswap (m, n, iw);
			break;
		}
		if (F0 < ans)
			ans = F0;
		t *= 0.999;																			// �𽥽����¶� 
	}
	cout << ans;
	finish = clock ();
	runtime = (double)(finish - start);
	cout << "����ʱ��Ϊ" << runtime << "ms" << endl;
	return 0;
}