#Ametsuchi's Design

#Purpose of Iroha

Iroha is a distributed ledger made to create a verifiable data store, where every action is digitally signed and keep track of digital assets and contracts is trivial. 

## End vision

Iroha will be the platform to easily manage any kind of digital asset, currency, or contract. All the major banks and insurance companies of the world will use Iroha and it will become the backbone of IT infrastructure to make the world more efficient.

#Iroha use cases

##Financial instutions

##Supply chain management

##Electronic payments

Electronic payments, especially mobile payments, can benefit from Iroha for managing digital currencies in real time. Iroha aims to achieve transaction finality within 2 seconds internally for many network configurations, so this allows for the creation of mobile apps for enabling face-to-face payments.


#Iroha's use of Ametsuchi

Iroha stores all its data as flatbuffers. From the data storage to the user-facing apps, they all use flatbuffers, provided via a grpc interface.

#Specific requirements

##Functional requirements

| Requirement ID | FR-01  |
|---|---|
|Title| Save flatbuffers to a URI location.|
| Description | We should be able to save a flatbuffer to a location specified by a URI. |
|Priority | 10|
|Risk| low| 

| Requirement ID | FR-02  |
|---|---|
|Title| Create Merkle tree from flatbuffers |
| Description | A binary hash tree (Merkle tree) should be constructed from flatbuffers store in Ametsuchi. Digital signatures from the 2f+1 validating nodes should be stored with each Merkle root for the global and block-level roots. |
|Priority | 10|
|Risk| medium| 

| Requirement ID | FR-03  |
|---|---|
|Title| Create cache of global state. |
| Description | There should be a cache of the global state, calculated by applying the transactions in the ledger in order. For example, if a digital Yen asset is created and 100 Yen are sent to account *A*, then the cache of the global state will have some record like: **A: 100 Yen**. This cache needs to be updated *atomically* when new transactions are committed to the ledger. There should also be some way to audit this cache periodically to guarantee that it has not been hacked. An attacker could potentially hack this cache to change an account balance, without touching the confirmed ledger. This is a risk for financial applications, as APIs used in production apps will simply access this cache. |
|Priority | 10|
|Risk| high | 


##Non-functional requirements


