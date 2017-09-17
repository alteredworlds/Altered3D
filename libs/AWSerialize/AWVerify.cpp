#define AWSERIAL_LENGTH       15
#define AWSERIAL_CHECK_LENGTH 10

#include <stdio.h>
#include "AWVerify.h"
#include <string.h>

//md5 hash of "AWSN2000-X"
unsigned char knownval[16] = {'\x0db', '\x0d4','\x092','\x076','\x09d','\x01a','\x0f2','\x0c8','\x038','\x082','\x09b','\x025','\x088','\x0a4','\x08e','\x0d0'};


void unspace(char* str)
{
   if (str && strlen(str))
   {
      char* test= strchr(str, ' ');
      while (test)
      {
         *test = '_';
         test = strchr(test, ' ');
      }
   }//if str && strlen(str))
}//void unspace(char* str)


bool
checkDeliveryID(const char* serial)
{
   //must be "AWSN2000-Xnnnnnn"
   bool retVal = false;
   if (strlen(serial) >= AWSERIAL_LENGTH)
   {
      md5_state_t state;
      md5_byte_t  digest[16];
      md5_init(&state);
      //note that AWSERIAL_CHECK_LENGTH < AWSERIAL_LENGTH
      md5_append(&state, (const md5_byte_t *)serial, AWSERIAL_CHECK_LENGTH);
      md5_finish(&state, digest);
      retVal = true;
      //so now we have the md5 digest - compare with correct value
      for (int di = 0; di < 16; di++)
      {
         retVal &= (knownval[di] == digest[di]);
         //sprintf(temp, "src hex %02x checking hex %02x \n", knownval[di], digest[di]);
         //strcat(fullMsg, temp);
      }
      //::MessageBox(NULL, fullMsg, "SERIAL NUMBER DIGEST!", MB_OK);
   }//if (strlen(serial) >= AWSERIAL_LENGTH)
   return retVal;
}//bool checkDeliveryID(const char* serial)



bool validate(const char* name, const char* email, const char* del, const char* serial)
{
   bool retVal = false;
   if (name && email && del)
   {
      char useName[USER_INFO_FIELD_LEN+1];
      strncpy(useName, name, USER_INFO_FIELD_LEN);
      useName[USER_INFO_FIELD_LEN]=0;
      unspace(useName);

      md5_state_t state;
      md5_byte_t  digest[16];

      md5_init(&state);
      md5_append(&state, (const md5_byte_t*)useName,  strlen(useName));
      md5_append(&state, (const md5_byte_t*)email, strlen(email));
      md5_append(&state, (const md5_byte_t*)del,   strlen(del));
      md5_finish(&state, digest);
      //fprintf(stderr, "md5 for: %s len %u  %s len %u  %s len %u ",
      //            name, strlen(name), email, strlen(email), del, strlen(del));
      //
      //final simple check - serial[i] XOR digest[i]
      bool match = false;     
      for (int di = 0; di < 16; di++)
      { 
         int i = 0;             
         match = true;
         sscanf(serial + 2 * di,"%02x", &i);
         //serial created via  (unsigned char)0xff ^ (unsigned char)digest[di];   
         if(0xff !=  ((unsigned char)i ^ (unsigned char)digest[di]))
         {//incorrect serial number
            match = false;
            break;
         }//if(0xff !=  ((unsigned char)i ^ (unsigned char)digest[di]))
      }//for (int di = 0; di < 16; di++)
      retVal = match;
   }//if (name && email && del)
   return retVal;
}//bool validate(char* name, char* email, char* del, char* serial)
