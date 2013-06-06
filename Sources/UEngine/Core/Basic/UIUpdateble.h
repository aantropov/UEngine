#pragma once

class UIUpdateble
{
protected:
public:
    
    virtual void Update(double delta) = 0;

    UIUpdateble(void);
    virtual ~UIUpdateble(void);
};
