// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015-2020 The Zcash Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#include "key_io.h"
#include "main.h"
#include "crypto/equihash.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, const uint256& nNonce, const std::vector<unsigned char>& nSolution, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    // To create a genesis block for a new chain which is Overwintered:
    //   txNew.nVersion = OVERWINTER_TX_VERSION
    //   txNew.fOverwintered = true
    //   txNew.nVersionGroupId = OVERWINTER_VERSION_GROUP_ID
    //   txNew.nExpiryHeight = <default value>
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 545259519 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nSolution = nSolution;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = genesis.BuildMerkleTree();
    return genesis;
}

 /**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database (and is in any case of zero value).
 *
 * >>> from pyblake2 import blake2s
 * >>> 'VECT' + blake2s(b'2020 - Vectorium - In CryptoEnergy we trust').hexdigest()
 */
static CBlock CreateGenesisBlock(uint32_t nTime, const uint256& nNonce, const std::vector<unsigned char>& nSolution, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "VECTeece7ff66b017b9091ecad65593e137d88183f9a2e355c76ae3d8c6d477b84af";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nSolution, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

const arith_uint256 maxUint = UintToArith256(uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        strCurrencyUnits = "VECT";
        // bip44CoinType = 133; // 133 is Zcash registered in https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        consensus.fCoinbaseMustBeShielded = false;
        consensus.nSubsidySlowStartInterval = 0;
        consensus.nPreBlossomSubsidyHalvingInterval = Consensus::PRE_BLOSSOM_HALVING_INTERVAL;
        consensus.nPostBlossomSubsidyHalvingInterval = POST_BLOSSOM_HALVING_INTERVAL(Consensus::PRE_BLOSSOM_HALVING_INTERVAL);
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 4000;
        const size_t N = 192, K = 7;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        consensus.nEquihashN = N;
        consensus.nEquihashK = K;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 2;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 30; // 30% adjustment down
        consensus.nPowMaxAdjustUp = 10; // 10% adjustment up
        consensus.nPreBlossomPowTargetSpacing = Consensus::PRE_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPostBlossomPowTargetSpacing = Consensus::POST_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = boost::none;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170005;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight = 1;
        /*
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].hashActivationBlock =
            uint256S("004fe53899a2cca519c58c6cb3877feec9ecee258f2fde9f70ed274a74cd12c9");
        */
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight = 1;
        
        /*
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].hashActivationBlock =
            uint256S("004fe53899a2cca519c58c6cb3877feec9ecee258f2fde9f70ed274a74cd12c9");
        */

        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nProtocolVersion = 170009;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nProtocolVersion = 170011;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nActivationHeight = 
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nProtocolVersion = 170013;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight = 
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        //consensus.nFutureTimestampSoftForkHeight = 2000000;
        consensus.nFutureTimestampSoftForkHeight = 1;

        consensus.nFundingPeriodLength = consensus.nPostBlossomSubsidyHalvingInterval / 48;

        // guarantees the first 2 characters, when base58 encoded, are "V2"
        keyConstants.base58Prefixes[PUBKEY_ADDRESS]     = {0x0F,0xC7};
        // guarantees the first 2 characters, when base58 encoded, are "t3"
        keyConstants.base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBD};
        // the first character, when base58 encoded, is "5" or "K" or "L" (as in Bitcoin)
        keyConstants.base58Prefixes[SECRET_KEY]         = {0x80};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        keyConstants.base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x88,0xB2,0x1E};
        keyConstants.base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x88,0xAD,0xE4};
        // guarantees the first 2 characters, when base58 encoded, are "zc"
        keyConstants.base58Prefixes[ZCPAYMENT_ADDRESS]  = {0x16,0x9A};
        // guarantees the first 4 characters, when base58 encoded, are "ZiVK"
        keyConstants.base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAB,0xD3};
        // guarantees the first 2 characters, when base58 encoded, are "SK"
        keyConstants.base58Prefixes[ZCSPENDING_KEY]     = {0xAB,0x36};

        keyConstants.bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "zs";
        keyConstants.bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "zviews";
        keyConstants.bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "zivks";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-main";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_FVK]         = "zxviews";

        /*
        {
            std::vector<std::string> ecc_addresses = {
                "t3LmX1cxWPPPqL4TZHx42HU3U5ghbFjRiif",
                "t3Toxk1vJQ6UjWQ42tUJz2rV2feUWkpbTDs",
                "t3ZBdBe4iokmsjdhMuwkxEdqMCFN16YxKe6",
                "t3ZuaJziLM8xZ32rjDUzVjVtyYdDSz8GLWB",
                "t3bAtYWa4bi8VrtvqySxnbr5uqcG9czQGTZ",
                "t3dktADfb5Rmxncpe1HS5BRS5Gcj7MZWYBi",
                "t3hgskquvKKoCtvxw86yN7q8bzwRxNgUZmc",
                "t3R1VrLzwcxAZzkX4mX3KGbWpNsgtYtMntj",
                "t3ff6fhemqPMVujD3AQurxRxTdvS1pPSaa2",
                "t3cEUQFG3KYnFG6qYhPxSNgGi3HDjUPwC3J",
                "t3WR9F5U4QvUFqqx9zFmwT6xFqduqRRXnaa",
                "t3PYc1LWngrdUrJJbHkYPCKvJuvJjcm85Ch",
                "t3bgkjiUeatWNkhxY3cWyLbTxKksAfk561R",
                "t3Z5rrR8zahxUpZ8itmCKhMSfxiKjUp5Dk5",
                "t3PU1j7YW3fJ67jUbkGhSRto8qK2qXCUiW3",
                "t3S3yaT7EwNLaFZCamfsxxKwamQW2aRGEkh",
                "t3eutXKJ9tEaPSxZpmowhzKhPfJvmtwTEZK",
                "t3gbTb7brxLdVVghSPSd3ycGxzHbUpukeDm",
                "t3UCKW2LrHFqPMQFEbZn6FpjqnhAAbfpMYR",
                "t3NyHsrnYbqaySoQqEQRyTWkjvM2PLkU7Uu",
                "t3QEFL6acxuZwiXtW3YvV6njDVGjJ1qeaRo",
                "t3PdBRr2S1XTDzrV8bnZkXF3SJcrzHWe1wj",
                "t3ZWyRPpWRo23pKxTLtWsnfEKeq9T4XPxKM",
                "t3he6QytKCTydhpztykFsSsb9PmBT5JBZLi",
                "t3VWxWDsLb2TURNEP6tA1ZSeQzUmPKFNxRY",
                "t3NmWLvZkbciNAipauzsFRMxoZGqmtJksbz",
                "t3cKr4YxVPvPBG1mCvzaoTTdBNokohsRJ8n",
                "t3T3smGZn6BoSFXWWXa1RaoQdcyaFjMfuYK",
                "t3gkDUe9Gm4GGpjMk86TiJZqhztBVMiUSSA",
                "t3eretuBeBXFHe5jAqeSpUS1cpxVh51fAeb",
                "t3dN8g9zi2UGJdixGe9txeSxeofLS9t3yFQ",
                "t3S799pq9sYBFwccRecoTJ3SvQXRHPrHqvx",
                "t3fhYnv1S5dXwau7GED3c1XErzt4n4vDxmf",
                "t3cmE3vsBc5xfDJKXXZdpydCPSdZqt6AcNi",
                "t3h5fPdjJVHaH4HwynYDM5BB3J7uQaoUwKi",
                "t3Ma35c68BgRX8sdLDJ6WR1PCrKiWHG4Da9",
                "t3LokMKPL1J8rkJZvVpfuH7dLu6oUWqZKQK",
                "t3WFFGbEbhJWnASZxVLw2iTJBZfJGGX73mM",
                "t3L8GLEsUn4QHNaRYcX3EGyXmQ8kjpT1zTa",
                "t3PgfByBhaBSkH8uq4nYJ9ZBX4NhGCJBVYm",
                "t3WecsqKDhWXD4JAgBVcnaCC2itzyNZhJrv",
                "t3ZG9cSfopnsMQupKW5v9sTotjcP5P6RTbn",
                "t3hC1Ywb5zDwUYYV8LwhvF5rZ6m49jxXSG5",
                "t3VgMqDL15ZcyQDeqBsBW3W6rzfftrWP2yB",
                "t3LC94Y6BwLoDtBoK2NuewaEbnko1zvR9rm",
                "t3cWCUZJR3GtALaTcatrrpNJ3MGbMFVLRwQ",
                "t3YYF4rPLVxDcF9hHFsXyc5Yq1TFfbojCY6",
                "t3XHAGxRP2FNfhAjxGjxbrQPYtQQjc3RCQD",
            };

            // ZF and MG each use a single address repeated 48 times,
            // once for each funding period.
            std::vector<std::string> zf_addresses(48, "t3dvVE3SQEi7kqNzwrfNePxZ1d4hUyztBA1");
            std::vector<std::string> mg_addresses(48, "t3XyYW8yBFRuMnfvm5KLGFbEVz25kckZXym");

            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_ECC,
                consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight, 2726400,
                ecc_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_ZF,
                consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight, 2726400,
                zf_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_MG,
                consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight, 2726400,
                mg_addresses);
        }
        */

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");

        /**
         * The message start string should be awesome! ⓩ❤
         */
        pchMessageStart[0] = 0x58; // V + 2
        pchMessageStart[1] = 0x47; // E + 2
        pchMessageStart[2] = 0x45; // C + 2
        pchMessageStart[3] = 0x56; // T + 2
        vAlertPubKey = ParseHex("73B0");
        nDefaultPort = 23141;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(
            1600672674,
            uint256S("0000000000000000000000000000000000000000000000000000000000000025"),
            ParseHex("0129cdc0d0b04c221fdc57b481caa8f726e80901a6f313762e04410d6d0345033c70d5cc6d24585b9a369bb936321b98c5630fb05240727e71ae033b58e995781daaf99ae9fd965d7b39f810a6957cfbb697527c7d653db4015f76b0aa964662c96a5f57044ca52d0dd36d3cd9bda46ab27261442c3aa8baefeeb87e1722ed519c9b944c28321aced89410bc92e9a4e197f5ccea02772b3213e315f917ebc05a574be5a64f62ef295a68922138e7e542652e76e83d95f7853da7c5351bc1bb64751f5ed705f527a90378e0b23bfed4b27c15f5c39a99a287771167dda985fb628911611873afbd1b32287c0d818a91d1e75bc0cf655091ffb1e80dc5893242128a8702ac884202020cb62a8b54aaed1fb048cb13580d3762a9a1057578fad714f4e8dada72ddbd7e1347425705ec763609d082ff19a370c279f819f5a5a6b2d254c5ff0a052e768158d81d9fb70cb3d25ce85f78cc98f3ae22078dd765d8063092a032803434f27c894d213150f30d450d902d7bc537f731523eb260ad23c75e74777666121573ecb0be6ec0ed9162f1"),
            0x207fffff, 4, 0);
        consensus.hashGenesisBlock = genesis.GetHash();

        printf("MAIN GENESIS BLOCK HASH %s\n", consensus.hashGenesisBlock.GetHex().c_str());

        assert(consensus.hashGenesisBlock == uint256S("45c180cf1ea3213f4f66bbc70fbc97a9e41a5d165b173b9dee883b9ab9247f33"));
        assert(genesis.hashMerkleRoot == uint256S("0x6c1ec97352d0263df06de4276cdafc57854c8be0123e51d2340ef3957ddd8661"));

        vFixedSeeds.clear();
        vSeeds.clear();
        /*
        vSeeds.push_back(CDNSSeedData("vectorium.co", "seed1.vectorium.co")); //Vectorium seed node
		vSeeds.push_back(CDNSSeedData("vectorium.co", "seed2.vectorium.co")); //Vectorium seed node
		vSeeds.push_back(CDNSSeedData("vectorium.co", "seed3.vectorium.co")); //Vectorium seed node
		vSeeds.push_back(CDNSSeedData("vectorium.co", "seed4.vectorium.co")); //Vectorium seed node
		vSeeds.push_back(CDNSSeedData("vectorium.co", "seed5.vectorium.co")); //Vectorium seed node
        */
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        
        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock)
            /*
            , // remove ',' on nex checkpoint added
            //(150000, uint256S("0x00006fcaba09059d8affe7ebd5e4403fc52a1c66fb9307e495c7a915436d0f34")),
            1600672674, // * UNIX timestamp of last checkpoint block
            0, // * total number of transactions between genesis and last checkpoint
            2900 // * estimated number of transactions per day after checkpoint
            */
        };

        // Hardcoded fallback value for the Sprout shielded value pool balance
        // for nodes that have not reindexed since the introduction of monitoring
        // in #2795.
        nSproutValuePoolCheckpointHeight = 1000;
        nSproutValuePoolCheckpointBalance = 0;
        fZIP209Enabled = true;
        //hashSproutValuePoolCheckpointBlock = uint256S("0000000000c7b46b6bc04b4cbf87d8bb08722aebd51232619b214f7273f8460e");
        
        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = {};
        assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight(0));

        // Licensed miners
		consensus.nMinerLicenseActivationHeight = 1;
		vLicensedMiners = {
			"V2HCBY842NAEsRmHRQmGKUtU2NsVsG6MfCe",
            "V2WovBQYkcrBJHXzJvvsFHFo7pjcwVg4vvP",
            "V2KdJYmLmE2KjpAEjbWyYnVtwpwE3m43shV",
            "V2Xs2B5P44wifA9UdcQDqiVGsHfPJ5yan61",
            "V2USbTUzb8kaxnD86seSHpMZdYHCsrY61Rv",
            "V2Mjokjy9hp2fWEoxRN1zW5GeH7DuDwSySk",
            "V2bs7WkccgPZt1diz7PyKXxqixajYB6Pw3Y",
            "V2TPYdag9Vn4WUT6NA6M33ntAdFGEnHgEUA",
            "V2Rekv5TxU3fc5YoA5d22zC9BtXtSNnYB1b",
            "V2ZdizCZFyBV3B3EhFGoGACo3uzekh5Nefs",
            "V2YcDbKP65hEC5uirP8TQucjRuJDZ6RYUHN",
            "V2NMeiL4aTvnkyzhcrVMuLfCX5J2QywWrrT",
            "V2apzmaPBwGq5faYcr642FiHC3uH7Qr2Pk7",
            "V2MGfKMMpupaaWuyYfW5ewhBC7bHiRqFnZo",
            "V2bDoyssGSHaLnkJDgL3d56ujEsVXjvtccE",
            "V2USXPya3ieu7Sjb1RAuiieJwpkfEQayVpx",
            "V2DqLxpXJ4ZwhDxmNch7MbqDemNxwprGqNo",
            "V2DnR3XecmSiWXbxu5sYVe9EBpamp9P2Smu",
            "V2M71XMnTgd3uHqDzvQ5yVRLLKyFA8sniyR",
            "V2G2hzTNLh4ZpQ3uvdWNW8B34g4xLQMQoD3",
            "V2XWdfgadkzRZbAhPaioysaWo8pR5EkcXSU",
            "V2SvdDn8tcmCbkuwFAM6qSCtUzdJfWzfq9w",
            "V2GcbvHt4KpNFW3PTkpiJPjppCCwWYhNKkr",
            "V2EeFrTALRLwEyKTmMRK2fW1p9Q1ZEByvNd",
            "V2EecTJu6fjRwxYunFvWGsiwsyL2fRBjSUg"
		};

    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        strCurrencyUnits = "TVEC";
        bip44CoinType = 1;
        consensus.fCoinbaseMustBeShielded = false;
        consensus.nSubsidySlowStartInterval = 0;
        consensus.nPreBlossomSubsidyHalvingInterval = Consensus::PRE_BLOSSOM_HALVING_INTERVAL;
        consensus.nPostBlossomSubsidyHalvingInterval = POST_BLOSSOM_HALVING_INTERVAL(Consensus::PRE_BLOSSOM_HALVING_INTERVAL);
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 400;
        const size_t N = 200, K = 9;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        consensus.nEquihashN = N;
        consensus.nEquihashK = K;
        consensus.powLimit = uint256S("07ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 2;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 30; // 30% adjustment down
        consensus.nPowMaxAdjustUp = 10; // 10% adjustment up
        consensus.nPreBlossomPowTargetSpacing = Consensus::PRE_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPostBlossomPowTargetSpacing = Consensus::POST_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = boost::none;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170003;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight = 1000;
        // n/a, testnet not active
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].hashActivationBlock =
            uint256S("0000257c4331b098045023fcfbfa2474681f4564ab483f84e4e1ad078e4acf44");
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight = 1000;
        // n/a, testnet not active
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].hashActivationBlock =
            uint256S("000420e7fcc3a49d729479fb0b560dd7b8617b178a08e9e389620a9d1dd6361a");
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight = 
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        // n/a, testnet not active
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].hashActivationBlock =
            uint256S("00367515ef2e781b8c9358b443b6329572599edd02c59e8af67db9785122f298");
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nProtocolVersion = 170010;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nActivationHeight = 
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        // n/a, testnet not active
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].hashActivationBlock =
            uint256S("05688d8a0e9ff7c04f6f05e6d695dc5ab43b9c4803342d77ae360b2b27d2468e");
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nProtocolVersion = 170012;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight = 
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        consensus.nFundingPeriodLength = consensus.nPostBlossomSubsidyHalvingInterval / 48;

        // guarantees the first 2 characters, when base58 encoded, are "tm"
        keyConstants.base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0x25};
        // guarantees the first 2 characters, when base58 encoded, are "t2"
        keyConstants.base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        // the first character, when base58 encoded, is "9" or "c" (as in Bitcoin)
        keyConstants.base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        keyConstants.base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        keyConstants.base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        // guarantees the first 2 characters, when base58 encoded, are "zt"
        keyConstants.base58Prefixes[ZCPAYMENT_ADDRESS]  = {0x16,0xB6};
        // guarantees the first 4 characters, when base58 encoded, are "ZiVt"
        keyConstants.base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAC,0x0C};
        // guarantees the first 2 characters, when base58 encoded, are "ST"
        keyConstants.base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        keyConstants.bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "ztestsapling";
        keyConstants.bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "zviewtestsapling";
        keyConstants.bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "zivktestsapling";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-test";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_FVK]         = "zxviewtestsapling";

        /*
        // Testnet funding streams
        {
            std::vector<std::string> ecc_addresses = {
                "t26ovBdKAJLtrvBsE2QGF4nqBkEuptuPFZz",
                "t26ovBdKAJLtrvBsE2QGF4nqBkEuptuPFZz",
                "t26ovBdKAJLtrvBsE2QGF4nqBkEuptuPFZz",
                "t26ovBdKAJLtrvBsE2QGF4nqBkEuptuPFZz",
                "t2NNHrgPpE388atmWSF4DxAb3xAoW5Yp45M",
                "t2VMN28itPyMeMHBEd9Z1hm6YLkQcGA1Wwe",
                "t2CHa1TtdfUV8UYhNm7oxbzRyfr8616BYh2",
                "t2F77xtr28U96Z2bC53ZEdTnQSUAyDuoa67",
                "t2ARrzhbgcpoVBDPivUuj6PzXzDkTBPqfcT",
                "t278aQ8XbvFR15mecRguiJDQQVRNnkU8kJw",
                "t2Dp1BGnZsrTXZoEWLyjHmg3EPvmwBnPDGB",
                "t2KzeqXgf4ju33hiSqCuKDb8iHjPCjMq9iL",
                "t2Nyxqv1BiWY1eUSiuxVw36oveawYuo18tr",
                "t2DKFk5JRsVoiuinK8Ti6eM4Yp7v8BbfTyH",
                "t2CUaBca4k1x36SC4q8Nc8eBoqkMpF3CaLg",
                "t296SiKL7L5wvFmEdMxVLz1oYgd6fTfcbZj",
                "t29fBCFbhgsjL3XYEZ1yk1TUh7eTusB6dPg",
                "t2FGofLJXa419A76Gpf5ncxQB4gQXiQMXjK",
                "t2ExfrnRVnRiXDvxerQ8nZbcUQvNvAJA6Qu",
                "t28JUffLp47eKPRHKvwSPzX27i9ow8LSXHx",
                "t2JXWPtrtyL861rFWMZVtm3yfgxAf4H7uPA",
                "t2QdgbJoWfYHgyvEDEZBjHmgkr9yNJff3Hi",
                "t2QW43nkco8r32ZGRN6iw6eSzyDjkMwCV3n",
                "t2DgYDXMJTYLwNcxighQ9RCgPxMVATRcUdC",
                "t2Bop7dg33HGZx3wunnQzi2R2ntfpjuti3M",
                "t2HVeEwovcLq9RstAbYkqngXNEsCe2vjJh9",
                "t2HxbP5keQSx7p592zWQ5bJ5GrMmGDsV2Xa",
                "t2TJzUg2matao3mztBRJoWnJY6ekUau6tPD",
                "t29pMzxmo6wod25YhswcjKv3AFRNiBZHuhj",
                "t2QBQMRiJKYjshJpE6RhbF7GLo51yE6d4wZ",
                "t2F5RqnqguzZeiLtYHFx4yYfy6pDnut7tw5",
                "t2CHvyZANE7XCtg8AhZnrcHCC7Ys1jJhK13",
                "t2BRzpMdrGWZJ2upsaNQv6fSbkbTy7EitLo",
                "t2BFixHGQMAWDY67LyTN514xRAB94iEjXp3",
                "t2Uvz1iVPzBEWfQBH1p7NZJsFhD74tKaG8V",
                "t2CmFDj5q6rJSRZeHf1SdrowinyMNcj438n",
                "t2ErNvWEReTfPDBaNizjMPVssz66aVZh1hZ",
                "t2GeJQ8wBUiHKDVzVM5ZtKfY5reCg7CnASs",
                "t2L2eFtkKv1G6j55kLytKXTGuir4raAy3yr",
                "t2EK2b87dpPazb7VvmEGc8iR6SJ289RywGL",
                "t2DJ7RKeZJxdA4nZn8hRGXE8NUyTzjujph9",
                "t2K1pXo4eByuWpKLkssyMLe8QKUbxnfFC3H",
                "t2TB4mbSpuAcCWkH94Leb27FnRxo16AEHDg",
                "t2Phx4gVL4YRnNsH3jM1M7jE4Fo329E66Na",
                "t2VQZGmeNomN8c3USefeLL9nmU6M8x8CVzC",
                "t2RicCvTVTY5y9JkreSRv3Xs8q2K67YxHLi",
                "t2JrSLxTGc8wtPDe9hwbaeUjCrCfc4iZnDD",
                "t2Uh9Au1PDDSw117sAbGivKREkmMxVC5tZo",
                "t2FDwoJKLeEBMTy3oP7RLQ1Fihhvz49a3Bv",
                "t2FY18mrgtb7QLeHA8ShnxLXuW8cNQ2n1v8",
                "t2L15TkDYum7dnQRBqfvWdRe8Yw3jVy9z7g",
            };

            // ZF and MG use the same address for each funding period
            std::vector<std::string> zf_addresses(51, "t27eWDgjFYJGVXmzrXeVjnb5J3uXDM9xH9v");
            std::vector<std::string> mg_addresses(51, "t2Gvxv2uNM7hbbACjNox4H6DjByoKZ2Fa3P");

            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_ECC,
                consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight, 2796000,
                ecc_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_ZF,
                consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight, 2796000,
                zf_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_MG,
                consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight, 2796000,
                mg_addresses);
        }
        */

        // On testnet we activate this rule 6 blocks after Blossom activation. From block 299188 and
        // prior to Blossom activation, the testnet minimum-difficulty threshold was 15 minutes (i.e.
        // a minimum difficulty block can be mined if no block is mined normally within 15 minutes):
        // <https://zips.z.cash/zip-0205#change-to-difficulty-adjustment-on-testnet>
        // However the median-time-past is 6 blocks behind, and the worst-case time for 7 blocks at a
        // 15-minute spacing is ~105 minutes, which exceeds the limit imposed by the soft fork of
        // 90 minutes.
        //
        // After Blossom, the minimum difficulty threshold time is changed to 6 times the block target
        // spacing, which is 7.5 minutes:
        // <https://zips.z.cash/zip-0208#minimum-difficulty-blocks-on-the-test-network>
        // 7 times that is 52.5 minutes which is well within the limit imposed by the soft fork.

        static_assert(6 * Consensus::POST_BLOSSOM_POW_TARGET_SPACING * 7 < MAX_FUTURE_BLOCK_TIME_MTP - 60,
                      "MAX_FUTURE_BLOCK_TIME_MTP is too low given block target spacing");
        consensus.nFutureTimestampSoftForkHeight = consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight + 6;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");

        pchMessageStart[0] = 0x59; // V + 3
        pchMessageStart[1] = 0x48; // E + 3
        pchMessageStart[2] = 0x46; // C + 3
        pchMessageStart[3] = 0x57; // T + 3
        vAlertPubKey = ParseHex("73B0");
        nDefaultPort = 23242;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(
            1600672020,
            uint256S("0000000000000000000000000000000000000000000000000000000000000149"),
            ParseHex("0006775abac75182307bc0dd8048d7d0cc8b7121d51b4bdfb706427091086f4ee6280871e2d42dc207255496edcf4f8a6f3c14f4dd493054876b5979722bc2c84e4227131a11497b1a95f028cb59aa5391d6210e9d3366c613e753aa773f8a9595efc3d0026564d144800eca8635d6a4f177bd5cadb84c64087f549c8125b7846cfc609719b83ecb3c79372115486dcb497681dc236011c7ea202c0ab628fc1b35a7a199fb0edbf7851cd715ac03851dbd183957490b993c6a098ac1e4feadb6d9f9b76a07a330fa055e7c762d832f030b5a2fe15059386e3ee4f06a68b53fdf4b13b1e9cbe8d5651741977f53e41c98ff6d6cf9274910cc5e511034597c0b1d8c183d6fa8fb836a4eda41faeedbcfedeff5e41fc3cc447217946d76758519332d1a65c3db36e6ab878b1af2150cdfe359d849ccd9dd445506f43f7543c7aa19ecffd11ca1183360747af460adc83086498427def21cf6210d34e99cc9d8312867c6d061dc4b733fae18d673c977dfca230ecd4ffd7ffa695653ff1cb79db17b7ab851f7293ba3e924db71019bfd4836"),
            0x2007ffff, 4, 0);
        consensus.hashGenesisBlock = genesis.GetHash();

        printf("TESTNET GENESIS BLOCK HASH %s\n", consensus.hashGenesisBlock.GetHex().c_str());

        assert(consensus.hashGenesisBlock == uint256S("004fe6aa44c6c717bd0bca97488539803ab93a44e05ca21c142b5d9ec803953b"));
        assert(genesis.hashMerkleRoot == uint256S("0x6c1ec97352d0263df06de4276cdafc57854c8be0123e51d2340ef3957ddd8661"));

        vFixedSeeds.clear();
        vSeeds.clear();
    
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;


        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock)
        };

        // Hardcoded fallback value for the Sprout shielded value pool balance
        // for nodes that have not reindexed since the introduction of monitoring
        // in #2795.
        nSproutValuePoolCheckpointHeight = 0;
        nSproutValuePoolCheckpointBalance = 0;
        fZIP209Enabled = true;
        hashSproutValuePoolCheckpointBlock = uint256S("06bb79575e1c22e2b4b1f13c2e6ed4f00c8cc84216375cfe7d01bc9f63dc9ea8");
        
        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = {};
        assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight(0));

        // Licensed miners
        consensus.nMinerLicenseActivationHeight = 4200;
		vLicensedMiners = {
			"tmGYsG7Xux1VzjWMWcR4r6AfPXRdxbnDixP",
			"tmRoDACm8Du6AaVtiL8dExQPXYkYR1TyWDE",
			"tmD4hDRe6n8j96PgbshRLSfw2eQEwYhyRk9",
			"tmTD7ARusdZoe5fub9kLwbqoPhthYxF3S3C",
			"tmQNp3gsyKFEQxrYzDJ9ju1jBW4S1gFg6QP",
			"tmKMfe7GLXKfhZ1GuZmpcdMU6bHyeHiLA6q",
			"tmD5ngr5YBhWMbiGqcSWGsowxuZxHmWSK6H",
			"tmNcNsDNMiZGBTQ3FLp1voJdfYCZXGfXSxr",
			"tmHABLhavt1Z5124UTWXGkkCG7kt8T72CnR",
			"tmFSoh6hehcWPPNPw6r1Ha4Gx9D3fMKrmW2",
			"tmBJU9MRbCTuRcUE3TgzAStQkmuyhei3ekK",
			"tmLR2TaN2JwgreWPtkoG3agCa3xBaY4FFxZ"
		};

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        strCurrencyUnits = "REG";
        bip44CoinType = 1;
        consensus.fCoinbaseMustBeShielded = false;
        consensus.nSubsidySlowStartInterval = 0;
        consensus.nPreBlossomSubsidyHalvingInterval = Consensus::PRE_BLOSSOM_REGTEST_HALVING_INTERVAL;
        consensus.nPostBlossomSubsidyHalvingInterval = POST_BLOSSOM_HALVING_INTERVAL(Consensus::PRE_BLOSSOM_REGTEST_HALVING_INTERVAL);
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        const size_t N = 48, K = 5;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        consensus.nEquihashN = N;
        consensus.nEquihashK = K;
        consensus.powLimit = uint256S("0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
        consensus.nPowAveragingWindow = 2;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 0; // Turn off adjustment down
        consensus.nPowMaxAdjustUp = 0; // Turn off adjustment up
        consensus.nPreBlossomPowTargetSpacing = Consensus::PRE_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPostBlossomPowTargetSpacing = Consensus::POST_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = 0;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170003;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170006;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nProtocolVersion = 170010;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nProtocolVersion = 170012;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        consensus.nFundingPeriodLength = consensus.nPostBlossomSubsidyHalvingInterval / 48;
        // Defined funding streams can be enabled with node config flags.

        // These prefixes are the same as the testnet prefixes
        keyConstants.base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0x25};
        keyConstants.base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        keyConstants.base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        keyConstants.base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        keyConstants.base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        keyConstants.base58Prefixes[ZCPAYMENT_ADDRESS]  = {0x16,0xB6};
        keyConstants.base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAC,0x0C};
        keyConstants.base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        keyConstants.bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "zregtestsapling";
        keyConstants.bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "zviewregtestsapling";
        keyConstants.bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "zivkregtestsapling";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-regtest";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_FVK]         = "zxviewregtestsapling";

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        pchMessageStart[0] = 0xaa;
        pchMessageStart[1] = 0xe8;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x5f;
        vAlertPubKey = ParseHex("73B0");
        nDefaultPort = 18344;
        nPruneAfterHeight = 1000;

       genesis = CreateGenesisBlock(
            1600672020,
            uint256S("000000000000000000000000000000000000000000000000000000000000077a"),
            ParseHex("06f7878990d244d1832f3c7760734cf5978a12f3d52b0422b9e1a22329b35ad5cc5b59ea"),
            0x200f0f0f, 4, 0);
        consensus.hashGenesisBlock = genesis.GetHash();

        printf("REGTEST GENESIS BLOCK HASH %s\n", consensus.hashGenesisBlock.GetHex().c_str());

        assert(consensus.hashGenesisBlock == uint256S("0a8e1431dbd9d109c50cffdced89bb8367c71c71d67f89adca2fc6e4263b55b5"));
        assert(genesis.hashMerkleRoot == uint256S("6c1ec97352d0263df06de4276cdafc57854c8be0123e51d2340ef3957ddd8661"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock)
        };
        
        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = {};
        assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight(0));
    }

    void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
    {
        assert(idx > Consensus::BASE_SPROUT && idx < Consensus::MAX_NETWORK_UPGRADES);
        consensus.vUpgrades[idx].nActivationHeight = nActivationHeight;
    }

    void UpdateFundingStreamParameters(Consensus::FundingStreamIndex idx, Consensus::FundingStream fs)
    {
        assert(idx >= Consensus::FIRST_FUNDING_STREAM && idx < Consensus::MAX_FUNDING_STREAMS);
        consensus.vFundingStreams[idx] = fs;
    }

    void UpdateRegtestPow(int64_t nPowMaxAdjustDown, int64_t nPowMaxAdjustUp, uint256 powLimit)
    {
        consensus.nPowMaxAdjustDown = nPowMaxAdjustDown;
        consensus.nPowMaxAdjustUp = nPowMaxAdjustUp;
        consensus.powLimit = powLimit;
    }

    void SetRegTestZIP209Enabled() {
        fZIP209Enabled = true;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);

    // Some python qa rpc tests need to enforce the coinbase consensus rule
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestshieldcoinbase")) {
        regTestParams.SetRegTestCoinbaseMustBeShielded();
}

    // When a developer is debugging turnstile violations in regtest mode, enable ZIP209
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-developersetpoolsizezero")) {
        regTestParams.SetRegTestZIP209Enabled();
    }
}


// Block height must be >0 and <=last founders reward block height
// Index variable i ranges from 0 - (vFoundersRewardAddress.size()-1)
std::string CChainParams::GetFoundersRewardAddressAtHeight(int nHeight) const {
    int preBlossomMaxHeight = consensus.GetLastFoundersRewardBlockHeight(0);
    // zip208
    // FounderAddressAdjustedHeight(height) :=
    // height, if not IsBlossomActivated(height)
    // BlossomActivationHeight + floor((height - BlossomActivationHeight) / BlossomPoWTargetSpacingRatio), otherwise
    bool blossomActive = consensus.NetworkUpgradeActive(nHeight, Consensus::UPGRADE_BLOSSOM);
    if (blossomActive) {
        int blossomActivationHeight = consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight;
        nHeight = blossomActivationHeight + ((nHeight - blossomActivationHeight) / Consensus::BLOSSOM_POW_TARGET_SPACING_RATIO);
    }
    assert(nHeight > 0 && nHeight <= preBlossomMaxHeight);
    size_t addressChangeInterval = (preBlossomMaxHeight + vFoundersRewardAddress.size()) / vFoundersRewardAddress.size();
    size_t i = nHeight / addressChangeInterval;
    return vFoundersRewardAddress[i];
}

// Block height must be >0 and <=last founders reward block height
// The founders reward address is expected to be a multisig (P2SH) address
CScript CChainParams::GetFoundersRewardScriptAtHeight(int nHeight) const {
    assert(nHeight > 0 && nHeight <= consensus.GetLastFoundersRewardBlockHeight(nHeight));

    KeyIO keyIO(*this);
    CTxDestination address = keyIO.DecodeDestination(GetFoundersRewardAddressAtHeight(nHeight).c_str());
    assert(IsValidDestination(address));
    assert(IsScriptDestination(address));
    CScriptID scriptID = boost::get<CScriptID>(address); // address is a boost variant
    CScript script = CScript() << OP_HASH160 << ToByteVector(scriptID) << OP_EQUAL;
    return script;
}

std::string CChainParams::GetFoundersRewardAddressAtIndex(int i) const {
    assert(i >= 0 && i < vFoundersRewardAddress.size());
    return vFoundersRewardAddress[i];
}

bool CChainParams::IsLicensedMiner(std::string address) const {
	auto result = std::find(std::begin(vLicensedMiners), std::end(vLicensedMiners), address);
	return (result != std::end(vLicensedMiners));
}

void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
{
    regTestParams.UpdateNetworkUpgradeParameters(idx, nActivationHeight);
}

void UpdateFundingStreamParameters(Consensus::FundingStreamIndex idx, Consensus::FundingStream fs)
{
    regTestParams.UpdateFundingStreamParameters(idx, fs);
}

void UpdateRegtestPow(int64_t nPowMaxAdjustDown, int64_t nPowMaxAdjustUp, uint256 powLimit) {
    regTestParams.UpdateRegtestPow(nPowMaxAdjustDown, nPowMaxAdjustUp, powLimit);
}
