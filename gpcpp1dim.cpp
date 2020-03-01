#include <unistd.h>
#include "vector"
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>
#include <complex>
#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <fstream>
#include<iostream>
#include "math.h"

int main()
{
	int X[20];
	int Y[20];
	for(int i=0;i<20;i++)
		{X[i]=i;}
	for(int i=0;i<20;i++)
		{Y[i]=X[i]*X[i];}
	for(int i=0;i<20;i++)
	{printf("X[%d] = %d \n",i,X[i] );}
	for(int i=0;i<20;i++)
	{printf("Y[%d] = %d \n",i,Y[i] );}
}