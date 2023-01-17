#include "kinematics.h"
#include "parser.h"
#include "math.h"

//DATA CONSTANTS
#define BAUD                  57600
#define COMMAND_BUFFER_SIZE   128

//MOTOR CONSTANTS
#define STEPS_PER_TURN        200
#define MIN_STEP_DELAY        1
#define MAX_FEEDRATE          1000000
#define MIN_FEEDRATE          0.0001
#define STEPS_PER_MM          1280
#define ACCELERATION          100

//ARC COMMAND CONSTANTS
#define ARC_CW                1
#define ARC_CCW               -1
#define MM_PER_SEGMENT        1

//PIN NUMBERS
//laser_power
//laser_toggle


//DATA
char commandBuffer[COMMAND_BUFFER_SIZE];
int commandBufferIndex = 0;

//POSITION
float px, py; //position is in steps
bool homed = false;
float xOffset = 0;
float yOffset = 0;

//SPEED
float feedrate = 0;
long step_delay;

//Stepper class
//Define a stepper motor and give it a name

Stepper X1("X1");

Stepper Y1("Y1");
Stepper Y2("Y2");

Stepper Z1("Z1");
Stepper Z2("Z2");
Stepper Z3("Z3");
Stepper Z4("Z4");


//PARSING METHODS

//MOTION METHODS
void line(int newX, int newY, int laserPower, bool doAccel=false){
  int dx = newX-px;
  int dy = newY-py;
  
  int dirX = dx>0?1:-1;
  int dirY = dy>0?-1:1;
  
  float slope = abs(dy/dx);
  float interval = 1 / (feedrate / 60 * STEPS_PER_MM * 10) * 1000000;
  //float accelDelay = 10;

  // point on the slope being walked
  float sx = 0;
  float sy = 0;

  // travelled x and y
  int tx = 0;
  int ty = 0;

  while(px != newX && py != newY){
    sx += interval;
    sy += interval * slope;
    
    if(sx > tx + 1){
      X1.step(dirX);
      tx += 1;
    }
    
    if(sy > ty + 1){
      Y1.step(dirY);
      Y2.step(dirY);
      ty += 1;
    }
    /*
     * acceleration is a work in progress
    if(doAccel){
      if(accelDelay >= 0){
        pause(accelDelay);
        accelDelay -= ACCELERATION / 2000000;
      }
      else if(accelDelay < 0){
        accelDelay = 0;
      }
    }
    */
    pause(interval);
  }
  
}

void arc(float cx,float cy,float x,float y,float dir) {
  // This method assumes the limits have already been checked.
  // This method assumes the start and end radius match.
  // This method assumes arcs are not >180 degrees (PI radians)
  // cx/cy - center of circle
  // x/y - end position
  // dir - ARC_CW or ARC_CCW to control direction of arc\
  
  // get radius
  float dx = px - cx;
  float dy = py - cy;
  float radius=sqrt(dx*dx+dy*dy);

  // find angle of arc (sweep)
  float angle1=atan3(dy,dx);
  float angle2=atan3(y-cy,x-cx);
  float theta=angle2-angle1;
 
  if(dir>0 && theta<0) angle2+=2*PI;
  else if(dir<0 && theta>0) angle1+=2*PI;
 
  theta=angle2-angle1;
 
  // get length of arc
  // float circ=PI*2.0*radius;
  // float len=theta*circ/(PI*2.0);
  // simplifies to
  float len = abs(theta) * radius;

  int i, segments = ceil( len * MM_PER_SEGMENT );
 
  float nx, ny, angle3, scale;

  for(i=0;i<segments;++i) {
    // interpolate around the arc
    scale = ((float)i)/((float)segments);
   
    angle3 = ( theta * scale ) + angle1;
    nx = cx + cos(angle3) * radius;
    ny = cy + sin(angle3) * radius;
    // send it to the planner
    line(nx * STEPS_PER_MM,ny * STEPS_PER_MM, 0);
  }
 
  line(x * STEPS_PER_MM,y * STEPS_PER_MM, 0);
} 


//SYSTEM METHODS
void pause(long microseconds) {
  delay(microseconds/1000);
  delayMicroseconds(microseconds%1000);
}

void setFeedrate(float newFeedrate) {
  if(feedrate==newFeedrate) return;  // same as last time?  quit now.

  if(newFeedrate>MAX_FEEDRATE || newFeedrate<MIN_FEEDRATE) return; // restrains feedrate
  
  step_delay = 1000000.0/newFeedrate;
  feedrate = newFeedrate;
}

void setPosition(float newX, float newY){
  //position is in steps
  px = newX;
  py = newY;
}

//MATH METHODS
float atan3(float dy,float dx) {
  // returns angle of dy/dx as a value from 0...2PI  
  float a = atan2(dy,dx);
  if(a<0) a = (PI*2.0)+a;
  return a;
}

//RUNTIME METHODS
void setup(){
  Serial1.begin(BAUD);
  Serial1.setTimeout(5); // serial will stop searching for data within 5ms
}

void loop() {
  // Check if there is a new line of input available
  if (Serial.available() > 0) {
    char c = Serial.read();
    // If the character is a newline, assume the command is complete
    if (c == '\n' || c == '*') {
      // Terminate the string and parse the command
      commandBuffer[commandBufferIndex] = '\0';
      parseCommand(commandBuffer);
      // Reset the command buffer index
      commandBufferIndex = 0;
    } else {
      // Otherwise, add the character to the command buffer
      commandBuffer[commandBufferIndex] = c;
      commandBufferIndex++;
    }
  }
}
