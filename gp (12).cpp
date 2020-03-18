#include "../include/iLQR.hpp"
#define points 10 //number of training samples
#include "../include/iLQR.hpp"
#include <cppad/speed/det_by_minor.hpp>
using namespace ros;
using namespace std;
using namespace Eigen;
typedef Matrix<double, 6, 1> Vector;
typedef Matrix<Vector, 1, points + 1> dataset;
int learn_N = 10;
double learn_rate = 0.06;
double scale = 10;
dataset datasetX,datasetY;
MatrixXd KTT(points,points);
MatrixXd Ktt(points,1);
MatrixXd KtT(1,points);
MatrixXd Cov(6,6);
MatrixXd G(4,1);
MatrixXd G_grad(6,4);
Vector Xmue;
Vector Xtest,Xtest_shifted;
MatrixXd state_correction(4,1);
MatrixXd Y(points,1);
ilqr::train result;
ilqr::G Gmsg;
ilqr:G_grad G_gradmsg;
Publisher gp_correction_pub;
Publisher G_pub;
Publisher G_grad_pub;
int t=0;
void calc_KTT(){
		for(int i=0;i<points;i++)
		{
			for(int j=0;j<points;j++)
			{					
				KTT(i,j)=scale*exp(-(datasetX[i]-datasetX[j]).transpose()*0.5*Cov.inverse()*(datasetX[i]-datasetX[j]));
			}
		}
}

void calc_grad(MatrixXd& gradM,int Cov_i){
	gradM=MatrixXd::Zero(points,points);
	if(Cov_i==6){
		gradM=(1/scale)*KTT;
		//cout<<"done1"<<endl;
		return;
	}
	
	
	for(int i=0;i<points;i++){
		for(int j=0;j<points;j++){
			gradM(i,j)=KTT(i,j)*pow((datasetX[i](Cov_i,0)-datasetX[j](Cov_i,0))/(Cov(Cov_i,Cov_i)),2);
		}
	}

}


void hyper_learn(){
	//cout<<"entered"<<endl;
	MatrixXd M1;
	MatrixXd gradM;
	for(int i=0;i<learn_N;i++){
			for(int j=0;j<7;j++){		
				calc_grad(gradM,j);		
				M1=KTT.inverse()*gradM;	
				MatrixXd inverse_temp(6,6);
				inverse_temp = KTT.inverse();		
				MatrixXd temp_Mat(1,1);
				temp_Mat = Y.transpose()*inverse_temp*gradM*inverse_temp*Y;		
				if(j<6)
					Cov(j,j)=Cov(j,j)-learn_rate*0.5*M1.trace()-learn_rate*0.5*(temp_Mat(0,0));
				else
					scale=scale-learn_rate*0.5*M1.trace()-learn_rate*0.5*(temp_Mat(0,0));		
				calc_KTT();
		
			}
	}
}

void calc_Cov(){
	Cov<<10,0,0,0,0,0,
		0,10,0,0,0,0,
		0,0,10,0,0,0,
		0,0,0,10,0,0,
		0,0,0,0,10,0,
		0,0,0,0,0,10;
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
double predictYfor(MatrixXd Y){
		double ymean;
	    double yvariance;
		MatrixXd temp(1,1);
		temp=(KtT*(KTT.inverse()))*Y;
		return temp(0,0);
	}
void calc_G(int index){
	MatrixXd temp =scale - KtT*KTT.inverse()*KtT.transpose();
	G(index,0)=sqrt(temp(0,0));
}
void calc_G_grad(int index){
	MatrixXd *k_grad = new MatrixXd[points];	
	MatrixXd cov_inverse = Cov.inverse();
	MatrixXd var_grad(4,1);	
	for(int i=0;i<points;i++){
		k_grad[i]=MatrixXd::Zero(6,1);
		k_grad[i]=-0.5*KtT(0,i)*cov_inverse*(Xtest-datasetX[i]);
	}	
	var_grad = MatrixXd::Zero(6,1);
	for(int i=0;i<points;i++){
		for(int j=0;j<points;j++){
			var_grad+=KtT(0,i)*KTT(i,j)*k_grad[j]+KtT(0,j)*KTT(i,j)*k_grad[i];
		}
	}
	
	G_grad(0,index)=(0.5/G(index,0))*var_grad(0,0);
	G_grad(1,index)=(0.5/G(index,0))*var_grad(1,0);
	G_grad(2,index)=(0.5/G(index,0))*var_grad(2,0);
	G_grad(3,index)=(0.5/G(index,0))*var_grad(3,0);
	G_grad(4,index)=(0.5/G(index,0))*var_grad(4,0);
	G_grad(5,index)=(0.5/G(index,0))*var_grad(5,0);
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
	if(t>0&&t<points+1){
		datasetX[t](0,0)=data.x;
		datasetX[t](1,0)=data.y;
		datasetX[t](2,0)=data.v;
		datasetX[t](3,0)=data.yaw;
		datasetX[t](4,0)=data.acc;
		datasetX[t](5,0)=data.yaw_rate;
		datasetY[t-1](0,0) = -datasetX[t-1](0,0) - ( (datasetX[t-1](2,0)*T) - (0.5*datasetX[t-1](4,0)*T*T) )*cos( datasetX[t-1](3,0) ) + data.x;//in radians  
		datasetY[t-1](1,0) = -datasetX[t-1](1,0) - ( (datasetX[t-1](2,0)*T) - (0.5*datasetX[t-1](4,0)*T*T) )*sin( datasetX[t-1](3,0) ) + data.y;//in radians 
		datasetY[t-1](2,0) = -datasetX[t-1](2,0) - (datasetX[t-1](4,0)*T) + data.v;
		datasetY[t-1](3,0) = -datasetX[t-1](3,0) - (datasetX[t-1](5,0)*T) + data.yaw;

	}
	
	if(t==points+1){
		calcXmue();
		Xupdate_mean_normalized();
		calc_KTT();	
		t=0;
		return;
	}
		t++;
		
}
void test_callback(const ilqr::train data){		
		Xtest(0,0)=data.x - Xmue(0,0);
		Xtest(1,0)=data.y - Xmue(1,0);
		Xtest(2,0)=data.v - Xmue(2,0);
		Xtest(3,0)=data.yaw - Xmue(3,0);
		Xtest(4,0)=data.acc - Xmue(4,0);
		Xtest(5,0)=data.yaw_rate - Xmue(5,0);
		for(int i=0; i<6; i++){
			for(int j=0; j<points; j++){
				Y(j,0)=datasetY[j](i,0);
			}	
				calc_KtT();		
				state_correction(i,0)=predictYfor(Y);
				calc_G(i);
				calc_G_grad(i);
		}
		copy_state_to_msg(result, state_correction);	
		gp_correction_pub.publish(result);
		copy_G_to_msg(Gmsg,G);
		copy_Ggrad_to_msg(G_gradmsg,G_grad):
		G_pub(Gmsg);
		G_grad_pub(G_grad_pub);
		G_grad=MatrixXd::Zero(6,4);
}
int main(int argc, char **argv)
 { 
	init(argc, argv, "GP");
	NodeHandle n;
 	Subscriber train_sub = n.subscribe("train",10,datacollect);
 	Subscriber model_improve = n.subscribe<ilqr::train>("test",1,test_callback);
	gp_correction_pub = n.advertise<ilqr::train>("gp_result",1);
	G_pub = n.advertise<ilqr::G>("G",1);
	G_grad_pub = n.advertise<ilqr::G_grad>("G_grad",1);

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