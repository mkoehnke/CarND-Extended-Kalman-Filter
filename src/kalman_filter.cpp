#include "kalman_filter.h"
#include "tools.h"
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
    x_ = F_ * x_;
    MatrixXd Ft = F_.transpose();
    P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
    VectorXd z_pred = H_ * x_;
    VectorXd y = z - z_pred;
    CommonUpdate(y);
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  double rho = std::sqrt(x_(0)*x_(0) + x_(1)*x_(1));
  double theta = std::atan2(x_(1), x_(0));
  double rho_dot = (x_(0)*x_(2) + x_(1)*x_(3)) / rho;
  VectorXd h = VectorXd(3);
  h << rho, theta, rho_dot;
  VectorXd y = z - h;
  CommonUpdate(NormalizeAngle(y));
}

void KalmanFilter::CommonUpdate(const VectorXd &y) {
    MatrixXd Ht = H_.transpose();
    MatrixXd S = H_ * P_ * Ht + R_;
    MatrixXd Si = S.inverse();
    MatrixXd PHt = P_ * Ht;
    MatrixXd K = PHt * Si;
    
    //new estimate
    x_ = x_ + (K * y);
    long x_size = x_.size();
    MatrixXd I = MatrixXd::Identity(x_size, x_size);
    P_ = (I - K * H_) * P_;
}

VectorXd KalmanFilter::NormalizeAngle(VectorXd &y) {
    while (y(1) < -M_PI) {
        y(1) += 2 * M_PI;
    }
    while (y(1) > M_PI) {
        y(1) -= 2 * M_PI;
    }
    return y;
}
