/*
 * w32ole.cpp:  Winvile's OLE implementation (the editor currently
 *              supports only OLE Automation).
 *
 * Note:  A great deal of the code included in this file was copied from
 * the Microsoft platform sdk samples directory, specifically from:
 *
 *    samples\com\oleaut\hello\hello .
 *
 * However, this implementation handles the Release() methods quite
 * differently and so, I wouldn't use this code as a guide for writing
 * any other automation servers :-) .
 *
 * Caveats
 * =======
 * - Due to a conflict between estruct.h and MS names, the Windows macros
 *   "FAILED" may not be used to test an OLE return code.  Use SUCCEEDED
 *   instead.
 *
 * $Header: /usr/build/vile/vile/RCS/w32ole.cpp,v 1.6 1998/10/24 15:11:33 cmorgan Exp $
 */

#include <windows.h>
#include <ole2.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

extern "C"
{
    #include "estruct.h"
    #undef FAILED          /* Don't want this defn of FAILED */
    #include "edef.h"
}
#include "w32ole.h"
#include "w32reg.h"

static size_t   ansibuf_len,   /* scaled in bytes   */
                olebuf_len;    /* scaled in wchar_t */
static char     *ansibuf;
static DWORD    dwRegisterCF, dwRegisterActiveObj;
static vile_oa  *g_pvile_oa;
static int      oleauto_server;
static OLECHAR  *olebuf;
static HWND     redirect_hwnd;
static DWORD    redirect_tid, winvile_tid;   // tid == thread id

/* ------------------------ C Linkage Helpers ---------------------- */

extern "C"
{

int
oleauto_init(OLEAUTO_OPTIONS *opts)
{
    char           *dummy = NULL;
    HRESULT        hr;
    LPCLASSFACTORY pcf = NULL;

    olebuf_len = ansibuf_len = 512;
    ansibuf    = (char *) malloc(ansibuf_len);
    olebuf     = (OLECHAR *) malloc(olebuf_len * sizeof(OLECHAR));
    if (! (olebuf && ansibuf))
        return (FALSE);

    hr = OleInitialize(NULL);   // Intialize OLE
    if (! SUCCEEDED(hr))
    {
        disp_win32_error(hr, NULL);
        return (FALSE);
    }

    oleauto_server = TRUE;

    // Create an instance of the Vile Application object. Object is
    // created with refcount 0.
    if (! SUCCEEDED(vile_oa::Create(&g_pvile_oa, ! opts->invisible)))
        return (FALSE);

    // Expose class factory for application object.
    pcf = new vile_oa_cf;
    if (! pcf)
        return (FALSE);
    pcf->AddRef();
    hr = CoRegisterClassObject(CLSID_VileAuto,
                               pcf,
                               CLSCTX_LOCAL_SERVER,
                    (opts->multiple) ? REGCLS_SINGLEUSE : REGCLS_MULTIPLEUSE,
                               &dwRegisterCF);
    pcf->Release();
    if (! SUCCEEDED(hr))
    {
        disp_win32_error(hr, NULL);
        return (FALSE);
    }

    /*
     * Register Vile application object in the Running Object Table (ROT).
     * This allows controllers to connect to a running application object
     * instead of creating a new instance.  Use weak registration so that
     * the ROT releases it's reference when all external references are
     * released.  If strong registration is used, the ROT will not release
     * it's reference until RevokeActiveObject is called and so will keep
     * the object alive even after all external references have been
     * released.
     */
    hr = RegisterActiveObject(g_pvile_oa,
                              CLSID_VileAuto,
                              ACTIVEOBJECT_WEAK,
                              &dwRegisterActiveObj);
    if (! SUCCEEDED(hr))
    {
        disp_win32_error(hr, NULL);
        return (FALSE);
    }
    if (! opts->invisible)
    {
        hr = CoLockObjectExternal(g_pvile_oa, TRUE, TRUE);
        if (! SUCCEEDED(hr))
        {
            disp_win32_error(hr, NULL);
            return (FALSE);
        }
    }
    return (TRUE);
}

void
oleauto_exit(int code)
{
    if (ansibuf)
        free(ansibuf);
    if (olebuf)
        free(olebuf);
    if (oleauto_server)
    {
        if (g_pvile_oa)
        {
            /*
             * For a local server, CoDisconnectObject will disconnect the
             * object from external connections.  So the controller will
             * get an RPC error if it accesses the object after calling
             * Quit and the controller will not GP fault.
             */

            CoDisconnectObject(g_pvile_oa, 0);
            delete g_pvile_oa;
        }
        if (dwRegisterCF != 0)
            CoRevokeClassObject(dwRegisterCF);
        if (dwRegisterActiveObj != 0)
            RevokeActiveObject(dwRegisterActiveObj, NULL);
        OleUninitialize();
    }
    exit(code);
}

} /* Extern "C" */

/* ----------------------- C++ Helper Functions --------------------- */

/*
 * Quick & Dirty ANSI/Unicode conversion routines.  These routines use a
 * dynamic buffer to hold the converted string so it may be any arbitrary
 * size.  However, the same dynamic buffer is reused when the routine is
 * called a second time.  So make sure that the converted string is
 * used/copied before the conversion routine is called again.
 *
 */
#if (! defined(_UNICODE) || defined(UNICODE))
char *
ConvertToAnsi(OLECHAR *szW)
{
    size_t len;

    len = wcstombs(NULL, szW, 1) + 1;
    if (len > ansibuf_len)
    {
        if (ansibuf)
            free(ansibuf);
        ansibuf_len = ansibuf_len * 2 + len;
        if ((ansibuf = (char *) malloc(ansibuf_len)) == NULL)
            return (ansibuf);  /* We're gonna' die */
    }
    wcstombs(ansibuf, szW, len);
    return (ansibuf);
}

OLECHAR *
ConvertToUnicode(char *szA)
{
    size_t len;

    len = strlen(szA) + 1;
    if (len > olebuf_len)
    {
        if (olebuf)
            free(olebuf);
        olebuf_len = olebuf_len * 2 + len;
        if ((olebuf = (OLECHAR *) malloc(olebuf_len * sizeof(OLECHAR))) == NULL)
            return (olebuf);  /* We're gonna' die */
    }
    mbstowcs(olebuf, szA, len);
    return (olebuf);
}
#endif

HRESULT
LoadTypeInfo(ITypeInfo **pptinfo, REFCLSID clsid)
{
    HRESULT hr;
    LPTYPELIB ptlib = NULL;
    LPTYPEINFO ptinfo = NULL;

    *pptinfo = NULL;

    hr = LoadRegTypeLib(LIBID_VileAuto, VTL_MAJOR, VTL_MINOR, 0x09, &ptlib);
    if (! SUCCEEDED(hr))
        return hr;

    // Get type information for interface of the object.
    hr = ptlib->GetTypeInfoOfGuid(clsid, &ptinfo);
    ptlib->Release();
    if (! SUCCEEDED(hr))
        return hr;

    *pptinfo = ptinfo;
    return NOERROR;
}

/* ------------------------ Class Factory -------------------------- */

vile_oa_cf::vile_oa_cf(void)
{
    m_cRef = 0;
}

STDMETHODIMP
vile_oa_cf::QueryInterface(REFIID iid, void **ppv)
{
    *ppv = NULL;

    if (iid == IID_IUnknown || iid == IID_IClassFactory)
        *ppv = this;
    else
        return E_NOINTERFACE;

    AddRef();
    return NOERROR;
}

STDMETHODIMP_(ULONG)
vile_oa_cf::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG)
vile_oa_cf::Release(void)
{
    if(--m_cRef == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

STDMETHODIMP
vile_oa_cf::CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv)
{
    HRESULT hr;

    *ppv = NULL;

    // This implementation doesn't allow aggregation
    if (punkOuter)
        return CLASS_E_NOAGGREGATION;

    /*
     * If this is a REGCLS_SINGLEUSE class factory, CreateInstance will be
     * called at most once.  The global application object has already been
     * created when CreateInstance is called.  A REGCLS_MULTIPLEUSE class
     * factory's CreateInstance would be called multiple times and would
     * create a new object each time.
     */
    hr = g_pvile_oa->QueryInterface(riid, ppv);
    if (!  SUCCEEDED(hr))
    {
        g_pvile_oa->Quit();
        return hr;
    }
    return (NOERROR);
}

STDMETHODIMP
vile_oa_cf::LockServer(BOOL fLock)
{
    HRESULT hr;

    hr = CoLockObjectExternal(g_pvile_oa, fLock, TRUE);
    if (! SUCCEEDED(hr))
        disp_win32_error(hr, NULL);
    return (hr);
}

/* ------------------------------- vile_oa ------------------------------ */

HRESULT
vile_oa::Create(vile_oa **ppvile, BOOL visible)
{
    HRESULT hr;
    vile_oa *pvile = NULL;
    OLECHAR *tmp;

    *ppvile = NULL;

    // Create Application object
    pvile = new vile_oa();
    if (pvile == NULL)
        return E_OUTOFMEMORY;

    pvile->m_hwnd     = (HWND) winvile_hwnd();
    pvile->m_bVisible = (visible) ? VARIANT_TRUE : VARIANT_FALSE;

    // Name
    tmp = TO_OLE_STRING(prognam);
    if (! (tmp && (pvile->m_bstrName = SysAllocString(tmp))))
        return E_OUTOFMEMORY;

    // Load type information from type library.
    hr = LoadTypeInfo(&pvile->m_ptinfo, IID_IVileAuto);
    if (! SUCCEEDED(hr))
    {
        MessageBox(pvile->m_hwnd,
"Cannot load type library.\r\r Register type info using winvile's '-Or' command line switch",
                   prognam,
                   MB_OK | MB_ICONSTOP);
        return (hr);
    }

    *ppvile = pvile;
    return NOERROR;
}

vile_oa::vile_oa()
{
    m_bstrFullName = NULL;
    m_bstrName     = NULL;
    m_ptinfo       = NULL;
    m_cRef         = 0;
}

vile_oa::~vile_oa()
{
    if (m_bstrFullName) SysFreeString(m_bstrFullName);
    if (m_bstrName)     SysFreeString(m_bstrName);
    if (m_ptinfo)       m_ptinfo->Release();
}

STDMETHODIMP
vile_oa::QueryInterface(REFIID iid, void **ppv)
{
    *ppv = NULL;

    if (iid == IID_IUnknown || iid == IID_IDispatch || iid == IID_IVileAuto)
        *ppv = this;
    else
        return E_NOINTERFACE;

    AddRef();
    return NOERROR;
}


STDMETHODIMP_(ULONG)
vile_oa::AddRef(void)
{
    return (++m_cRef);
}

STDMETHODIMP_(ULONG)
vile_oa::Release(void)
{
    if (--m_cRef == 0)
        PostQuitMessage(0);
    return (m_cRef);
}

STDMETHODIMP
vile_oa::GetTypeInfoCount(UINT *pctinfo)
{
    *pctinfo = 1;
    return NOERROR;
}

STDMETHODIMP
vile_oa::GetTypeInfo(
      UINT itinfo,
      LCID lcid,
      ITypeInfo **pptinfo)
{
    *pptinfo = NULL;

    if(itinfo != 0)
        return DISP_E_BADINDEX;

    m_ptinfo->AddRef();
    *pptinfo = m_ptinfo;

    return NOERROR;
}

STDMETHODIMP
vile_oa::GetIDsOfNames(
      REFIID riid,
      OLECHAR **rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID *rgdispid)
{
    return DispGetIDsOfNames(m_ptinfo, rgszNames, cNames, rgdispid);
}

STDMETHODIMP
vile_oa::Invoke(
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS *pdispparams,
      VARIANT *pvarResult,
      EXCEPINFO *pexcepinfo,
      UINT *puArgErr)
{
    return DispInvoke(
        this, m_ptinfo,
        dispidMember, wFlags, pdispparams,
        pvarResult, pexcepinfo, puArgErr);
}

STDMETHODIMP
vile_oa::get_Application(IVileAuto **ppvile)
{
    HRESULT hr;

    *ppvile = NULL;

    hr = QueryInterface(IID_IDispatch, (void **)ppvile);
    return ((SUCCEEDED(hr)) ? NOERROR : E_UNEXPECTED);
}

STDMETHODIMP
vile_oa::get_FullName(BSTR *pbstr)
{
    if (! m_bstrFullName)
    {
        char    *cp, key[NFILEN];
        HKEY    hk;
        long    rc;
        DWORD   regtype = REG_SZ;
        OLECHAR *tmp;
        BYTE    value[NFILEN * 2];
        ULONG   val_len;

        *pbstr  = NULL;
        val_len = sizeof(value);

        /* Extract server path from registry. */
        sprintf(key, "CLSID\\%s\\LocalServer32", CLSID_VILEAUTO_KEY);
        if (RegOpenKeyEx(HKEY_CLASSES_ROOT,
                         key,
                         0,
                         KEY_QUERY_VALUE,
                         &hk) != ERROR_SUCCESS)
        {
            disp_win32_error(W32_SYS_ERROR, m_hwnd);
            return (E_UNEXPECTED);
        }
        rc = RegQueryValueEx(hk, NULL, NULL, &regtype, value, &val_len);
        RegCloseKey(hk);
        if (rc != ERROR_SUCCESS)
        {
            disp_win32_error(W32_SYS_ERROR, m_hwnd);
            return (E_UNEXPECTED);
        }
        if ((cp = strchr((char *) value, ' ')) != NULL)
            *cp = '\0';
        tmp = TO_OLE_STRING((char *) value);
        if (! (tmp && (m_bstrFullName = SysAllocString(tmp))))
            return (E_OUTOFMEMORY);
    }
    *pbstr = SysAllocString(m_bstrFullName);
    return ((*pbstr) ? NOERROR : E_OUTOFMEMORY);
}

STDMETHODIMP
vile_oa::get_Name(BSTR *pbstr)
{
    *pbstr = SysAllocString(m_bstrName);
    return ((*pbstr) ? NOERROR : E_OUTOFMEMORY);
}

STDMETHODIMP
vile_oa::get_Parent(IVileAuto **ppvile)
{
    HRESULT hr;

    *ppvile = NULL;

    hr = QueryInterface(IID_IDispatch, (void **)ppvile);
    return ((SUCCEEDED(hr)) ? NOERROR : E_UNEXPECTED);
}

STDMETHODIMP
vile_oa::Quit()
{
    PostQuitMessage(0);
    return NOERROR;
}

STDMETHODIMP
vile_oa::VileKeys(BSTR keys)
{
    HRESULT hr   = NOERROR;
    char    *msg = FROM_OLE_STRING(keys);

    while (*msg)
    {
        if (! PostMessage(m_hwnd, WM_CHAR, *msg, 0))
        {
            disp_win32_error(W32_SYS_ERROR, m_hwnd);
            hr = E_UNEXPECTED;
            break;
        }
        msg++;
    }
    return (hr);
}

STDMETHODIMP
vile_oa::put_Visible(VARIANT_BOOL bVisible)
{
    if (bVisible != m_bVisible)
    {
        HRESULT hr;

        m_bVisible = bVisible;
        hr = CoLockObjectExternal(this, (m_bVisible) ? TRUE : FALSE, TRUE);
        if (! SUCCEEDED(hr))
        {
            disp_win32_error(hr, m_hwnd);
            return (hr);
        }
        ::ShowWindow(m_hwnd, bVisible ? SW_SHOW : SW_HIDE);
    }
    return (NOERROR);
}

STDMETHODIMP
vile_oa::get_Visible(VARIANT_BOOL *pbool)
{
    *pbool = m_bVisible;
    return NOERROR;
}

STDMETHODIMP
vile_oa::get_IsMinimized(VARIANT_BOOL *pbool)
{
    WINDOWPLACEMENT wp;

    wp.length = sizeof(wp);
    if (! GetWindowPlacement(m_hwnd, &wp))
        return (GetLastError());
    *pbool = (wp.showCmd == SW_MINIMIZE ||
              wp.showCmd == SW_SHOWMINIMIZED ||
              wp.showCmd == SW_SHOWMINNOACTIVE) ? VARIANT_TRUE : VARIANT_FALSE;
    return NOERROR;
}

STDMETHODIMP
vile_oa::get_InsertMode(VARIANT_BOOL *pbool)
{
    *pbool = (insertmode) ? VARIANT_TRUE : VARIANT_FALSE;
    return NOERROR;
}

STDMETHODIMP
vile_oa::ForegroundWindow()
{
    if (! m_bVisible)
        put_Visible(TRUE);   /* Force window to be visible, first. */
    ::SetForegroundWindow(m_hwnd);
    return NOERROR;
}

STDMETHODIMP
vile_oa::Minimize()
{
    ::ShowWindow(m_hwnd, SW_MINIMIZE);
    if (! m_bVisible)
    {
        /*
         * Minimizing an invisible window will make it visible, so keep the
         * m_bVisible bookkeeping up-to-date.
         */

        put_Visible(TRUE);
    }
    return (NOERROR);
}

STDMETHODIMP
vile_oa::Restore()
{
    ::ShowWindow(m_hwnd, SW_RESTORE);
    if (! m_bVisible)
    {
        /*
         * Restoring an invisible window will make it visible, so keep the
         * m_bVisible bookkeeping up-to-date.
         */

        put_Visible(TRUE);
    }
    return (NOERROR);
}

STDMETHODIMP
vile_oa::WindowRedirect(DWORD hwnd /* Null HANDLE => don't redirect */)
{
    redirect_hwnd = (HWND) hwnd;
    ntwinio_redirect_hwnd(redirect_hwnd != NULL);
    redirect_tid = GetWindowThreadProcessId(redirect_hwnd, NULL);
    winvile_tid  = GetWindowThreadProcessId(m_hwnd, NULL);
    return (NOERROR);
}

/* ----------- C Linkage To OLE-specific Editor Functions ---------------- */

extern "C"
{

/*
 * Wait for a file to spring into existence with > 0 bytes of data.  This
 * editor function is used to work around a bug in DevStudio 5, which
 * creates its build log _after_ notifying visvile that a build is complete.
 */
int
waitfile(int f, int n)
{
#define SLEEP_INCR 100

    char         fname[NFILEN];  /* wait for this file */
    DWORD        how_long, curr_sleep;
    int          s;              /* status return */
    struct _stat statdata;

    fname[0] = '\0';
    how_long = (f) ? n : 2000;  /* If wait period not specified as argument,
                                 * wait up to 2 seconds.
                                 */
    if ((s = mlreply_no_opts("Wait for file: ", fname, sizeof(fname))) == TRUE)
    {
        curr_sleep = 0;
        for (;;)
        {
            if (_stat(fname, &statdata) == 0)
            {
                if (statdata.st_size > 0)
                    break;
            }
            Sleep(SLEEP_INCR);
            curr_sleep += SLEEP_INCR;
            if (curr_sleep > how_long)
            {
                mlwrite("timed out waiting for \"%s\"", fname);
                break;
            }
        }
    }
    return (s);
}

/* ------------------- Key Redirection Implementation -------------------- */

#define FLUSH_BUFFERS 0x1
#define SWITCH_FOCUS  0x2

static int rebuild_cache_tbl = TRUE;
                              /* Boolean, T -> the GVAL_REDIRECT_KEYS list
                               * has either changed or has not been initially
                               * parsed.  In either case, parse this list and
                               * change it into a dynamic data structure
                               * stored in "cache_tbl".
                               */
typedef struct redir_key_struct
{
    unsigned vk;              /* virtual key code */
    unsigned modifier;        /* vk modifier, possible values are:
                               *
                               *   (LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED),
                               *   (LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED),
                               *   SHIFT_PRESSED, or
                               *   0                      (no modifier)
                               */
    unsigned action;          /* action taken before key is pressed.  Possible
                               * values are:
                               *
                               *   FLUSH_BUFFERS, SWITCH_FOCUS.
                               */
} REDIR_KEY;

static REDIR_KEY *cache_tbl;      /* User-specified list of redirected keys. */
static unsigned  cache_tbl_nelem; /* # elements in cache_tbl.                */

/*
 * Win32 virtual key code -> string mapping table.  Mappings are only
 * provided for keys which DevStudio supports as shortcuts _and_ which
 * can be found on my keyboard :-) .  A mapping string of "" is unsupported
 * because it fails either of the previously mentioned criteria or would
 * be stupid to redirect (cf ESC).
 *
 * CONTROL, SHIFT, and MENU (aka ALT) are not explicitly redirected--they
 * are handled as separate key modifiers.
 */
static char *keymap[] =
{
    /* ??            0x0  */ "",
    /* VK_LBUTTON    0x01 */ "",
    /* VK_RBUTTON    0x02 */ "",
    /* VK_CANCEL     0x03 */ "",        /* ctrl-break, nuh-uh */
    /* VK_MBUTTON    0x04 */ "",
    /* ??            0x05 */ "",
    /* ??            0x06 */ "",
    /* ??            0x07 */ "",
    /* VK_BACK       0x08 */ "BACK",
    /* VK_TAB        0x09 */ "TAB",
    /* ??            0x0A */ "",
    /* ??            0x0B */ "",
    /* VK_CLEAR      0x0C */ "",
    /* VK_RETURN     0x0D */ "",        /* I wouldn't redirect this key :-) */
    /* ??            0x0E */ "",
    /* ??            0x0F */ "",
    /* VK_SHIFT      0x10 */ "",
    /* VK_CONTROL    0x11 */ "",
    /* VK_MENU       0x12 */ "",
    /* VK_PAUSE      0x13 */ "PAUSE",
    /* VK_CAPITAL    0x14 */ "",        /* caps lock...I don't think so */
    /* ??            0x15 */ "",
    /* ??            0x16 */ "",
    /* ??            0x17 */ "",
    /* ??            0x18 */ "",
    /* ??            0x19 */ "",
    /* ??            0x1A */ "",
    /* VK_ESCAPE     0x1B */ "",        /* I wouldn't redirect this key :-) */
    /* ??            0x1C */ "",
    /* ??            0x1D */ "",
    /* ??            0x1E */ "",
    /* ??            0x1F */ "",
    /* VK_SPACE      0x20 */ "SPACE",
    /* VK_PRIOR      0x21 */ "PRIOR",
    /* VK_NEXT       0x22 */ "NEXT",
    /* VK_END        0x23 */ "END",
    /* VK_HOME       0x24 */ "HOME",
    /* VK_LEFT       0x25 */ "LEFT",
    /* VK_UP         0x26 */ "UP",
    /* VK_RIGHT      0x27 */ "RIGHT",
    /* VK_DOWN       0x28 */ "DOWN",
    /* VK_SELECT     0x29 */ "",
    /* VK_PRINT      0x2A */ "",
    /* VK_EXECUTE    0x2B */ "",
    /* VK_SNAPSHOT   0x2C */ "SNAPSHOT",  /* print screen */
    /* VK_INSERT     0x2D */ "INSERT",
    /* VK_DELETE     0x2E */ "DELETE",
    /* VK_HELP       0x2F */ "",
    /* VK_0          0x30 */ "0",
    /* VK_1          0x31 */ "1",
    /* VK_2          0x32 */ "2",
    /* VK_3          0x33 */ "3",
    /* VK_4          0x34 */ "4",
    /* VK_5          0x35 */ "5",
    /* VK_6          0x36 */ "6",
    /* VK_7          0x37 */ "7",
    /* VK_8          0x38 */ "8",
    /* VK_9          0x39 */ "9",
    /* ??            0x3A */ "",
    /* ??            0x3B */ "",
    /* ??            0x3C */ "",
    /* ??            0x3D */ "",
    /* ??            0x3E */ "",
    /* ??            0x3F */ "",
    /* ??            0x40 */ "",
    /* VK_A          0x41 */ "A",
    /* VK_B          0x42 */ "B",
    /* VK_C          0x43 */ "C",
    /* VK_D          0x44 */ "D",
    /* VK_E          0x45 */ "E",
    /* VK_F          0x46 */ "F",
    /* VK_G          0x47 */ "G",
    /* VK_H          0x48 */ "H",
    /* VK_I          0x49 */ "I",
    /* VK_J          0x4A */ "J",
    /* VK_K          0x4B */ "K",
    /* VK_L          0x4C */ "L",
    /* VK_M          0x4D */ "M",
    /* VK_N          0x4E */ "N",
    /* VK_O          0x4F */ "O",
    /* VK_P          0x50 */ "P",
    /* VK_Q          0x51 */ "Q",
    /* VK_R          0x52 */ "R",
    /* VK_S          0x53 */ "S",
    /* VK_T          0x54 */ "T",
    /* VK_U          0x55 */ "U",
    /* VK_V          0x56 */ "V",
    /* VK_W          0x57 */ "W",
    /* VK_X          0x58 */ "X",
    /* VK_Y          0x59 */ "Y",
    /* VK_Z          0x5A */ "Z",
    /* VK_LWIN       0x5B */ "",
    /* VK_RWIN       0x5C */ "",
    /* VK_APPS       0x5D */ "APPS",      /* properties key  (Win95 kybd) */
    /* ??            0x5E */ "",
    /* ??            0x5F */ "",
    /* VK_NUMPAD0    0x60 */ "",
    /* VK_NUMPAD1    0x61 */ "",
    /* VK_NUMPAD2    0x62 */ "",
    /* VK_NUMPAD3    0x63 */ "",
    /* VK_NUMPAD4    0x64 */ "",
    /* VK_NUMPAD5    0x65 */ "",
    /* VK_NUMPAD6    0x66 */ "",
    /* VK_NUMPAD7    0x67 */ "",
    /* VK_NUMPAD8    0x68 */ "",
    /* VK_NUMPAD9    0x69 */ "",
    /* VK_MULTIPLY   0x6A */ "MULTIPLY",  /* keypad */
    /* VK_ADD        0x6B */ "ADD",       /* keypad */
    /* VK_SEPARATOR  0x6C */ "",
    /* VK_SUBTRACT   0x6D */ "SUBTRACT",  /* keypad */
    /* VK_DECIMAL    0x6E */ "DECIMAL",   /* keypad */
    /* VK_DIVIDE     0x6F */ "DIVIDE",    /* keypad */
    /* VK_F1         0x70 */ "F1",
    /* VK_F2         0x71 */ "F2",
    /* VK_F3         0x72 */ "F3",
    /* VK_F4         0x73 */ "F4",
    /* VK_F5         0x74 */ "F5",
    /* VK_F6         0x75 */ "F6",
    /* VK_F7         0x76 */ "F7",
    /* VK_F8         0x77 */ "F8",
    /* VK_F9         0x78 */ "F9",
    /* VK_F10        0x79 */ "F10",
    /* VK_F11        0x7A */ "F11",
    /* VK_F12        0x7B */ "F12",
    /* VK_F13        0x7C */ "F13",
    /* VK_F14        0x7D */ "F14",
    /* VK_F15        0x7E */ "F15",
    /* VK_F16        0x7F */ "F16",
    /* VK_F17        0x80 */ "F17",
    /* VK_F18        0x81 */ "F18",
    /* VK_F19        0x82 */ "F19",
    /* VK_F20        0x83 */ "F20",
    /* VK_F21        0x84 */ "F21",
    /* VK_F22        0x85 */ "F22",
    /* VK_F23        0x86 */ "F23",
    /* VK_F24        0x87 */ "F24",
    /* ??            0x88 */ "",
    /* ??            0x89 */ "",
    /* ??            0x8A */ "",
    /* ??            0x8B */ "",
    /* ??            0x8C */ "",
    /* ??            0x8D */ "",
    /* ??            0x8E */ "",
    /* ??            0x8F */ "",
    /* VK_NUMLOCK    0x90 */ "NUMLOCK",
    /* VK_SCROLL     0x91 */ "SCROLL",
};



int
chgd_redir_keys(VALARGS *unused1, int unused2, int testing)
{
    if (! testing)
        rebuild_cache_tbl = TRUE;
    return (TRUE);
}



/* Return FALSE, which indicates redirection has been disabled. */
static int
invalid_keylist(void)
{
    ntwinio_redirect_hwnd(FALSE);

    /* Killed keyboard redirection, so make sure user sees error */
    MessageBox((HWND) winvile_hwnd(),
               "invalid keylist syntax, redirection disabled",
               prognam,
               MB_OK|MB_ICONSTOP);
    return (FALSE);
}



/* Return FALSE, which indicates redirection has been disabled. */
static int
unsupported_key(char *key)
{
    char msg[128];

    ntwinio_redirect_hwnd(FALSE);
    sprintf(msg, "VK_%s redir unsupported, redirection disabled", key);

    /* Killed keyboard redirection, so make sure user sees error */
    MessageBox((HWND) winvile_hwnd(), msg, prognam, MB_OK|MB_ICONSTOP);
    return (FALSE);
}



static int
execute_action(int action)
{
    int rc = TRUE;

    if (action & FLUSH_BUFFERS)
        rc = writeall(FALSE, 1, FALSE, FALSE, FALSE);
    if (rc && (action & SWITCH_FOCUS))
        (void) SetForegroundWindow(redirect_hwnd);
    return (rc);
}



/*
 * FUNCTION
 *   build_keylist(void)
 *
 * DESCRIPTION
 *   Extract a list of redirection keys extracted from a vile "mode" and
 *   turn same into a dynamic, binary data structure.  The redirection list
 *   must be in the following format:
 *
 *      <keyspec>,...
 *
 *   where
 *
 *      <keyspec>     :== <virt_key>:[<modifier>...]:[<action>...],
 *      <virt_key>    :== virtual keycode macro name sans "VK_".  Refer to
 *                        include file winuser.h for a list of virtual keycode
 *                        names, or refer to the keymap array above.
 *      <modifier>    :== S|C|A         <-- mnemonics for shift, control, alt
 *      <action>      :== <flush>|<switch>
 *      <flush>       :== F             <-- flush all modified buffers to
 *                                          disk prior to redirecting key
 *      <switch>      :== S             <-- switch focus to redirected window
 *                                          prior to redirecting key
 * RETURNS
 *   Boolean, T -> list successfully parsed and built.
 */

static int
build_keylist(void)
{
    char      *codestr, *redirlist, *tmp;
    REDIR_KEY *lcl_tbl;

    if (cache_tbl)
    {
        free(cache_tbl);      /* Blow away old table. */
        cache_tbl = NULL;
    }

    /* Count number of items to place in table. */
    cache_tbl_nelem = 1;
    redirlist       = tmp = global_g_val_ptr(GVAL_REDIRECT_KEYS);
    do
    {
        if ((tmp = strchr(tmp, ',')) != NULL)
        {
            cache_tbl_nelem++;
            tmp++;
        }
    }
    while (tmp);

    /* allocate the table */
    if ((lcl_tbl = cache_tbl = typecallocn(REDIR_KEY, cache_tbl_nelem)) == NULL)
    {
        no_memory("build_keylist()");
        return (FALSE);
    }

    cache_tbl_nelem = 0;
    while(isspace(*redirlist))
        redirlist++;
    while (*redirlist)
    {
        int  done, hit, indx;
        char **str, *delim;

        if ((delim = strchr(redirlist, ':')) == NULL)
            return (invalid_keylist());
        *delim = '\0';
        str    = keymap;
        for (hit = indx = 0; indx < TABLESIZE(keymap) && !hit; str++)
        {
            if (**str && stricmp(*str, redirlist) == 0)
                hit = 1;
            else
                indx++;
        }
        if (! hit)
        {
            (void) unsupported_key(redirlist);
            *delim = ':';
            return (FALSE);
        }
        lcl_tbl->vk = indx;   /* Found key desc, update virtual key code. */
        *delim++  = ':';
        redirlist = delim;
        while(isspace(*redirlist))
            redirlist++;
        for (done = 0; !done; redirlist++)
        {
            switch (*redirlist)
            {
                case ':':
                    done = 1;
                    break;
                case 'S':
                case 's':
                    lcl_tbl->modifier |= SHIFT_PRESSED;
                    break;
                case 'C':
                case 'c':
                    lcl_tbl->modifier |= (LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED);
                    break;
                case 'A':
                case 'a':
                    lcl_tbl->modifier |= (LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED);
                    break;
                default:
                    if (! isspace(*redirlist))
                        return (invalid_keylist());
                    break;
            }
        }
        while(isspace(*redirlist))
            redirlist++;
        for (done = 0; ! done; )
        {
            switch (*redirlist)
            {
                case ',':
                case '\0':
                    done = 1;
                    break;
                case 'F':
                case 'f':
                    lcl_tbl->action |= FLUSH_BUFFERS;
                    break;
                case 'S':
                case 's':
                    lcl_tbl->action |= SWITCH_FOCUS;;
                    break;
                default:
                    if (! isspace(*redirlist))
                        return (invalid_keylist());
                    break;
            }
            if (*redirlist)
                redirlist++;
        }
        cache_tbl_nelem++;
        lcl_tbl++;
        while(isspace(*redirlist))
            redirlist++;
    }
    rebuild_cache_tbl = FALSE;
    return (TRUE);
}



/*
 * FUNCTION
 *   oleauto_redirected_key(ULONG vk, ULONG modifier)
 *
 *   vk       - standard win32 virtual keycode.
 *
 *   modifier - win32 key state specifying which, if any, key modifiers
 *              were pressed (i.e., ALT, CTRL, SHIFT).
 *
 * DESCRIPTION
 *   Given a user keypress (and possible key modifier), see if it matches
 *   any of the keys selected for redirection to the winvile redirect
 *   HWND.  If a match, do the redirection after executing associated
 *   actions (if any).
 *
 * RETURNS
 *   Boolean, T -> key redirected.
 */

int
oleauto_redirected_key(ULONG vk, ULONG modifier)
{
    int       attached, i, hit;
    REDIR_KEY *lcl_tbl;

    if (vk >= TABLESIZE(keymap))
        return (FALSE);
    if (*(keymap[vk]) == '\0')
        return (FALSE);              /* Can't redirect that key. */
    if (rebuild_cache_tbl && ! build_keylist())
    {
        /*
         * Bogus keylist -- error already reported and redirection
         * disabled.  Return TRUE to signal the winvile message pump that
         * key has been processed (in this case, discarded).
         */

        return (TRUE);
    }

    /* Is user's vk in the list of winvile redirected keys? */
    for (hit = i = 0, lcl_tbl = cache_tbl;
                        !hit && i < cache_tbl_nelem; i++)
    {
        if (lcl_tbl->vk == vk && modifier == lcl_tbl->modifier)
            hit = 1;
        else
            lcl_tbl++;
    }
    if (! hit)
        return (FALSE);              /* Key not selected for redirection. */

    /* If this key has an associated action, execute it now. */
    if (lcl_tbl->action && ! execute_action(lcl_tbl->action))
    {
        /*
         * An action failed and an error has been reported.  Return TRUE to
         * signal the winvile message pump that key has been processed (in
         * this case, discarded).
         */

        return (TRUE);
    }

    attached = FALSE;
    if (modifier)
    {
        /*
         * It's not possible to send a key modifier to another window's
         * message queue.  Instead, that window's key state must be directly
         * modified.
         */

        BYTE keystate[256];
        BOOL ok;

        if ((ok = AttachThreadInput(winvile_tid, redirect_tid, TRUE)) == TRUE)
        {
            attached = TRUE;
            if ((ok = GetKeyboardState(keystate)) == TRUE)
            {
                if (modifier & (LEFT_CTRL_PRESSED|RIGHT_CTRL_PRESSED))
                    keystate[VK_CONTROL] = 1;
                if (modifier & SHIFT_PRESSED)
                    keystate[VK_SHIFT] = 1;
                if (modifier & (LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED))
                    keystate[VK_MENU] = 1;
                ok = SetKeyboardState(keystate);
            }
        }
        if (! ok)
        {
            char msg[128];

            ntwinio_redirect_hwnd(FALSE);
            if (attached)
                (void) AttachThreadInput(winvile_tid, redirect_tid, FALSE);
            sprintf(msg,
              "Input attachment to thread ID %#0x failed, redirection disabled",
                    redirect_tid);

            /* Killed keyboard redirection, so make sure user sees error */
            MessageBox((HWND) winvile_hwnd(), msg, prognam, MB_OK|MB_ICONSTOP);

            /*
             * Return TRUE to signal the winvile message pump that key has
             * been processed (in this case, discarded).
             */
            return (TRUE);
        }
    }
    if (! PostMessage(redirect_hwnd, WM_KEYDOWN, vk, 0))
    {
        char msg[128];

        ntwinio_redirect_hwnd(FALSE);
        sprintf(msg,
        "Keyboard redirection to window hwnd %#0x failed, redirection disabled",
                redirect_hwnd);

        /* Killed keyboard redirection, so make sure user sees error */
        MessageBox((HWND) winvile_hwnd(), msg, prognam, MB_OK|MB_ICONSTOP);
    }
    if (attached)
    {
        /*
         * Detach.  AttachThreadInput() resets the previously manipulated
         * keyboard state.
         */

        (void) AttachThreadInput(winvile_tid, redirect_tid, FALSE);
    }

    /*
     * Return TRUE to signal the winvile message pump that key has either
     * been processed or discarded.
     */
    return (TRUE);
}

} /* Extern "C" */
