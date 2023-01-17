#ifndef KINEMATICS_H
#define KINEMATICS_H
#include "math.h"

#define STEPS_PER_TURN        200
#define MIN_STEP_DELAY        1
#define MAX_FEEDRATE          1000000
#define MIN_FEEDRATE          0.0001
#define STEPS_PER_MM          1280
#define ACCELERATION          100
#define MAX_NO_ACCEL          300

class Stepper{

  public:

    String name;
    int position;
    bool limit = false;

    Stepper(String name){
      this->name = name;
      init();
    }

    void init(){
      position = 0;
    }

    void step(int dir){
      Serial1.println(name + String(dir));
      position += dir;
    }

    bool getLimit(){
      if (Serial1.find(name+"L")){
        return true;
      }
      return false;
    }
};

struct line{
    
  float fr;

  float lp;
  
  float xs;
  float ys;
  float zs;
    
  float xf;
  float yf;
  float zf;
};

class Path{
  
  public:
  
    float vs;
    float vf;
  
    line prev = {0, 0, 0, 0, 0, 0, 0, 0};
    line current = {0, 0, 0, 0, 0, 0, 0, 0};
    line next = {0, 0, 0, 0, 0, 0, 0, 0};

    Path(line p, line c, line n){
      this->prev = p;
      this->current = c;
      this->next = n;
    }
    
    Vector3 getStart(){
      Vector3 p = {current.xs, current.ys, current.zs};
      return p;
    }

    Vector3 getEnd(){
      Vector3 p = {current.xf, current.yf, current.zf};
      return p;
    }

    float feedrate(){
      return current.fr;
    }
    
  
};

void step(

void moveLine(Path path){
  Vector3 startPos = path.getStart();
  Vector3 endPos = path.getEnd();
  Vector3 travelled = subtractVector(endPos, startPos);

  int dirx = travelled.x>0?1:-1;
  int diry = travelled.y>0?-1:1;
  int dirz = travelled.z>0?-1:1;
  
  float xProportion = travelled.x / vectorMagnitude(travelled);
  float yProportion = travelled.y / vectorMagnitude(travelled);
  float zProportion = travelled.z / vectorMagnitude(travelled);

  float vs = max(MAX_NO_ACCEL, path.vs); 
  Vector3 vs_c = {vs*xProportion, vs*yProportion, vs*zProportion};

  float vf = path.feedrate();
  Vector3 vf_c = {vf*xProportion, vf*yProportion, vf*zProportion};

  float a = ACCELERATION;
  Vector3 a_c =  {a*xProportion, a*yProportion, a*zProportion};

  float t = ((vf - vs) / a) * 60;

  float d = vs*t + t*t*a/2;
  Vector3 d_c = {d*xProportion, d*yProportion, d*zProportion};
  
  float ts = STEPS_PER_MM * d;
  Vector3 ts_c = {ts_c*xProportion, ts_c*yProportion, ts_c*zProportion};

  float target_steps = max(ts_c.x, ts_c.y, ts_c.z);

  float startInterval = 1 / (vs * 60 * STEPS_PER_MM);
  float peakInterval = 1 / (vf * 60 * STEPS_PER_MM);
  float accelInterval = t / target_steps;

  //NEEDS TO BE TESTED FOR TIMING TO MAKE EACH ITERATION EXACTLY 1 MICROSECOND
  //ACCELERATION
  Vector3 slopes = {d_c.x/t, d_c.y/t, d_c.z/t};
  Vector3 lengthTravelled = {0, 0, 0};
  
  for(float f = 0; f < target_steps; f += 0.25){
    
    if(f * slopes.x > lengthTravelled.x){
      lengthTravelled.x += dirx;
      //x.step(dirx);
    }
    if(f * slopes.y > lengthTravelled.y){
      lengthTravelled.y += diry;
      //y.step(diry);
    }
    if(f * slopes.z > lengthTravelled.z){
      lengthTravelled.z += dirz;
      //z.step(dirz);
    }
    delayMicroseconds(startInterval - (accelInterval * 4 * f));
  }
  
}
#endif
