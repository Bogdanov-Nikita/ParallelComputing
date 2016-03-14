#pragma once
#include <Windows.h>
#include <iostream>
#include <omp.h>

#define MAX_THREADS 4

//��� ������
struct data{
	int size;
	LONGLONG time;
};

template <typename T>
class Algorithm
{
private:
	//���������� ��� ������ � ��������.
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
public:
	//FIXME ����������� ������� � �����
	/*T *A, *B, *C;//������� �������� ��� �����������.
	T *M;// ��������� �����������. ������� �������������������� ������� � ����� ���������� ������
	int length;*/

	Algorithm(void){}
	~Algorithm(void){}
	data algorithm0(T *A,T *B,T *C,T *M,int length){
		start_time();
		int index=0;
		for(int i = 0; i < length; i++){
			for(int j = 0; j < length; j++){
				if(A[i] == B[j]){
					for(int e = 0; e < length;e++){
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
	//��������������� ������ ����.
	data algorithm1(T *A,T *B,T *C,T *M,int length){
		start_time();
		int index=0;
		#pragma omp parallel for
		for(int i = 0; i < length; i++){
			#pragma omp parallel for
			for(int j = 0; j < length; j++){
				if(A[i] == B[j]){
					#pragma omp parallel for
					for(int e = 0; e< length;e++){
						if( B[j]==C[e]){
							#pragma omp critical 
							{
								M[index] = C[e];
								index++;
							}
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
	//��������������� ������ ������� ����. 
	//FIXME �������� � �����.
	data algorithm2(T *A,T *B,T *C,T *M,int length){
		start_time();
		int index=0;
		#pragma omp parallel for
		for(int i = 0; i < length; i++){
			for(int j = 0; j < length; j++){
				if(A[i] == B[j]){
					for(int e = 0; e< length;e++){
						if( B[j]==C[e]){
							#pragma omp critical 
							{
								M[index] = C[e];
								index++;
							}
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
};

