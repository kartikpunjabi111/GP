//Matrix formation for given m indices in a ranked inital ordered n indices
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
#define points_needed 10
#define hyper_p1 10000000
#include "math.h"
using namespace std;
using namespace Eigen;
typedef Matrix<double, 6, 1>Vector ;
typedef Matrix<Vector, 1, points> Fux;
int indices[points_needed];
int b_indices[points_needed];
MatrixXd K_v(points,points_needed);
MatrixXd K_s(points_needed,points_needed);
MatrixXd K_h(points_needed,points);
MatrixXd sparseK(points,points);
int s;
double sve_c,sve_p; // prev ,current

MatrixXd KTT(points,points);////TT Train-Train

int tag,counter;
  
void calc_K_v()
	{
		for(int i=0;i<points_needed;i++)
			{
				K_v.col(i)=KTT.col(indices[i]-1);
			}
	}

void calc_K_s()
	{for(int a=0;a<points_needed;a++)
		{
			for(int b=0;b<points_needed;b++)
			{
				K_s(a,b)=KTT(indices[a]-1,indices[b]-1);
			}
		}
	}

void calc_K_h()
	{
		for(int i=0;i<points_needed;i++)
		{
			K_h.row(i)=KTT.row(indices[i]-1);
		}
	}


void K_calc()
	{
		sparseK=(K_v*K_s*K_h)/(hyper_p1);
	}

void take_r_ind()
	{ s=0;
		if(counter==1)
			{s++;counter=indices[s]-s;}
		indices[s]=(counter+1);
		counter--;
	}

void ind_update()
	{for(int i=0;i<points_needed;i++)
		{
			b_indices[i]=indices[i];
		}
	}
void f_pass()
	{
	calc_K_h();
	calc_K_s();
	calc_K_v();
	K_calc();
	
	sve_c=(KTT-sparseK).determinant();
	//cout<<sparseK<<endl<<endl<<endl;
	cout<<sve_c<<endl<<endl<<endl;
	for(int i=0;i<=points_needed;i++)
		{cout<<"b_indices"<<b_indices[i]<<" ";}
	for(int i=0;i<=points_needed;i++)
		{cout<<"indices"<<indices[i]<<" ";}
	}

void update()
	{	
		for(int a=0;a<500;a++)                 // a is iteration theshold
			{if(s==points_needed){cout<<"iske karan";break;}
			
			take_r_ind();
			f_pass();
			if(sve_c<sve_p)
				{
					ind_update();
					sve_p=sve_c;
				}
			}
		
	}



int main()
{sve_c=0;sve_p=0;counter=points-points_needed;
	int a=0;
	
	for (int i=0;i<points;i++)
	{
		for (int j=0;j<points;j++)
			{
				KTT(i,j)=a;
				a++;
			}
	}

	for(int i=0;i<points_needed;i++)
	{
		indices[i]=points-i;
		cout<<"value getted"<<indices[i];
	}
		for(int i=0;i<=points_needed;i++)
		{cout<<b_indices[i]<<" ";}

f_pass();
update();
cout<<indices;
}






















































