#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;
  previous_timestamp_ = 0;
  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_ = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
  		0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
        0, 0.0009, 0,
        0, 0, 0.09;
  cout<<"Reached till f";
  // Process Noise
  Hj_<< 1,1,0,0,
        1,1,0,0,
        1,1,1,1;

  ekf_.F_ = MatrixXd(4, 4);
  ekf_.F_ << 1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1;
  
   H_laser_ << 1,0,0,0,
  				0,1,0,0;
  /**
  TODO:
    * Finish initializing the FusionEKF.
    * Set the process and measurement noises
  */
  //setting the initial timestamp to the timestamp of the first measurement


}

/**
* Destructor.
*/
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {


  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  
  if (!is_initialized_) {
    /**
    TODO:
      * Initialize the state ekf_.x_ with the first measurement.
      * Create the covariance matrix.
      * Remember: you'll need to convert radar from polar to cartesian coordinates.
    */
     previous_timestamp_ = measurement_pack.timestamp_;
    // first measurement
   
   
    ekf_.x_ = VectorXd(4);
    ekf_.x_ << 1, 1, 1, 1;
    ekf_.P_ = MatrixXd(4, 4);
  ekf_.P_ << 1.0,0.0,0.0,0.0,
             0.0,1.0,0.0,0.0,
              0.0,0.0,1.0,0.0,
              0.0,0.0,0.0,1.0;
     
   
    
    if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      /**
      Initialize state.
      */
      cout<<"Timestamp2"<< endl;
      ekf_.x_(0)=measurement_pack.raw_measurements_(0);
      ekf_.x_(1)=measurement_pack.raw_measurements_(1); 
      ekf_.x_(2) = 0.0;
      ekf_.x_(3) = 0.0;
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      /**
      Convert radar from polar to cartesian coordinates and initialize state.
      */
      double ro = measurement_pack.raw_measurements_(0);
      double theta= measurement_pack.raw_measurements_(1);
      double ro_theta = measurement_pack.raw_measurements_(2);
      
      ekf_.x_(0) = ro * cos(theta);
      ekf_.x_(1) = ro * sin(theta);
      ekf_.x_(2) = ro_theta * cos(theta);
      ekf_.x_(3) = ro_theta * sin(theta);
      //vx = ?
      //vy = ? 
      
    }
     ekf_.x_(2) = 5.2;
     ekf_.x_(3) = 1.8/1000.0;
   

    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  /*****************************************************************************
   *  Prediction
   ****************************************************************************/

  /**
   TODO:
     * Update the state transition matrix F according to the new elapsed time.
      - Time is measured in seconds.
     * Update the process noise covariance matrix.
     * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  
  double dt = (measurement_pack.timestamp_ - previous_timestamp_ ) / 1000000.0;
  previous_timestamp_ = measurement_pack.timestamp_;
  
  float noise_ax=9.0;
  float noise_ay=9.0;
  
 
 
  
  double dt2 = dt*dt;
  double dt3= dt2*dt;
  double dt4= dt2*dt2;
  ekf_.Q_= MatrixXd(4,4);
  ekf_.Q_<< dt4/4 * noise_ax , 0 , dt3/2 * noise_ax, 0,
            0, dt4/4 * noise_ay, 0 , dt3/2* noise_ay,
            dt3/2*noise_ax, 0 , dt2* noise_ax,0,
            0, dt3/2 * noise_ay, 0, dt2*noise_ay;
  
   ekf_.F_(0,2)= dt;
    ekf_.F_(1,3)= dt;
  
       
  
  ekf_.Predict();

  /*****************************************************************************
   *  Update
   ****************************************************************************/

  /**
   TODO:
     * Use the sensor type to perform the update step.
     * Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
    // Laser updates
    ekf_.H_= H_laser_;
   	ekf_.R_ = R_laser_;
    ekf_.Update(measurement_pack.raw_measurements_);
  } else if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR){
    Hj_= tools.CalculateJacobian(ekf_.x_);
    ekf_.H_ =Hj_;
    ekf_.R_= R_radar_;
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);
    
    // Radar updates
  }

  // print the output

  
}