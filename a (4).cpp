#include "json.hpp"
#include <bits/stdc++.h>
#include <openssl/sha.h> // OpenSSL for SHA hashing
#include <filesystem>
#include <fstream>



#define int long long


using namespace std;
namespace fs = std::filesystem;

vector <string> valid_trans;

const string MEMPOOL_DIR = "web3";
const string PREV_BLOCK_HASH = "0000111100000000000000000000000000000000000000000000000000000000";
const string DIFFICULTY_TARGET = "0000ffff00000000000000000000000000000000000000000000000000000000";

struct TransactionInput {
    string txid;
    int vout;
    int sequence;
    bool is_coinbase;
};

struct TransactionOutput {
    int value;
    string scriptpubkey;
};

struct Transaction {
    int version;
    int locktime;
    vector<TransactionInput> vin;
    vector<TransactionOutput> vout;
};

using json = nlohmann::json;


Transaction COINBASE_TRANSACTION = {
    1,
    0,
    {{ "", -1, 0xffffffff, true }},
    {{ 50, "" }}
};

string sha256(const string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

string merkleroot(){
    if (valid_trans.empty()) return "";
    deque <string> dq;
    for(int i=0;i<valid_trans.size();i++)
        dq.push_back(valid_trans[i]);

    while(dq.size()>1){
        if(dq.size()%2!=0)
            dq.push_back(dq.back());
        int n = dq.size();
        for(int i=0;i<n;i=i+2){
            string u = dq.front();
            dq.pop_front();
            u += dq.front();
            dq.pop_front();
            dq.push_back(sha256(u));
        }
    }
    return dq.front();
}

void mineBlock()
{
    string m = merkleroot();
    unsigned int timestamp = static_cast<unsigned int>(time(nullptr));
    int nonce = 0;

    while (true) {
        ofstream output("output.txt");
            // for (const auto& transaction : valid_trans) {
            //     output << transaction << '\n';
            // }
            // break;
        string blockHeader = "1" + PREV_BLOCK_HASH + m + to_string(timestamp) + DIFFICULTY_TARGET + to_string(nonce);
        string current_hash = sha256(blockHeader);
        if (current_hash < DIFFICULTY_TARGET) {
            json blockh;
            blockh["version"] = 1;
            blockh["previous_block_hash"] = PREV_BLOCK_HASH;
            blockh["timestamp"] = timestamp;
            blockh["difficulty_target"] = DIFFICULTY_TARGET;
            blockh["nonce"] = nonce;
            output << "Block Header:\n";
            output << setw(4) << blockh << "\n";
            // output << blockHeader << '\n';
            output << "Coinbase Transaction:\n";
            // output << "Version: " << COINBASE_TRANSACTION.version << '\n';
            // output << "Vin:\n";
            // output << "  Txid: " << COINBASE_TRANSACTION.vin[0].txid << '\n';
            // output << "  Vout: " << COINBASE_TRANSACTION.vin[0].vout << '\n';
            // output << "  Sequence: " << COINBASE_TRANSACTION.vin[0].sequence << '\n';
            // output << "  Is_coinbase: " << std::boolalpha << COINBASE_TRANSACTION.vin[0].is_coinbase << '\n';
            // output << "Vout:\n";
            // output << "  Value: " << COINBASE_TRANSACTION.vout[0].value << '\n';
            // output << "  Scriptpubkey: " << COINBASE_TRANSACTION.vout[0].scriptpubkey << '\n';

            json j;
            j["Version"] = COINBASE_TRANSACTION.version;
            j["Vin"] = json::array({{
                {"Txid", COINBASE_TRANSACTION.vin[0].txid},
                {"Vout", COINBASE_TRANSACTION.vin[0].vout},
                {"Sequence", COINBASE_TRANSACTION.vin[0].sequence},
                {"Is_coinbase", COINBASE_TRANSACTION.vin[0].is_coinbase}
            }});
            j["Vout"] = json::array({{
                {"Value", COINBASE_TRANSACTION.vout[0].value},
                {"Scriptpubkey", COINBASE_TRANSACTION.vout[0].scriptpubkey}
            }});
            output << setw(4) << j << "\n";
            output << "Transaction IDs:\n";
            for (auto transaction : valid_trans) {
                output << transaction << '\n';
            }
            break;
        }
        nonce++;
}
}
bool validate_transaction(const nlohmann::json& j){
    int sum_vin = 0;
    int sum_vout = 0;
    bool found = false;
    for(const auto& vin : j["vin"]){
        found |= vin.contains("txid");
        if (vin.contains("prevout") && vin["prevout"].contains("value"))
        {
            sum_vin += vin["prevout"]["value"].get<int64_t>();
        }
    }
    for (const auto& vout : j["vout"]) {
        if (vout.contains("value")) {
            sum_vout += vout["value"].get<int64_t>();
        }
    }
   // cout << sum_vin << " " << sum_vout << "\n";
    if(sum_vin > sum_vout && found)
        return true;
    else
        return false;

}



int32_t main(){
    for(const auto& entry : fs::directory_iterator(MEMPOOL_DIR)){
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                std::cerr << "Could not open the file: " << entry.path() << std::endl;
                continue;
            }

            nlohmann::json_abi_v3_11_3::json j;
            try {
                file >> j;
            } catch (const json::parse_error& e) {
                std::cerr << "Parse error in file: " << entry.path() << " - " << e.what() << std::endl;
                continue;
            }

            if (validate_transaction(j)) {
                // cout << "YES\n";
                // Extract filename without extension and add to vector
                valid_trans.push_back(entry.path().stem().string());
            }
        }

    }
    mineBlock();
    return 0;
}