#pragma once
#include "URenderer.h"
#include "UBuffer.h"

class UIndexBuffer : public UBuffer
{
    unsigned int num_indices;    
    unsigned int *indices;
    
public:
    
    void* GetPointer() {return (void*)indices;}
    int GetNum() { return num_indices; }
        
    void Create(int num_faces)
    {
        Free();
        num_indices = num_faces*3;
        indices = new unsigned int[num_indices]();
    }
        
    bool Initialize()
    {
        _id = -1;
        _id = URenderer::GetInstance()->CreateVBO(this, UVBO_STATIC );        
        return (_id != -1);
    }
        
    void Free() 
    {
        if(_id != -1)
            URenderer::GetInstance()->DeleteVBO(this);    
        _id = -1;
        delete[] indices;
    }

    void* Lock()
    {
        glBindBuffer(GL_ARRAY_BUFFER, UGLObject::_id);
        glBufferData(GL_ARRAY_BUFFER, num_indices * sizeof(indices), 0, GL_STREAM_DRAW_ARB);
        UVertex* pBuffer = (UVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_ARB);            
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return pBuffer;
    }

    void Unlock()
    {        
        glBindBuffer(GL_ARRAY_BUFFER, UGLObject::_id);
        GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    UIndexBuffer(void);
    virtual ~UIndexBuffer(void);
};