//
//  Config.h
//  nyangame
//
//  Created by tomi on 7/3/13.
//
//

#ifndef __nyangame__Config__
#define __nyangame__Config__

enum kBlock
{
    kBlockRed,
    kBlockBlue,
    kBlockYellow,
    kBlockGreen,
    kBlockGray,
    kBlockcount,
};

#define ccsf(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

#endif /* defined(__nyangame__Config__) */
