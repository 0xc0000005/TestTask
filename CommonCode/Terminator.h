#pragma once
#include "Interfaces.h"

class CTerminator :
    public ITerminator
{
public:

    CTerminator() : m_terminated(false) {}
    ~CTerminator() {}

    virtual void Terminate() { m_terminated = true; }
    virtual bool IsTerminated() { return m_terminated; }

private:
    volatile bool m_terminated;
};

