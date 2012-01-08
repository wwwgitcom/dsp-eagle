#include "stdafx.h"
#include "dsp_buffer.h"
#include "dsp_sysconfig.h"


static void t3_body ()
{
    int	nitems = (64 * (1L << 10)) / sizeof (int);

    static const int N = 5;
    dsp_buffer_ptr buf(new dsp_buffer(nitems, sizeof (int), NULL));
    dsp_buffer_reader_ptr 	reader[N];
    int			read_counter[N];
    int			write_counter = 0;

    for (int i = 0; i < N; i++){
        read_counter[i] = 0;
        reader[i] = dsp_buffer_add_reader(buf, NULL);
    }

    int* p2 = (int*)buf->m_vmcircbuf->pointer_to_second_copy();
    for (int lc = 0; lc < 1000; lc++){

        // write some
        float rnd = (float)(std::rand() % 10) / 10.0f;
        int n = (int) (buf->space_available () * rnd);
        int *wp = (int *) buf->write_pointer ();

        cout << "writer write " << n << " of " << buf->space_available() << endl;

        for (int i = 0; i < n; i++)
            *wp++ = write_counter++;

        if (wp > p2)
        {
            cout << "writer out of boundary " << wp - p2 << endl;
        }

        buf->update_write_pointer (n);

        // pick a random reader and read some
        rnd = (float)(std::rand() % 10) / 10.0f;
        int r = (int) (N * rnd);
        assert (0 <= r && r < N);

        int m = reader[r]->items_available ();

        cout << "reader " << r << " read " << m << endl;

        int *rp = (int *) reader[r]->read_pointer ();

        for (int i = 0; i < m; i++){
            assert (read_counter[r] == *rp);
            read_counter[r]++;
            rp++;
        }
        
        if (rp > p2)
        {
            cout << "reader " << r << " read out of boundary " << rp - p2 << endl;
        }

        reader[r]->update_read_pointer (m);
    }
}

/*****************************************************************
   LoggedSetLockPagesPrivilege: a function to obtain or
   release the privilege of locking physical pages.

   Inputs:

       HANDLE hProcess: Handle for the process for which the
       privilege is needed

       BOOL bEnable: Enable (TRUE) or disable?

   Return value: TRUE indicates success, FALSE failure.

*****************************************************************/

BOOL LoggedSetLockPagesPrivilege ( HANDLE hProcess, BOOL bEnable)
{
    struct 
    {
        DWORD Count;
        LUID_AND_ATTRIBUTES Privilege [1];
    } Info;

    HANDLE Token;
    BOOL Result;

    // Open the token.

    Result = OpenProcessToken ( hProcess,
        TOKEN_ADJUST_PRIVILEGES,
        & Token);

    if( Result != TRUE ) 
    {
        _tprintf( _T("Cannot open process token.\n") );
        return FALSE;
    }

    // Enable or disable?

    Info.Count = 1;
    if( bEnable ) 
    {
        Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
    } 
    else 
    {
        Info.Privilege[0].Attributes = 0;
    }

    // Get the LUID.

    Result = LookupPrivilegeValue ( NULL,
        SE_LOCK_MEMORY_NAME,
        &(Info.Privilege[0].Luid));

    if( Result != TRUE ) 
    {
        _tprintf( _T("Cannot get privilege for %s.\n"), SE_LOCK_MEMORY_NAME );
        return FALSE;
    }

    // Adjust the privilege.

    Result = AdjustTokenPrivileges ( Token, FALSE,
        (PTOKEN_PRIVILEGES) &Info,
        0, NULL, NULL);

    // Check the result.

    if( Result != TRUE ) 
    {
        _tprintf (_T("Cannot adjust token privileges (%u)\n"), GetLastError() );
        return FALSE;
    } 
    else 
    {
        if( GetLastError() != ERROR_SUCCESS ) 
        {
            _tprintf (_T("Cannot enable the SE_LOCK_MEMORY_NAME privilege; "));
            _tprintf (_T("please check the local policy.\n"));
            return FALSE;
        }
    }

    CloseHandle( Token );

    return TRUE;
}
#define MEMORY_REQUESTED 64*1024 // request a megabyte
#define VMMEMORY_REQUESTED (MEMORY_REQUESTED << 1) // request virtual memory size

void MakeVMCircleBuffer()
{
    BOOL bResult;                   // generic Boolean value
    ULONG_PTR NumberOfPages;        // number of pages to request
    ULONG_PTR NumberOfPagesInitial; // initial number of pages requested
    ULONG_PTR *aPFNs;               // page info; holds opaque data
    PVOID lpMemReserved;            // AWE window
    SYSTEM_INFO sSysInfo;           // useful system information
    int PFNArraySize;               // memory to request for PFN array

    GetSystemInfo(&sSysInfo);  // fill the system information structure

    _tprintf(_T("This computer has page size %d.\n"), sSysInfo.dwPageSize);

    // Calculate the number of pages of memory to request.

    NumberOfPages = MEMORY_REQUESTED / sSysInfo.dwPageSize;
    _tprintf (_T("Requesting %d pages of physical memory.\n"), NumberOfPages);

    // Calculate the size of the user PFN array.

    PFNArraySize = NumberOfPages * sizeof (ULONG_PTR);

    _tprintf (_T("Requesting a PFN array of %d bytes.\n"), PFNArraySize);

    aPFNs = (ULONG_PTR *) HeapAlloc(GetProcessHeap(), 0, PFNArraySize);

    if (aPFNs == NULL) 
    {
        _tprintf (_T("Failed to allocate on heap.\n"));
        return;
    }

    // Enable the privilege.

    if( ! LoggedSetLockPagesPrivilege( GetCurrentProcess(), TRUE ) ) 
    {
        return;
    }

    // Allocate the physical memory.

    NumberOfPagesInitial = NumberOfPages;
    bResult = AllocateUserPhysicalPages( GetCurrentProcess(),
        &NumberOfPages,
        aPFNs );

    if( bResult != TRUE ) 
    {
        _tprintf(_T("Cannot allocate physical pages (%u)\n"), GetLastError() );
        return;
    }

    if( NumberOfPagesInitial != NumberOfPages ) 
    {
        _tprintf(_T("Allocated only %p pages.\n"), NumberOfPages );
        return;
    }

    // Reserve the virtual memory.

    lpMemReserved = VirtualAlloc( NULL,
        VMMEMORY_REQUESTED,
        MEM_RESERVE | MEM_PHYSICAL,
        PAGE_READWRITE );

    _tprintf(_T("Alloc virtual memory %p.\n"), lpMemReserved);

    if( lpMemReserved == NULL ) 
    {
        _tprintf(_T("Cannot reserve memory.\n"));
        return;
    }

    // Map the physical memory into the window.

    bResult = MapUserPhysicalPages( lpMemReserved,//
        NumberOfPages,
        aPFNs );

    if( bResult != TRUE ) 
    {
        _tprintf(_T("MapUserPhysicalPages 1 failed (%u)\n"), GetLastError() );
        return;
    }

    int counter = 0;
    int nItems = MEMORY_REQUESTED / sizeof(int);
    int *p = (int*)lpMemReserved;
    for (int i = 0; i < nItems; i++)
    {
        p[i] = counter++;
    }

    //unmap
    bResult = MapUserPhysicalPages( lpMemReserved,
        NumberOfPages,
        NULL );

    if( bResult != TRUE ) 
    {
        _tprintf(_T("MapUserPhysicalPages 1 failed (%u)\n"), GetLastError() );
        return;
    }

    //map
    bResult = MapUserPhysicalPages( (PVOID)((char*)lpMemReserved + MEMORY_REQUESTED),
        NumberOfPages,
        aPFNs );

    if( bResult != TRUE ) 
    {
        _tprintf(_T("MapUserPhysicalPages 2 failed (%u)\n"), GetLastError() );
        //return;
    }

    // check
    p = (int *)((char*)lpMemReserved + MEMORY_REQUESTED);
    counter = 0;
    for (int i = 0; i < nItems; i++)
    {
        assert(p[i] == counter++);
        p[i] = 0xcc;
    }

    // unmap
    bResult = MapUserPhysicalPages( (PVOID)((char*)lpMemReserved + MEMORY_REQUESTED),
        NumberOfPages,
        NULL );

    if( bResult != TRUE ) 
    {
        _tprintf(_T("MapUserPhysicalPages 2 failed (%u)\n"), GetLastError() );
        return;
    }

    // Free the physical pages.

    bResult = FreeUserPhysicalPages( GetCurrentProcess(),
        &NumberOfPages,
        aPFNs );

    if( bResult != TRUE ) 
    {
        _tprintf(_T("Cannot free physical pages, error %u.\n"), GetLastError());
        return;
    }

    // Free virtual memory.

    bResult = VirtualFree( lpMemReserved,
        0,
        MEM_RELEASE );

    // Release the aPFNs array.

    bResult = HeapFree(GetProcessHeap(), 0, aPFNs);

    if( bResult != TRUE )
    {
        _tprintf(_T("Call to HeapFree has failed (%u)\n"), GetLastError() );
    }
}


#define BUF_SIZE 256
TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[]=TEXT("Message from first process.");

int MakeSharedMemory()
{
    HANDLE hMapFile;
    LPCTSTR pBuf;
    LPCTSTR pBuf2;
    LPVOID  lpMemReserved;

    lpMemReserved = VirtualAlloc( NULL,
        VMMEMORY_REQUESTED,
        MEM_RESERVE,
        PAGE_READWRITE );

    _tprintf(_T("Alloc virtual memory %p.\n"), lpMemReserved);

    if( lpMemReserved == NULL ) 
    {
        _tprintf(_T("Cannot reserve memory.\n"));
        return 0;
    }

    VirtualFree( lpMemReserved,
        0,
        MEM_RELEASE );

    //////////////////////////////////////////////////////////////////////////

    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        MEMORY_REQUESTED,        // maximum object size (low-order DWORD)
        szName);                 // name of mapping object

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not create file mapping object (%d).\n"),
            GetLastError());
        return 1;
    }

    pBuf = (LPTSTR) MapViewOfFileEx(hMapFile,   // handle to map object
        FILE_MAP_ALL_ACCESS, // read/write permission
        0,
        0,
        MEMORY_REQUESTED,
        lpMemReserved);

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }

    MEMORY_BASIC_INFORMATION mbi;

    VirtualQuery(pBuf, &mbi, sizeof(mbi));

    printf("AllocationBase      = 0x%p\n", mbi.AllocationBase);
    printf("AllocationProtect   = 0x%p\n", mbi.AllocationProtect);
    printf("BaseAddress         = 0x%p\n", mbi.BaseAddress);
    printf("Protect             = 0x%p\n", mbi.Protect);
    printf("RegionSize          = 0x%p\n", mbi.RegionSize);
    printf("State               = 0x%p\n", mbi.State);
    printf("Type                = 0x%p\n", mbi.Type);


    if ( !VirtualLock((LPVOID)pBuf, MEMORY_REQUESTED) )
    {
        printf("Cannot lock spaces into physical memory!\n");
    }

    pBuf2 = (LPTSTR) MapViewOfFileEx(hMapFile,   // handle to map object
        FILE_MAP_ALL_ACCESS, // read/write permission
        0,
        0,
        MEMORY_REQUESTED,
        (PVOID)((char*)lpMemReserved + MEMORY_REQUESTED));

    if (pBuf2 == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }

    CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

    UnmapViewOfFile(pBuf);
    UnmapViewOfFile(pBuf2);

    CloseHandle(hMapFile);

    return 0;
}

static void memory_test()
{
    dsp_sysconfig* psysconfig = dsp_sysconfig::Instance();
    int pagesize = psysconfig->GetPageSize();
    char * p = new char[4 * pagesize];

    printf("%p %p %p\n", p, p + pagesize, p + 2 * pagesize);

    ULONG nPages = 16;
    ULONG* UserPfnArray = new ULONG[nPages];

    BOOL bRet = AllocateUserPhysicalPages(GetCurrentProcess(), &nPages, UserPfnArray);
    
    if (bRet)
    {
        printf("Allocated %d pages of physical pages\n", nPages);
        for (ULONG i = 0; i < nPages; i++)
        {
            printf("%p ", UserPfnArray[i]);
        }
        printf("\n");
        FreeUserPhysicalPages(GetCurrentProcess(), &nPages, UserPfnArray);
    }

    psysconfig->GetSysMemoryInfo();


    delete[] UserPfnArray;
    delete[] p;
}

void dsp_buffer_test()
{
    //memory_test();
    //t3_body ();

    dsp_buffer_ptr buf = new dsp_buffer(100, sizeof(int), NULL);
    dsp_buffer_reader_ptr reader = dsp_buffer_add_reader(buf, NULL);

    int *pw;
    int *pr;

    memset(buf->m_base, 0xCC, buf->m_bufsize * 2);
    int iloop = 100;
    do 
    {
        cout << "writer space available " << buf->space_available() << endl;
        if ( buf->space_available() > 35 )
        {
            pw = (int *)buf->write_pointer();

            cout << "writer::: " << (pw - (int*)buf->m_base) << endl;

            for (int i = 0; i < 35; i++)
            {
                pw[i] = i;
            }
            buf->update_write_pointer(35);
        }

        cout << "reader items available " << reader->items_available() << endl;
        if (reader->items_available() > 18)
        {
            pr = (int *)reader->read_pointer();

            cout << "reader::: " << (pr - (int*)reader->buffer()->m_base) << endl;

            for (int i = 0; i < 18; i++)
            {
                cout << pr[i] << " ";
            }
            cout << endl;
            reader->update_read_pointer(18);
        }

    } while (iloop-- > 0);

    dsp_sysconfig::Instance()->GetSysMemoryInfo();

    delete reader;
    delete buf;
}