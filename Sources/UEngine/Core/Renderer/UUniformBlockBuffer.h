#pragma once
#include "URenderer.h"

class UUniformBlockBuffer : public UGLObject
{
public:

    class UUniformBlock
    {
        public:
        
        unsigned int count;

        vector<unsigned int> index;
        vector<int> offset;
        unsigned int blockIndex;
        int    blockSize;
    
    };

    void Initialize(){}

    UUniformBlockBuffer(void);
    virtual ~UUniformBlockBuffer(void);
};