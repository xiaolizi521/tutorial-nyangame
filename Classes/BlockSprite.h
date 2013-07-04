//
//  BlockSprite.h
//  nyangame
//
//  Created by tomi on 7/3/13.
//
//

#ifndef __nyangame__BlockSprite__
#define __nyangame__BlockSprite__

#include "cocos2d.h"
#include "Config.h"

class BlockSprite : public cocos2d::CCSprite {
protected:
    const char* getBlockImageFileName(kBlock blockType);
    
public:
    CC_SYNTHESIZE_READONLY(kBlock, m_blockType, BlockType);
    CC_SYNTHESIZE_READONLY(int, m_nextPosX, NextPosX);
    CC_SYNTHESIZE_READONLY(int, m_nextPosY, NextPosY);
    
    void initNextPos();
    void setNextPos(int nextPosX, int nextPosY);
    
    BlockSprite();
    virtual ~BlockSprite();
    
    virtual bool initWithBlockType(kBlock blockType);
    static BlockSprite* createWithBlockType(kBlock blockType);
};

#endif /* defined(__nyangame__BlockSprite__) */
