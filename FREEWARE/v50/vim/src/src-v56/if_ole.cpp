/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
#ifdef HAVE_OLE

#include <windows.h>
#include <oleauto.h>

extern "C" {
#include "vim.h"
}

#include "if_ole.h"	// Interface definitions
#include "iid_ole.c"	// UUID definitions (compile here)

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

class CVim;
class CVimCF;

/* Internal data */
// The identifier of the registered class factory
static unsigned long cf_id = 0;

// The identifier of the running application object
static unsigned long app_id = 0;

// The single global instance of the class factory
static CVimCF *cf = 0;

// The single global instance of the application object
static CVim *app = 0;

/* GUIDs, versions and type library information */
#define MYCLSID CLSID_Vim
#define MYLIBID LIBID_Vim
#define MYIID IID_IVim

#define MAJORVER 1
#define MINORVER 0
#define LOCALE 0x0409

#define MYNAME "Vim"
#define MYPROGID "Vim.Application.1"
#define MYVIPROGID "Vim.Application"

#define MAX_CLSID_LEN 100

/*****************************************************************************
 2. The application object
*****************************************************************************/

/* Definition
 * ----------
 */

class CVim : public IVim
{
public:
    ~CVim();
    static CVim *Create(int* pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, void ** ppv);
    STDMETHOD_(unsigned long, AddRef)(void);
    STDMETHOD_(unsigned long, Release)(void);

    // IDispatch members
    STDMETHOD(GetTypeInfoCount)(UINT *pCount);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(const IID& iid, OLECHAR** names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, const IID& iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(SendKeys)(BSTR keys);
    STDMETHOD(Eval)(BSTR expr, BSTR *result);
    STDMETHOD(SetForeground)(void);

private:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    unsigned long ref;

    // The object's TypeInfo
    ITypeInfo *typeinfo;
};

/* Implementation
 * --------------
 */

CVim *CVim::Create(int* pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = new CVim();
    if (me == NULL)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	return NULL;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);

    if (FAILED(hr))
    {
	delete me;
	if (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
	    RegisterMe();
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.", "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	}
	return NULL;
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    if (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information", "Vim Initialisation", 0);
	delete me;
	return NULL;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    return me;
}

CVim::~CVim()
{
    if (typeinfo)
	typeinfo->Release();
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = this;
	return S_OK;
    }

    *ppv = 0;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    return ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    if (ref > 0)
	--ref;
    return ref;
}

STDMETHODIMP
CVim::GetTypeInfoCount(UINT *pCount)
{
    *pCount = 1;
    return S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    if (iTypeInfo != 0)
	return DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    return S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	const IID& iid,
	OLECHAR** names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    if (iid != IID_NULL)
	return DISP_E_UNKNOWNINTERFACE;

    return typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	const IID& iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    if (iid != IID_NULL)
	return DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, NULL);
    return typeinfo->Invoke(static_cast<IDispatch*>(this),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::SetForeground(void)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    return S_OK;
}

STDMETHODIMP
CVim::SendKeys(BSTR keys)
{
    int len;
    char *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, 0, 0, 0, 0);
    buffer = (char *)alloc(len+1);

    if (buffer == NULL)
	return E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, buffer, len, 0, 0);

    if (len == 0)
    {
	vim_free(buffer);
	return E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, FALSE, TRUE);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    if (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more that 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    if ((int)(STRLEN(str)) > (vim_free_in_input_buf() - 10))
    {
	vim_free(str);
	return E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.
     */
    PostMessage(NULL, WM_OLE, 0, (LPARAM)str);

    return S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR expr, BSTR *result)
{
#ifdef WANT_EVAL
    size_t len;
    char *buffer;
    char *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, 0, 0, 0, 0);
    buffer = (char *)alloc(len);

    if (buffer == NULL)
	return E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, buffer, len, 0, 0);
    if (len == 0)
	return E_INVALIDARG;

    /* Evaluate the expression */
    str = (char *)eval_to_string((char_u *)buffer, NULL);
    vim_free(buffer);

    /* Get a suitable buffer to store the result in wide characters */
    len = MultiByteToWideChar(CP_ACP, 0, str, -1, 0, 0);
    w_buffer = (wchar_t *)alloc(len * sizeof(wchar_t));

    if (w_buffer == NULL)
    {
	vim_free(str);
	return E_OUTOFMEMORY;
    }

    /* Convert the result to wide characters */
    len = MultiByteToWideChar(CP_ACP, 0, str, -1, w_buffer, len);
    vim_free(str);

    if (len == 0)
    {
	vim_free(w_buffer);
	return E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    return S_OK;
#else
    return E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

class CVimCF : public IClassFactory
{
public:
    static CVimCF *Create();

    STDMETHOD(QueryInterface)(REFIID riid, void ** ppv);
    STDMETHOD_(unsigned long, AddRef)(void);
    STDMETHOD_(unsigned long, Release)(void);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, void ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

private:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    unsigned long ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = new CVimCF();

    if (me == NULL)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    return me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = this;
	return S_OK;
    }

    *ppv = 0;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    return ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    if (ref > 0)
	--ref;
    return ref;
}

STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv)
{
    return app->QueryInterface(riid, ppv);
}

STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    return S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
static void SetKeyAndValue(const char* path, const char* subkey, const char* value);
static void GUIDtochar(const GUID& guid, char* GUID, int length);
static void RecursiveDeleteKey(HKEY hKeyParent, const char* child);
static const int GUID_STRING_SIZE = 39;

// Register the component in the registry
//
// Note: There is little error checking in this code at present.

extern "C" void RegisterMe()
{
    BOOL ok = TRUE;

    // Get the application startup command
    char module[MAX_PATH];

    ::GetModuleFileName(NULL, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    char clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, sizeof(clsid));

    // Convert the LIBID into a char
    char libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, sizeof(libid));

    // Build the key CLSID\\{...}
    char Key[MAX_CLSID_LEN];
    strcpy(Key, "CLSID\\");
    strcat(Key, clsid);

    // Add the CLSID to the registry
    SetKeyAndValue(Key, NULL, MYNAME);
    SetKeyAndValue(Key, "LocalServer32", module);
    SetKeyAndValue(Key, "ProgID", MYPROGID);
    SetKeyAndValue(Key, "VersionIndependentProgID", MYVIPROGID);
    SetKeyAndValue(Key, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    SetKeyAndValue(MYVIPROGID, NULL, MYNAME);
    SetKeyAndValue(MYVIPROGID, "CLSID", clsid);
    SetKeyAndValue(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    SetKeyAndValue(MYPROGID, NULL, MYNAME);
    SetKeyAndValue(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = NULL;
    if (FAILED(LoadTypeLib(w_module, &typelib)))
    {
	MessageBox(0, "Cannot load type library to register", "Vim Registration", 0);
	ok = FALSE;
    }
    else
    {
	if (FAILED(RegisterTypeLib(typelib, w_module, NULL)))
	{
	    MessageBox(0, "Cannot register type library", "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    if (ok)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
extern "C" void UnregisterMe(int bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    if (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	if (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->wMajorVerNum, tla->wMinorVerNum,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    char clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, sizeof(clsid));

    // Build the key CLSID\\{...}
    char Key[MAX_CLSID_LEN];
    strcpy(Key, "CLSID\\");
    strcat(Key, clsid);

    // Delete the CLSID Key - CLSID\{...}
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, Key);

    // Delete the version-independent ProgID Key
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, MYVIPROGID);

    // Delete the ProgID key
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, MYPROGID);

    if (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
static void GUIDtochar(const GUID& guid, char* GUID, int length)
{
    // Get wide string version
    LPOLESTR wGUID = NULL;
    StringFromCLSID(guid, &wGUID);

    // Covert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendents
static void RecursiveDeleteKey(HKEY hKeyParent, const char* child)
{
    // Open the child
    HKEY hKeyChild;
    LONG result = RegOpenKeyEx(hKeyParent, child, 0, KEY_ALL_ACCESS, &hKeyChild);
    if (result != ERROR_SUCCESS)
	return;

    // Enumerate all of the decendents of this child
    FILETIME time;
    char buffer[1024];
    DWORD size = 1024;

    while (RegEnumKeyEx(hKeyChild, 0, buffer, &size, NULL,
			NULL, NULL, &time) == S_OK)
    {
	// Delete the decendents of this child
	RecursiveDeleteKey(hKeyChild, buffer);
	size = 256;
    }

    // Close the child
    RegCloseKey(hKeyChild);

    // Delete this child
    RegDeleteKey(hKeyParent, child);
}

// Create a key and set its value
static void SetKeyAndValue(const char* key, const char* subkey, const char* value)
{
    HKEY hKey;
    char buffer[1024];

    strcpy(buffer, key);

    // Add subkey name to buffer.
    if (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = RegCreateKeyEx(HKEY_CLASSES_ROOT,
				 buffer,
				 0, NULL, REG_OPTION_NON_VOLATILE,
				 KEY_ALL_ACCESS, NULL,
				 &hKey, NULL);
    if (result != ERROR_SUCCESS)
	return;

    // Set the value
    if (value)
	RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)value, strlen(value)+1);

    RegCloseKey(hKey);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
extern "C" void InitOLE(int* pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(NULL);
    if (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	goto error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    if (app == NULL)
	goto error1;

    // Create the class factory
    cf = CVimCF::Create();
    if (cf == NULL)
	goto error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    if (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	goto error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	NULL,
	&app_id);

    if (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	goto error1;
    }

    return;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    return;
}

extern "C" void UninitOLE()
{
    // Unregister the application object
    if (app_id)
    {
	RevokeActiveObject(app_id, NULL);
	app_id = 0;
    }

    // Unregister the class factory
    if (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    if (app)
    {
	delete app;
	app = NULL;
    }

    // Delete the class factory
    if (cf)
    {
	delete cf;
	cf = NULL;
    }
}
#endif /* HAVE_OLE */
