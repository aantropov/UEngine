#pragma once

class UGLObject
{
protected:

    int _id;

public:

    virtual int GetId() { return _id; }
    void SetId(unsigned int id) { _id = (int)id; }
    bool IsInitialized(){ return _id > 0;}

    UGLObject(void);
    virtual ~UGLObject(void) { _id = -1; } 
};