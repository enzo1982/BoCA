#pragma once

namespace APE
{

/**************************************************************************************************
CSmartPtr - a simple smart pointer class that can automatically initialize and free memory
    note: (doesn't do garbage collection / reference counting because of the many pitfalls)
**************************************************************************************************/
template <class TYPE> class CSmartPtr
{
public:
    TYPE * m_pObject;
    bool m_bArray;
    bool m_bDelete;

    __forceinline CSmartPtr()
    {
        m_bDelete = true;
        m_pObject = APE_NULL;
        m_bArray = false;
    }
    __forceinline CSmartPtr(TYPE * pObject, bool bArray = false, bool bDelete = true)
    {
        m_bDelete = true;
        m_pObject = APE_NULL;
        m_bArray = false;
        Assign(pObject, bArray, bDelete);
    }
    __forceinline CSmartPtr(int64 nElements, bool bEmpty = false)
    {
        m_bDelete = true;
        m_pObject = APE_NULL;
        m_bArray = false;
        AllocateArray(nElements, bEmpty);
    }

    __forceinline ~CSmartPtr()
    {
        Delete();
    }

    __forceinline void Assign(TYPE * pObject, bool bArray = false, bool bDelete = true)
    {
        Delete();

        m_bDelete = bDelete;
        m_bArray = bArray;
        m_pObject = pObject;
    }

    __forceinline void AllocateArray(int64 nElements, bool bEmpty = false)
    {
        Delete();

        Assign(new TYPE [static_cast<size_t>(nElements)], true);

        if (bEmpty)
        {
            memset(GetPtr(), 0, static_cast<size_t>(nElements) * sizeof(TYPE));
        }
    }

    __forceinline void Delete()
    {
        if (m_pObject)
        {
            TYPE * pObject = m_pObject;
            m_pObject = APE_NULL;

            if (m_bDelete)
            {
                if (m_bArray)
                    delete [] pObject;
                else
                    delete pObject;
            }
        }
    }

    void SetDelete(const bool bDelete)
    {
        m_bDelete = bDelete;
    }

    __forceinline TYPE * GetPtr() const
    {
        return m_pObject;
    }

    __forceinline operator TYPE * () const
    {
        return m_pObject;
    }

    __forceinline TYPE * operator ->() const
    {
        return m_pObject;
    }

private:
    // declare assignment, but mark it private so it can't be used
    // that way we can't carelessly mix smart pointers and regular pointers
    __forceinline void * operator =(void *) const { return APE_NULL; }
};

}
