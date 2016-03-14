#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>	
#include <fstream>
using namespace std;

#define MAX_THREADS 4
#define BUFFER_SIZE 256

unsigned int length = 1024;
int *A,*B,*C,*M;
unsigned int ThradArrayIndex = 0;
CRITICAL_SECTION cr_s; //создание критической секции
//для вывода
struct data{
	int size;
	LONGLONG time;
};
data algorithm0();
data algorithm1();

int _tmain(int argc, _TCHAR* argv[]){
	//без арументов не запускать.
	if(argc != 3 ){return -1;}

    size_t   i;
    char      *fileName = (char *)malloc( BUFFER_SIZE );
    wcstombs_s(&i, fileName, (size_t)BUFFER_SIZE, 
               argv[2], (size_t)BUFFER_SIZE );

    // Output
    printf("   Characters converted: %u\n", i);
    printf("    Multibyte character: %s\n\n",
     fileName );

	int templength = _wtoi(argv[1]); 
	if( templength <=0 || templength > (1024*1024)){return -2;}
	length = templength;
	//main programm

	InitializeCriticalSection(&cr_s); //инициализация критической секции
	A = new int[length];
	B = new int[length];
	C = new int[length];
	M = new int[length];
	
	//проверить оптимальное количество потоков
	//int NumberOfThreads = 4;
	// Устанавливаем желаемое количество потоков
	//omp_set_num_threads(NumberOfThreads);
	
	fstream log_out = fstream(fileName,ios_base::out);
	if(log_out.is_open()){
		
		data d;
		log_out<<"start"<<endl;

		///////////////////////////////////////////////////////empty test
		for(unsigned int i = 0; i < length; i++){
			A[i] = i*2+1;
			B[i] = i;
			C[i] = i*2;
			M[i] = 0;
		}

		d = algorithm0();
		log_out<<"algorithm0 emty set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = algorithm1();
		log_out<<"algorithm1 emty set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;
		//////////////////////////////////////////////////////random test
		int temp1 = 1;
		int temp2 = 2;
		int temp3 = 3;
		for(unsigned int i = 0; i < length; i++){
			//генератор для чисел множества можно испльзовать для получения входных множеств
			//int temp = любое число кроме 0
			A[i] = (temp1*temp1) + ((temp1 <<  temp1) | (temp1 >> temp1));
			B[i] = (temp2*temp2) + ((temp2 <<  temp2) | (temp2 >> temp2));
			C[i] = (temp3*temp3) + ((temp3 <<  temp3) | (temp3 >> temp3));
			M[i] = 0;
		}
		
		d = algorithm0();
		log_out<<"algorithm0 random\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = algorithm1();
		log_out<<"algorithm1 random\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 
		///////////////////////////////////////////////////fibonacci test
		B[0] = 1;B[1] = 2;
		for(unsigned int i = 0; i < length; i++){
			A[i] = length-i;
			if(i>1){
			B[i] = B[i-2]+B[i-1];
			}
			C[i] = i;
			M[i] = 0;
		}

		d = algorithm0();
		log_out<<"algorithm0 fibonacci\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = algorithm1();
		log_out<<"algorithm1 fibonacci\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;
		////////////////////////////////////////////////////2X 3X 5X test
		for(unsigned int i = 0; i < length; i++){
			A[i] = i*2;
			B[i] = i*3;
			C[i] = i*5;
			M[i] = 0;
		}

		d = algorithm0();
		log_out<<"algorithm0 2X 3X 5X\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = algorithm1();
		log_out <<"algorithm1 2X 3X 5X\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;
		////////////////////////////////////////////////////////full test
		for(unsigned int i = 0; i < length; i++){
			A[i] = i;
			B[i] = i;
			C[i] = i;
			M[i] = 0;
		}

		d = algorithm0();
		log_out<<"algorithm0 full set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 
		
		d = algorithm1();//проблема!!!! с переполнением
		log_out <<"algorithm1 full set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;
		/////////////////////////////////////////////////////////////////
		log_out <<"end"<<endl;
		
		log_out.close();
	}else{
		cout<<"error"<<endl;
	}

	// Free multibyte character buffer
    if (fileName){free(fileName);}
	//free other memory
	delete A;
	delete B;
	delete C;
	delete M;
	DeleteCriticalSection(&cr_s);//удаление критической секции.
	//system("pause");
	return 0;
}

//переменные для работы с таймером.
LARGE_INTEGER QPC_StartingTime, QPC_EndingTime, ElapsedMicroseconds;
LARGE_INTEGER Frequency;
	
void start_time(){
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&QPC_StartingTime);
}

LONGLONG end_time(){
	QueryPerformanceCounter(&QPC_EndingTime);
	ElapsedMicroseconds.QuadPart = QPC_EndingTime.QuadPart - QPC_StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	return ElapsedMicroseconds.QuadPart;
}

data algorithm0(){
	start_time();
	unsigned int index=0;
	for(unsigned int i = 0; i < length; i++){
		for(unsigned int j = 0; j < length; j++){
			if(A[i] == B[j]){
				for(unsigned int e = 0; e < length;e++){
					if( B[j]==C[e]){
						if(index >= length){DebugBreak();}
						M[index]= C[e];
						index++;
					}
				}
			}
		}
	}
	data d;
	d.time = end_time();
	d.size = index;
	return d;
}

DWORD WINAPI function1(LPVOID lpParam){
	int* pDataArray = (int*)lpParam;
	//выполнение программного алгоритма.
	
	unsigned int start = ((unsigned int)*pDataArray)*(length/MAX_THREADS);//беда с числами в последнем случае.
	unsigned int end =  (((unsigned int)*pDataArray) + 1)*(length/MAX_THREADS);
	//cout<<"start "<<start<<" end "<<end<<endl;
	for(unsigned int i = start; i < end; i++){
		for(unsigned int j = 0; j < length; j++){
			if(A[i] == B[j]){
				for(unsigned int e = 0; e < length;e++){
					if( B[j]==C[e]){
						if(ThradArrayIndex >= length){DebugBreak();}
						EnterCriticalSection(&cr_s);//вызов критической секции
						M[ThradArrayIndex]= C[e];
						ThradArrayIndex++;
						LeaveCriticalSection(&cr_s);//освобождение критической секции
					}
				}
			}
		}
	}
	return 0;
}

void ErrorHandler(LPTSTR lpszFunction) { 
	// Retrieve the system error message for the last-error code.

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message.

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 
	MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK); 

	// Free error-handling buffer allocations.

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

data algorithm1(){
	ThradArrayIndex = 0;
	data d;
	int *pDataArray[MAX_THREADS];
	DWORD   dwThreadIdArray[MAX_THREADS];
	HANDLE  hThreadArray[MAX_THREADS]; 

	for( int i = 0; i < MAX_THREADS; i++ ){
		pDataArray[i] = (int*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
				sizeof(int));

		if( pDataArray[i] == NULL ){
			ExitProcess(2);
		}
		(*pDataArray[i]) =  i;

		start_time();
		hThreadArray[i] = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			function1,       // thread function name
			(LPVOID)pDataArray[i],          // argument to thread function 
			0,                      // use default creation flags 
			&dwThreadIdArray[i]);   // returns the thread identifier 

		if (hThreadArray[i] == NULL){
			ErrorHandler(TEXT("CreateThread"));
			ExitProcess(3);
		}
	}

	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
		
	d.time = end_time();

	for(int i=0; i<MAX_THREADS; i++){
		CloseHandle(hThreadArray[i]);
		if(pDataArray[i] != NULL){
			HeapFree(GetProcessHeap(), 0, (LPVOID)pDataArray[i]);
			pDataArray[i] = NULL;
		}
	}
	d.size = ThradArrayIndex;
	return d;
}