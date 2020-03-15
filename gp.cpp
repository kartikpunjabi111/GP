#include "../include/iLQR.hpp"
#define points 21 //number of training samples
#include "../include/iLQR.hpp"
using namespace ros;
using namespace std;
using namespace Eigen;
typedef Matrix<double, 6, 1> Vector;
typedef Matrix<Vector, 1, points + 1> dataset;

//Vector=[x,y,theta,v,a,delta]
dataset datasetX,datasetY;//arrray of matrices with knoweldge of all points
// Vector datasetY[points];
MatrixXd KTT(points,points);////TT Train-Train
MatrixXd Ktt(points,1);////tt test-test
MatrixXd KtT(1,points);////tT test-Train
//(points,1);
MatrixXd Cov(6,6);
Vector Xmue;
Vector Xtest,Xtest_shifted;

int t=0;
void calc_KTT()
	{
		for(int i=0;i<points;i++)
		{
			for(int j=0;j<points;j++)
			{	
				KTT(i,j)=exp(-(datasetX[i]-datasetX[j]).transpose()*0.5*Cov.inverse()*(datasetX[i]-datasetX[j]));
			
			}
		}
//cout<<"kernel"<<KTT<<endl<<endl;
}
void calc_Cov(){
	Cov<<0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
	for(int i=0;i<points;i++){
			Cov+=datasetX[i]*datasetX[i].transpose();
		}
	Cov=Cov/points;
	for(int i=0;i<6;i++){
			for(int j=0;j<6;j++){
				if(i!=j)
					Cov(i,j)=0;
		}
	}
	//cout<<"Covariance mat"<<Cov<<endl<<endl;
	}
	
void calc_KtT()
	
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
void Xupdate_mean_normalized(){
	for(int r=0;r<6;r++){
		for(int i=0;i<points;i++){
			datasetX[i](r,0)=datasetX[i](r,0)-Xmue(r,0);
		}
	}
}
void predictYfor(MatrixXd Y)
	{	double ymean;
	    double yvariance;
		////cout<<"KtT"<<KtT<<endl;
		////cout<<"KTT"<<KTT<<endl;
		////cout<<"y: "<<Y<<endl;	
		//cout<<"entered2"<<endl;
		MatrixXd temp(1,1);
		temp=(KtT*(KTT.inverse()))*Y;
		ymean=temp(0,0);
		cout<<"ymean"<<ymean<<endl;
		temp=(KTT.inverse()*KtT.transpose());
		cout<<"varriance"<<temp(0,0);
	}

void datacollect(ilqr::train data){
	if(t==0){
		datasetX[0](0,0)=data.x;
		datasetX[0](1,0)=data.y;
		datasetX[0](2,0)=data.v;
		datasetX[0](3,0)=data.yaw;
		datasetX[0](4,0)=data.acc;
		datasetX[0](5,0)=data.yaw_rate;
	}
	if(t>0&&t<22){
		datasetX[t](0,0)=data.x;
		datasetX[t](1,0)=data.y;
		datasetX[t](2,0)=data.v;
		datasetX[t](3,0)=data.yaw;
		datasetX[t](4,0)=data.acc;
		datasetX[t](5,0)=data.yaw_rate;
		datasetY[t-1](0,0) = datasetX[t-1](0,0) + ( (datasetX[t-1](2,0)*T) + (0.5*datasetX[t-1](4,0)*T*T) )*cos( datasetX[t-1](3,0) ) - data.x;//in radians  
		datasetY[t-1](1,0) = datasetX[t-1](1,0) + ( (datasetX[t-1](2,0)*T) + (0.5*datasetX[t-1](4,0)*T*T) )*sin( datasetX[t-1](3,0) ) - data.y;//in radians 
		datasetY[t-1](2,0) = datasetX[t-1](2,0) + (datasetX[t-1](4,0)*T) - data.v;
		datasetY[t-1](3,0) = datasetX[t-1](3,0) + (datasetX[t-1](5,0)*T) - data.yaw;
	}
	
	if(t==22){
		//cout<<"entered"<<endl;
		calcXmue();
		//cout<<"done"<<endl;
		Xupdate_mean_normalized();
		//cout<<"done"<<endl;
		calc_Cov();
		//cout<<"done"<<endl;
		calc_KTT();
		//cout<<"done"<<endl;
		Xtest(0,0)=data.x - Xmue(0,0);
		Xtest(1,0)=data.y - Xmue(1,0);
		Xtest(2,0)=data.v - Xmue(2,0);
		Xtest(3,0)=data.yaw - Xmue(3,0);
		Xtest(4,0)=data.acc - Xmue(4,0);
		Xtest(5,0)=data.yaw_rate - Xmue(5,0);
		//cout<<"done"<<endl;
		Xtest_shifted=Xtest + Xmue;
		calc_KtT();
		//cout<<"done"<<endl;
		MatrixXd Y(points,1);
		for(int i=0; i<points; i++){
			Y(i,0)=datasetY[i](0,0);
		}
		predictYfor(Y);
		t=0;
		Cov<<0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
		//cout<<"recieved"<<datasetX[t]<<endl;
		return;
	}
	//cout<<"recieved"<<datasetX[t]<<endl;
		t++;
}
int main(int argc, char **argv)
 { 
	init(argc, argv, "GP");
	NodeHandle n;
 	Subscriber train_sub = n.subscribe("train",10,datacollect);
	n.getParam("/ilqr/loop/NS", NS);
    n.getParam("/ilqr/loop/T", T);
    n.getParam("/ilqr/loop/epsilon", epsilon);
    n.getParam("/ilqr/loop/number_iteration", NUMBER_ITERATION);
    n.getParam("/ilqr/car/wheelBase", wheelBase);
    
	while(ok()){
		spinOnce();
	}	
	
	
	return 0;
}