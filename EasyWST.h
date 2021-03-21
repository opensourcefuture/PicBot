#ifndef __ESAYWST_H__
#define __ESAYWST_H__
#ifdef __cplusplus

#ifdef __GNUC__
#define __declspec(dllexport)
#endif

extern "C"
{
#endif
    void __declspec(dllexport) Bot_Connect(const char * ws_url,void((*callback)(const char *,void *)),void * funData);
    void __declspec(dllexport) Bot_Send(const char * msg);
    int __declspec(dllexport) Bot_IsConnect();
#ifdef __cplusplus
}
#endif
#endif
