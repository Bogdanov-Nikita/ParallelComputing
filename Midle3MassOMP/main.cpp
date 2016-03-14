#include "Algorithm.h"
#include <fstream>
#include <tchar.h>
#define BUFFER_SIZE 256
using namespace std; 

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
	//main programm
	
	unsigned int length = templength;
	int *A,*B,*C,*M;

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
		
		Algorithm<int> al = Algorithm<int>();
		data d;

		log_out<<"start"<<endl;

		///////////////////////////////////////////////////////empty test
		for(unsigned int i = 0; i < length; i++){
			A[i] = i*2+1;
			B[i] = i;
			C[i] = i*2;
			M[i] = 0;
		}

		d = al.algorithm0(A,B,C,M,length);
		log_out<<"algorithm0 emty set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = al.algorithm1(A,B,C,M,length);
		log_out<<"algorithm1 emty set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;

		d = al.algorithm2(A,B,C,M,length);
		log_out<<"algorithm2 emty set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;
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
		
		d = al.algorithm0(A,B,C,M,length);
		log_out<<"algorithm0 random\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = al.algorithm1(A,B,C,M,length);
		log_out<<"algorithm1 random\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = al.algorithm2(A,B,C,M,length);
		log_out<<"algorithm2 random\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 
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

		d = al.algorithm0(A,B,C,M,length);
		log_out<<"algorithm0 fibonacci\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = al.algorithm1(A,B,C,M,length);
		log_out<<"algorithm1 fibonacci\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = al.algorithm2(A,B,C,M,length);
		log_out<<"algorithm2 fibonacci\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 
		////////////////////////////////////////////////////2X 3X 5X test
		for(unsigned int i = 0; i < length; i++){
			A[i] = i*2;
			B[i] = i*3;
			C[i] = i*5;
			M[i] = 0;
		}

		d = al.algorithm0(A,B,C,M,length);
		log_out<<"algorithm0 2X 3X 5X\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 

		d = al.algorithm1(A,B,C,M,length);
		log_out <<"algorithm1 2X 3X 5X\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;

		d = al.algorithm2(A,B,C,M,length);
		log_out <<"algorithm2 2X 3X 5X\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 
		////////////////////////////////////////////////////////full test
		for(unsigned int i = 0; i < length; i++){
			A[i] = i;
			B[i] = i;
			C[i] = i;
			M[i] = 0;
		}

		d = al.algorithm0(A,B,C,M,length);
		log_out<<"algorithm0 full set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 
		
		d = al.algorithm1(A,B,C,M,length);
		log_out <<"algorithm1 full set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl;

		d = al.algorithm2(A,B,C,M,length);
		log_out <<"algorithm2 full set\t"<<"size:\t"<<d.size<<"\ttime:\t"<<d.time<<endl; 
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
	//wait user DELETE THEN RUN SCRIPT EXE!
	//system("pause");
	return 0;
}
//#pragma omp for nowait
//#pragma omp single
//#pragma omp critical 