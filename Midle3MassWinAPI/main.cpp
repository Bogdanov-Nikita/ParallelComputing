#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>	
#include <fstream>
using namespace std;

int NumberOfThreads = 4;
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
void test(int index,const char *TestName,int *A,int *B,int *C,int *M,int length,fstream &log_out,LONGLONG ***table,int number);

int _tmain(int argc, _TCHAR* argv[]){
	//без арументов не запускать.
	if(argc != 5 ){return -1;}

    size_t   i;
    char      *fileName = (char *)malloc( BUFFER_SIZE );
    wcstombs_s(&i, fileName, (size_t)BUFFER_SIZE, 
               argv[4], (size_t)BUFFER_SIZE );

    // Output
    printf("   Characters converted: %u\n", i);
    printf("    Multibyte character: %s\n\n",
     fileName );

	int measurement = _wtoi(argv[3]);
	if(measurement < 1){return -4;}

	int NumberOfThreads = _wtoi(argv[2]);
	if(NumberOfThreads < 2){return -3;} 

	int templength = _wtoi(argv[1]); 
	if( templength <= 0 || templength > (1024*1024)){return -2;}
	length = templength;
	//main programm


	InitializeCriticalSection(&cr_s); //инициализация критической секции
	A = new int[length];
	B = new int[length];
	C = new int[length];
	M = new int[length];
	
	fstream log_out = fstream(fileName,ios_base::out);
	if(log_out.is_open()){
		log_out<<"тест\tпоследовательно\tпареллельно\tкоэффицент"<<endl; 
		cout<<"start"<<endl;
		LONGLONG ***table = new LONGLONG**[3];
		for(int j = 0; j < 2; j++){
			table[j] = new LONGLONG*[5];
			for(int k = 0; k < 5; k++){
				table[j][k] = new LONGLONG[measurement];
			}
		}
		//[measurement];//[последов - 0,паралл-1,паралле-2][test N][number]
		for(int index = 0; index < measurement; index++){
			///////////////////////////////////////////////////////empty test
			for(unsigned int i = 0; i < length; i++){
				A[i] = i*2+1;
				B[i] = i;
				C[i] = i*2;
				M[i] = 0;
			}
			test(1,"empty test",A,B,C,M,length,log_out,table,index);
			//////////////////////////////////////////////////////random test
			int temp1 = 1;
			int temp2 = 2;
			int temp3 = 3;
			for(unsigned int i = 0; i < length; i++){
				A[i] = (temp1*temp1) + ((temp1 <<  temp1) | (temp1 >> temp1));
				B[i] = (temp2*temp2) + ((temp2 <<  temp2) | (temp2 >> temp2));
				C[i] = (temp3*temp3) + ((temp3 <<  temp3) | (temp3 >> temp3));
				M[i] = 0;
			}
			test(2,"random test",A,B,C,M,length,log_out,table,index);
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
			test(3,"fibonacci test",A,B,C,M,length,log_out,table,index);
			////////////////////////////////////////////////////2X 3X 5X test
			for(unsigned int i = 0; i < length; i++){
				A[i] = i*2;
				B[i] = i*3;
				C[i] = i*5;
				M[i] = 0;
			}
			test(4,"2X 3X 5X test",A,B,C,M,length,log_out,table,index);
			////////////////////////////////////////////////////////full test
			for(unsigned int i = 0; i < length; i++){
				A[i] = i;
				B[i] = i;
				C[i] = i;
				M[i] = 0;
			}
			test(5,"full test",A,B,C,M,length,log_out,table,index);
			/////////////////////////////////////////////////////////////////
		}

		for(int test = 0; test < 5; test++){
			for(int i = 0; i < measurement; i++){
				log_out<< (test+1) << "\t" << 
				table[0][test][i] << "\t" << 
				table[1][test][i] << "\t" << endl;
			}
			log_out<<endl;
		}
		for(int j = 0; j < 2; j++){
			for(int k = 0; k < 5; k++){
				delete[]table[j][k];
			}
		}
		cout<<"end"<<endl;
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
	return 0;
}
void test(int index,const char *TestName,int *A,int *B,int *C,int *M,int length,fstream &log_out,LONGLONG ***table,int number){
	LONGLONG successively,parallel;
	data d;
	d = algorithm0();
	cout<<"successively "<<TestName<<"\t"<<"size:\t"<<d.size<<endl;
	successively = d.time; 
	d = algorithm1();
	cout<<"parallel "<<TestName<<"\t"<<"size:\t"<<d.size<<endl; 
	parallel = d.time;
	table[0][index-1][number] = successively;
	table[1][index-1][number] = parallel;
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
	int pDataArray = (unsigned int)((int)lpParam);
	//выполнение программного алгоритма.
	unsigned int start = (pDataArray)*(length/NumberOfThreads);
	unsigned int end =   (pDataArray + 1)*(length/NumberOfThreads);
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
	int *pDataArray = new int [NumberOfThreads];
	DWORD *dwThreadIdArray = new DWORD [NumberOfThreads];
	HANDLE *hThreadArray = new HANDLE[NumberOfThreads]; 

	for( int i = 0; i < NumberOfThreads; i++ ){
		pDataArray[i] =  i;
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

	WaitForMultipleObjects(NumberOfThreads, hThreadArray, TRUE, INFINITE);
		
	d.time = end_time();
	d.size = ThradArrayIndex;

	delete pDataArray;
	delete dwThreadIdArray;
	delete hThreadArray;
	return d;
}