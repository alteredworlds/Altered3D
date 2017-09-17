//AlteredWorlds serializer for Linux, initial version
#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#else
#include <windows.h> //OutputDebugString
char tempStr[1000];
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "AWVerify.h"

char           name[USER_INFO_FIELD_LEN];
char           email[USER_INFO_FIELD_LEN];
char           del[SERIAL_FIELD_LEN]; 
unsigned char  sn[SERIAL_FIELD_LEN];

void
removeDangerousCharacters(char* str)
{
   int len = strlen(str);
   /* make sure we're watching for a ~ on the first line */		
   int filterState = 1;
   for (int i=0; i < len; i++) 
   {			
      switch(filterState) 
      {				
      case 0:
         if ((str[i] == '\n') || (str[i] == '\r')) 
         {
            filterState = 1;        
         }        
         break;        
      case 1:
         if ((str[i] == '\n') || (str[i] == '\r')) 
         {          
            break;
         }        
         if (str[i] == '~') 
         {
         // Quash the dangerous character           
            str[i] = ' '; 	
         }        
         filterState = 0;        
         break;      
      }//switch(filterState)    
   }//for (int i=0; i < len; i++) 
}//void removeDangerousCharacters(char* str)


bool
sendEmail()
{
   //send the mail to spacegnatsdemo@alteredworlds.com
   //content is email and name
   char buf[300];
   FILE* mailPipe;
   strcpy(buf, "/usr/sbin/sendmail spacegnats@alteredworlds.com");
   mailPipe = popen(buf, "w");		
   fprintf(mailPipe, "Subject: SpaceGnats Full Install\n");
   fprintf(mailPipe, "Reply-To: none@alteredworlds.com\n");
   fprintf(mailPipe, "--\n");
   // we need to filter out ~ escapes. So we
   // watch for a line break (or series of line-breaking characters)
   // followed by a ~. If we see one, we replace the ~ with a space.
   removeDangerousCharacters(name);
   removeDangerousCharacters(email);
   removeDangerousCharacters(del);
   fprintf(mailPipe, "Name: %s\nEmail: %s\nSerial: %s", name, email, del);		
   pclose(mailPipe);
}//bool sendEmail()



bool
getUserInfo(char* cmdLine, int len)
{  //information is built up as follows-
   //%s*%s*%s userName email delivery
   bool retVal = false;
   memset(name,  0, sizeof(name));
   memset(email, 0, sizeof(email));
   memset(del,   0, sizeof(del));
   memset(sn,    0, sizeof(sn));
   if (!cmdLine || !len) return false;

   char* startEmail = strchr(cmdLine, AWSN_DELIMIT_CHAR);
   if (startEmail)
   {
      *startEmail++ = 0;
      char* startDel = strchr(startEmail, ':');
      if (startDel)
      {
         *startDel++ = 0;
         //so now get the data without overruning any buffers
         strncpy(name,  cmdLine,    USER_INFO_FIELD_LEN);
         strncpy(email, startEmail, USER_INFO_FIELD_LEN);
         strncpy(del,   startDel,   SERIAL_FIELD_LEN);
         //does it pass initial level of checking?
         if (strchr(email, '@') && strchr(email, '.') && checkDeliveryID(del))
         {
            retVal = true;
         }
      }//if (startDel)
   }//if (sartEmail)
   return retVal;
}//bool getUserInfo(char* cmdLine, int len)


bool
generateSerialNum(char* argStr, int len)
{
   bool retVal = false;   //indicates malformed user information
   char* inStr = argStr;
   char* destStr = NULL;
   int curLen = 0;
   if (getUserInfo(argStr, len))
   { //OK - we have all the data
      md5_state_t state;
      md5_byte_t  digest[16];
      md5_init(&state);
      md5_append(&state, (const md5_byte_t*)name,  strlen(name));
      //printf("\nname: %s\n", name);
      md5_append(&state, (const md5_byte_t*)email, strlen(email));
      //printf("\nemail %s\n", email);
      md5_append(&state, (const md5_byte_t*)del,   strlen(del));
      //printf("\ndel %s\n", del);
      md5_finish(&state, digest);
      retVal = true;
      //final simple key based on check - serial[i] XOR digest[i]
      for (int di = 0; di < 16; di++)
      {
         sn[di] =  (unsigned char)0xff ^ digest[di];           
#ifdef _WIN32
         bool test = (bool)((const md5_byte_t&)sn[di] ^ digest[di]);
         sprintf(tempStr, "src hex %02x bitwize INVERT %02x  test returns %i\n", digest[di], sn[di], (int)test);
         OutputDebugString(tempStr);
#endif
      }//for (int di = 0; di < 16; di++)
   }//if (getUserInfo(argStr, len))
   return retVal;
}//bool generateSerialNum(char* argStr, int len)



bool
generateAndSendSerialNumber(char* argStr, int len)
{
   bool retVal = false;
   if (generateSerialNum(argStr, len))
   {
      fprintf(stdout, "AWSN#%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                          sn[0], sn[1],  sn[2],  sn[3],
                          sn[4], sn[5],  sn[6],  sn[7],
                          sn[8], sn[9],  sn[10], sn[11],
                          sn[12],sn[13], sn[14], sn[15]);
      retVal = true;
   }//if (generateSerialNum(argc, argv, sn))
   return retVal;
}//void generateAndSendSerialNumber(long serialNum)




int
main(int argc, char* argv[], char *envp[])
{
   bool flagSendEmail = false;
   fprintf(stdout, "content-type: text/html \n\n");
   fprintf(stdout, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">");
   fprintf(stdout, "<html><body>");
   /*
   printf( "Command-line arguments:\n" );
   for( int count = 0; count < argc; count++ )
     printf( "  argv[%d]   %s\n", count, argv[count] );

   printf( "\nEnvironment variables:\n" );
   while( *envp != NULL )
     printf( "  %s\n", *(envp++) );
   */   
   if (argc>1)
   {
      char* argStr = argv[1];
      int len      = strlen(argStr);
      flagSendEmail = generateAndSendSerialNumber(argStr, len);
   }
   fprintf(stdout, "</body></html>");
   fflush(stdout);
   if (flagSendEmail)
   {
      sendEmail();
   }
   return 1;
}//int main(int argc, char* argv[])
