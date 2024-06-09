## Design Approach

### Objective
The objective of this assignment is to duplicate a mining process,where we need to construct a block, validate transactions and find a hash which is less than the target hash so that the block gets mined. 

###  Functions
1. #### Validate_transaction : 
    It validates transaction if the value of "vin" is greater than value of "vout", as each transaction uses output from previous transaction as input called "prevout" , so to maintain a balance "vin" must be greater than "vout".It does so by parsing the json files using "json.hpp" library. It also ensures that txid is not empty by using a boolean found.

2. #### merkleroot:
    It calculates merkle root by hashing every valid txid together in pairs till only one hash remains. For this I made a deque which will iterate its size/2 times to hash the pairs, and repeat until only one hash remains.I used a deque as it allows me to pop,push both from back and front in O(1) time. The hash is calculated using sha256 function which uses <openssl/sha.h> library.

3. #### mineBlock:
    It creates a block header which includes basic info like version,Previous block hash,Merkle root ,Timestamp ,Difficulty target ,Nonce which is initially 0 and is incremented till the block hash(hash of concatenated string of block header) becomes lesser than the target hash in order to be mined. And then when it becomes lesser, it creates an output file output.txt which contains Block header ,coin base transaction and valid transaction Ids.

### Variables
- `valid_trans`: Vector to store valid transaction IDs.
- `MEMPOOL_DIR`: Directory containing JSON files with transactions.
- `PREV_BLOCK_HASH`: Hash of the previous block.
- `DIFFICULTY_TARGET`: Target value that the hash of the block header must be less than.
- `TransactionInput`, `TransactionOutput`, `Transaction`: Structures to represent transactions.
- `COINBASE_TRANSACTION`: Predefined coinbase transaction included in the block.

### Insights
The project successfully demonstrates the fundamental concepts of blockchain such as transaction validation, Merkle root calculation, block elements and mining process.

### Result
The program reads JSON files from the specified directory, validates transactions, constructs the Merkle root, and mines a block by iterating through nonce values.

### Future Improvements
Enhance transaction validation with more criterias like locktime, sequence.
