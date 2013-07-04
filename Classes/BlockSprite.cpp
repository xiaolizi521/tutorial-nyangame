//
//  BlockSprite.cpp
//  nyangame
//
//  Created by tomi on 7/3/13.
//
//

#include "BlockSprite.h"

BlockSprite::BlockSprite() {
    
}

BlockSprite::~BlockSprite() {
    
}

BlockSprite* BlockSprite::createWithBlockType(kBlock blockType) {
    BlockSprite *pRet = new BlockSprite();
    if (pRet && pRet->initWithBlockType(blockType)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool BlockSprite::initWithBlockType(kBlock blockType) {
    if (!CCSprite::initWithFile(getBlockImageFileName(blockType))) {
        return false;
    }
    
    m_blockType = blockType;
    
    return true;
}
const char* BlockSprite::getBlockImageFileName(kBlock blockType) {
    switch (blockType) {
        case kBlockRed:
            return "red.png";

        case kBlockBlue:
            return "blue.png";

        case kBlockYellow:
            return "yellow.png";
            
        case kBlockGreen:
            return "green.png";
            
        case kBlockGray:
            return "gray.png";

        default:
            CCAssert(false, "invalid blockType");
            break;
    }
}

