#include <gtest/gtest.h>

#include "primitives/transaction.h"
#include "snowgem/Note.hpp"
#include "snowgem/Address.hpp"

extern ZCJoinSplit* params;
extern int GenZero(int n);
extern int GenMax(int n);

TEST(Transaction, JSDescriptionRandomized) {
    // construct a merkle tree
    ZCIncrementalMerkleTree merkleTree;

    libsnowgem::SpendingKey k = libsnowgem::SpendingKey::random();
    libsnowgem::PaymentAddress addr = k.address();

    libsnowgem::Note note(addr.a_pk, 100, uint256(), uint256());

    // commitment from coin
    uint256 commitment = note.cm();

    // insert commitment into the merkle tree
    merkleTree.append(commitment);

    // compute the merkle root we will be working with
    uint256 rt = merkleTree.root();

    auto witness = merkleTree.witness();

    // create JSDescription
    uint256 pubKeyHash;
    boost::array<libsnowgem::JSInput, ZC_NUM_JS_INPUTS> inputs = {
        libsnowgem::JSInput(witness, note, k),
        libsnowgem::JSInput() // dummy input of zero value
    };
    boost::array<libsnowgem::JSOutput, ZC_NUM_JS_OUTPUTS> outputs = {
        libsnowgem::JSOutput(addr, 50),
        libsnowgem::JSOutput(addr, 50)
    };
    boost::array<size_t, ZC_NUM_JS_INPUTS> inputMap;
    boost::array<size_t, ZC_NUM_JS_OUTPUTS> outputMap;

    {
        auto jsdesc = JSDescription::Randomized(
            *params, pubKeyHash, rt,
            inputs, outputs,
            inputMap, outputMap,
            0, 0, false);

        std::set<size_t> inputSet(inputMap.begin(), inputMap.end());
        std::set<size_t> expectedInputSet {0, 1};
        EXPECT_EQ(expectedInputSet, inputSet);

        std::set<size_t> outputSet(outputMap.begin(), outputMap.end());
        std::set<size_t> expectedOutputSet {0, 1};
        EXPECT_EQ(expectedOutputSet, outputSet);
    }

    {
        auto jsdesc = JSDescription::Randomized(
            *params, pubKeyHash, rt,
            inputs, outputs,
            inputMap, outputMap,
            0, 0, false, nullptr, GenZero);

        boost::array<size_t, ZC_NUM_JS_INPUTS> expectedInputMap {1, 0};
        boost::array<size_t, ZC_NUM_JS_OUTPUTS> expectedOutputMap {1, 0};
        EXPECT_EQ(expectedInputMap, inputMap);
        EXPECT_EQ(expectedOutputMap, outputMap);
    }

    {
        auto jsdesc = JSDescription::Randomized(
            *params, pubKeyHash, rt,
            inputs, outputs,
            inputMap, outputMap,
            0, 0, false, nullptr, GenMax);

        boost::array<size_t, ZC_NUM_JS_INPUTS> expectedInputMap {0, 1};
        boost::array<size_t, ZC_NUM_JS_OUTPUTS> expectedOutputMap {0, 1};
        EXPECT_EQ(expectedInputMap, inputMap);
        EXPECT_EQ(expectedOutputMap, outputMap);
    }
}
