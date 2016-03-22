#include "Algorithm.h"
#include <fstream>
#include <tchar.h>
#define BUFFER_SIZE 256
using namespace std;
void test(int index,const char *TestName,int *A,int *B,int *C,int *M,int length,Algorithm<int>& al,fstream &log_out,LONGLONG ***table,int number);
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
	//main programm
	
	unsigned int length = templength;
	int *A,*B,*C,*M;

	A = new int[length];
	B = new int[length];
	C = new int[length];
	M = new int[length];
	
	// Устанавливаем желаемое количество потоков
	omp_set_num_threads(NumberOfThreads);
	
	fstream log_out = fstream(fileName,ios_base::out);
	if(log_out.is_open()){
		Algorithm<int> al = Algorithm<int>();
		log_out<<"тест\tпоследовательно\talgorithm0\talgorithm1\tкоэффицент"<<endl; 
		cout<<"start"<<endl;
		LONGLONG ***table = new LONGLONG**[3];
		for(int j = 0; j < 3; j++){
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
			test(1,"empty test",A,B,C,M,length,al,log_out,table,index);
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
			test(2,"random test",A,B,C,M,length,al,log_out,table,index);
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
			test(3,"fibonacci test",A,B,C,M,length,al,log_out,table,index);
			////////////////////////////////////////////////////2X 3X 5X test
			for(unsigned int i = 0; i < length; i++){
				A[i] = i*2;
				B[i] = i*3;
				C[i] = i*5;
				M[i] = 0;
			}
			test(4,"2X 3X 5X test",A,B,C,M,length,al,log_out,table,index);
			////////////////////////////////////////////////////////full test
			for(unsigned int i = 0; i < length; i++){
				A[i] = i;
				B[i] = i;
				C[i] = i;
				M[i] = 0;
			}
			test(5,"full test",A,B,C,M,length,al,log_out,table,index);
			/////////////////////////////////////////////////////////////////
		}
		for(int test = 0; test < 5; test++){
			for(int i = 0; i < measurement; i++){
				log_out<< (test+1) << "\t" << 
				table[0][test][i] << "\t" << 
				table[1][test][i] << "\t" << 
				table[2][test][i] << endl;
			}
			log_out<<endl;
		}
		for(int j = 0; j < 3; j++){
			for(int k = 0; k < 5; k++){
				delete[]table[j][k];
			}
		}
		cout<<"end"<<endl;
		log_out.close();
	}else{
		cout<<"error"<<endl;
	}
    //free memory
	if (fileName){free(fileName);}
	delete A;
	delete B;
	delete C;
	delete M;
	return 0;
}
void test(int index,const char *TestName,int *A,int *B,int *C,int *M,int length,
	Algorithm<int>& al,fstream &log_out,LONGLONG ***table,int number){
	LONGLONG successively,parallel_a0,parallel_a1;
	data d;
	d = al.algorithm0(A,B,C,M,length);
	cout<<"successively "<<TestName<<"\t"<<"size:\t"<<d.size<<endl; 
	successively = d.time;//последовательно. 
		
	d = al.algorithm1(A,B,C,M,length);
	cout<<"parallel 1 "<<TestName<<"\t"<<"size:\t"<<d.size<<endl; 
	parallel_a0 = d.time;//параллельно.

	d = al.algorithm2(A,B,C,M,length);
	cout<<"parallel 2 " <<TestName<<"\t"<<"size:\t"<<d.size<<endl;
	parallel_a1 = d.time;//паралельно.
	table[0][index-1][number] = successively;
	table[1][index-1][number] = parallel_a0;
	table[2][index-1][number]= parallel_a1;
}
//#pragma omp for nowait
//#pragma omp single
//#pragma omp critical 