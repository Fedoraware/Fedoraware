#pragma once
#include "dt_recv_redef.h"
class ProxyFnHook
{
public:
    ProxyFnHook() : hooked(false)
    {
    }
    void init(RecvPropRedef* prop)
    {
        this->prop = prop;
    }
    void setHook(RecvVarProxyFn new_fn)
    {
        if (!prop || hooked)
            return;
        hooked = true;
        original_fn = prop->m_ProxyFn;
        prop->m_ProxyFn = new_fn;
    }
    void restore()
    {
        if (prop && hooked)
        {
            prop->m_ProxyFn = original_fn;
            hooked = false;
        }
    }

private:
    bool hooked;
    RecvPropRedef* prop;
    RecvVarProxyFn original_fn;
};
