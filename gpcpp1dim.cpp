#include <unistd.h>
#include "vector"
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>
#include <complex>
#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <fstream>
#include <complex>
#include <iostream>
#include<iostream>
#include "math.h"
using namespace std;
using namespace Eigen;
int dim_i=21;
int dim_j=21;
float Cov=1;
//TT Train-Train
//tt test-test
//tT test-Train

MatrixXd KTT(dim_i,dim_j);  // bigger whole clubbed matrix 
MatrixXd Ktt(1,1); //correspnds to E(x),E(y)
MatrixXd KtT(1,dim_i);//correspnds to E(xy) 

double sigma(int a,int b,int Xi[],int Xj[])
	{
		return exp(-pow(Xi[a]-Xj[b],2)/(2*Cov));
	}
void kernel(int Xi[],int Xj[])
	{
		for(int i=0;i<dim_i;i++)
		{
			for(int j=0;j<dim_j;j++)
			{
				KTT(i,j)=sigma(i,j,Xi,Xj);

			}
		}
	}

int main()
{ double Xtest;
	float y=0,yvar=0;
	Xtest=12;
	int X[21];
	int Y[21];
	for(int i=0;i<21;i++)
		{X[i]=i;}
	for(int i=0;i<21;i++)
		{Y[i]=X[i]*X[i]*X[i];}
	for(int i=0;i<21;i++)
	{printf("X[%d] = %d \n",i,X[i] );}
	for(int i=0;i<21;i++)
	{printf("Y[%d] = %d \n",i,Y[i] );}
double sum=0;
for(int i=0;i<21;i++){
	sum = sum + pow(X[i],2);
}
Cov=((double)sum)/21;
cout<<"Covariance"<<Cov<<endl;
kernel(X,X);
cout<<"KTT: "<<KTT<<endl;

for(int i=0;i<dim_i;i++)
		{
			for(int j=0;j<dim_j;j++)
			{
				printf("KTT(%d,%d)=%f \n",i,j,KTT(i,j));

			}
		}

for(int i=0;i<21;i++){
	KtT(0,i)=exp(-pow(Xtest-X[i],2)/(2*Cov));
	cout<<"Exp val"<< exp(-pow(Xtest-X[i],2)/(2*Cov))<<endl;
}

		cout<<"done1"<<endl;
MatrixXd temp(1,21);
temp=KtT*KTT.inverse();
cout<<temp<<endl;
for(int i=0;i<21;i++)
	{y+=temp(0,i)*Y[i];}
cout<<"done1"<<endl;
cout<<"output: "<<y<<endl;
	return 0;
}

	

