#ifndef MATH_H
#define MATH_H
#include "kinematics.h"

struct Vector3{
  float x;
  float y;
  float z;
};

float vectorMagnitude(Vector3 A){
  return sqrt((A.x * A.x) + (A.y * A.y) + (A.z * A.z));
}

float dotProduct(Vector3 A, Vector3 B){
  float x = A.x * B.x;
  float y = A.y * B.y;
  float z = A.z * B.z;
  float sum = x + y + z;
  
  float mag_A = vectorMagnitude(A);
  float mag_B = vectorMagnitude(B);
  
  return sum / (mag_A * mag_B); 
}

Vector3 addVector(Vector3 A, Vector3 B){
  Vector3 temp = {A.x + B.x, A.y + B.y, A.z + B.z};
  return temp;
}

Vector3 subtractVector(Vector3 A, Vector3 B){
  Vector3 temp = {A.x - B.x, A.y - B.y, A.z - B.z};
  return temp;
}

#endif
