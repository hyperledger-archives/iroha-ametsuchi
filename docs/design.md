#Ametsuchi's Design

#Purpose of Iroha

Iroha is a distributed ledger made to create a verifiable data store, where every action is digitally signed and keep track of digital assets and contracts is trivial. 

## End vision

Iroha will be the platform to easily manage any kind of digital asset, currency, or contract. All the major banks and insurance companies of the world will use Iroha and it will become the backbone of IT infrastructure to make the world more efficient.

#Iroha use cases

##Financial instutions

##Supply chain management



#Iroha's use of Ametsuchi

Iroha stores all its data as flatbuffers. From the data storage to the user-facing apps, they all use flatbuffers, provided via a grpc interface.

#Specific requirements

##Functional requirements

| Requirement ID | FR-01  |
|---|---|
|Title| Save freebuffers to a URI location.|
| Description | We should be able to save a freebuffer to a location specified by a URI. |
|Priority | 10|
|Risk| low| 

| Requirement ID | FR-02  |
|---|---|
|Title| Create Merkle tree from flatbuffers |
| Description | A binary hash tree (Merkle tree) should be constructed from flatbuffers store in Ametsuchi. Digital signatures from the 2f+1 validating nodes should be stored with each Merkle root for the global and block-level roots. |
|Priority | 10|
|Risk| low| 


##Non-functional requirements


