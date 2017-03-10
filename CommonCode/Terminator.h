#pragma once
#include "Interfaces.h"

class CTerminator :
    public ITerminator
{
public:

    CTerminator() {}
    ~CTerminator() {}

    virtual void Terminate() override { m_terminated = true; }
    virtual bool IsTerminated() override { return m_terminated; }

private:
    volatile bool m_terminated = false;
};

