/******************************************************************************
 * Copyright © 2014-2019 The SuperNET Developers.                             *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * SuperNET software, including this file may be copied, modified, propagated *
 * or distributed except according to the terms contained in the LICENSE file *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/


#ifndef komodo_rpcblockchain_h
#define komodo_rpcblockchain_h

#include <node/context.h>
#include <rpc/request.h>
#include <validation.h>

int32_t komodo_MoMdata(int32_t *notarized_htp,uint256 *MoMp,uint256 *kmdtxidp,int32_t height,uint256 *MoMoMp,int32_t *MoMoMoffsetp,int32_t *MoMoMdepthp,int32_t *kmdstartip,int32_t *kmdendip);
uint256 komodo_calcMoM(int32_t height,int32_t MoMdepth);
extern char ASSETCHAINS_SYMBOL[65];
uint32_t DPOWCONFS = 1;
extern int32_t NOTARIZED_HEIGHT;

int32_t komodo_dpowconfs(int32_t txheight,int32_t numconfs)
{
    // DPoW confs are on by default
    int32_t dpowconfs = 1;
    DPOWCONFS = gArgs.GetArg("-dpowconfs",dpowconfs);
    if ( DPOWCONFS != 0 && txheight > 0 && numconfs > 0 )
    {
        if ( NOTARIZED_HEIGHT > 0 )
        {
            if ( txheight < NOTARIZED_HEIGHT )
                return(numconfs);
            else return(1);
        } else return(1);
    }
    return(numconfs);
}

int32_t komodo_MoM(int32_t *notarized_heightp,uint256 *MoMp,uint256 *kmdtxidp,int32_t nHeight,uint256 *MoMoMp,int32_t *MoMoMoffsetp,int32_t *MoMoMdepthp,int32_t *kmdstartip,int32_t *kmdendip)
{
    int32_t depth,notarized_ht; uint256 MoM,kmdtxid;
    depth = komodo_MoMdata(&notarized_ht,&MoM,&kmdtxid,nHeight,MoMoMp,MoMoMoffsetp,MoMoMdepthp,kmdstartip,kmdendip);
    memset(MoMp,0,sizeof(*MoMp));
    memset(kmdtxidp,0,sizeof(*kmdtxidp));
    *notarized_heightp = 0;
    if ( depth > 0 && notarized_ht > 0 && nHeight > notarized_ht-depth && nHeight <= notarized_ht )
    {
        *MoMp = MoM;
        *notarized_heightp = notarized_ht;
        *kmdtxidp = kmdtxid;
    }
    return(depth);
}

#endif /* komodo_rpcblockchain_h */
