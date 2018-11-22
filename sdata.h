#if ! defined(_SDATA_KUDUSHTEEV_H_)
#define _SDATA_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif


//только для POD-типов
template<typename T>
class pod_array {
private:
	T*     arr;
	DWORD  size;
	DWORD  cnt;
public:
	pod_array(void):arr(NULL), size(0), cnt(0){}

	explicit pod_array(const T* a, const T* b):arr(NULL), size(0), cnt(0){
		copy(a, b);
	}

	~pod_array(){
		clear();
	}

	pod_array(const pod_array&);
	pod_array& operator = (const pod_array&);
public:
	//вставка в конец массива
	void add(const T& val){
		if(_alloc())
			arr[cnt++] = val;
	}

	//вставка в произвольное место
	void insert(DWORD index, const T& val){
		if(_alloc()){
			if(index >= cnt)
				arr[cnt] = val;
			else {
				T* m = arr + index;
				for(T* p = arr + cnt; p > m; --p)
					*p = *(p - 1);
				arr[index] = val;
			}
			++cnt;
		}
	}

	//копирование входного массива
	void copy(const T* a, const T* b){
		while(a != b)
			add(*a++);
	}

	//произвольное удаление
	void erase(DWORD index){
		erase(index, 1);
	}

	void erase(DWORD index, DWORD num){
		if((index + num) > cnt)
			return;

		T* p1 = arr + index;
		T* p2 = arr + (index + num);
		T* e  = arr + cnt;
		while(p2 != e)
			*p1++ = *p2++;
		cnt -= num;
	}

	//удаление по значению
	void remove(const T& val){
		T* p = arr;
		T* e = arr + cnt;
		while((p != e) && (*p != val))
			++p;

		for(T* d = p; d != e; ++d){
			if(*d != val)
				*p++ = *d;
		}
		cnt = static_cast<DWORD>(p - arr);
	}

	template<typename Cmp>
	void remove(Cmp cmp){
		T* p = arr;
		T* e = arr + cnt;
		while((p != e) && !cmp(*p))
			++p;

		for(T* d = p; d != e; ++d){
			if(!cmp(*d))
				*p++ = *d;
		}
		cnt = static_cast<DWORD>(p - arr);
	}

	//очистка массива
	void clear(void){
		if(arr != NULL){
			HeapFree(GetProcessHeap(), 0, arr);
			arr = NULL;
		}
		cnt = size = 0;
	}

	//сброс без удаления
	void reset(void){
		cnt = 0;
	}

	//кол-во элементов
	DWORD getCount(void) const {
		return cnt;
	}

	bool empty(void) const {
		return (cnt == 0);
	}

	T* data(void){
		return arr;
	}

	const T* data(void) const {
		return arr;
	}

	T& operator [] (DWORD index){
		return arr[index];
	}

	const T& operator [] (DWORD index) const {
		return arr[index];
	}

private:

	bool _alloc(void){
		T*    p;
		DWORD m;
		if(arr == NULL){
			m   = 2;
			arr = reinterpret_cast<T*>(HeapAlloc(GetProcessHeap(), 0, m * sizeof(T)));
			if(arr == NULL)
				return false;
			size = m;
		} else if(cnt >= size){
			m = size << 1;
			p = reinterpret_cast<T*>(HeapReAlloc(GetProcessHeap(), 0, arr, m * sizeof(T)));
			if(p == NULL)
				return false;
			arr  = p;
			size = m;
		}
		return true;
	}
};



//---------------------------------------------------------------------------------------------------------


//статический фиксированный блок
template<typename T, DWORD size = 16>
class sblock {
private:
	T     arr[size];
	DWORD cnt;
public:
	sblock(void):cnt(0){}

	~sblock(){
		reset();
	}

	sblock(const sblock&);
	sblock& operator = (const sblock&);
public:
	//добавление
	void add(const T& val){
		if(cnt < size)
			arr[cnt++] = val;
	}

	//удаление по индексу
	void removeAt(DWORD index){
		if(index < cnt){
			T* e = arr + (--cnt);
			for(T* p = arr + index; p < e; ++p)
				*p = *(p + 1);
		}
	}

	//кол-во элементов
	void setCount(DWORD pos){
		if(pos <= size)
			cnt = pos;
	}

	//сброс
	void reset(void){
		cnt = 0;
	}

	//кол-во памяти
	DWORD getSize(void) const {
		return size;
	}

	//кол-во элементов
	DWORD getCount(void) const {
		return cnt;
	}

	T& operator [] (DWORD index){
		return arr[index];
	}

	const T& operator [] (DWORD index) const {
		return arr[index];
	}
};


//---------------------------------------------------------------------------------------------------------


//односвязной список в виде пула
template<typename T>
class pslist {

	struct node {
		node* next;
		T     val;
	};

private:
	node* head;
	node* tail;
	node* arr;
	DWORD cnt;
	DWORD num;
public:
	pslist(void):head(NULL), tail(NULL), arr(NULL), cnt(0), num(0){}

	explicit pslist(DWORD size):head(NULL), tail(NULL), arr(NULL), cnt(0), num(0){
		setSize(size);
	}

	~pslist(){
		clear();
	}

	typedef node* offset;

	pslist(const pslist&);
	pslist& operator = (const pslist&);
public:
	//добавить
	void add(const T& val){
		if(cnt >= num)
			return;

		arr[cnt].val  = val;
		arr[cnt].next = NULL;

		if(head == NULL)
			head = tail = &arr[cnt];
		else
			tail = tail->next = &arr[cnt];
		++cnt;
	}

	//удаление
	offset removeAt(offset pos){
		node* t = pos->next;
		if(pos == head)
			head = head->next;
		else {
			node* p = head;
			for(node* i = head; i != pos; i = i->next)
				p = i;
			p->next = pos->next;
		}
		--cnt;
		return t;
	}

	//выделение памяти под пул
	void setSize(DWORD size){
		clear();
		arr = new node[size];
		num = size;
		reset();
	}

	//сброс
	void reset(void){
		head = tail = NULL;
		cnt  = 0;
		for(DWORD i = 0; i < num; ++i)
			arr[i].next = NULL;
	}

	//очистка
	void clear(void){
		if(arr != NULL)
			delete[] arr;
		arr = NULL;

		head = tail = NULL;
		cnt  = num  = 0;
	}

	offset getOffset(void){
		return head;
	}

	const offset* getOffset(void) const {
		return head;
	}

	DWORD getSize(void) const {
		return num;
	}

	DWORD getCount(void) const {
		return cnt;
	}

	bool empty(void) const {
		return (cnt == 0);
	}
};


#endif
