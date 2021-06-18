#ifndef BETPROTOCOL_H
#define BETPROTOCOL_H

#include "cc/eval.h"
#include "pubkey.h"
#include "consensus/merkle.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include "cryptoconditions/include/cryptoconditions.h"


static uint256 ComputeMerkleRootFromBranch(const uint256& leaf, const std::vector<uint256>& vMerkleBranch, uint32_t nIndex) {
    uint256 hash = leaf;
    for (std::vector<uint256>::const_iterator it = vMerkleBranch.begin(); it != vMerkleBranch.end(); ++it) {
        if (nIndex & 1) {
            hash = Hash(*it, hash);
        } else {
            hash = Hash(hash, *it);
        }
        nIndex >>= 1;
    }
    return hash;
}

class MoMProof
{
public:
    unsigned long nIndex;
    std::vector<uint256> branch;
    uint256 notarisationHash;

    MoMProof() {}
    MoMProof(unsigned long i, std::vector<uint256> b, uint256 n) :
        nIndex(i), branch(b), notarisationHash(n) {}
    uint256 Exec(uint256 hash) { return ComputeMerkleRootFromBranch(hash, branch, nIndex); }

    SERIALIZE_METHODS(MoMProof, obj) { READWRITE(obj.nIndex, obj.branch, obj.notarisationHash); }
};


class BetProtocol
{
protected:
    std::vector<CC*> playerConditions();
public:
    EvalCode disputeCode;
    uint32_t waitBlocks;
    std::vector<unsigned char> vmParams;
    std::vector<CPubKey> players;

    // Utility
    BetProtocol(EvalCode dc, std::vector<CPubKey> ps, uint32_t wb, std::vector<uint8_t> vmp)
        : disputeCode(dc), waitBlocks(wb), vmParams(vmp), players(ps) {}
    std::vector<CC*> PlayerConditions();

    // on PANGEA
    CC* MakeDisputeCond();
    CMutableTransaction MakeSessionTx(CAmount spendFee);
    CMutableTransaction MakeDisputeTx(uint256 signedSessionTxHash, uint256 vmResultHash);
    CMutableTransaction MakePostEvidenceTx(uint256 signedSessionTxHash,
            int playerIndex, std::vector<unsigned char> state);

    // on KMD
    CC* MakePayoutCond(uint256 signedSessionTxHash);
    CMutableTransaction MakeStakeTx(CAmount totalPayout, uint256 signedSessionTx);
    CMutableTransaction MakeAgreePayoutTx(std::vector<CTxOut> payouts, uint256 signedStakeTxHash);
    CMutableTransaction MakeImportPayoutTx(std::vector<CTxOut> payouts,
            CTransaction signedDisputeTx, uint256 signedStakeTxHash, MoMProof momProof);
};



bool GetOpReturnHash(CScript script, uint256 &hash);


#endif /* BETPROTOCOL_H */
