// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util/system.h>

#define ASSETCHAINS_BLOCKTIME 10
#define T 10
#define K ((int64_t)1000000)

#ifdef original_algo
arith_uint256 oldRT_CST_RST(int32_t height, uint32_t nTime, arith_uint256 bnTarget, uint32_t* ts, arith_uint256* ct, int32_t numerator, int32_t denominator, int32_t W, int32_t past)
{

    int64_t altK;
    int32_t i, j, k, ii = 0;
    if (height < 64)
        return (bnTarget);

    if ((int32_t)(ts[0] - ts[W]) < (int32_t)(T * numerator) / denominator) {

        bnTarget = ct[0] / arith_uint256(K);

        altK = (K * (nTime - ts[0]) * (ts[0] - ts[W]) * denominator) / (numerator * (T * T));
        fprintf(stderr, "ht.%d initial altK.%lld %d * %d * %d / %d\n", height, (long long)altK, (nTime - ts[0]), (ts[0] - ts[W]), denominator, numerator);
        if (altK > K)
            altK = K;
        bnTarget *= arith_uint256(altK);
        if (altK < K)
            return (bnTarget);
    }

    for (j = past - 1; j >= 2; j--) {
        if (ts[j] - ts[j + W] < T * numerator / denominator) {
            ii = 0;
            for (i = j - 2; i >= 0; i--) {
                ii++;

                if ((ts[i] - ts[j + W]) > (ii + W) * T)
                    break;

                if (i == 0) {

                    bnTarget = ct[0];
                    for (k = 1; k < W; k++)
                        bnTarget += ct[k];
                    bnTarget /= arith_uint256(W * K);
                    altK = (K * (nTime - ts[0]) * (ts[0] - ts[W])) / (W * T * T);
                    fprintf(stderr, "ht.%d made it to i == 0, j.%d ii.%d altK %lld (%d * %d) %u - %u W.%d\n", height, j, ii, (long long)altK, (nTime - ts[0]), (ts[0] - ts[W]), ts[0], ts[W], W);
                    bnTarget *= arith_uint256(altK);
                    j = 0;
                }
            }
        }
    }
    return (bnTarget);
}
#endif

arith_uint256 RT_CST_RST_outer(int32_t height, uint32_t nTime, arith_uint256 bnTarget, uint32_t* ts, arith_uint256* ct, int32_t numerator, int32_t denominator, int32_t W, int32_t past)
{
    int64_t outerK;
    arith_uint256 mintarget = bnTarget / arith_uint256(2);
    if ((int32_t)(ts[0] - ts[W]) < (int32_t)(T * numerator) / denominator) {
        outerK = (K * (nTime - ts[0]) * (ts[0] - ts[W]) * denominator) / (numerator * (T * T));
        if (outerK < K) {
            bnTarget = ct[0] / arith_uint256(K);
            bnTarget *= arith_uint256(outerK);
        }
        if (bnTarget > mintarget)
            bnTarget = mintarget;
        {
            int32_t z;
            for (z = 31; z >= 0; z--)
                fprintf(stderr, "%02x", ((uint8_t*)&bnTarget)[z]);
        }
        fprintf(stderr, " ht.%d initial outerK.%lld %d * %d * %d / %d\n", height, (long long)outerK, (nTime - ts[0]), (ts[0] - ts[W]), denominator, numerator);
    }
    return (bnTarget);
}

arith_uint256 RT_CST_RST_target(int32_t height, uint32_t nTime, arith_uint256 bnTarget, uint32_t* ts, arith_uint256* ct, int32_t width)
{
    int32_t i;
    int64_t innerK;
    bnTarget = ct[0];
    for (i = 1; i < width; i++)
        bnTarget += ct[i];
    bnTarget /= arith_uint256(width * K);
    innerK = (K * (nTime - ts[0]) * (ts[0] - ts[width])) / (width * T * T);
    bnTarget *= arith_uint256(innerK);
    if (0) {
        int32_t z;
        for (z = 31; z >= 0; z--)
            fprintf(stderr, "%02x", ((uint8_t*)&bnTarget)[z]);
        fprintf(stderr, " ht.%d innerK %lld (%d * %d) %u - %u width.%d\n", height, (long long)innerK, (nTime - ts[0]), (ts[0] - ts[width]), ts[0], ts[width], width);
    }
    return (bnTarget);
}

arith_uint256 RT_CST_RST_inner(int32_t height, uint32_t nTime, arith_uint256 bnTarget, uint32_t* ts, arith_uint256* ct, int32_t W, int32_t outeri)
{
    arith_uint256 mintarget;
    int32_t expected, elapsed, width = outeri + W;
    expected = (width + 1) * T;
    if ((elapsed = (ts[0] - ts[width])) < expected) {
        mintarget = (bnTarget / arith_uint256(11)) * arith_uint256(10);
        bnTarget = RT_CST_RST_target(height, nTime, bnTarget, ts, ct, W);
        if (bnTarget > mintarget)
            bnTarget = mintarget;
        {
            int32_t z;
            for (z = 31; z >= 0; z--)
                fprintf(stderr, "%02x", ((uint8_t*)&bnTarget)[z]);
        }
        fprintf(stderr, " height.%d O.%-2d, W.%-2d width.%-2d %4d vs %-4d, deficit %4d tip.%d\n", height, outeri, W, width, (ts[0] - ts[width]), expected, expected - (ts[0] - ts[width]), nTime - ts[0]);
    }
    return (bnTarget);
}

arith_uint256 zawy_targetMA(arith_uint256 easy, arith_uint256 bnSum, int32_t num, int32_t numerator, int32_t divisor)
{
    bnSum /= arith_uint256(10 * num * num * divisor);
    bnSum *= arith_uint256(numerator);
    if (bnSum > easy)
        bnSum = easy;
    return (bnSum);
}

int64_t zawy_exponential_val360000(int32_t num)
{
    int32_t i, n, modval;
    int64_t A = 1, B = 3600 * 100;
    if ((n = (num / ASSETCHAINS_BLOCKTIME)) > 0) {
        for (i = 1; i <= n; i++)
            A *= 3;
    }
    if ((modval = (num % ASSETCHAINS_BLOCKTIME)) != 0) {
        B += (3600 * 110 * modval) / ASSETCHAINS_BLOCKTIME;
        B += (3600 * 60 * modval * modval) / (ASSETCHAINS_BLOCKTIME * ASSETCHAINS_BLOCKTIME);
    }
    return (A * B);
}

arith_uint256 zawy_exponential(arith_uint256 bnTarget, int32_t mult)
{
    bnTarget /= arith_uint256(100 * 3600);
    bnTarget *= arith_uint256(zawy_exponential_val360000(mult));
    return (bnTarget);
}

arith_uint256 zawy_ctB(arith_uint256 bnTarget, uint32_t solvetime)
{
    int64_t num;
    num = ((int64_t)1000 * solvetime * solvetime * 1000) / (T * T * 784);
    if (num > 1) {
        bnTarget /= arith_uint256(1000);
        bnTarget *= arith_uint256(num);
    }
    return (bnTarget);
}

arith_uint256 zawy_TSA_EMA(int32_t height, int32_t tipdiff, arith_uint256 prevTarget, int32_t solvetime)
{
    arith_uint256 A, B, C, bnTarget;
    if (tipdiff < 4)
        tipdiff = 4;
    tipdiff &= ~1;
    bnTarget = prevTarget / arith_uint256(K * T);
    A = bnTarget * arith_uint256(T);
    B = (bnTarget / arith_uint256(360000)) * arith_uint256(tipdiff * zawy_exponential_val360000(tipdiff / 2));
    C = (bnTarget / arith_uint256(360000)) * arith_uint256(T * zawy_exponential_val360000(tipdiff / 2));
    bnTarget = ((A + B - C) / arith_uint256(tipdiff)) * arith_uint256(K * T);
    {
        int32_t z;
        for (z = 31; z >= 0; z--)
            fprintf(stderr, "%02x", ((uint8_t*)&bnTarget)[z]);
    }
    fprintf(stderr, " ht.%d TSA bnTarget tipdiff.%d\n", height, tipdiff);
    return (bnTarget);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    if (pindexLast->nHeight + 1 <= params.nAdaptivePoWActivationThreshold) {

        assert(pindexLast != nullptr);
        unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

        if ((pindexLast->nHeight + 1) % params.DifficultyAdjustmentInterval() != 0) {
            if (params.fPowAllowMinDifficultyBlocks) {

                if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2)
                    return nProofOfWorkLimit;
                else {

                    const CBlockIndex* pindex = pindexLast;
                    while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                        pindex = pindex->pprev;
                    return pindex->nBits;
                }
            }
            return pindexLast->nBits;
        }

        int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval() - 1);
        assert(nHeightFirst >= 0);
        const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
        assert(pindexFirst);

        return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
    } else {

        arith_uint256 bnLimit;
        bnLimit = UintToArith256(params.powLimit);

        unsigned int nProofOfWorkLimit = bnLimit.GetCompact();

        if (pindexLast == NULL)
            return nProofOfWorkLimit;

        const CBlockIndex* pindexFirst = pindexLast;
        arith_uint256 ct[64]{0}, ctinv[64], bnTmp, bnPrev, bnTarget, bnTarget6, bnTarget12, bnTot { 0 };
        uint32_t nbits, blocktime, ts[sizeof(ct) / sizeof(*ct)];
        int32_t zflags[sizeof(ct) / sizeof(*ct)], i, diff, height = 0, mult = 0, tipdiff = 0;
        memset(ts, 0, sizeof(ts));
        memset(ctinv, 0, sizeof(ctinv));
        memset(zflags, 0, sizeof(zflags));
        if (pindexLast != 0)
            height = (int32_t)pindexLast->nHeight + 1;
        if (pindexFirst != 0 && pblock != 0 && height >= (int32_t)(sizeof(ct) / sizeof(*ct))) {
            tipdiff = (pblock->nTime - pindexFirst->nTime);
            mult = tipdiff - 7 * params.nPowTargetSpacing;
            bnPrev.SetCompact(pindexFirst->nBits);
            for (i = 0; pindexFirst != 0 && i < (int32_t)(sizeof(ct) / sizeof(*ct)); i++) {
                zflags[i] = (pindexFirst->nBits & 3);
                ct[i].SetCompact(pindexFirst->nBits);
                ts[i] = pindexFirst->nTime;
                pindexFirst = pindexFirst->pprev;
            }
            for (i = 0; pindexFirst != 0 && i < (int32_t)(sizeof(ct) / sizeof(*ct)) - 1; i++) {
                if (zflags[i] == 1 || zflags[i] == 2)
                    ct[i] = zawy_ctB(ct[i], ts[i] - ts[i + 1]);
            }
            if (0) {
                bnTarget = zawy_TSA_EMA(height, tipdiff, ct[0], ts[0] - ts[1]);
                nbits = bnTarget.GetCompact();
                nbits = (nbits & 0xfffffffc) | 0;
                return (nbits);
            }
        }
        pindexFirst = pindexLast;
        for (i = 0; pindexFirst && i < params.nPowAveragingWindow; i++) {
            bnTmp.SetCompact(pindexFirst->nBits);
            if (pblock != 0) {
                blocktime = pindexFirst->nTime;
                diff = (pblock->nTime - blocktime);

                if (i < 6) {
                    diff -= (8 + i) * params.nPowTargetSpacing;
                    if (diff > mult) {

                        mult = diff;
                    }
                }
                if (zflags[i] != 0 && zflags[0] != 0)
                    bnTmp = (ct[i] / arith_uint256(3));
            }
            bnTot += bnTmp;
            pindexFirst = pindexFirst->pprev;
        }

        if (pindexFirst == NULL)
            return nProofOfWorkLimit;

        bool fNegative, fOverflow;
        int32_t past, zawyflag = 0;
        arith_uint256 easy, origtarget, bnAvg { bnTot / params.nPowAveragingWindow };
        nbits = CalculateNextWorkRequired(bnAvg, pindexLast->GetMedianTimePast(), pindexFirst->GetMedianTimePast(), params);
        if (1) {
            bnTarget = arith_uint256().SetCompact(nbits);
            if (height > (int32_t)(sizeof(ct) / sizeof(*ct)) && pblock != 0 && tipdiff > 0) {
                easy.SetCompact(0x1e007fff & (~3), &fNegative, &fOverflow);
                if (pblock != 0) {
                    origtarget = bnTarget;
                    past = 20;
                    if (zflags[0] == 0 || zflags[0] == 3) {
                        bnTarget = RT_CST_RST_outer(height, pblock->nTime, bnTarget, ts, ct, 1, 2, 3, past);
                        if (bnTarget < origtarget)
                            zawyflag = 2;
                        else {
                            bnTarget = RT_CST_RST_outer(height, pblock->nTime, bnTarget, ts, ct, 7, 3, 6, past + 10);
                            if (bnTarget < origtarget)
                                zawyflag = 2;
                            else {
                                bnTarget = RT_CST_RST_outer(height, pblock->nTime, bnTarget, ts, ct, 12, 7, 12, past + 20);
                                if (bnTarget < origtarget)
                                    zawyflag = 2;
                            }
                        }
                    } else {
                        for (i = 0; i < 40; i++)
                            if (zflags[i] == 2)
                                break;
                        if (i < 40) {
                            bnTarget = RT_CST_RST_inner(height, pblock->nTime, bnTarget, ts, ct, 3, i);
                            bnTarget6 = RT_CST_RST_inner(height, pblock->nTime, bnTarget, ts, ct, 6, i);
                            bnTarget12 = RT_CST_RST_inner(height, pblock->nTime, bnTarget, ts, ct, 12, i);
                            if (bnTarget6 < bnTarget12)
                                bnTmp = bnTarget6;
                            else
                                bnTmp = bnTarget12;
                            if (bnTmp < bnTarget)
                                bnTarget = bnTmp;
                            if (bnTarget != origtarget)
                                zawyflag = 1;
                        }
                    }
                }
                if (mult > 1) {
                    origtarget = bnTarget;
                    bnTarget = zawy_exponential(bnTarget, mult);
                    if (bnTarget < origtarget || bnTarget > easy) {
                        bnTarget = easy;
                        fprintf(stderr, "cmp.%d mult.%d ht.%d -> easy target\n", mult > 1, (int32_t)mult, height);
                        return (0x1e007fff & (~3));
                    }
                    {
                        int32_t z;
                        for (z = 31; z >= 0; z--)
                            fprintf(stderr, "%02x", ((uint8_t*)&bnTarget)[z]);
                    }
                    fprintf(stderr, " exp() to the rescue cmp.%d mult.%d for ht.%d\n", mult > 1, (int32_t)mult, height);
                }
                if (0 && zflags[0] == 0 && zawyflag == 0 && mult <= 1) {
                    bnTarget = zawy_TSA_EMA(height, tipdiff, (bnTarget + ct[0] + ct[1]) / arith_uint256(3), ts[0] - ts[1]);
                    if (bnTarget < origtarget)
                        zawyflag = 3;
                }
            }
            nbits = bnTarget.GetCompact();
            nbits = (nbits & 0xfffffffc) | zawyflag;
        }
        return (nbits);
    }
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan / 4)
        nActualTimespan = params.nPowTargetTimespan / 4;
    if (nActualTimespan > params.nPowTargetTimespan * 4)
        nActualTimespan = params.nPowTargetTimespan * 4;

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;
    if (0) {
        int32_t i;
        for (i = 31; i >= 0; i--)
            printf("%02x", ((uint8_t*)&bnNew)[i]);

        for (i = 31; i >= 0; i--)
            printf("%02x", ((uint8_t*)&bnPowLimit)[i]);
    }
    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int CalculateNextWorkRequired(arith_uint256 bnAvg,
    int64_t nLastBlockTime, int64_t nFirstBlockTime,
    const Consensus::Params& params)
{
    int64_t nActualTimespan = nLastBlockTime - nFirstBlockTime;
    nActualTimespan = params.AveragingWindowTimespan() + (nActualTimespan - params.AveragingWindowTimespan()) / 4;

    arith_uint256 bnLimit = UintToArith256(params.powLimit);
    const arith_uint256 bnPowLimit = bnLimit;
    arith_uint256 bnNew { bnAvg };
    bnNew /= params.AveragingWindowTimespan();
    bnNew *= nActualTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
