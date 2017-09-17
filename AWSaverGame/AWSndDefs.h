#ifndef __AWSndDefs_H
#define __AWSndDefs_H

#define AWSndExport

//following is (for now) identical to QSound vector
typedef
struct AWVector 
{
   float x;                // meters
   float y;                // meters
   float z;                // meters

   AWVector(float xx=0.0f, float yy=0.0f, float zz=0.0f) 
            {x=xx;y=yy;z=zz;}
   AWVector(const AWVector& o)                           
            {x=o.x;y=o.y;z=o.z;}
} AWVector;


#define gameLoopSndID      1
#define gunSndID           2
#define failLevelSndID     3
#define doneLevelSndID     4
#define explodeSndID       5

#define SNDHDR	        "sounds/"
#define SNDFTR	        ".wav"


#endif //sentinel __AWSndDefs_H