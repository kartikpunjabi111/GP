#include <unistd.h>
#include "vector"
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>
#include <complex>
#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <fstream>
#define points 21 //number of training samples
#include "math.h"
using namespace std;
using namespace Eigen;
typedef Matrix<float, 6, 1> Vector;
//Vector=[x,y,theta,v,a,delta]
Vector datasetX[points];  			// arrray of matrices with knoweldge of all points
Vector datasetY[points];
MatrixXd KTT(points,points);//TT Train-Train
MatrixXd Ktt(points,1);//tt test-test
MatrixXd KtT(1,points);//tT test-Train
// MatrixMf Y(points,1);
MatrixXd Cov(6,6);
Vector Xmue;
void calc_KTT()
	{
		for(int i=0;i<points;i++)
		{
			for(int j=0;j<points;j++)
			{	KTT(i,j)=exp((datasetX[i]-datasetX[j]).transpose()*Cov.inverse()*(datasetX[i]-datasetX[j]));
			}
		}
	}
void calc_Cov()
	{Cov<<0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
	for(int i=0;i<points;i++)
		{
			Cov+=datasetX[i]*datasetX[i].transpose();
		}
	Cov=Cov/points;
	cout<<"Covariance"<<Cov<<endl;
	}
	cout<<"KTT: "<<KTT<<endl;
	
void calc_Ktt()
	for(int i=0;i<points;i++)
	{
		KtT(0,i)=exp((Xtest-datasetX[j]).transpose()*Cov.inverse()*(Xtest-datasetX[j]));
	}
void calcXmue()
	{Xmue<<0,0,0,0,0,0;
		for(int i=0;i<points;i++)
		{
			Xmue+=datasetX[i];
		}
		Xmue=Xmue/points;
	}
void Xupdate_mean_normalized()
	{for(int r=0;r<6;r++)
		{for(i=0;i<points;i++)
			{datasetX[i](r,0)=datasetX[i](r,0)-Xmue(r,0);}}
	}
void predictYfor(Vector Xtest)
	{	float ymean,yvariance;
		cout<<"chala";
		cout<<KtT;
		cout<<KTT.inverse();
		cout<<Y;
		ymean=(KtT*KTT.inverse()*Y).determinant();
		//yvariance=(Ktt*KTT.inverse()*KtT.transpose()).determinant();
		cout<<"Y for Xtest="<<Xtest<<"is "<<ymean;
		//cout<<"Variance of y="<<ymean<<"is "<<yvariance;
	}

void get_y(int num)
	{for(int i=0;i<points;i++)
		{
			Y(i,0)=datasetY[i](num-1,0);
		}
	}
int main()
{ Vector Xtest;
	Xtest<<1,1,1,1,1,1;
	
	//initialization random training data
	// for(int i=0;i<21;i++)
	// 	{	X(i,0)=i-10;	}
	// for(int i=0;i<21;i++)
	// 	{Y(i,0)=pow(X(i,0),3);}

	// cout<<"X"<<X;
	// cout<<"Y"<<Y;

	calcXmue();
	Xupdate_mean_normalized();
	calc_Cov();  // sigma square matrix
	calc_KTT();
	calc_Ktt(); 
	get_y(1);  // 1st training to predict for component 1st
	predictYfor(Xtest);

	return 0;
}