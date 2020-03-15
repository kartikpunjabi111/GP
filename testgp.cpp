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
typedef Matrix<double, 6, 1> Vector;
typedef Matrix<Vector, 1, points> Fux;
//Vector=[x,y,theta,v,a,delta]
Fux datasetX,datasetY;  			// arrray of matrices with knoweldge of all points
// Vector datasetY[points];
MatrixXd KTT(points,points);////TT Train-Train
MatrixXd Ktt(points,1);////tt test-test
MatrixXd KtT(1,points);////tT test-Train
MatrixXd Y(points,1);//(points,1);
// KTT=MatrixXd::Zero(points,points);
// Ktt=MatrixXd::Zero(points,1);
// KtT=MatrixXd::Zero(1,points);
// Y=MatrixXd::Zero(points,1);
// MatrixMf Y(points,1);
MatrixXd Cov(6,6);
Vector Xmue;
Vector Xtest;
Vector Yttest;
void calc_KTT()
	{
		for(int i=0;i<points;i++)
		{
			for(int j=0;j<points;j++)
			{	KTT(i,j)=exp((datasetX[i]-datasetX[j]).transpose()*0.5*Cov.inverse()*(datasetX[i]-datasetX[j]));
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
	//cout<<"Covariance"<<Cov<<endl;
	
	//cout<<"KTT: "<<KTT<<endl;
	}
	
void calc_Ktt()
	
	{for(int i=0;i<points;i++)
			{
			KtT(0,i)=exp((Xtest-datasetX[i]).transpose()*Cov.inverse()*(Xtest-datasetX[i]));
			}
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
	{
			for(int r=0;r<6;r++)
		{for(int i=0;i<points;i++)
			{datasetX[i](r,0)=datasetX[i](r,0)-Xmue(r,0);}}
	}
void predictYfor(Vector Xtest)
	{	MatrixXd ymean(1,1);
	    double yvariance;
		//cout<<"chala";
		//cout<<KtT;
		//cout<<KTT.inverse();
		//cout<<Y;
		cout<<"KtT"<<KtT<<endl;
		cout<<"KTT"<<KTT<<endl;
		cout<<"y: "<<Y<<endl;	
		MatrixXd temp(1,1);
		temp=(KtT*(KTT.inverse()))*Y;
		//ymean(0,0)=temp(0,0);
		//yvariance=(Ktt*KTT.inverse()*KtT.transpose()).determinant();
		//cout<<"Y for Xtest="<<Xtest<<"is "<<ymean;
		////cout<<"Variance of y="<<ymean<<"is "<<yvariance;
	}

void get_y(int num)
	{for(int i=0;i<points;i++)
		{
			Y(i,0)=datasetY[i](num-1,0);
		}
	}

int main()
{ 
 
	Xtest<<1,1,1,1,1,1;
	Xmue<<1,2,13244,4,5,6;

	datasetX[1]<<1,1,13245,1,1,1;
	datasetX[2]<<2,1,1,1,1,1;
	datasetX[1](2,0)=datasetX[1](2,0)-Xmue(2,0);
	//for(int i=0;i<points;i++)Xmue.rows()
	//cout<<datasetX[1]<<endl<<endl;
	Cov=datasetX[1]*(datasetX[2]).transpose();
	//cout<<"elemental"<<datasetX[1]*datasetX[1].transpose();
	//cout<<"Xmue.rows()"<< datasetX[1].transpose().rows();
	//cout<<"Cov"<<Cov;

	
	Cov=(datasetX[1])*(datasetX[1].transpose());	//initialization random training data

	calcXmue();
	Xupdate_mean_normalized();
	calc_Cov();  // sigma square matrix
	calc_KTT();
	calc_Ktt(); 
	get_y(1);  // 1st training to predict for component 1st
	predictYfor(Xtest);
	return 0;
}