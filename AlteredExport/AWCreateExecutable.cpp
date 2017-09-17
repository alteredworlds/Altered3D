#include "AlteredExport.h"
#include "AWSettings\res\AlteredSettingsRes.h"

//includes EXE creation part of hte code.
//Not much to it, but might as well separate into new CPP
//IN: exeFile - gives location of EXE file containing resource IDR_AWLOGO
//IN: filenmae - file to be stored as binary chunk as IDR_AWLOGO


AWBoolean
TnTExprt::produceExeVersion(const AWCString& sceneName)
{  //first, check to see that the new resource data file exists
   int fileSizeInBytes = AWFile::size(sceneName);
   if (!fileSizeInBytes) return FALSE;
   //build the correctly name for the EXE file - from scene;
   //in the same place but with extension '.exe' instead of '.tnt'
   AWCString outputFile(sceneName);
   string::size_type pos = outputFile.find(".tnt");
   if (string::npos == pos) 
      pos = outputFile.find(".TNT");
   if (string::npos != pos)
   {
      outputFile.remove(pos);
      outputFile += ".exe";
   }//if (string::npos != pos)
   //the reference EXE is stored as a binary resource in this
   //exporter module. Extract it and save to disk.
   HRSRC  hrsrc = FindResource(hInstance, MAKEINTRESOURCE(IDR_EXEFILE), 
                                        MAKEINTRESOURCE(300));
   if (hrsrc==NULL) 
   {
      return FALSE;
   }
   DWORD  size  = SizeofResource(hInstance, hrsrc);  
   if (size)
   {
      LPVOID rdata = LockResource(LoadResource(hInstance, hrsrc));
      if (rdata)
      {
         FILE* fd = fopen(outputFile.c_str(), "wb");
         if (fd)
         {
            fwrite(rdata, size, 1, fd);
            fclose(fd);
         }//if (fd)
      }//if (rdata)
   }//if (size)

   return updateBinaryResource(IDR_AWLOGO, outputFile, sceneName);
}//AWBoolean TnTExprt::produceExeVersion(const AWCString& fileName)



AWBoolean
TnTExprt::updateBinaryResource(WORD resID, const AWCString& exeFile, const AWCString& filename)
{
   AWBoolean retVal = FALSE;
   //get the DATA to be inserted into the EXE file:
   HANDLE   hFile = NULL; 
   int fileSizeInBytes = AWFile::size(filename);
   if (!fileSizeInBytes || !AWFile::Exists(exeFile)) return FALSE;
   

   hFile = CreateFile(filename.c_str(),           // open filename
                GENERIC_READ,              // open for reading 
                FILE_SHARE_READ,           // share for reading 
                NULL,                      // no security 
                OPEN_EXISTING,             // existing file only 
                FILE_ATTRIBUTE_NORMAL,     // normal file 
                NULL);                     // no attr. template 
 
   if (hFile == INVALID_HANDLE_VALUE) 
   { 
      return FALSE;
   } 

   HANDLE hMapFile = NULL;
   hMapFile = CreateFileMapping(hFile,    // Current file handle. 
                               NULL,      // Default security. 
                               PAGE_READONLY, // Read permission. 
                               0,         // Max. object size. 
                               0,         // Size of hFile. 
                               "AlteredExportFileMappingObject"); // Name of mapping object. 
 
   if (hMapFile) 
   {  
      //map the file into memory
      LPVOID data = MapViewOfFile(hMapFile,  // file-mapping object to map
                           FILE_MAP_READ,      // access mode
                           0,     // high-order 32 bits of file offset
                           0,      // low-order 32 bits of file offset
                           0  // number of bytes to map
                           );

      if (data)
      {
         //first, get hold of EXE version as a file
         //now open it with resource editing win32 function
         //indicating that existing resources should be RETAINED
         HANDLE hRes = BeginUpdateResource(exeFile.c_str(), FALSE);
         if (hRes)
         {
            UpdateResource(hRes,      // update-file handle
                           MAKEINTRESOURCE(300),    // LPCTSTR address of resource type to update
                           MAKEINTRESOURCE(resID),    // LPCTSTR address of resource name to update
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), // WORD language identifier of resource
                           data,      // LPVOID address of resource data
                           fileSizeInBytes  // length of resource data, in bytes
                            );
         //write the changes
            retVal = (AWBoolean)EndUpdateResource(hRes, FALSE);
         }//if (hRes)
         UnmapViewOfFile(data);
      }//if (data)
      CloseHandle(hMapFile);
   }//if (hMapFile)
   if (hFile)
   {
      CloseHandle(hFile);
   }//if (hFile)
   
   return retVal;
}//AWBoolean TnTExprt::updateBinaryResource(WORD resID, const AWCString& exeFile, const AWCString& filename)
